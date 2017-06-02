/*
 * ntp_assert.h - design by contract stuff
 *
 * Copyright (C) 2004-2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1997-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
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

#define REQUIRE(x)      assert(x)
#define INSIST(x)       assert(x)
#define INVARIANT(x)    assert(x)
#define ENSURE(x)       assert(x)

# else	/* not FlexeLint */

/*% isc assertion type */
typedef enum {
	isc_assertiontype_require,
	isc_assertiontype_ensure,
	isc_assertiontype_insist,
	isc_assertiontype_invariant
} isc_assertiontype_t;

typedef void (*isc_assertioncallback_t)(const char *, int, isc_assertiontype_t,
					const char *);

/* coverity[+kill] */
void isc_assertion_failed(const char *, int, isc_assertiontype_t,
			  const char *) ISC_PLATFORM_NORETURN_POST;

void
isc_assertion_setcallback(isc_assertioncallback_t);

const char *
isc_assertion_typetotext(isc_assertiontype_t type)
			__attribute__((const));

#ifndef ISC_CHECK_REQUIRE
#define ISC_CHECK_REQUIRE		1
#endif

#ifndef ISC_CHECK_ENSURE
#define ISC_CHECK_ENSURE		1
#endif

#ifndef ISC_CHECK_INSIST
#define ISC_CHECK_INSIST		1
#endif

#ifndef ISC_CHECK_INVARIANT
#define ISC_CHECK_INVARIANT		1
#endif

#if ISC_CHECK_REQUIRE != 0
#define ISC_REQUIRE(cond) \
	((void) ((cond) || \
		 ((isc_assertion_failed)(__FILE__, __LINE__, \
					 isc_assertiontype_require, \
					 #cond), 0)))
#else
#define ISC_REQUIRE(cond)	((void) 0)
#endif /* ISC_CHECK_REQUIRE */

#if ISC_CHECK_ENSURE != 0
#define ISC_ENSURE(cond) \
	((void) ((cond) || \
		 ((isc_assertion_failed)(__FILE__, __LINE__, \
					 isc_assertiontype_ensure, \
					 #cond), 0)))
#else
#define ISC_ENSURE(cond)	((void) 0)
#endif /* ISC_CHECK_ENSURE */

#if ISC_CHECK_INSIST != 0
#define ISC_INSIST(cond) \
	((void) ((cond) || \
		 ((isc_assertion_failed)(__FILE__, __LINE__, \
					 isc_assertiontype_insist, \
					 #cond), 0)))
#else
#define ISC_INSIST(cond)	((void) 0)
#endif /* ISC_CHECK_INSIST */

#if ISC_CHECK_INVARIANT != 0
#define ISC_INVARIANT(cond) \
	((void) ((cond) || \
		 ((isc_assertion_failed)(__FILE__, __LINE__, \
					 isc_assertiontype_invariant, \
					 #cond), 0)))
#else
#define ISC_INVARIANT(cond)	((void) 0)
#endif /* ISC_CHECK_INVARIANT */

#define REQUIRE(x)      ISC_REQUIRE(x)
#define INSIST(x)       ISC_INSIST(x)
#define INVARIANT(x)    ISC_INVARIANT(x)
#define ENSURE(x)       ISC_ENSURE(x)

# endif /* not FlexeLint */

# ifdef DEBUG
#define	DEBUG_REQUIRE(x)	REQUIRE(x)
#define	DEBUG_INSIST(x)		INSIST(x)
#define	DEBUG_ENSURE(x)		ENSURE(x)
# else
#define	DEBUG_REQUIRE(x)	do {} while (false)
#define	DEBUG_INSIST(x)		do {} while (false)
#define	DEBUG_ENSURE(x)		do {} while (false)
# endif

#endif	/* GUARD_NTP_ASSERT_H */
