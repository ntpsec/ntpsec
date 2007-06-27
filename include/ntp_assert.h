/*
 * ntp_assert.h - design by contract stuff
 */

#ifndef NTP_ASSSERT_H
#define NTP_ASSERT_H

#include "isc/assertions.h"

#define NTP_REQUIRE(x)	ISC_REQUIRE(x)
#define NTP_INSIST(x)	ISC_INSIST(x)
#define NTP_INVARIANT(x) ISC_INVARIANT(x)
#define NTP_ENSURE(x)	ISC_ENSURE(x)

#endif
