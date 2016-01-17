#ifndef GUARD_TESTS_SOCKADDRTEST_H
#define GUARD_TESTS_SOCKADDRTEST_H

#include "ntp.h"

bool IsEqualS(const sockaddr_u *expected, const sockaddr_u *actual);
bool IsDiffS(const sockaddr_u *expected, const sockaddr_u *actual);
sockaddr_u CreateSockaddr4(const char* address, unsigned int port);


#endif // GUARD_TESTS_SOCKADDRTEST_H
