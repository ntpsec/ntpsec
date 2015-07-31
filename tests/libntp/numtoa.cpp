extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(numtoa);

TEST_SETUP(numtoa) {}

TEST_TEAR_DOWN(numtoa) {}

#include "libntptest.h"

class numtoaTest : public libntptest {
};

TEST(numtoa, Address) {
	u_int32 input = htonl(3221225472UL+512UL+1UL); // 192.0.2.1

	TEST_ASSERT_EQUAL_STRING("192.0.2.1", numtoa(input));
}

TEST(numtoa, Netmask) {
	// 255.255.255.0
	u_int32 hostOrder = 255UL*256UL*256UL*256UL + 255UL*256UL*256UL + 255UL*256UL;
	u_int32 input = htonl(hostOrder);

	TEST_ASSERT_EQUAL_STRING("255.255.255.0", numtoa(input));
}
