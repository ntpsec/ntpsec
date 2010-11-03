#include "libntptest.h"

/*
 * calyearstart uses a pivot time, which defaults to the current system
 * time if not given. Since this is not a good idea in an regression
 * test, we use 2020-01-01 for the pivot.
 */

static const time_t hold = 1577836800; // 2020-01-01 00:00:00

class calyearstartTest : public libntptest {
};

TEST_F(calyearstartTest, NoWrapInDateRange) {
	const u_int32 input = 3486372600UL; // 2010-06-24 12:50:00.
	const u_int32 expected = 3471292800UL; // 2010-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input, &hold));
}

TEST_F(calyearstartTest, NoWrapInDateRangeLeapYear) {
	const u_int32 input = 3549528000UL; // 2012-06-24 12:00:00
	const u_int32 expected = 3534364800UL; // 2012-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input, &hold));
}

TEST_F(calyearstartTest, WrapInDateRange) {
	const u_int32 input = 19904UL; // 2036-02-07 12:00:00
	const u_int32 expected = 4291747200UL; // 2036-01-01 00:00:00

	EXPECT_EQ(expected, calyearstart(input, &hold));
}
