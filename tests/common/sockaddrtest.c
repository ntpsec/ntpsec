#include <inttypes.h>

#include "config.h"

#include "ntp.h"
#include "sockaddrtest.h"

bool IsEqualS(const sockaddr_u *expected, const sockaddr_u *actual) {
	if (AF(expected) != AF(actual)) {
		printf("Expected sa_family: %" PRIuMAX " but got: %" PRIuMAX "\n",
		       (uintmax_t)AF(expected), (uintmax_t)AF(actual));
		return false;
	}

	if (AF(actual) == AF_INET) { // IPv4
		if (NSRCPORT(expected) == NSRCPORT(actual) &&
		    memcmp(&SOCK_ADDR4(expected), &SOCK_ADDR4(actual),
				   sizeof(in_addr_t)) == 0) {
			return true;
		} else {
			printf("IPv4 comparison failed, expected: %u (%s) but was: %u (%s)\n", SOCK_ADDR4(expected).s_addr, socktoa(expected), SOCK_ADDR4(actual).s_addr, socktoa(actual));
			return false;
		}
	} else if (AF(actual) == AF_INET6) { //IPv6
		if (expected->sa6.sin6_port == actual->sa6.sin6_port &&
			memcmp(&SOCK_ADDR6(expected), &SOCK_ADDR6(actual),
				   sizeof(struct in6_addr)) == 0) {
			return true;
		} else {
			printf("IPv6 comparison failed\n");
			return false;
		}
	} else { // Unknown family
		printf("Unknown sa_family: %" PRIuMAX "\n", (uintmax_t)AF(actual));
		return false;
	}
}

/* Similar to IsEqualS, but doesn't print misleading messages */
bool IsDiffS(const sockaddr_u *expected, const sockaddr_u *actual) {
	if (AF(expected) != AF(actual)) {
		return true;
	}

	if (AF(actual) == AF_INET) { // IPv4
		if (NSRCPORT(expected) == NSRCPORT(actual) &&
			memcmp(&SOCK_ADDR4(expected), &SOCK_ADDR4(actual),
				   sizeof(in_addr_t)) == 0) {
			printf("IPv4 address matches: %u (%s)\n", SOCK_ADDR4(expected).s_addr, socktoa(expected));
			return false;
		} else {
			return true;
		}
	} else if (AF(actual) == AF_INET6) { //IPv6
		if (expected->sa6.sin6_port == actual->sa6.sin6_port &&
			memcmp(&SOCK_ADDR6(expected), &SOCK_ADDR6(actual),
				   sizeof(struct in6_addr)) == 0) {
			printf("IPv6 address matches\n");
			return false;
		} else {
			return true;
		}
	} else { // Unknown family
		printf("Can't compare unknown address family\n");
		return false;
	}
}

sockaddr_u CreateSockaddr4(const char* address, unsigned int port) {
	sockaddr_u s;
	SET_AF(&s, AF_INET);
	PSOCK_ADDR4(&s)->s_addr = inet_addr(address);
	SET_PORT(&s, port);
	return s;
}
