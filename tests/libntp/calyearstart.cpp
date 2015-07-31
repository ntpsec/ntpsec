extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(calyearstart);

TEST_SETUP(calyearstart) {}

TEST_TEAR_DOWN(calyearstart) {}

#include "libntptest.h"

class calyearstartTest : public libntptest {
protected:
	virtual void SetUp();
	virtual void TearDown();
};

void calyearstartTest::SetUp()
{
    ntpcal_set_timefunc(timefunc);
    settime(1970, 1, 1, 0, 0, 0);
}

void calyearstartTest::TearDown()
{
    ntpcal_set_timefunc(NULL);
}


TEST(calyearstart, NoWrapInDateRange) {
	const u_int32 input = 3486372600UL; // 2010-06-24 12:50:00.
	const u_int32 expected = 3471292800UL; // 2010-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}

TEST(calyearstart, NoWrapInDateRangeLeapYear) {
	const u_int32 input = 3549528000UL; // 2012-06-24 12:00:00
	const u_int32 expected = 3534364800UL; // 2012-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}

TEST(calyearstart, WrapInDateRange) {
	const u_int32 input = 19904UL; // 2036-02-07 12:00:00
	const u_int32 expected = 4291747200UL; // 2036-01-01 00:00:00

	TEST_ASSERT_EQUAL(expected, calyearstart(input, &nowtime));
	TEST_ASSERT_EQUAL(expected, calyearstart(input, NULL));
}
