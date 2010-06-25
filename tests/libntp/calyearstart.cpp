#include "libntptest.h"

/*
 * Currently calyearstart() uses the caljulian() functions, which
 * in turn uses the current system time to wrap a timestamp around
 * the current year.
 *
 * Ideally, some sort of mock around caljulian() should be provided
 * so that tests doesn't depend on the current system time.
 */

class calyearstartTest : public libntptest {
};

TEST_F(calyearstartTest, NoWrapInDateRange) {
	const u_long input = 3486372600UL; // 2010-06-24 12:50:00.
	const u_long expected = 3471292800UL; // 2010-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input));
}

TEST_F(calyearstartTest, NoWrapInDateRangeLeapYear) {
	const u_long input = 3549528000UL; // 2012-06-24 12:00:00
	const u_long expected = 3534364800UL; // 2012-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input));
}

TEST_F(calyearstartTest, WrapInDateRange) {
	const u_long input = 19904UL; // 2036-02-07 12:00:00
	const u_long expected = 4291747200UL; // 2036-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input));
}
