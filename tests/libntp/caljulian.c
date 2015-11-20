extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(caljulian);

TEST_SETUP(caljulian) {}

TEST_TEAR_DOWN(caljulian) {}

#include "libntptest.h"

extern "C" {
#include "ntp_calendar.h"
}

#include <string>
#include <sstream>

class caljulianTest : public libntptest {
protected:
	virtual void SetUp();
	virtual void TearDown();

	std::string CalendarToString(const calendar &cal) {
		std::ostringstream ss;
		ss << cal.year << "-" << (u_int)cal.month << "-" << (u_int)cal.monthday
		   << " (" << cal.yearday << ") " << (u_int)cal.hour << ":"
		   << (u_int)cal.minute << ":" << (u_int)cal.second;
		return ss.str();
	}

	bool IsEqual(const calendar &expected, const calendar &actual) {
		if (expected.year == actual.year &&
			(expected.yearday == actual.yearday ||
			 (expected.month == actual.month &&
			  expected.monthday == actual.monthday)) &&
			expected.hour == actual.hour &&
			expected.minute == actual.minute &&
			expected.second == actual.second) {
			return true;
		} else {
			return false
				<< "expected: " << CalendarToString(expected) << " but was "
				<< CalendarToString(actual);
		}
	}
};

void caljulianTest::SetUp()
{
    ntpcal_set_timefunc(timefunc);
    settime(1970, 1, 1, 0, 0, 0);
}

void caljulianTest::TearDown()
{
    ntpcal_set_timefunc(NULL);
}


TEST(caljulian, RegularTime) {
	u_long testDate = 3485080800UL; // 2010-06-09 14:00:00
	calendar expected = {2010,160,6,9,14,0,0};

	calendar actual;

	caljulian(testDate, &actual);

	TEST_ASSERT_TRUE(IsEqual(expected, actual));
}

TEST(caljulian, LeapYear) {
	u_long input = 3549902400UL; // 2012-06-28 20:00:00Z
	calendar expected = {2012, 179, 6, 28, 20, 0, 0};

	calendar actual;

	caljulian(input, &actual);

	TEST_ASSERT_TRUE(IsEqual(expected, actual));
}

TEST(caljulian, uLongBoundary) {
	u_long time = 4294967295UL; // 2036-02-07 6:28:15
	calendar expected = {2036,0,2,7,6,28,15};

	calendar actual;

	caljulian(time, &actual);

	TEST_ASSERT_TRUE(IsEqual(expected, actual));
}

TEST(caljulian, uLongWrapped) {
	u_long time = 0;
	calendar expected = {2036,0,2,7,6,28,16};

	calendar actual;

	caljulian(time, &actual);

	TEST_ASSERT_TRUE(IsEqual(expected, actual));
}
