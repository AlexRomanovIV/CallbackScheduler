#include <atomic>
#include <type_traits>

#include <gtest/gtest.h>

#include "lib/simple_thread_executor.h"
#include "lib/callback_scheduler.h"

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

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
