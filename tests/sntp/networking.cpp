#include "sntptest.h"

extern "C" {
#include "networking.h"
};

class networkingTest : public sntptest {
protected:
	::testing::AssertionResult CompareAddrinfo(const char* host,
											   int family, int flags,
											   const addrinfo& actual) {
		if (family != actual.ai_family)
			return ::testing::AssertionFailure()
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
			return ::testing::AssertionFailure()
				<< "inet_pton failed!";
		
		if (flags != actual.ai_flags)
			return ::testing::AssertionFailure()
				<< "Flags mismatch, expected: " << flags
				<< " but was: " << actual.ai_flags;

		if (size != actual.ai_addrlen)
			return ::testing::AssertionFailure()
				<< "Address length mismatch, expected: " << size
				<< " but was: " << actual.ai_addrlen;

		if (memcmp(expectedaddr, actualaddr, addrsize) != 0)
			return ::testing::AssertionFailure()
				<< "Address mismatch";
		return ::testing::AssertionSuccess();
	}
};

TEST_F(networkingTest, ResolveSingleAddress) {
	const char* HOSTS[1] = {"192.0.2.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));

	ASSERT_TRUE(actual != NULL);
	EXPECT_TRUE(CompareAddrinfo(HOSTS[0], AF_INET, 0, **actual));
}

TEST_F(networkingTest, ResolveMultipleAddresses) {
	const char* HOSTS[3] = {"192.0.2.1", "192.0.2.5", "192.0.2.10"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(3, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));

	ASSERT_TRUE(actual != NULL);
	for (int i=0; i<HOSTCOUNT; i++) {
		EXPECT_TRUE(CompareAddrinfo(HOSTS[i], AF_INET, 0, *actual[i]))
			<< "Failed for host number " << i;
	}
}

TEST_F(networkingTest, ResolveIPv6Address) {
	const char* HOSTS[1] = {"2001:0DB8:AC10:FE01:0:0:0:0"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	ASSERT_TRUE(actual != NULL);

	EXPECT_TRUE(CompareAddrinfo(HOSTS[0], AF_INET6, 0, **actual));
}

TEST_F(networkingTest, ResolveIPv6InvalidComparision) {
	const char* HOSTS[1] = {"2001:0DB8:AC10:FE01::"};
	const char* INVALID = "2001:0db8:ac10:fe01:0:1:0:0";
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	ASSERT_TRUE(actual != NULL);

	EXPECT_FALSE(CompareAddrinfo(INVALID, AF_INET6, 0, **actual));
}

TEST_F(networkingTest, ResolveMixedAddressTypes) {
	const char* HOSTS[4] = {"2001:0db8:ac10:fe01::", "192.0.2.10",
							"192.0.2.30", "2001:ab0:1000::"};
	const int FAMILIES[4] = {AF_INET6, AF_INET, AF_INET, AF_INET6};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(4, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	ASSERT_TRUE(actual != NULL);

	for (int i=0; i<HOSTCOUNT; i++) {
		EXPECT_TRUE(CompareAddrinfo(HOSTS[i], FAMILIES[i], 0, *actual[i]))
			<< "Failed for host number " << i;
	}
}

TEST_F(networkingTest, ResolveInvalidAddress) {
	const char* HOSTS[1] = {"192.258.2.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(0, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
}

TEST_F(networkingTest, ResolveMixedAddressValidity) {
	const char* HOSTS[3] = {"2001:52ij:130:1::", "192.0.2.13", "192.0.257.1"};
	const int HOSTCOUNT = COUNTOF(HOSTS);

	addrinfo** actual = NULL;

	ASSERT_EQ(1, resolve_hosts(HOSTS, HOSTCOUNT, &actual, PF_UNSPEC));
	ASSERT_TRUE(actual != NULL);

	EXPECT_TRUE(CompareAddrinfo(HOSTS[1], AF_INET, 0, **actual));
}
