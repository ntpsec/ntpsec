extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(caltontp);

TEST_SETUP(caltontp) {}

TEST_TEAR_DOWN(caltontp) {}

#include "libntptest.h"

extern "C" {
#include "ntp_calendar.h"
}

class caltontpTest : public libntptest {
};

TEST(caltontp, DateGivenMonthDay) {
	// 2010-06-24 12:50:00
	calendar input = {2010, 0, 6, 24, 12, 50, 0};

	u_long expected = 3486372600UL; // This is the timestamp above.

	TEST_ASSERT_EQUAL(expected, caltontp(&input));
}

TEST(caltontp, DateGivenYearDay) {
	// 2010-06-24 12:50:00
	// This is the 175th day of 2010.
	calendar input = {2010, 175, 0, 0, 12, 50, 0};

	u_long expected = 3486372600UL; // This is the timestamp above.

	TEST_ASSERT_EQUAL(expected, caltontp(&input));
}

TEST(caltontp, DateLeapYear) {
	// 2012-06-24 12:00:00
	// This is the 176th day of 2012 (since 2012 is a leap year).
	calendar inputYd = {2012, 176, 0, 0, 12, 00, 00};
	calendar inputMd = {2012, 0, 6, 24, 12, 00, 00};

	u_long expected = 3549528000UL;

	TEST_ASSERT_EQUAL(expected, caltontp(&inputYd));
	TEST_ASSERT_EQUAL(expected, caltontp(&inputMd));
}

TEST(caltontp, WraparoundDateIn2036) {
	// 2036-02-07 06:28:16
	// This is (one) wrapping boundary where we go from ULONG_MAX to 0.
	calendar input = {2036, 0, 2, 7, 6, 28, 16};

	u_long expected = 0UL;

	TEST_ASSERT_EQUAL(expected, caltontp(&input));
}
