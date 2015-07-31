extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(atoint);

TEST_SETUP(atoint) {}

TEST_TEAR_DOWN(atoint) {}

#include "libntptest.h"

class atointTest : public libntptest {
};

TEST(atoint, RegularPositive) {
	const char *str = "17";
	long val;

	TEST_ASSERT_TRUE(atoint(str, &val));
	TEST_ASSERT_EQUAL(17, val);
}

TEST(atoint, RegularNegative) {
	const char *str = "-20";
	long val;

	TEST_ASSERT_TRUE(atoint(str, &val));
	TEST_ASSERT_EQUAL(-20, val);
}

TEST(atoint, PositiveOverflowBoundary) {
	const char *str = "2147483648";
	long val;

	TEST_ASSERT_FALSE(atoint(str, &val));
}

TEST(atoint, NegativeOverflowBoundary) {
	const char *str = "-2147483649";
	long val;

	TEST_ASSERT_FALSE(atoint(str, &val));
}

TEST(atoint, PositiveOverflowBig) {
	const char *str = "2300000000";
	long val;

	TEST_ASSERT_FALSE(atoint(str, &val));
}

TEST(atoint, IllegalCharacter) {
	const char *str = "4500l";
	long val;

	TEST_ASSERT_FALSE(atoint(str, &val));
}
