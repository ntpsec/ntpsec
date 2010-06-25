#include "libntptest.h"

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
	 * Time sent into function is 04:00:00
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

TEST_F(clocktimeTest, ) {

}
