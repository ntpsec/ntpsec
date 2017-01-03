#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"


TEST_GROUP(hextolfp);

TEST_SETUP(hextolfp) {}

TEST_TEAR_DOWN(hextolfp) {}

#include "lfptest.h"

TEST(hextolfp, PositiveInteger) {
	const char *str = "00001000.00000000";
	l_fp actual;

	l_fp expected = lfpinit(4096, 0); // 16^3, no fraction part.

	TEST_ASSERT_TRUE(hextolfp(str, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST(hextolfp, NegativeInteger) {
	const char *str = "ffffffff.00000000"; // -1 decimal
	l_fp actual;

	l_fp expected = lfpinit(-1, 0);

	TEST_ASSERT_TRUE(hextolfp(str, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST(hextolfp, PositiveFraction) {
	const char *str = "00002000.80000000"; // 8196.5 decimal
	l_fp actual;

	l_fp expected = lfpinit(8192, HALF);

	TEST_ASSERT_TRUE(hextolfp(str, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST(hextolfp, NegativeFraction) {
	const char *str = "ffffffff.40000000"; // -1 + 0.25 decimal
	l_fp actual;

	l_fp expected = lfpinit(-1, QUARTER); //-1 + 0.25

	TEST_ASSERT_TRUE(hextolfp(str, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST(hextolfp, IllegalNumberOfInteger) {
	const char *str = "1000000.00000000"; // Missing one digit in integral part.
	l_fp actual;

	TEST_ASSERT_FALSE(hextolfp(str, &actual));
}

TEST(hextolfp, IllegalChar) {
	const char *str = "10000000.0000h000"; // Illegal character h.
	l_fp actual;

	TEST_ASSERT_FALSE(hextolfp(str, &actual));
}

TEST_GROUP_RUNNER(hextolfp) {
	RUN_TEST_CASE(hextolfp, PositiveInteger);
	RUN_TEST_CASE(hextolfp, NegativeInteger);
	RUN_TEST_CASE(hextolfp, PositiveFraction);
	RUN_TEST_CASE(hextolfp, NegativeFraction);
	RUN_TEST_CASE(hextolfp, IllegalNumberOfInteger);
	RUN_TEST_CASE(hextolfp, IllegalChar);
}
