/*
 * timevalops.h -- calculations on 'struct timeval' values
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 *
 * For a rationale look at 'timespecops.h'; we do the same here, but the
 * normalisation keeps the microseconds in [0 .. 10^6], of course.
 * NTPsec implements fewer operations here as we are trying to do as
 * much arithmetic as possible in nanoseconds for code-hygiene reasons.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */
#ifndef GUARD_TIMEVALOPS_H
#define GUARD_TIMEVALOPS_H

#include <sys/types.h>
#include <stdio.h>

#include "ntp.h"
#include "ntp_calendar.h"

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


/* make sure microseconds are in nominal range */
static inline struct timeval
normalize_tval(
	struct timeval	x
	)
{
	long		z;

	/*
	 * If the fraction becomes excessively denormal, we use division
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

#endif	/* GUARD_TIMEVALOPS_H */
