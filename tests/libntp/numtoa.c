#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(numtoa);

TEST_SETUP(numtoa) {}

TEST_TEAR_DOWN(numtoa) {}


TEST(numtoa, RefidStr) {
	const char *res;

	// Test stratum > 1
	res = refid_str(0x11223344, 8);
	TEST_ASSERT_EQUAL_STRING("68.51.34.17", res);
	// Test !(stratum > 1)
	res = refid_str(0x535047, 0);
	TEST_ASSERT_EQUAL_STRING(".GPS.", res);
}

TEST_GROUP_RUNNER(numtoa) {
	RUN_TEST_CASE(numtoa, RefidStr);
}
