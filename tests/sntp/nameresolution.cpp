extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(nameresolution);

TEST_SETUP(nameresolution) {}

TEST_TEAR_DOWN(nameresolution) {}

#include "sntptest.h"

extern "C" {
#include "networking.h"
};

class networkingTest : public sntptest {
protected:
	bool CompareAddrinfo(const char* host,
											   int family, int flags,
											   const addrinfo& actual) {
		if (family != actual.ai_family)
			return false
				<< "Family mismatch, expected: " << family
				<< " but was: " << actual.ai_family;
		sockaddr_u* sock = new sockaddr_u;
		void* expectedaddr = NULL, *actualaddr = NULL;
		int size = 0, addrsize = 0;
		if (family == AF_INET) {
			expectedaddr = &sock->sa4.sin_addr;
			actualaddr = &((sockaddr_u*)actual.ai_addr)->sa4.sin_addr;
			size = sizeof(sock->sa4);
			addrsize = sizeof(sock->sa4.sin_addr);
		} else {
			expectedaddr = &sock->sa6.sin6_addr;
			actualaddr = &((sockaddr_u*)actual.ai_addr)->sa6.sin6_addr;
			size = sizeof(sock->sa6);
			addrsize = sizeof(sock->sa6.sin6_addr);
		}
		sock->sa.sa_family = family;

		if (inet_pton(family, host, expectedaddr) != 1)
			return false
				<< "inet_pton failed!";

		if (flags != actual.ai_flags)
			return false
				<< "Flags mismatch, expected: " << flags
				<< " but was: " << actual.ai_flags;

		if (size != actual.ai_addrlen)
			return false
				<< "Address length mismatch, expected: " << size
				<< " but was: " << actual.ai_addrlen;

		if (memcmp(expectedaddr, actualaddr, addrsize) != 0)
			return false
				<< "Address mismatch";
		return true;
	}
};

TEST(networking, ResolveSingleAddress) {
	const char* HOSTS[1] = {"192.0.2.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));

	TEST_ASSERT_TRUE(actual != NULL);
	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[0], AF_INET, 0, **actual));
}

TEST(networking, ResolveMultipleAddresses) {
	const char* HOSTS[3] = {"192.0.2.1", "192.0.2.5", "192.0.2.10"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(3, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));

	TEST_ASSERT_TRUE(actual != NULL);
	for (int i=0; i<HOSTCOUNT; i++) {
		TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[i], AF_INET, 0, *actual[i]))
			<< "Failed for host number " << i;
	}
}

TEST(networking, ResolveIPv6Address) {
	const char* HOSTS[1] = {"2001:0DB8:AC10:FE01:0:0:0:0"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	TEST_ASSERT_TRUE(actual != NULL);

	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[0], AF_INET6, 0, **actual));
}

TEST(networking, ResolveIPv6InvalidComparision) {
	const char* HOSTS[1] = {"2001:0DB8:AC10:FE01::"};
	const char* INVALID = "2001:0db8:ac10:fe01:0:1:0:0";
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	TEST_ASSERT_TRUE(actual != NULL);

	TEST_ASSERT_FALSE(CompareAddrinfo(INVALID, AF_INET6, 0, **actual));
}

TEST(networking, ResolveMixedAddressTypes) {
	const char* HOSTS[4] = {"2001:0db8:ac10:fe01::", "192.0.2.10",
							"192.0.2.30", "2001:ab0:1000::"};
	const int FAMILIES[4] = {AF_INET6, AF_INET, AF_INET, AF_INET6};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(4, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	TEST_ASSERT_TRUE(actual != NULL);

	for (int i=0; i<HOSTCOUNT; i++) {
		TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[i], FAMILIES[i], 0, *actual[i]))
			<< "Failed for host number " << i;
	}
}

TEST(networking, ResolveInvalidAddress) {
	const char* HOSTS[1] = {"192.258.2.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(0, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
}

TEST(networking, ResolveMixedAddressValidity) {
	const char* HOSTS[3] = {"2001:52ij:130:1::", "192.0.2.13", "192.0.257.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	TEST_ASSERT_TRUE(actual != NULL);

	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[1], AF_INET, 0, **actual));
}

TEST(networking, ResolveIgnoringIPv6) {
	const char* HOSTS[4] = {"2001:0db8:ac10:fe01::", "192.0.2.10",
							"192.0.2.30", "2001:ab0:1000::"};
	const int FAMILIES[4] = {AF_INET6, AF_INET, AF_INET, AF_INET6};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(2, resolve_hosts(HOSTS, HOSTCOUNT, &actual, AF_INET));
	TEST_ASSERT_TRUE(actual != NULL);

	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[1], FAMILIES[1], 0, *actual[0]));
	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[2], FAMILIES[2], 0, *actual[1]));
}

TEST(networking, ResolveIgnoringIPv4) {
	const char* HOSTS[4] = {"2001:0db8:ac10:fe01::", "192.0.2.10",
							"192.0.2.30", "2001:ab0:1000::"};
	const int FAMILIES[4] = {AF_INET6, AF_INET, AF_INET, AF_INET6};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	TEST_ASSERT_EQUAL(2, resolve_hosts(HOSTS, HOSTCOUNT, &actual, AF_INET6));
	TEST_ASSERT_TRUE(actual != NULL);

	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[0], FAMILIES[0], 0, *actual[0]));
	TEST_ASSERT_TRUE(CompareAddrinfo(HOSTS[3], FAMILIES[3], 0, *actual[1]));
}
