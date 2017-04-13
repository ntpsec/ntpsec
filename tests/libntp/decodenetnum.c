#include <unistd.h>          /* for close() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(decodenetnum);

TEST_SETUP(decodenetnum) {}

TEST_TEAR_DOWN(decodenetnum) {}

#include "sockaddrtest.h"


TEST(decodenetnum, Services) {
	const char *str = "/etc/services";
        int ret;

        /* try to open /etc/services for read */
        ret = open(str, O_RDONLY);
	TEST_ASSERT_NOT_EQUAL(-1, ret);
        if ( -1 != ret) {
		close(ret);
        }
}

TEST(decodenetnum, IPv4AddressOnly) {
	const char *str = "192.0.2.1";
        int ret;
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.1");
	SET_PORT(&expected, NTP_PORT);

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

/* test with numeric port */
TEST(decodenetnum, IPv4AddressWithPort) {
	const char *str = "192.0.2.2:2000";
	sockaddr_u actual;
        int ret;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.2");
	SET_PORT(&expected, 2000);

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

/* test for named port */
TEST(decodenetnum, IPv4AddressWithPort2) {
	const char *str = "192.168.2.2:ntp";
	sockaddr_u actual;
        int ret;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.168.2.2");
	SET_PORT(&expected, 123);

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IPv6AddressOnly) {
        int ret;
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

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IPv6AddressWithPort) {
	const struct in6_addr address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	}}};
        int ret;

	const char *str = "[2001:0db8:85a3:08d3:1319:8a2e:0370:7334]:3000";
	sockaddr_u actual;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET6);
	SET_SOCK_ADDR6(&expected, address);
	SET_PORT(&expected, 3000);

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(IsEqualS(&expected, &actual));
}

TEST(decodenetnum, IllegalAddress) {
	const char *str = "192.0.2.270:2000";
	sockaddr_u actual;
        int ret;

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_NOT_EQUAL(0, ret);
}

TEST(decodenetnum, IllegalCharInPort) {
	/* An illegal port does not make the decodenetnum fail, but instead
	 * makes it use the standard port.
	 */
	const char *str = "192.0.2.1:a700";
	sockaddr_u actual;
        int ret;

	sockaddr_u expected;
	SET_AF(&expected, AF_INET);
	PSOCK_ADDR4(&expected)->s_addr = inet_addr("192.0.2.1");
	SET_PORT(&expected, NTP_PORT);

	ret = decodenetnum(str, &actual);
	TEST_ASSERT_NOT_EQUAL(0, ret);
	TEST_ASSERT_TRUE(IsDiffS(&expected, &actual));
}

TEST_GROUP_RUNNER(decodenetnum) {
	RUN_TEST_CASE(decodenetnum, Services);
	RUN_TEST_CASE(decodenetnum, IPv4AddressOnly);
	RUN_TEST_CASE(decodenetnum, IPv4AddressWithPort);
	RUN_TEST_CASE(decodenetnum, IPv4AddressWithPort2);
	RUN_TEST_CASE(decodenetnum, IPv6AddressOnly);
	RUN_TEST_CASE(decodenetnum, IPv6AddressWithPort);
	RUN_TEST_CASE(decodenetnum, IllegalAddress);
	RUN_TEST_CASE(decodenetnum, IllegalCharInPort);
}
