#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ymd2yd);

TEST_SETUP(ymd2yd) {}

TEST_TEAR_DOWN(ymd2yd) {}



TEST(ymd2yd, NonLeapYearFebruary) {
	TEST_ASSERT_EQUAL(31+20, ymd2yd(2010,2,20)); //2010-02-20
}

TEST(ymd2yd, NonLeapYearJune) {
	int expected = 31+28+31+30+31+18; // 18 June non-leap year
	TEST_ASSERT_EQUAL(expected, ymd2yd(2011,6,18));
}

TEST(ymd2yd, LeapYearFebruary) {
	TEST_ASSERT_EQUAL(31+20, ymd2yd(2012,2,20)); //2012-02-20 (leap year)
}

TEST(ymd2yd, LeapYearDecember) {
	// 2012-12-31
	int expected = 31+29+31+30+31+30+31+31+30+31+30+31;
	TEST_ASSERT_EQUAL(expected, ymd2yd(2012,12,31));
}

TEST_GROUP_RUNNER(ymd2yd) {
	RUN_TEST_CASE(ymd2yd, NonLeapYearFebruary);
	RUN_TEST_CASE(ymd2yd, NonLeapYearJune);
	RUN_TEST_CASE(ymd2yd, LeapYearFebruary);
	RUN_TEST_CASE(ymd2yd, LeapYearDecember);
}
