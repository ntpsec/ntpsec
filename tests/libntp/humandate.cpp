extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(humandate);

TEST_SETUP(humandate) {}

TEST_TEAR_DOWN(humandate) {}

#include "libntptest.h"

#include <sstream>
#include <string>

class humandateTest : public libntptest {
};

TEST(humandate, RegularTime) {
	time_t sample = 1276601278;
	std::ostringstream expected;
	struct tm;

	tm* time;
	time = localtime_r(&sample, &tmbuf);
	TEST_ASSERT_TRUE(time != NULL);

	expected << std::setfill('0')
			 << std::setw(2) << time->tm_hour << ":"
			 << std::setw(2) << time->tm_min << ":"
			 << std::setw(2) << time->tm_sec;

	TEST_ASSERT_EQUAL_STRING(expected.str().c_str(), humantime(sample));
}

TEST(humandate, CurrentTime) {
	time_t sample;
	std::ostringstream expected;
	struct tm;

	time(&sample);

	tm* time;
	time = localtime_r(&sample, &tmbuf);
	TEST_ASSERT_TRUE(time != NULL);

	expected << std::setfill('0')
			 << std::setw(2) << time->tm_hour << ":"
			 << std::setw(2) << time->tm_min << ":"
			 << std::setw(2) << time->tm_sec;

	TEST_ASSERT_EQUAL_STRING(expected.str().c_str(), humantime(sample));
}
