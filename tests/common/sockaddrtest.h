#ifndef GUARD_TESTS_SOCKADDRTEST_H
#define GUARD_TESTS_SOCKADDRTEST_H

#include "libntptest.h"

#include "ntp.h"

bool IsEqualS(const sockaddr_u *expected, const sockaddr_u *actual) {
	if (expected->sa.sa_family != actual->sa.sa_family) {
		printf("Expected sa_family: %hhu but got: %hhu\n", expected->sa.sa_family, actual->sa.sa_family);
		return false;
	}

	if (actual->sa.sa_family == AF_INET) { // IPv4
		if (expected->sa4.sin_port == actual->sa4.sin_port &&
			memcmp(&expected->sa4.sin_addr, &actual->sa4.sin_addr,
				   sizeof(in_addr_t)) == 0) {
			return true;
		} else {
			printf("IPv4 comparision failed, expected: %u (%s) but was: %u (%s)\n", expected->sa4.sin_addr.s_addr, socktoa(expected), actual->sa4.sin_addr.s_addr, socktoa(actual));
			return false;
		}
	} else if (actual->sa.sa_family == AF_INET6) { //IPv6
		if (expected->sa6.sin6_port == actual->sa6.sin6_port &&
			memcmp(&expected->sa6.sin6_addr, &actual->sa6.sin6_addr,
				   sizeof(struct in6_addr)) == 0) {
			return true;
		} else {
			printf("IPv6 comparision failed\n");
			return false;
		}
	} else { // Unknown family
		printf("Unknown sa_family: %hhu\n", actual->sa.sa_family);
		return false;
	}
}

sockaddr_u CreateSockaddr4(const char* address, unsigned int port) {
	sockaddr_u s;
	s.sa4.sin_family = AF_INET;
	s.sa4.sin_addr.s_addr = inet_addr(address);
	SET_PORT(&s, port);
	return s;
}


#endif // GUARD_TESTS_SOCKADDRTEST_H
