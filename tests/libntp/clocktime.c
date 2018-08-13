#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ntp_calendar.h"
#include "caltime.h"

TEST_GROUP(clocktime);

// ---------------------------------------------------------------------
// test fixture
//
// The clocktimeTest uses the NTP calendar feature to use a mockup
// function for getting the current system time, so the tests are not
// dependent on the actual system time.

static time_t fixedpivot;

TEST_SETUP(clocktime) {
	fixedpivot = settime(2000, 1, 1, 0, 0, 0);
}

TEST_TEAR_DOWN(clocktime) {
}

// ---------------------------------------------------------------------
// test cases

TEST(clocktime, CurrentYear) {
	// Timestamp: 2010-06-24 12:50:00Z, no year passed in
	const uint32_t timestamp = 3486372600UL;
	const uint32_t expected	= timestamp; // exactly the same.

	const int year=0, yday=175, hour=12, minute=50, second=0;

	uint32_t yearstart=0;
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	TEST_ASSERT_EQUAL(expected, actual);
	TEST_ASSERT_EQUAL(yearstart, 3471292800);
}

TEST(clocktime, CurrentYearExplicit) {
	// Timestamp: 2010-06-24 12:50:00Z, explicit year passed in
	const uint32_t timestamp = 3486372600UL;
	const uint32_t expected	= timestamp; // exactly the same.

	const int year=2010, yday=175, hour=12, minute=50, second=0;

	uint32_t yearstart=0;
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	/* If this assertion fails with "Expected 3486372600 was
	 * 104913720" that's a 32-bit integer overflow and your compiler
	 * is failing to cast to int properly inside clocktime.
	 * Observed on Mac OS X.
	 */
	TEST_ASSERT_EQUAL(expected, actual);
	TEST_ASSERT_EQUAL(yearstart, 3471292800);
}

TEST(clocktime, CurrentYearFuzz) {
	/*
	 * Timestamp (rec_ui) is: 2010-06-24 12:50:00
	 * Time sent into function is 12:00:00.
	 *
	 * Since the fuzz is rather small, we should get a NTP
	 * timestamp for the 12:00:00 time.
	 */

	const uint32_t timestamp = 3486372600UL; // 2010-06-24 12:50:00Z
	const uint32_t expected	= 3486369600UL; // 2010-06-24 12:00:00Z

	const int year=0, yday=175, hour=12, minute=0, second=0;

	uint32_t yearstart=0;
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	TEST_ASSERT_EQUAL(expected, actual);
}

TEST(clocktime, WrongYearStart) {
	/*
	 * Timestamp (rec_ui) is: 2010-01-02 11:00:00Z
	 * Time sent into function is 11:00:00.
	 * Yearstart sent into function is the yearstart of 2009!
	 */
	const uint32_t timestamp = 3471418800UL;
	const uint32_t expected	= timestamp;

	const int year=0, yday=2, hour=11, minute=0, second=0;

	uint32_t yearstart = 302024100UL; // Yearstart of 2009.
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	TEST_ASSERT_EQUAL(expected, actual);
}

TEST(clocktime, PreviousYear) {
	/*
	 * Timestamp is: 2010-01-01 01:00:00Z
	 * Time sent into function is 23:00:00
	 * (which is meant to be 2009-12-31 23:00:00Z)
	 */
	const uint32_t timestamp = 3471296400UL;
	const uint32_t expected	= 3471289200UL;

	const int year=0, yday=365, hour=23, minute=0, second=0;

	uint32_t yearstart = 0;
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	TEST_ASSERT_EQUAL(expected, actual);
}

TEST(clocktime, NextYear) {
	/*
	 * Timestamp is: 2009-12-31 23:00:00Z
	 * Time sent into function is 01:00:00
	 * (which is meant to be 2010-01-01 01:00:00Z)
	 */
	const uint32_t timestamp = 3471289200UL;
	const uint32_t expected	= 3471296400UL;

	const int year=0, yday=1, hour=1, minute=0, second=0;
	uint32_t yearstart = 0;
	uint32_t actual;

	TEST_ASSERT_TRUE(clocktime(year, yday, hour, minute, second,
				   fixedpivot, timestamp, &yearstart, &actual));
	TEST_ASSERT_EQUAL(expected, actual);
}

TEST(clocktime, NoReasonableConversion) {
	/* Timestamp is: 2010-01-02 11:00:00Z */
	const uint32_t timestamp = 3471418800UL;

	const int year=0, yday=100, hour=12, minute=0, second=0;
	uint32_t yearstart = 0;
	uint32_t actual;

	TEST_ASSERT_FALSE(clocktime(year, yday, hour, minute, second,
				    fixedpivot, timestamp, &yearstart, &actual));
}

TEST(clocktime, AlwaysInLimit) {
	/* Timestamp is: 2010-01-02 11:00:00Z */
	const uint32_t timestamp = 3471418800UL;
	const unsigned short prime_incs[] = { 127, 151, 163, 179 };
	int		cyc;
	int		yday;
	unsigned char	whichprime;
	unsigned short	ydayinc;
	int		hour;
	int		minute;
	uint32_t	yearstart;
	uint32_t	actual;
	uint32_t	diff;

	yearstart = 0;
	for (cyc = 0; cyc < 5; cyc++) {
		settime(1900 + cyc * 65, 1, 1, 0, 0, 0);
		for (yday = -26000; yday < 26000; yday += ydayinc) {
			whichprime = abs(yday) % (int)COUNTOF(prime_incs);
			ydayinc = prime_incs[whichprime];
			for (hour = -204; hour < 204; hour += 2) {
				for (minute = -60; minute < 60; minute++) {
				    (void)clocktime(0, yday, hour, minute, 30,
						    fixedpivot, timestamp, &yearstart, &actual);
					diff = actual - timestamp;
					if (diff >= 0x80000000UL) {
						diff = ~diff + 1;
					}
					TEST_ASSERT_TRUE(diff <= (183u * SECSPERDAY));
				}
			}
		}
	}
}

TEST_GROUP_RUNNER(clocktime) {
	RUN_TEST_CASE(clocktime, CurrentYear);
	RUN_TEST_CASE(clocktime, CurrentYearExplicit);
	RUN_TEST_CASE(clocktime, CurrentYearFuzz);
	RUN_TEST_CASE(clocktime, WrongYearStart);
	RUN_TEST_CASE(clocktime, PreviousYear);
	RUN_TEST_CASE(clocktime, NextYear);
	RUN_TEST_CASE(clocktime, NoReasonableConversion);
	RUN_TEST_CASE(clocktime, AlwaysInLimit);
}
