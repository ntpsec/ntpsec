extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(octtoint);

TEST_SETUP(octtoint) {}

TEST_TEAR_DOWN(octtoint) {}

#include "libntptest.h"

class octtointTest : public libntptest {
};

TEST(octtoint, SingleDigit) {
	const char* str = "5";
	u_long actual;

	TEST_ASSERT_TRUE(octtoint(str, &actual));
	TEST_ASSERT_EQUAL(5, actual);
}

TEST(octtoint, MultipleDigits) {
	const char* str = "271";
	u_long actual;

	TEST_ASSERT_TRUE(octtoint(str, &actual));
	TEST_ASSERT_EQUAL(185, actual);
}

TEST(octtoint, Zero) {
	const char* str = "0";
	u_long actual;

	TEST_ASSERT_TRUE(octtoint(str, &actual));
	TEST_ASSERT_EQUAL(0, actual);
}

TEST(octtoint, MaximumUnsigned32bit) {
	const char* str = "37777777777";
	u_long actual;

	TEST_ASSERT_TRUE(octtoint(str, &actual));
	TEST_ASSERT_EQUAL(4294967295UL, actual);
}

TEST(octtoint, Overflow) {
	const char* str = "40000000000";
	u_long actual;

	TEST_ASSERT_FALSE(octtoint(str, &actual));
}

TEST(octtoint, IllegalCharacter) {
	const char* str = "5ac2";
	u_long actual;

	TEST_ASSERT_FALSE(octtoint(str, &actual));
}

TEST(octtoint, IllegalDigit) {
	const char* str = "5283";
	u_long actual;

	TEST_ASSERT_FALSE(octtoint(str, &actual));
}
