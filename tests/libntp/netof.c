#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(netof6);

TEST_SETUP(netof6) {}

TEST_TEAR_DOWN(netof6) {}

#include "sockaddrtest.h"


TEST(netof6, IPv6Address) {
	/* IPv6 addresses are assumed to have 64-bit host
         * and 64-bit network parts.
         */
	const struct in6_addr input_address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	}}}; // 2001:0db8:85a3:08d3:1319:8a2e:0370:7334

	const struct in6_addr expected_address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
	}}}; // 2001:0db8:85a3:08d3:0000:0000:0000:0000

	sockaddr_u input;
	SET_AF(&input, AF_INET6);
	SET_SOCK_ADDR6(&input, input_address);
	SET_PORT(&input, 3000);

	sockaddr_u expected;
	SET_AF(&expected, AF_INET6);
	SET_SOCK_ADDR6(&expected, expected_address);
	SET_PORT(&expected, 3000);

	sockaddr_u* actual = netof6(&input);

	TEST_ASSERT_NOT_NULL(actual);
	TEST_ASSERT_TRUE(IsEqualS(&expected, actual));
}

TEST_GROUP_RUNNER(netof6) {
	RUN_TEST_CASE(netof6, IPv6Address);
}
