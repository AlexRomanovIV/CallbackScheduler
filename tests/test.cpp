#include <gtest/gtest.h>

#include "lib/callback_scheduler.h"

TEST(try, test_1) {
	ASSERT_TRUE(true);
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
