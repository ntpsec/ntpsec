#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(prettydate);

TEST_SETUP(prettydate) {}

TEST_TEAR_DOWN(prettydate) {}


#include "ntp_fp.h"

static const u_int32_t HALF = 2147483648UL;

static l_fp lfpinit(int32_t hi, uint32_t lo)
{
    l_fp tmp;
    setlfpsint(tmp, hi);
    setlfpfrac(tmp, lo);
    return tmp;
}

TEST(prettydate, ConstantDate) {
    l_fp time = lfpinit(3485080800UL, HALF); // 2010-06-09 14:00:00.5

	TEST_ASSERT_EQUAL_STRING("cfba1ce0.80000000 2010-06-09T14:00:00.500Z", gmprettydate(&time));
}

TEST_GROUP_RUNNER(prettydate) {
	RUN_TEST_CASE(prettydate, ConstantDate);
}
