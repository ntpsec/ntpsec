extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(hextoint);

TEST_SETUP(hextoint) {}

TEST_TEAR_DOWN(hextoint) {}

#include "libntptest.h"

class hextointTest : public libntptest {
};

TEST(hextoint, SingleDigit) {
	const char *str = "a"; // 10 decimal
	u_long actual;

	TEST_ASSERT_TRUE(hextoint(str, &actual));
	TEST_ASSERT_EQUAL(10, actual);
}

TEST(hextoint, MultipleDigits) {
	const char *str = "8F3"; // 2291 decimal
	u_long actual;

	TEST_ASSERT_TRUE(hextoint(str, &actual));
	TEST_ASSERT_EQUAL(2291, actual);
}

TEST(hextoint, MaxUnsigned) {
	const char *str = "ffffffff"; // 4294967295 decimal
	u_long actual;

	TEST_ASSERT_TRUE(hextoint(str, &actual));
	TEST_ASSERT_EQUAL(4294967295UL, actual);
}

TEST(hextoint, Overflow) {
	const char *str = "100000000"; // Overflow by 1
	u_long actual;

	TEST_ASSERT_FALSE(hextoint(str, &actual));
}

TEST(hextoint, IllegalChar) {
	const char *str = "5gb"; // Illegal character g
	u_long actual;

	TEST_ASSERT_FALSE(hextoint(str, &actual));
}
