#include <atomic>
#include <mutex>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include "lib/simple_thread_executor.h"
#include "lib/callback_scheduler.h"

using namespace std::chrono_literals;

void IncTestExecutor(std::unique_ptr<ICallbackExecutor> executor) {
	constexpr int ITER_COUNT = 10000;
	std::atomic<int> number{0};

	auto inc = [&number]() {
		++number;
	};

	for (int i = 0; i < ITER_COUNT; ++i ) {
		executor->Invoke(inc);
	}
	executor.reset();

	EXPECT_EQ(number, ITER_COUNT);
}

TEST(simple_thread_executor, inc) {
	IncTestExecutor(std::make_unique<TSimpleThreadExecutor>());
}

TEST(callback_scheduler, inc) {
	TCallbackScheduler scheduler(std::make_shared<TSimpleThreadExecutor>());
	constexpr int ITER_COUNT = 10;
	std::atomic<int> number{0};

	auto inc = [&number]() {
		++number;
	};
	for (int i = 1; i <= ITER_COUNT; ++i) {
		scheduler.AddTask(inc, i*TCallbackScheduler::TDuration(100ms));
	}
	std::this_thread::sleep_for(50ms);

	for (int i = 0; i < ITER_COUNT; ++i) {
		EXPECT_EQ(number, i);
		std::this_thread::sleep_for(100ms);
	}

	EXPECT_EQ(number, ITER_COUNT);
}

TEST(callback_scheduler, complex) {
	TCallbackScheduler scheduler(std::make_shared<TSimpleThreadExecutor>());
	int number{0};
	std::mutex mutex;

	auto inc = [&number, &mutex]() {
		std::lock_guard guard(mutex);
		++number;
	};
	auto twice = [&number, &mutex]() {
		std::lock_guard guard(mutex);
		number*=2;
	};

	scheduler.AddTask(inc, TCallbackScheduler::TDuration(30ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(200ms));
	scheduler.AddTask(twice, TCallbackScheduler::TDuration(300ms));
	scheduler.AddTask(twice, TCallbackScheduler::TDuration(150ms));
	scheduler.AddTask(twice, TCallbackScheduler::TDuration(60ms));
	scheduler.AddTask(twice, TCallbackScheduler::TDuration(90ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(120ms));

	std::this_thread::sleep_for(135ms);
	EXPECT_EQ(number, 5);
	std::this_thread::sleep_for(40ms);
	EXPECT_EQ(number, 10);
	std::this_thread::sleep_for(200ms);
	EXPECT_EQ(number, 22);
}

TEST(callback_scheduler, none) {
	TCallbackScheduler scheduler(std::make_shared<TSimpleThreadExecutor>());
}

TEST(callback_scheduler, no_time) {
	TCallbackScheduler scheduler(std::make_shared<TSimpleThreadExecutor>());
	std::atomic<int> number{0};

	auto inc = [&number]() {
		++number;
	};
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(0ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(1ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(2ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(30ms));
	scheduler.AddTask(inc, TCallbackScheduler::TDuration(200ms));
	// not enough time to finish all
}

TEST(callback_scheduler, future) {
	TCallbackScheduler scheduler(std::make_shared<TSimpleThreadExecutor>());

	std::function<int()> funcNum = []() { return 2*2; };
	std::function<const char*()> funcString = []() { return "2*2"; };
	std::function<int()> funcException = []() { throw std::runtime_error("exception"); return 42; };

	std::future<int> fnum = scheduler.AddTaskWithFuture(funcNum, TCallbackScheduler::TDuration(0ms));
	std::future<const char*> fstring = scheduler.AddTaskWithFuture(funcString, TCallbackScheduler::TDuration(5ms));
	std::future<int> fexception = scheduler.AddTaskWithFuture(funcException, TCallbackScheduler::TDuration(10ms));

	EXPECT_EQ(fnum.get(), 4);
	EXPECT_STREQ(fstring.get(), "2*2");
	EXPECT_THROW(fexception.get(), std::runtime_error);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
