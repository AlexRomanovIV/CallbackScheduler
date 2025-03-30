#include <atomic>
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
	std::atomic<int> number{0};

	auto inc = [&number]() {
		++number;
	};
	auto twice = [&number]() {
		number+=number;
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

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
