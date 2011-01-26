/*
 * timespecops.h -- calculations on 'struct timespec' values
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 *
 * Rationale
 * ---------
 *
 * Doing basic arithmetic on a 'struct timespec' is not exceedingly
 * hard, but it requires tedious and repetitive code to keep the result
 * normalised. We consider a timespec normalised when the nanosecond
 * fraction is in the interval [0 .. 10^9[ ; there are multiple value
 * pairs of seconds and nanoseconds that denote the same time interval,
 * but the normalised representation is unique. No two different
 * intervals can have the same normalised representation.
 *
 * Another topic is the representation of negative time intervals.
 * There's more than one way to this, since both the seconds and the
 * nanoseconds of a timespec are signed values. IMHO, the easiest way is
 * to use a complement representation where the nanoseconds are still
 * normalised, no matter what the sign of the seconds value. This makes
 * normalisation easier, since the sign of the integer part is
 * irrelevant, and it removes several sign decision cases during the
 * calculations.
 *
 * As long as no signed integer overflow can occur with the nanosecond
 * part of the operands, all operations work as expected and produce a
 * normalised result.
 *
 * The exception to this are functions fix a '_fast' suffix, which do no
 * normalisation on input data and therefore expect the input data to be
 * normalised.
 *
 * Input and output operands may overlap; all input is consumed before
 * the output is written to.
 */
#ifndef TIMESPECOPS_H
#define TIMESPECOPS_H

#include <sys/types.h>
#include <stdio.h>

#include "ntp.h"
#include "ntp_unixtime.h"


/*
 * We avoid to use/define NANOSECONDS here, as it is also defined in
 * some other files and we don't want to resolve the possible clashes
 * here.
 */

/* predicate: returns TRUE if the nanoseconds are in nominal range */
#define timespec_isnormal(x) \
	((x)->tv_nsec >= 0 && (x)->tv_nsec < 1000000000)

/* predicate: returns TRUE if the nanoseconds are out-of-bounds */
#define timespec_isdenormal(x)	(!timespec_isnormal(x))


/*make sure nanoseconds are in nominal range */
extern void timespec_norm(struct timespec *x);

/* x = a, normalised */
extern void timespec_copy(struct timespec *x, const struct timespec *a);

/* x = a + b */
extern void timespec_add(struct timespec *x, const struct timespec *a,
			 const struct timespec *b);

/* x = a + b, b is fraction only */
extern void timespec_addns(struct timespec *x, const struct timespec *a,
			   long b);

/* x = a - b */
extern void timespec_sub(struct timespec *x, const struct timespec *a,
			 const struct timespec *b);

/* x = a - b, b is fraction only */
extern void timespec_subns(struct timespec *x, const struct timespec *a,
			   long	b);

/* x = -a */
extern void timespec_neg(struct timespec *x, const struct timespec *a);

/*
 * x = abs(a)
 * returns nonzero if negation was needed
 */
extern int timespec_abs(struct timespec *x, const struct timespec *a);

/*
 * compare previously-normalised a and b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
extern int timespec_cmp_fast(const struct timespec *a,
			     const struct timespec *b);

/*
 * compare possibly-denormal a and b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
extern int timespec_cmp(const struct timespec *a,
			const struct timespec *b);

/*
 * test previously-normalised a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
extern int timespec_test_fast(const struct timespec *a);

/*
 * test possibly-denormal a
 * return 1 / 0 / -1 if a < / == / > 0
 */
extern int timespec_test(const struct timespec *a);

/* return LIB buffer ptr to string rep */
extern const char * timespec_tostr(const struct timespec *x);

/*
  convert to l_fp type, relative and absolute
*/

/* convert from timespec duration to l_fp duration */
extern void timespec_reltolfp(l_fp *y, const struct timespec *x);

/* x must be UN*X epoch, output *y will be in NTP epoch */
extern void timespec_abstolfp(l_fp *y, const struct timespec *x);

/* convert to l_fp type, relative signed/unsigned and absolute */
extern void timespec_relfromlfp(struct timespec *y, const l_fp *x);
extern void timespec_urelfromlfp(struct timespec *y, const l_fp *x);

/*
 * absolute (timestamp) conversion. Input is time in NTP epoch, output
 * is in UN*X epoch. The NTP time stamp will be expanded around the
 * pivot time *p or the current time, if p is NULL.
 */
extern void timespec_absfromlfp(struct timespec *y, const l_fp *x,
				const time_t *p);

#endif	/* TIMESPECOPS_H */
