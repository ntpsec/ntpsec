#include "config.h"
#include "ntp.h"
#include "ntp_stdlib.h"
#include "parse.h"

#include "unity.h"
#include "unity_fixture.h"

#include "caltime.h"

TEST_GROUP(calendar);

TEST_SETUP(calendar) {}

TEST_TEAR_DOWN(calendar) {}


#include "ntp_calendar.h"

static const char *DateToString(char *, const struct calendar *);

static const char *DateToString(char *str, const struct calendar *cal) {
	snprintf(str, 255, "%hu-%u-%u(%u)\n", cal->year, (unsigned int)cal->month, (unsigned int)cal->monthday, cal->yearday);
	return str;
}


static bool IsEqualDate(const struct calendar *expected,
                        const struct calendar *actual) {
	char str[255];
	char str1[255];
	if (expected->year == actual->year &&
	    (!expected->yearday || expected->yearday == actual->yearday) &&
	    expected->month == actual->month &&
	    expected->monthday == actual->monthday) {
			return true;
	} else {
		printf("Expected: %s but was %s\n",
                       DateToString(str, expected),
                       DateToString(str1, actual));
		return false;
	}
}


// ---------------------------------------------------------------------
// test cases
// ---------------------------------------------------------------------
static const unsigned short real_month_table[2][13] = {
	/* -*- table for regular years -*- */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* -*- table for leap years -*- */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

// days in month, with one month wrap-around at both ends
static const unsigned short real_month_days[2][14] = {
	/* -*- table for regular years -*- */
	{ 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31 },
	/* -*- table for leap years -*- */
	{ 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31 }
};

TEST(calendar, is_leapyear) {
        /* check is_leapyear() */
	TEST_ASSERT_EQUAL(false, is_leapyear(1900));
	TEST_ASSERT_EQUAL(false, is_leapyear(1970));
	TEST_ASSERT_EQUAL(false, is_leapyear(1999));
	TEST_ASSERT_EQUAL(true, is_leapyear(2000));
	TEST_ASSERT_EQUAL(false, is_leapyear(2001));
	TEST_ASSERT_EQUAL(true, is_leapyear(2004));
	TEST_ASSERT_EQUAL(true, is_leapyear(2040));
}

TEST(calendar, julian0) {
        /* check julian0() */
	TEST_ASSERT_EQUAL(693961, julian0(1900));
	TEST_ASSERT_EQUAL(719528, julian0(1970));
	TEST_ASSERT_EQUAL(730120, julian0(1999));
	TEST_ASSERT_EQUAL(730485, julian0(2000));
	TEST_ASSERT_EQUAL(730851, julian0(2001));
	TEST_ASSERT_EQUAL(745095, julian0(2040));
}

TEST(calendar, days_per_year) {
        /* check is_leapyear() */
	TEST_ASSERT_EQUAL(365, days_per_year(1900));
	TEST_ASSERT_EQUAL(365, days_per_year(1970));
	TEST_ASSERT_EQUAL(365, days_per_year(1999));
	TEST_ASSERT_EQUAL(366, days_per_year(2000));
	TEST_ASSERT_EQUAL(365, days_per_year(2001));
	TEST_ASSERT_EQUAL(366, days_per_year(2004));
	TEST_ASSERT_EQUAL(366, days_per_year(2040));
}

#ifdef CLOCK_GENERIC
TEST(calendar, parse_to_unixtime) {
        /* check is_leapyear() */
        clocktime_t  	ct;
        time_t       	result;
        unsigned long       Flag;

        ct.day = 1;
        ct.month = 1;
        ct.year = 1970;
        ct.hour = ct.minute = ct.second = ct.usecond = 0;
        ct.utcoffset = 0;
        ct.utctime = 0;
        ct.flags = 0;

        Flag = 0;
        result = parse_to_unixtime( &ct, &Flag );

	TEST_ASSERT_EQUAL(0, result);

        ct.year = 2000;
        ct.hour = 2;
        ct.utctime = 0;
        result = parse_to_unixtime( &ct, &Flag );
	TEST_ASSERT_EQUAL(946692000L, result);

        ct.year = 2037;
        ct.minute = 2;
        ct.second = 3;
        ct.utctime = 0;
        result = parse_to_unixtime( &ct, &Flag );
	TEST_ASSERT_EQUAL(2114388123L, result);
}
#endif

TEST(calendar, PeriodicExtend1) {
    // Test positive cycle, pivot > value
    TEST_ASSERT_EQUAL(1001, ntpcal_periodic_extend(1000, 5, 2));
    // Test positive cycle, pivot < value
    TEST_ASSERT_EQUAL(6, ntpcal_periodic_extend(5, 1000, 2));
    // Test negative cycle, pivot > value
    TEST_ASSERT_EQUAL(999, ntpcal_periodic_extend(1000, 5, -2));
    // Test negative cycle, pivot < value
    TEST_ASSERT_EQUAL(4, ntpcal_periodic_extend(5, 1000, -2));
}

// test the NTP to 64-bit Unix scale time conversion
TEST(calendar, NtpToTime1) {
    TEST_ASSERT_EQUAL(2085978538, ntpcal_ntp_to_time(42, 23));
}

// test the NTP to 64-bit NTP scale time conversion
TEST(calendar, NtpToNtp1) {
    TEST_ASSERT_EQUAL(4294967338, ntpcal_ntp_to_ntp(42, 23));
}

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

TEST(calendar, SplitEraDays1) {
    ntpcal_split res;
    int32_t isleapyear = 42;

    // Test no flag, no-leap, positive
    res = ntpcal_split_eradays(4, NULL);
    TEST_ASSERT_EQUAL(0, res.hi);
    TEST_ASSERT_EQUAL(4, res.lo);
    TEST_ASSERT_EQUAL(42, isleapyear);

    // Test flag, no-leap, positive
    res = ntpcal_split_eradays(4, &isleapyear);
    TEST_ASSERT_EQUAL(0, res.hi);
    TEST_ASSERT_EQUAL(4, res.lo);
    TEST_ASSERT_EQUAL(0, isleapyear);

    // Test flag, leap, positive
    res = ntpcal_split_eradays(1400, &isleapyear);
    TEST_ASSERT_EQUAL(3, res.hi);
    TEST_ASSERT_EQUAL(305, res.lo);
    TEST_ASSERT_EQUAL(1, isleapyear);

    isleapyear = 0;

    // Test flag, leap, negative
    res = ntpcal_split_eradays(-100, &isleapyear);
    TEST_ASSERT_EQUAL(-1, res.hi);
    TEST_ASSERT_EQUAL(266, res.lo);
    TEST_ASSERT_EQUAL(1, isleapyear);
}

TEST(calendar, SplitYearDays1) {
	int32_t eyd;

	for (eyd = -1; eyd <= 365; eyd++) {
		ntpcal_split split = ntpcal_split_yeardays(eyd, 0);
		if (split.lo >= 0 && split.hi >= 0) {
			TEST_ASSERT_LESS_THAN_INT32(12, split.hi);
			TEST_ASSERT_LESS_THAN_INT32(real_month_days[0][split.hi+1], split.lo);
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
			TEST_ASSERT_LESS_THAN_INT32(12, split.hi);
			TEST_ASSERT_LESS_THAN_INT32(real_month_days[1][split.hi+1], split.lo);
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

TEST(calendar, DaysecToDate1) {
    struct calendar cal;
    int32_t days;

    // Test normal date
    days = ntpcal_daysec_to_date(&cal, 100000);
    TEST_ASSERT_EQUAL(days, 1);
    TEST_ASSERT_EQUAL(cal.hour, 3);
    TEST_ASSERT_EQUAL(cal.minute, 46);
    TEST_ASSERT_EQUAL(cal.second, 40);

    // Test negative date
    days = ntpcal_daysec_to_date(&cal, -100000);
    TEST_ASSERT_EQUAL(-2, days);
    TEST_ASSERT_EQUAL(20, cal.hour);
    TEST_ASSERT_EQUAL(13, cal.minute);
    TEST_ASSERT_EQUAL(20, cal.second);
}

TEST(calendar, TimeToDate1) {
    struct calendar jd = {0, 0, 0, 0, 0, 0, 0, 0};
    int res;

    res = ntpcal_time_to_date(&jd, 1000000);
    TEST_ASSERT_EQUAL(0, res);
    TEST_ASSERT_EQUAL(1970, jd.year);
    TEST_ASSERT_EQUAL(12, jd.yearday);
    TEST_ASSERT_EQUAL(1, jd.month);
    TEST_ASSERT_EQUAL(12, jd.monthday);
    TEST_ASSERT_EQUAL(13, jd.hour);
    TEST_ASSERT_EQUAL(46, jd.minute);
    TEST_ASSERT_EQUAL(40, jd.second);
    TEST_ASSERT_EQUAL(1, jd.weekday);
}

TEST(calendar, DayJoin1) {
    TEST_ASSERT_EQUAL(4323600, ntpcal_dayjoin(50, 3600));
}

TEST(calendar, DaysInYears1) {
    // Test positive less than one gregorian cycle of years
    TEST_ASSERT_EQUAL(109572, ntpcal_days_in_years(300));
    // Test positive one gregorian cycle of years
    TEST_ASSERT_EQUAL(146097, ntpcal_days_in_years(400));
    // Test positive greater than one gregorian cycle of years
    TEST_ASSERT_EQUAL(182621, ntpcal_days_in_years(500));
    // Test negative less than one gregorian cycle of years
    TEST_ASSERT_EQUAL(-109573, ntpcal_days_in_years(-300));
    // Test negative one gregorian cycle of years
    TEST_ASSERT_EQUAL(-146097, ntpcal_days_in_years(-400));
    // Test negative greater than one gregorian cycle of years
    TEST_ASSERT_EQUAL(-182622, ntpcal_days_in_years(-500));
}

TEST(calendar, EdateToEradays1) {
    // Test positive, no months
    TEST_ASSERT_EQUAL(1827, ntpcal_edate_to_eradays(5, 0, 1));
    // Test positive, with months
    TEST_ASSERT_EQUAL(1917, ntpcal_edate_to_eradays(5, 3, 1));
    // Test negative, no months
    TEST_ASSERT_EQUAL(-1828, ntpcal_edate_to_eradays(-5, 0, -1));
    // Test negative, with months
    TEST_ASSERT_EQUAL(-1920, ntpcal_edate_to_eradays(-5, -3, -1));
}

TEST(calendar, EtimeToSeconds1) {
    TEST_ASSERT_EQUAL(18181, ntpcal_etime_to_seconds(5, 3, 1));
}

TEST(calendar, TmToRd1) {
    struct tm utm;

    utm.tm_year = 10;
    utm.tm_mon = 5;
    utm.tm_mday = 1;
    TEST_ASSERT_EQUAL(697399, ntpcal_tm_to_rd(&utm));
}

// check last day of february for first 10000 years
TEST(calendar, LeapYears1) {
	struct calendar dateIn, dateOut;

	for (dateIn.year = 1; dateIn.year < 10000; ++dateIn.year) {
		dateIn.month	= 2;
		dateIn.monthday = is_leapyear(dateIn.year) ? 29 : 28;
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
		dateIn.yearday	= is_leapyear(dateIn.year) ? 61 : 60;

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
		leaps = is_leapyear(expDate.year) ? 1 : 0;
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

TEST(calendar, DateToDaysec1) {
    struct calendar jd;

    jd.hour = 18;
    jd.minute = 45;
    jd.second = 15;
    TEST_ASSERT_EQUAL(67515, ntpcal_date_to_daysec(&jd));
}

TEST(calendar, TmToDaysec1) {
    struct tm utm;

    utm.tm_hour = 18;
    utm.tm_min = 45;
    utm.tm_sec = 15;
    TEST_ASSERT_EQUAL(67515, ntpcal_tm_to_daysec(&utm));
}

TEST(calendar, DateToTime1) {
    struct calendar jd;

    jd.year = 2000;
    jd.month = 2;
    jd.monthday = 4;
    jd.hour = 8;
    jd.minute = 16;
    jd.second = 32;
    TEST_ASSERT_EQUAL(949652192, ntpcal_date_to_time(&jd));
}

TEST(calendar, Ntp64ToDate1) {
    struct calendar jd;

    TEST_ASSERT_EQUAL(0, ntpcal_ntp64_to_date(&jd, 10000000));
    TEST_ASSERT_EQUAL(1900, jd.year);
    TEST_ASSERT_EQUAL(4, jd.month);
    TEST_ASSERT_EQUAL(26, jd.monthday);
    TEST_ASSERT_EQUAL(17, jd.hour);
    TEST_ASSERT_EQUAL(46, jd.minute);
    TEST_ASSERT_EQUAL(40, jd.second);
}

TEST(calendar, NtpToDate1) {
    struct calendar jd;

    TEST_ASSERT_EQUAL(1, ntpcal_ntp_to_date(&jd, 86400, 1000000));
    TEST_ASSERT_EQUAL(2036, jd.year);
    TEST_ASSERT_EQUAL(2, jd.month);
    TEST_ASSERT_EQUAL(8, jd.monthday);
    TEST_ASSERT_EQUAL(6, jd.hour);
    TEST_ASSERT_EQUAL(28, jd.minute);
    TEST_ASSERT_EQUAL(16, jd.second);
}


TEST_GROUP_RUNNER(calendar) {
	RUN_TEST_CASE(calendar, is_leapyear);
	RUN_TEST_CASE(calendar, julian0);
	RUN_TEST_CASE(calendar, days_per_year);
#ifdef CLOCK_GENERIC
	RUN_TEST_CASE(calendar, parse_to_unixtime);
#endif
	RUN_TEST_CASE(calendar, PeriodicExtend1);
	RUN_TEST_CASE(calendar, NtpToTime1);
	RUN_TEST_CASE(calendar, NtpToNtp1);
	RUN_TEST_CASE(calendar, DaySplitMerge);
	RUN_TEST_CASE(calendar, DaysecToDate1);
	RUN_TEST_CASE(calendar, SplitEraDays1);
	RUN_TEST_CASE(calendar, SplitYearDays1);
	RUN_TEST_CASE(calendar, SplitYearDays2);
	RUN_TEST_CASE(calendar, RataDie1);
	RUN_TEST_CASE(calendar, TimeToDate1);
	RUN_TEST_CASE(calendar, DayJoin1);
	RUN_TEST_CASE(calendar, DaysInYears1);
	RUN_TEST_CASE(calendar, EdateToEradays1);
	RUN_TEST_CASE(calendar, EtimeToSeconds1);
	RUN_TEST_CASE(calendar, TmToRd1);
	RUN_TEST_CASE(calendar, LeapYears1);
	RUN_TEST_CASE(calendar, LeapYears2);
	RUN_TEST_CASE(calendar, RoundTripDate);
	RUN_TEST_CASE(calendar, DateToDaysec1);
	RUN_TEST_CASE(calendar, TmToDaysec1);
	RUN_TEST_CASE(calendar, DateToTime1);
	RUN_TEST_CASE(calendar, Ntp64ToDate1);
	RUN_TEST_CASE(calendar, NtpToDate1);
}
