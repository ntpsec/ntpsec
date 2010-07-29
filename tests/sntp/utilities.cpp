#include "sntptest.h"

extern "C" {
#include "utilities.h"
};

class utilitiesTest : public sntptest {
protected:
	sockaddr_u CreateSockaddr4(const char* address) {
		sockaddr_u s;
		s.sa4.sin_family = AF_INET;
		s.sa4.sin_addr.s_addr = inet_addr(address);
		SET_PORT(&s, 123);

		return s;
	}

	addrinfo CreateAddrinfo(sockaddr_u* sock) {
		addrinfo a;
		a.ai_family = sock->sa.sa_family;
		if (a.ai_family == AF_INET) {
			a.ai_addrlen = sizeof(sockaddr_in);
		} else {
			a.ai_addrlen = sizeof(sockaddr_in6);
		}
		a.ai_addr = &sock->sa;
		return a;
	}
};

/* 
 * These tests are essentially a copy of the tests for socktoa()
 * in libntp. If sntp switches to using that functions, these
 * tests can be removed.
 */

TEST_F(utilitiesTest, IPv4Address) {
	const char* ADDR = "192.0.2.10";

	sockaddr_u input = CreateSockaddr4(ADDR);
	addrinfo inputA = CreateAddrinfo(&input);

	EXPECT_STREQ(ADDR, ss_to_str(&input));
	EXPECT_STREQ(ADDR, addrinfo_to_str(&inputA));
}

TEST_F(utilitiesTest, IPv6Address) {
	const struct in6_addr address = {
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3, 
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	};

	const char* expected =
		"2001:db8:85a3:8d3:1319:8a2e:370:7334";

	sockaddr_u input;
	input.sa6.sin6_family = AF_INET6;
	input.sa6.sin6_addr = address;

	addrinfo inputA = CreateAddrinfo(&input);

	EXPECT_STREQ(expected, ss_to_str(&input));
	EXPECT_STREQ(expected, addrinfo_to_str(&inputA));
}
