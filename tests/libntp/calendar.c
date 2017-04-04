#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

#include "caltime.h"

TEST_GROUP(calendar);

TEST_SETUP(calendar) {}

TEST_TEAR_DOWN(calendar) {}


#include "ntp_calendar.h"

#define TEST_ASSERT_GREATER_THAN(a, b) TEST_ASSERT_TRUE(a > b)

int leapdays(int year) {
	if (year % 400 == 0)
		return 1;
	if (year % 100 == 0)
		return 0;
	if (year % 4 == 0)
		return 1;
	return 0;
}


bool IsEqualCA(const struct calendar *expected, const struct calendar *actual) {
	if (expected->year == actual->year &&
	    (!expected->yearday || expected->yearday == actual->yearday) &&
	    expected->month == actual->month &&
	    expected->monthday == actual->monthday &&
	    expected->hour == actual->hour &&
	    expected->minute == actual->minute &&
	    expected->second == actual->second) {
		return true;
	} else {
		/* coverity[leaked_storage] */
		printf("Expected: %s but was %s\n", CalendarToString(expected), CalendarToString(actual));
		return false;
	}
}


const char *DateToString(const struct calendar *cal) {
	char *str = malloc(255);
	snprintf(str, 255, "%hu-%u-%u(%u)\n", cal->year, (u_int)cal->month, (u_int)cal->monthday, cal->yearday);
	return str;
}


bool IsEqualDate(const struct calendar *expected, const struct calendar *actual) {
	if (expected->year == actual->year &&
	    (!expected->yearday || expected->yearday == actual->yearday) &&
	    expected->month == actual->month &&
	    expected->monthday == actual->monthday) {
			return true;
	} else {
		/* coverity[leaked_storage] */
		printf("Expected: %s but was %s\n", DateToString(expected), DateToString(actual));
		return false;
	}
}


// ---------------------------------------------------------------------
// test cases
// ---------------------------------------------------------------------
static const u_short real_month_table[2][13] = {
	/* -*- table for regular years -*- */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* -*- table for leap years -*- */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

// days in month, with one month wrap-around at both ends
static const u_short real_month_days[2][14] = {
	/* -*- table for regular years -*- */
	{ 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31 },
	/* -*- table for leap years -*- */
	{ 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31 }
};

// test the day/sec join & split ops, making sure that 32bit
// intermediate results would definitely overflow and the hi DWORD of
// the 'time64_t' is definitely needed.
TEST(calendar, DaySplitMerge) {
	int32_t day;
	int32_t sec;

	for (day = -1000000; day <= 1000000; day += 100) {
		for (sec = -100000; sec <= 186400; sec += 10000) {
			time64_t	     merge = ntpcal_dayjoin(day, sec);
			ntpcal_split split = ntpcal_daysplit(merge);
			int32_t	     eday  = day;
			int32_t	     esec  = sec;

			while (esec >= 86400) {
				eday += 1;
				esec -= 86400;
			}
			while (esec < 0) {
				eday -= 1;
				esec += 86400;
			}

			TEST_ASSERT_EQUAL(eday, split.hi);
			TEST_ASSERT_EQUAL(esec, split.lo);
		}
	}
}

TEST(calendar, SplitYearDays1) {
	int32_t eyd;

	for (eyd = -1; eyd <= 365; eyd++) {
		ntpcal_split split = ntpcal_split_yeardays(eyd, 0);
		if (split.lo >= 0 && split.hi >= 0) {
			TEST_ASSERT_GREATER_THAN(12, split.hi);
			TEST_ASSERT_GREATER_THAN(real_month_days[0][split.hi+1], split.lo);
			int32_t tyd = real_month_table[0][split.hi] + split.lo;
			TEST_ASSERT_EQUAL(eyd, tyd);
		} else
			TEST_ASSERT_TRUE(eyd < 0 || eyd > 364);
	}
}

TEST(calendar, SplitYearDays2) {
	int32_t eyd;

	for (eyd = -1; eyd <= 366; eyd++) {
		ntpcal_split split = ntpcal_split_yeardays(eyd, 1);
		if (split.lo >= 0 && split.hi >= 0) {
			TEST_ASSERT_GREATER_THAN(12, split.hi);
			TEST_ASSERT_GREATER_THAN(real_month_days[1][split.hi+1], split.lo);
			int32_t tyd = real_month_table[1][split.hi] + split.lo;
			TEST_ASSERT_EQUAL(eyd, tyd);
		} else
			TEST_ASSERT_TRUE(eyd < 0 || eyd > 365);
		}
}

TEST(calendar, RataDie1) {
	int32_t	 testDate = 1; // 0001-01-01 (proleptic date)
	struct calendar expected = { 1, 1, 1, 1, 0, 0, 0, 0};
	struct calendar actual;

	ntpcal_rd_to_date(&actual, testDate);
	TEST_ASSERT_TRUE(IsEqualDate(&expected, &actual));
}

// check last day of february for first 10000 years
TEST(calendar, LeapYears1) {
	struct calendar dateIn, dateOut;

	for (dateIn.year = 1; dateIn.year < 10000; ++dateIn.year) {
		dateIn.month	= 2;
		dateIn.monthday = 28 + leapdays(dateIn.year);
		dateIn.yearday	= 31 + dateIn.monthday;

		ntpcal_rd_to_date(&dateOut, ntpcal_date_to_rd(&dateIn));

		TEST_ASSERT_TRUE(IsEqualDate(&dateIn, &dateOut));
	}
}

// check first day of march for first 10000 years
TEST(calendar, LeapYears2) {
	struct calendar dateIn, dateOut;

	for (dateIn.year = 1; dateIn.year < 10000; ++dateIn.year) {
		dateIn.month	= 3;
		dateIn.monthday = 1;
		dateIn.yearday	= 60 + leapdays(dateIn.year);

		ntpcal_rd_to_date(&dateOut, ntpcal_date_to_rd(&dateIn));
		TEST_ASSERT_TRUE(IsEqualDate(&dateIn, &dateOut));
	}
}

// Full roundtrip for 1601-01-01 to 2400-12-31
// checks sequence of rata die numbers and validates date output
// (since the input is all nominal days of the calendar in that range
// and the result of the inverse calculation must match the input no
// invalid output can occur.)
TEST(calendar, RoundTripDate) {
	struct calendar truDate, expDate = { 1600, 0, 12, 31, 0, 0, 0, 0};
	int32_t	 truRdn, expRdn	= ntpcal_date_to_rd(&expDate);
	int	 leaps;

	while (expDate.year < 2400) {
		expDate.year++;
		expDate.month	= 0;
		expDate.yearday = 0;
		leaps = leapdays(expDate.year);
		while (expDate.month < 12) {
			expDate.month++;
			expDate.monthday = 0;
			while (expDate.monthday < real_month_days[leaps][expDate.month]) {
				expDate.monthday++;
				expDate.yearday++;
				expRdn++;

				truRdn = ntpcal_date_to_rd(&expDate);
				TEST_ASSERT_EQUAL(expRdn, truRdn);

				ntpcal_rd_to_date(&truDate, truRdn);
				TEST_ASSERT_TRUE(IsEqualDate(&expDate, &truDate));
			}
		}
	}
}


TEST_GROUP_RUNNER(calendar) {
	RUN_TEST_CASE(calendar, DaySplitMerge);
	RUN_TEST_CASE(calendar, SplitYearDays1);
	RUN_TEST_CASE(calendar, SplitYearDays2);
	RUN_TEST_CASE(calendar, RataDie1);
	RUN_TEST_CASE(calendar, LeapYears1);
	RUN_TEST_CASE(calendar, LeapYears2);
	RUN_TEST_CASE(calendar, RoundTripDate);
}
