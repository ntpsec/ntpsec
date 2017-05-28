#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(netof);

TEST_SETUP(netof) {}

TEST_TEAR_DOWN(netof) {}

#include "sockaddrtest.h"


TEST(netof, ClassBAddress) {
	sockaddr_u input = CreateSockaddr4("172.16.2.1", NTP_PORT);
	sockaddr_u expected = CreateSockaddr4("172.16.0.0", NTP_PORT);

	sockaddr_u* actual = netof(&input);

	TEST_ASSERT_NOT_NULL(actual);
	TEST_ASSERT_TRUE(IsEqualS(&expected, actual));
}

TEST(netof, ClassCAddress) {
	sockaddr_u input = CreateSockaddr4("192.0.2.255", NTP_PORT);
	sockaddr_u expected = CreateSockaddr4("192.0.2.0", NTP_PORT);

	sockaddr_u* actual = netof(&input);

	TEST_ASSERT_NOT_NULL(actual);
	TEST_ASSERT_TRUE(IsEqualS(&expected, actual));
}

TEST(netof, ClassAAddress) {
	/* Class A addresses are assumed to be classless,
	 * thus the same address should be returned.
	 */
	sockaddr_u input = CreateSockaddr4("10.20.30.40", NTP_PORT);
	sockaddr_u expected = CreateSockaddr4("10.20.30.40", NTP_PORT);

	sockaddr_u* actual = netof(&input);

	TEST_ASSERT_NOT_NULL(actual);
	TEST_ASSERT_TRUE(IsEqualS(&expected, actual));
}

TEST(netof, IPv6Address) {
	/* IPv6 addresses are assumed to have 64-bit host- and 64-bit network parts. */
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

	sockaddr_u* actual = netof(&input);

	TEST_ASSERT_NOT_NULL(actual);
	TEST_ASSERT_TRUE(IsEqualS(&expected, actual));
}

TEST_GROUP_RUNNER(netof) {
	RUN_TEST_CASE(netof, ClassBAddress);
	RUN_TEST_CASE(netof, ClassCAddress);
	RUN_TEST_CASE(netof, ClassAAddress);
	RUN_TEST_CASE(netof, IPv6Address);
}
