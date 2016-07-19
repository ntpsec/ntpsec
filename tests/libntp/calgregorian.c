#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ntp_calendar.h"

#include <string.h>
#include "caltime.h"

TEST_GROUP(calgregorian);

TEST_SETUP(calgregorian) {
	ntpcal_set_timefunc(timefunc);
	settime(1970, 1, 1, 0, 0, 0);
}

TEST_TEAR_DOWN(calgregorian) {
	ntpcal_set_timefunc(NULL);
}

bool IsEqualC(const struct calendar *expected, const struct calendar *actual) {
	if (expected->year == actual->year &&
		(expected->yearday == actual->yearday ||
		 (expected->month == actual->month &&
		  expected->monthday == actual->monthday)) &&
		expected->hour == actual->hour &&
		expected->minute == actual->minute &&
		expected->second == actual->second) {
		return true;
	} else {
		/* coverity[leaked_storage] */
		printf("expected: %s but was %s", CalendarToString(expected), CalendarToString(actual));

		return false;
	}
}


TEST(calgregorian, RegularTime) {
	u_long testDate = 3485080800UL; // 2010-06-09 14:00:00
	struct calendar expected = {2010, 160, 6, 9, 14, 0, 0, 0};

	struct calendar actual;

	calgregorian(testDate, &actual);

	TEST_ASSERT_TRUE(IsEqualC(&expected, &actual));
}

TEST(calgregorian, LeapYear) {
	u_long input = 3549902400UL; // 2012-06-28 20:00:00Z
	struct calendar expected = {2012, 179, 6, 28, 20, 0, 0, 0};

	struct calendar actual;

	calgregorian(input, &actual);

	TEST_ASSERT_TRUE(IsEqualC(&expected, &actual));
}

TEST(calgregorian, uLongBoundary) {
	u_long time = 4294967295UL; // 2036-02-07 6:28:15
	struct calendar expected = {2036, 0, 2, 7, 6, 28, 15, 0};

	struct calendar actual;

	calgregorian(time, &actual);

	TEST_ASSERT_TRUE(IsEqualC(&expected, &actual));
}

TEST(calgregorian, uLongWrapped) {
	u_long time = 0;
	struct calendar expected = {2036,0,2,7,6,28,16, 0};

	struct calendar actual;

	calgregorian(time, &actual);

	TEST_ASSERT_TRUE(IsEqualC(&expected, &actual));
}

TEST_GROUP_RUNNER(calgregorian) {
	RUN_TEST_CASE(calgregorian, RegularTime);
	RUN_TEST_CASE(calgregorian, LeapYear);
	RUN_TEST_CASE(calgregorian, uLongBoundary);
	RUN_TEST_CASE(calgregorian, uLongWrapped);
}
