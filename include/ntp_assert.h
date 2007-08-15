/*
 * ntp_assert.h - design by contract stuff
 */

#ifndef NTP_ASSSERT_H
#define NTP_ASSERT_H

# ifdef CALYSTO 

extern void calysto_assume(unsigned char cnd); /* assume this always holds */ 
extern void calysto_assert(unsigned char cnd); /* check whether this holds */ 
#define NTP_REQUIRE(x)  calysto_assert(x) 
#define NTP_INSIST(x)   calysto_assume(x) 
#define NTP_INVARIANT(x) calysto_assume(x) 
#define NTP_ENSURE(x)   calysto_assert(x) 
 
# else /* ~CALYSTO */

#include "isc/assertions.h"

#define NTP_REQUIRE(x)	ISC_REQUIRE(x)
#define NTP_INSIST(x)	ISC_INSIST(x)
#define NTP_INVARIANT(x) ISC_INVARIANT(x)
#define NTP_ENSURE(x)	ISC_ENSURE(x)

# endif /* ~CALYSTO */
#endif
