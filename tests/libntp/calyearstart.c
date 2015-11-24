#include "config.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ntp_stdlib.h"
#include "ntp_calendar.h"
#include "caltime.h"

static time_t nowtime;

TEST_GROUP(calyearstart);

TEST_SETUP(calyearstart) {
    ntpcal_set_timefunc(timefunc);
    settime(1970, 1, 1, 0, 0, 0);
}

TEST_TEAR_DOWN(calyearstart) {
    ntpcal_set_timefunc(NULL);
}


TEST(calyearstart, NoWrapInDateRange) {
	const u_int32_t input = 3486372600UL; // 2010-06-24 12:50:00.
	const u_int32_t expected = 3471292800UL; // 2010-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}

TEST(calyearstart, NoWrapInDateRangeLeapYear) {
	const u_int32_t input = 3549528000UL; // 2012-06-24 12:00:00
	const u_int32_t expected = 3534364800UL; // 2012-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}

TEST(calyearstart, WrapInDateRange) {
	const u_int32_t input = 19904UL; // 2036-02-07 12:00:00
	const u_int32_t expected = 4291747200UL; // 2036-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}

TEST_GROUP_RUNNER(calyearstart) {
	RUN_TEST_CASE(calyearstart, NoWrapInDateRange);
	RUN_TEST_CASE(calyearstart, NoWrapInDateRangeLeapYear);
	RUN_TEST_CASE(calyearstart, WrapInDateRange);
}
