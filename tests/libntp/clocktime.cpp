#include "libntptest.h"

/*
 * clocktime() uses calyearstart(), which in uses caljulian().
 * caljulian() should be ideally be mocked, because it uses
 * the current system time.
 */

class clocktimeTest : public libntptest {
};

TEST_F(clocktimeTest, CurrentYear) {
	// Timestamp: 2010-06-24 12:50:00Z
	const u_long timestamp = 3486372600UL;
	const u_int32 expected = timestamp; // exactly the same.

	const int yday=175, hour=12, minute=50, second=0, tzoff=0;

	u_long yearstart=0;
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, CurrentYearFuzz) {
	/* 
	 * Timestamp (rec_ui) is: 2010-06-24 12:50:00
	 * Time sent into function is 12:00:00.
	 *
	 * Since the fuzz is rather small, we should get a NTP
	 * timestamp for the 12:00:00 time.
	 */

	const u_long timestamp = 3486372600UL; // 2010-06-24 12:50:00Z
	const u_int32 expected = 3486369600UL; // 2010-06-24 12:00:00Z

	const int yday=175, hour=12, minute=0, second=0, tzoff=0;

	u_long yearstart=0;
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, TimeZoneOffset) {
	/*
	 * Timestamp (rec_ui) is: 2010-06-24 12:00:00 +0800
	 * (which is 2010-06-24 04:00:00Z)
	 *
	 * Time sent into function is 04:00:00 +0800
	 */
	const u_long timestamp = 3486369600UL;
	const u_int32 expected = timestamp;

	const int yday=175, hour=4, minute=0, second=0, tzoff=8;

	u_long yearstart=0;
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, WrongYearStart) {
	/* 
	 * Timestamp (rec_ui) is: 2010-01-02 11:00:00Z
	 * Time sent into function is 11:00:00.
	 * Yearstart sent into function is the yearstart of 2009!
	 */
	const u_long timestamp = 3471418800UL;
	const u_int32 expected = timestamp;

	const int yday=2, hour=11, minute=0, second=0, tzoff=0;

	u_long yearstart = 302024100UL; // Yearstart of 2009.
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, PreviousYear) {
	/*
	 * Timestamp is: 2010-01-01 01:00:00Z
	 * Time sent into function is 23:00:00
	 * (which is meant to be 2009-12-31 23:00:00Z)
	 */
	const u_long timestamp = 3471296400UL;
	const u_int32 expected = 3471289200UL;

	const int yday=365, hour=23, minute=0, second=0, tzoff=0;

	u_long yearstart = 0;
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, NextYear) {
	/*
	 * Timestamp is: 2009-12-31 23:00:00Z
	 * Time sent into function is 01:00:00
	 * (which is meant to be 2010-01-01 01:00:00Z)
	 */
	const u_long timestamp = 3471289200UL;
	const u_int32 expected = 3471296400UL;

	const int yday=1, hour=1, minute=0, second=0, tzoff=0;
	u_long yearstart = 0;
	u_int32 actual;

	ASSERT_TRUE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						  &yearstart, &actual));
	EXPECT_EQ(expected, actual);
}

TEST_F(clocktimeTest, NoReasonableConversion) {
	/* Timestamp is: 2010-01-02 11:00:00Z */
	const u_long timestamp = 3471418800UL;
	
	const int yday=100, hour=12, minute=0, second=0, tzoff=0;
	u_long yearstart = 0;
	u_int32 actual;

	ASSERT_FALSE(clocktime(yday, hour, minute, second, tzoff, timestamp,
						   &yearstart, &actual));
}
