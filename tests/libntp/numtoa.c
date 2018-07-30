#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(numtoa);

TEST_SETUP(numtoa) {}

TEST_TEAR_DOWN(numtoa) {}


TEST(numtoa, Address) {
	uint32_t input = htonl(3221225472UL+512UL+1UL); // 192.0.2.1

	TEST_ASSERT_EQUAL_STRING("192.0.2.1", numtoa(input));
}

TEST(numtoa, Netmask) {
	// 255.255.255.0
	uint32_t hostOrder = 255UL*256UL*256UL*256UL + 255UL*256UL*256UL + 255UL*256UL;
	uint32_t input = htonl(hostOrder);

	TEST_ASSERT_EQUAL_STRING("255.255.255.0", numtoa(input));
}

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
	RUN_TEST_CASE(numtoa, Address);
	RUN_TEST_CASE(numtoa, Netmask);
	RUN_TEST_CASE(numtoa, RefidStr);
}
