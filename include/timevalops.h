/*
 * timevalops.h -- calculations on 'struct timeval' values
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 *
 * For a rationale look at 'timespecops.h'; we do the same here, but the
 * normalisation keeps the microseconds in [0 .. 10^6[, of course.
 */
#ifndef TIMPEVALOPS_H
#define TIMPEVALOPS_H

#include <config.h>
#include <sys/types.h>
#include <stdio.h>
#include "ntp_unixtime.h"
#include "ntp_fp.h"

/*
 * We avoid to use/define MICROSECONDS here, as it is also possibly
 * Defined in some other files and we don't want to resolve the possible
 * clashes here.
 */

/* predicate: returns TRUE if the microseconds are out-of-bounds */
/* use like: int timeval_isdenormal(const struct timeval *x) */
#define timeval_isdenormal(x) \
	((u_long)(x)->tv_usec >= 1000000)

/* predicate: returns TRUE if the microseconds are in nominal range */
/* use like: int timeval_isnormal(const struct timeval *x) */
#define timeval_isnormal(x) \
	((u_long)(x)->tv_usec <	 1000000)


/*make sure microseconds are in nominal range */
extern void timeval_norm(struct timeval *x);

/* x = a, normlised */
extern void timeval_copy(struct timeval *x, const struct timeval *a);

/* x = a + b */
extern void timeval_add(struct timeval *x, const struct timeval *a,
			const struct timeval *b);

/* x = a + b, b is fraction only */
extern void timeval_addus(struct timeval *x, const struct timeval *a,
			  long b);

/* x = a + b */
extern void timeval_sub(struct timeval *x, const struct timeval *a,
			const struct timeval *b);

/* x = a - b, b is fraction only */
extern void timeval_subus(struct timeval *x, const struct timeval *a,
			  long	b);

/* x = -a */
extern void timeval_neg(struct timeval *x, const struct timeval *a);

/* x = ( a < 0) ? -a : a
 * return if negation was needed
 */
extern int timeval_abs(struct timeval *x, const struct timeval *a);

/* compare a <--> b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
extern int timeval_cmp_fast(const struct timeval *a,
			    const struct timeval *b);

extern int timeval_cmp(const struct timeval *a,
		       const struct timeval *b);

/* test a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
extern int timeval_test_fast(const struct timeval *a);
extern int timeval_test(const struct timeval *a);

/* return LIB buffer ptr to string rep */
extern const char* timeval_tostr(const struct timeval *x);

/*
  convert to l_fp type, relative and absolute
*/

/* convert from duration to duration */
extern void timeval_reltolfp(l_fp *y, const struct timeval *x);

/* 'x' must be UN*X epoch, output will be in NTP epoch */
extern void timeval_abstolfp(l_fp *y, const struct timeval *x);

/*
  convert to l_fp type, relative signed/unsigned and absolute
*/
extern void timeval_relfromlfp(struct timeval *y, const l_fp *x);
extern void timeval_urelfromlfp(struct timeval *y, const l_fp *x);

/* absolute (timestamp) conversion. Input is time in NTP epoch, output
 * is in UN*X epoch. The NTP time stamp will be expanded the pivot time
 * '*' or the current time, if 'p' is NULL.
 */
extern void timeval_absfromlfp(struct timeval *y, const l_fp *x,
			       const time_t *p);

#endif
/* -*- EOF -*- */
