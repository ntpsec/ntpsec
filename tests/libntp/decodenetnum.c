#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(decodenetnum);

TEST_SETUP(decodenetnum) {}

TEST_TEAR_DOWN(decodenetnum) {}

#include "sockaddrtest.h"


TEST(decodenetnum, IPv4AddressOnly) {
	const char *str = "192.0.2.1";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.1");
	SET_PORT(&expected, NTP_PORT);

	TEST_ASSERT_TRUE(decodenetnum(str, &actual));
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IPv4AddressWithPort) {
	const char *str = "192.0.2.2:2000";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.2");
	SET_PORT(&expected, 2000);

	TEST_ASSERT_TRUE(decodenetnum(str, &actual));
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IPv6AddressOnly) {
	const struct in6_addr address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	}}};

	const char *str = "2001:0db8:85a3:08d3:1319:8a2e:0370:7334";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET6);
	SET_SOCK_ADDR6(&expected, address);
	SET_PORT(&expected, NTP_PORT);

	TEST_ASSERT_TRUE(decodenetnum(str, &actual));
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IPv6AddressWithPort) {
	const struct in6_addr address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	}}};

	const char *str = "[2001:0db8:85a3:08d3:1319:8a2e:0370:7334]:3000";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET6);
	SET_SOCK_ADDR6(&expected, address);
	SET_PORT(&expected, 3000);

	TEST_ASSERT_TRUE(decodenetnum(str, &actual));
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IllegalAddress) {
	const char *str = "192.0.2.270:2000";
	sockaddr_u actual;

	TEST_ASSERT_FALSE(decodenetnum(str, &actual));
}

TEST(decodenetnum, IllegalCharInPort) {
	/* An illegal port does not make the decodenetnum fail, but instead
	 * makes it use the standard port.
	 */
	const char *str = "192.0.2.1:a700";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.1");
	SET_PORT(&expected, NTP_PORT);

	TEST_ASSERT_FALSE(decodenetnum(str, &actual));
	TEST_ASSERT_TRUE(IsDiffS(&expected, &actual));
}

TEST_GROUP_RUNNER(decodenetnum) {
	RUN_TEST_CASE(decodenetnum, IPv4AddressOnly);
	RUN_TEST_CASE(decodenetnum, IPv4AddressWithPort);
	RUN_TEST_CASE(decodenetnum, IPv6AddressOnly);
	RUN_TEST_CASE(decodenetnum, IPv6AddressWithPort);
	RUN_TEST_CASE(decodenetnum, IllegalAddress);
	RUN_TEST_CASE(decodenetnum, IllegalCharInPort);
}
