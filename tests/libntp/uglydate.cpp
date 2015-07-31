extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(uglydate);

TEST_SETUP(uglydate) {}

TEST_TEAR_DOWN(uglydate) {}

#include "libntptest.h"

extern "C" {
#include "ntp_fp.h"
};

class uglydateTest : public libntptest {
protected:
	static const u_int32 HALF = 2147483648UL;
};

TEST(uglydate, ConstantDateTime) {
	l_fp time = {3485080800UL, HALF}; // 2010-06-09 14:00:00.5

	TEST_ASSERT_EQUAL_STRING("3485080800.500000 10:159:14:00:00.500",
				 uglydate(&time));
}

