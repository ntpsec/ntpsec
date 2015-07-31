extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(atouint);

TEST_SETUP(atouint) {}

TEST_TEAR_DOWN(atouint) {}

#include "libntptest.h"

class atouintTest : public libntptest {
};

TEST(atouint, RegularPositive) {
	const char *str = "305";
	u_long actual;

	TEST_ASSERT_TRUE(atouint(str, &actual));
	TEST_ASSERT_EQUAL(305, actual);
}

TEST(atouint, PositiveOverflowBoundary) {
	const char *str = "4294967296";
	u_long actual;

	TEST_ASSERT_FALSE(atouint(str, &actual));
}

TEST(atouint, PositiveOverflowBig) {
	const char *str = "8000000000";
	u_long actual;

	TEST_ASSERT_FALSE(atouint(str, &actual));
}

TEST(atouint, Negative) {
	const char *str = "-1";
	u_long actual;

	TEST_ASSERT_FALSE(atouint(str, &actual));
}

TEST(atouint, IllegalChar) {
	const char *str = "50c3";
	u_long actual;

	TEST_ASSERT_FALSE(atouint(str, &actual));
}
