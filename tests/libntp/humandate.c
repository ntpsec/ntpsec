#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"


TEST_GROUP(humandate);

TEST_SETUP(humandate) {}

TEST_TEAR_DOWN(humandate) {}


TEST(humandate, RegularTime) {
	time_t sample = 1276601278;
	char expected[255];

	struct tm tmbuf;

	struct tm* time;
	time = localtime_r(&sample, &tmbuf);
	TEST_ASSERT_TRUE(time != NULL);

	snprintf(expected, 255, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);

	TEST_ASSERT_EQUAL_STRING(expected, humantime(sample));
}

TEST(humandate, CurrentTime) {
	time_t sample;
	char expected[255];

	struct tm tmbuf;

	time(&sample);

	struct tm* time;
	time = localtime_r(&sample, &tmbuf);
	TEST_ASSERT_TRUE(time != NULL);

	snprintf(expected, 255, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);

	TEST_ASSERT_EQUAL_STRING(expected, humantime(sample));
}

TEST_GROUP_RUNNER(humandate) {
	RUN_TEST_CASE(humandate, RegularTime);
	RUN_TEST_CASE(humandate, CurrentTime);
}
