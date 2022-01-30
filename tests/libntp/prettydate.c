#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "caltime.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(prettydate);

TEST_SETUP(prettydate) {}

TEST_TEAR_DOWN(prettydate) {}


static const uint32_t HALF = 2147483648UL;

TEST(prettydate, Rfc3339Date1) {
	TEST_ASSERT_EQUAL_STRING("2036-02-07T06:28:16.000Z", rfc3339date(0));
}

TEST(prettydate, Rfc3339Time1) {
	TEST_ASSERT_EQUAL_STRING("1970-01-01T00:00Z", rfc3339time(0));
}

TEST(prettydate, ConstantDate) {
    l_fp t = lfpinit((int32_t)3485080800LL, HALF); // 2010-06-09 14:00:00.5

	TEST_ASSERT_EQUAL_STRING("cfba1ce0.80000000 2010-06-09T14:00:00.500Z", prettydate(t));
}

TEST_GROUP_RUNNER(prettydate) {
	RUN_TEST_CASE(prettydate, ConstantDate);
	RUN_TEST_CASE(prettydate, Rfc3339Date1);
	RUN_TEST_CASE(prettydate, Rfc3339Time1);
}
