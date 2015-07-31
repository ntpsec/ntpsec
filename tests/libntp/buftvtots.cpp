extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(buftvtots);

TEST_SETUP(buftvtots) {}

TEST_TEAR_DOWN(buftvtots) {}

#include "lfptest.h"

extern "C" {
#include "ntp_unixtime.h"
};

// Required for Solaris.
#include <math.h>

class buftvtotsTest : public lfptest {
};

#ifndef SYS_WINNT
TEST(buftvtots, ZeroBuffer) {
	const timeval input = {0, 0};
	const l_fp expected = {0 + JAN_1970, 0};

	l_fp actual;

	TEST_ASSERT_TRUE(buftvtots((const char*)(&input), &actual));
	TEST_ASSERT_TRUE(IsEqual(expected, actual));
}

TEST(buftvtots, IntegerAndFractionalBuffer) {
	const timeval input = {5, 500000}; // 5.5
	const l_fp expected = {5 + JAN_1970, HALF};

	l_fp actual;

	TEST_ASSERT_TRUE(buftvtots((const char*)(&input), &actual));

	// Compare the fractional part with an absolute error given.
	TEST_ASSERT_EQUAL(expected.l_ui, actual.l_ui);

	double expectedDouble, actualDouble;
	M_LFPTOD(0, expected.l_uf, expectedDouble);
	M_LFPTOD(0, actual.l_uf, actualDouble);

	// The error should be less than 0.5 us
	TEST_ASSERT_FLOAT_WITHIN(0.0000005, expectedDouble, actualDouble);
}

TEST(buftvtots, IllegalMicroseconds) {
	const timeval input = {0, 1100000}; // > 999 999 microseconds.

	l_fp actual;

	TEST_ASSERT_FALSE(buftvtots((const char*)(&input), &actual));
}

#else
TEST(buftvtots, AlwaysFalseOnWindows) {
	/*
	 * Under Windows, buftvtots will just return
	 * 0 (false).
	 */
	l_fp actual;
	TEST_ASSERT_FALSE(buftvtots("", &actual));
}

#endif
