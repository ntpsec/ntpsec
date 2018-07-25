#include "gpstolfp.h"
#include "ntp_types.h"

#include "unity.h"
#include "unity_fixture.h"

#include "config.h"
#include "ntp_stdlib.h"

TEST_GROUP(gpstolfp);

TEST_SETUP(gpstolfp){};
TEST_TEAR_DOWN(gpstolfp){};

TEST(gpstolfp, check) {
	uint64_t build_t, gps_t;
	struct calendar in, out;
	unsigned int build_week, week, TOW;

	unsigned int bw[] = {MIN_BUILD_GPSWEEK, 2048, MAX_BUILD_GPSWEEK};
	uint16_t by[] = {2016, 2019, 2096};
	uint8_t bm[] = {6, 4,  7};
	uint8_t bd[] = {5, 7, 1};

	for (int i = 0; i < 3; i++) {
		ZERO(in);
		in.year=by[i];
		in.month=bm[i];
		in.monthday = bd[i];
		caltogps(&in, 0, &week, &TOW);
		TEST_ASSERT_TRUE(week == bw[i] && TOW == 0);
	}

	for (uint32_t b = MIN_BUILD_GPSWEEK; b <= MAX_BUILD_GPSWEEK; b++) {
		build_week = b;
		week = b;
		gpstocal(week, 0, 0, &out);
		build_t = ntpcal_dayjoin(ntpcal_date_to_rd(&out) - DAY_NTP_STARTS,
		       ntpcal_date_to_daysec(&out));
		for (week = 0; week < GPSWEEKS; week++) {
			gpsweekadj(&week, build_week);
			ZERO(out);
			gpstocal(week, 0, 0, &out);
			gps_t = ntpcal_dayjoin(ntpcal_date_to_rd(&out) - DAY_NTP_STARTS,
			       ntpcal_date_to_daysec(&out));
			TEST_ASSERT_FALSE(build_t > gps_t);
		}
	}
}

TEST_GROUP_RUNNER(gpstolfp) {
	RUN_TEST_CASE(gpstolfp, check);
}
