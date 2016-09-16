/*
 * timevalops.h -- calculations on 'struct timeval' values
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 *
 * For a rationale look at 'timespecops.h'; we do the same here, but the
 * normalisation keeps the microseconds in [0 .. 10^6], of course.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */
#ifndef GUARD_TIMEVALOPS_H
#define GUARD_TIMEVALOPS_H

#include <sys/types.h>
#include <stdio.h>

#include "ntp.h"
#include "timetoa.h"


/* microseconds per second */
#define MICROSECONDS 1000000

/*
 * Convert usec to a time stamp fraction.
 */
# define TVUTOTSF(tvu, tsf)						\
	((tsf) = (uint32_t)						\
		 ((((uint64_t)(tvu) << 32) + MICROSECONDS / 2) /		\
		  MICROSECONDS))

/*
 * Convert a time stamp fraction to microseconds.  The time stamp
 * fraction is assumed to be unsigned.
 */
# define TSFTOTVU(tsf, tvu)						\
	 ((tvu) = (int32_t)						\
		  (((uint64_t)(tsf) * MICROSECONDS + 0x80000000) >> 32))

/*
 * Convert a struct timeval to a time stamp.
 */
#define TVTOTS(tv, ts) \
	do { \
		(ts)->l_ui = (u_long)(tv)->tv_sec; \
		TVUTOTSF((tv)->tv_usec, (ts)->l_uf); \
	} while (false)

#define sTVTOTS(tv, ts) \
	do { \
		bool isneg = false; \
		long usec; \
		(ts)->l_ui = (tv)->tv_sec; \
		usec = (tv)->tv_usec; \
		if (((tv)->tv_sec < 0) || ((tv)->tv_usec < 0)) { \
			usec = -usec; \
			(ts)->l_ui = -(ts)->l_ui; \
			isneg = true; \
		} \
		TVUTOTSF(usec, (ts)->l_uf); \
		if (isneg) { \
			L_NEG((ts)); \
		} \
	} while (false)

/*
 * Convert a time stamp to a struct timeval.  The time stamp
 * has to be positive.
 */
#define	TSTOTV(ts, tv) \
	do { \
		(tv)->tv_sec = (ts)->l_ui; \
		TSFTOTVU((ts)->l_uf, (tv)->tv_usec); \
		if ((tv)->tv_usec == MICROSECONDS) { \
			(tv)->tv_sec++; \
			(tv)->tv_usec = 0; \
		} \
	} while (false)


/*
 * predicate: returns true if the microseconds are in nominal range
 * use like: int timeval_isnormal(const struct timeval *x)
 */
#define timeval_isnormal(x) \
	((x)->tv_usec >= 0 && (x)->tv_usec < MICROSECONDS)

/*
 * Convert milliseconds to a time stamp fraction.  Unused except for
 * refclock_leitch.c, so accompanying lookup tables were removed in
 * favor of reusing the microseconds conversion tables.
 */
#define	MSUTOTSF(msu, tsf)	TVUTOTSF((msu) * 1000, tsf)

/*
 * predicate: returns true if the microseconds are out-of-bounds
 * use like: int timeval_isdenormal(const struct timeval *x)
 */
#define timeval_isdenormal(x)	(!timeval_isnormal(x))

/* make sure microseconds are in nominal range */
static inline struct timeval
normalize_tval(
	struct timeval	x
	)
{
	long		z;

	/*
	 * If the fraction becomes excessive denormal, we use division
	 * to do first partial normalisation. The normalisation loops
	 * following will do the remaining cleanup. Since the size of
	 * tv_usec has a peculiar definition by the standard the range
	 * check is coded manually. And labs() is intentionally not used
	 * here: it has implementation-defined behaviour when applied
	 * to LONG_MIN.
	 */
	if (x.tv_usec < -3l * MICROSECONDS ||
	    x.tv_usec >  3l * MICROSECONDS  ) {
		z = x.tv_usec / MICROSECONDS;
		x.tv_usec -= z * MICROSECONDS;
		x.tv_sec += z;
	}

	/*
	 * Do any remaining normalisation steps in loops. This takes 3
	 * steps max, and should outperform a division even if the
	 * mul-by-inverse trick is employed. (It also does the floor
	 * division adjustment if the above division was executed.)
	 */
	if (x.tv_usec < 0)
		do {
			x.tv_usec += MICROSECONDS;
			x.tv_sec--;
		} while (x.tv_usec < 0);
	else if (x.tv_usec >= MICROSECONDS)
		do {
			x.tv_usec -= MICROSECONDS;
			x.tv_sec++;
		} while (x.tv_usec >= MICROSECONDS);

	return x;
}

/* x = a + b */
static inline struct timeval
add_tval(
	struct timeval	a,
	struct timeval	b
	)
{
	struct timeval	x;

	x = a;
	x.tv_sec += b.tv_sec;
	x.tv_usec += b.tv_usec;

	return normalize_tval(x);
}

/* x = a + b, b is fraction only */
static inline struct timeval
add_tval_us(
	struct timeval	a,
	long		b
	)
{
	struct timeval x;

	x = a;
	x.tv_usec += b;

	return normalize_tval(x);
}

/* x = a - b */
static inline struct timeval
sub_tval(
	struct timeval	a,
	struct timeval	b
	)
{	
	struct timeval	x;

	x = a;
	x.tv_sec -= b.tv_sec;
	x.tv_usec -= b.tv_usec;

	return normalize_tval(x);
}

/* x = a - b, b is fraction only */
static inline struct timeval
sub_tval_us(
	struct timeval	a,
	long		b
	)
{
	struct timeval x;

	x = a;
	x.tv_usec -= b;

	return normalize_tval(x);
}

/* x = -a */
static inline struct timeval
neg_tval(
	struct timeval	a
	)
{	
	struct timeval	x;

	x.tv_sec = -a.tv_sec;
	x.tv_usec = -a.tv_usec;

	return normalize_tval(x);
}

/* x = abs(a) */
static inline struct timeval
abs_tval(
	struct timeval	a
	)
{
	struct timeval	c;

	c = normalize_tval(a);
	if (c.tv_sec < 0) {
		if (c.tv_usec != 0) {
			c.tv_sec = -c.tv_sec - 1;
			c.tv_usec = MICROSECONDS - c.tv_usec;
		} else {
			c.tv_sec = -c.tv_sec;
		}
	}

	return c;
}

/*
 * compare previously-normalised a and b
 * return 1 / 0 / -1 if a < / == / > b
 */
static inline int
cmp_tval(
	struct timeval a,
	struct timeval b
	)
{
	int r;

	r = (a.tv_sec > b.tv_sec) - (a.tv_sec < b.tv_sec);
	if (0 == r)
		r = (a.tv_usec > b.tv_usec) -
		    (a.tv_usec < b.tv_usec);
	
	return r;
}

/*
 * compare possibly-denormal a and b
 * return 1 / 0 / -1 if a < / == / > b
 */
static inline int
cmp_tval_denorm(
	struct timeval	a,
	struct timeval	b
	)
{
	return cmp_tval(normalize_tval(a), normalize_tval(b));
}

/*
 * test previously-normalised a
 * return 1 / 0 / -1 if a < / == / > 0
 */
static inline int
test_tval(
	struct timeval	a
	)
{
	int		r;

	r = (a.tv_sec > 0) - (a.tv_sec < 0);
	if (r == 0)
		r = (a.tv_usec > 0);
	
	return r;
}

/*
 * test possibly-denormal a
 * return 1 / 0 / -1 if a < / == / > 0
 */
static inline int
test_tval_denorm(
	struct timeval	a
	)
{
	return test_tval(normalize_tval(a));
}

/* return LIB buffer ptr to string rep */
static inline const char *
tvaltoa(
	struct timeval	x
	)
{
	return format_time_fraction(x.tv_sec, x.tv_usec, 6);
}

/* convert from timeval duration to l_fp duration */
static inline l_fp
tval_intv_to_lfp(
	struct timeval	x
	)
{
	struct timeval	v;
	l_fp		y;
	
	v = normalize_tval(x);
	TVUTOTSF(v.tv_usec, y.l_uf);
	y.l_i = (int32_t)v.tv_sec;

	return y;
}

/* x must be UN*X epoch, output *y will be in NTP epoch */
static inline l_fp
tval_stamp_to_lfp(
	struct timeval	x
	)
{
	l_fp		y;

	y = tval_intv_to_lfp(x);
	y.l_ui += JAN_1970;

	return y;
}

/* convert to l_fp type, relative signed/unsigned and absolute */
static inline struct timeval
lfp_intv_to_tval(
	l_fp		x
	)
{
	struct timeval	out;
	l_fp		absx;
	int		neg;
	
	neg = L_ISNEG(&x);
	absx = x;
	if (neg) {
		L_NEG(&absx);	
	}
	TSFTOTVU(absx.l_uf, out.tv_usec);
	out.tv_sec = absx.l_i;
	if (neg) {
		out.tv_sec = -out.tv_sec;
		out.tv_usec = -out.tv_usec;
		out = normalize_tval(out);
	}

	return out;
}

static inline struct timeval
lfp_uintv_to_tval(
	l_fp		x
	)
{
	struct timeval	out;
	
	TSFTOTVU(x.l_uf, out.tv_usec);
	out.tv_sec = x.l_ui;

	return out;
}

/*
 * absolute (timestamp) conversion. Input is time in NTP epoch, output
 * is in UN*X epoch. The NTP time stamp will be expanded around the
 * pivot time *p or the current time, if p is NULL.
 */
static inline struct timeval
lfp_stamp_to_tval(
	l_fp		x,
	const time_t *	p
	)
{
	struct timeval	out;
	vint64		sec;

	sec = ntpcal_ntp_to_time(x.l_ui, p);
	TSFTOTVU(x.l_uf, out.tv_usec);

	/* copying a vint64 to a time_t needs some care... */
#if SIZEOF_TIME_T <= 4
	out.tv_sec = (time_t)sec.d_s.lo;
#else
	out.tv_sec = (time_t)sec.q_s;
#endif
	out = normalize_tval(out);

	return out;
}

#endif	/* GUARD_TIMEVALOPS_H */
