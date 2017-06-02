/*
 * ntp_assert.h - design by contract stuff
 *
 * example:
 *
 * int foo(char *a) {
 *	int result;
 *	int value;
 *
 *	REQUIRE(a != NULL);
 *	...
 *	bar(&value);
 *	INSIST(value > 2);
 *	...
 *
 *	ENSURE(result != 12);
 *	return result;
 * }
 *
 * open question: when would we use INVARIANT()?
 *
 * For cases where the overhead for non-debug builds is deemed too high,
 * use DEBUG_REQUIRE(), DEBUG_INSIST(), DEBUG_ENSURE(), and/or
 * DEBUG_INVARIANT().
 */

#ifndef GUARD_NTP_ASSERT_H
#define GUARD_NTP_ASSERT_H

#include <stdbool.h>

/* # if defined(__COVERITY__) */
/*
 * DH: try letting coverity scan our actual assertion macros, now that
 * isc_assertioncallback_t is marked __attribute__ __noreturn__.
 */

/*
 * Coverity has special knowledge that assert(x) terminates the process
 * if x is not true.  Rather than teach it about our assertion macros,
 * just use the one it knows about for Coverity Prevent scans.  This
 * means our assertion code (and ISC's) escapes Coverity analysis, but
 * that seems to be a reasonable trade-off.
 */

#if defined(__FLEXELINT__)

#include <assert.h>

#define ALWAYS_REQUIRE(x)	assert(x)
#define ALWAYS_INSIST(x)	assert(x)
#define ALWAYS_INVARIANT(x)	assert(x)
#define ALWAYS_ENSURE(x)	assert(x)

# else	/* not FlexeLint */

#include "isc/assertions.h"

#define ALWAYS_REQUIRE(x)	ISC_REQUIRE(x)
#define ALWAYS_INSIST(x)	ISC_INSIST(x)
#define ALWAYS_INVARIANT(x)	ISC_INVARIANT(x)
#define ALWAYS_ENSURE(x)	ISC_ENSURE(x)

# endif /* not FlexeLint */

#define	REQUIRE(x)		ALWAYS_REQUIRE(x)
#define	INSIST(x)		ALWAYS_INSIST(x)
#define	INVARIANT(x)		ALWAYS_INVARIANT(x)
#define	ENSURE(x)		ALWAYS_ENSURE(x)

# ifdef DEBUG
#define	DEBUG_REQUIRE(x)	REQUIRE(x)
#define	DEBUG_INSIST(x)		INSIST(x)
#define	DEBUG_INVARIANT(x)	INVARIANT(x)
#define	DEBUG_ENSURE(x)		ENSURE(x)
# else
#define	DEBUG_REQUIRE(x)	do {} while (false)
#define	DEBUG_INSIST(x)		do {} while (false)
#define	DEBUG_INVARIANT(x)	do {} while (false)
#define	DEBUG_ENSURE(x)		do {} while (false)
# endif

#endif	/* GUARD_NTP_ASSERT_H */
