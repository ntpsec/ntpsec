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
 *      int result;
 *      int value;
 *
 *      REQUIRE(a != NULL);
 *      ...
 *      bar(&value);
 *      INSIST(value > 2);
 *      ...
 *
 *      ENSURE(result != 12);
 *      return result;
 * }
 *
 * open question: when would we use INVARIANT()?
 *
 */

#ifndef GUARD_NTP_ASSERT_H
#define GUARD_NTP_ASSERT_H

#include <stdbool.h>

#if defined(__FLEXELINT__)

#include <assert.h>

#define REQUIRE(x)      assert(x)
#define INSIST(x)       assert(x)
#define INVARIANT(x)    assert(x)
#define ENSURE(x)       assert(x)

# else  /* not FlexeLint */

/*% isc assertion type */
typedef enum {
        assertiontype_require,
        assertiontype_ensure,
        assertiontype_insist,
        assertiontype_invariant
} assertiontype_t;


/* our assertion catcher */
extern void assertion_failed(const char *, int, assertiontype_t, const char *)
                        __attribute__   ((__noreturn__));

#define REQUIRE(cond) \
        ((void) ((cond) || (assertion_failed(__FILE__, __LINE__, \
                                         assertiontype_require, #cond), 0)))

#define ENSURE(cond) \
        ((void) ((cond) || (assertion_failed(__FILE__, __LINE__, \
                                         assertiontype_ensure, #cond), 0)))

#define INSIST(cond) \
        ((void) ((cond) || (assertion_failed(__FILE__, __LINE__, \
                                         assertiontype_insist, #cond), 0)))

#define INVARIANT(cond) \
        ((void) ((cond) || (assertion_failed(__FILE__, __LINE__, \
                                         assertiontype_invariant, #cond), 0)))
# endif /* not FlexeLint */

#if defined(USEBACKTRACE) && \
	(defined(HAVE_BACKTRACE_SYMBOLS_FD) || defined (HAVE__UNWIND_BACKTRACE))
    /* doing backtrace */

extern void backtrace_log(void);

#else
    /* not doing backtrace */
# define BACKTRACE_DISABLED 1
#endif   /* ! USEBACKTRACE */

#endif  /* GUARD_NTP_ASSERT_H */
