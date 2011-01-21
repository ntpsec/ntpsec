/*
 * timevalops.c -- calculations on 'struct timeval' values
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 */

#include <config.h>
#include <math.h>

#include "lib_strbuf.h"
#include "ntp_calendar.h"

#include "timevalops.h"

/* formatting to string needs at max 29 bytes (even with 64 bit time_t),
 * so we check LIB_BUFLENGTH is big enough for our pupose.
 */
#if LIB_BUFLENGTH < 29
#error LIB_BUFLENGTH not big enough
#endif

/* make sure we have the right definition for MICROSECONDS */
#undef MICROSECONDS
#define MICROSECONDS 1000000

/* conversion between l_fp fractions and microseconds */
#if SIZEOF_LONG >= 8
# define MYFTOTVU(tsf, tvu)						\
	((tvu) = (int32)						\
	 (((u_long)(tsf) * MICROSECONDS + 0x80000000) >> 32))
# define MYTVUTOF(tvu, tsf)						\
	((tsf) = (u_int32)						\
		((((u_long)(tvu) << 32) + MICROSECONDS / 2) /		\
		 MICROSECONDS))
#else
# define MYFTOTVU(tsf, tvu)	TSFTOTVU(tsf, tvu)
# define MYTVUTOF(tvu, tsf)	TVUTOTSF(tvu, tsf)
#endif

/* using snprintf is troublesome with time_t. Try to resolve it. */
#if SIZEOF_TIME_T <= SIZEOF_INT
typedef unsigned int u_time;
#define TIMEFMT ""
#elif SIZEOF_TIME_T <= SIZEOF_LONG
typedef unsigned long u_time;
#define TIMEFMT "l"
#elif defined(SIZEOF_LONG_LONG) && SIZEOF_TIME_T <= SIZEOF_LONG_LONG
typedef unsigned long long u_time;
#define TIMEFMT "ll"
#else
#include "GRONK: what size has a time_t here?"
#endif

/* copy and normalise. Used often enough to warrant a macro. */
#define COPYNORM(dst, src)				\
	do {						\
		*(dst) = *(src);			\
		if (timeval_isdenormal(dst))		\
			timeval_norm(dst);		\
	} while (0)


void
timeval_norm(
	struct timeval *x
	)
{
	/* If the fraction becomes excessive denormal, we use division
	 * to do first partial normalisation. The normalisation loops
	 * following will do the remaining cleanup. Since the size of
	 * tv_usec has a peculiar definition by the standard the range
	 * check is coded manualla.
	 */
	if (x->tv_usec < -3l * MICROSECONDS ||
	    x->tv_usec >  3l * MICROSECONDS  ) {
		long z;
		z = x->tv_usec / MICROSECONDS;
		x->tv_usec -= z * MICROSECONDS;
		x->tv_sec  += z;
	}
	/* Do any remaining normalisation steps in loops. This takes 3
	 * steps max, and should outperform a division even if the
	 * mul-by-inverse trick is employed. (It also does the floor
	 * division adjustment if the above division was executed.) */
	if (x->tv_usec < 0)
		do {
			x->tv_usec += MICROSECONDS;
			x->tv_sec  -= 1;
		} while (x->tv_usec < 0);
	else if (x->tv_usec >= MICROSECONDS)
		do {
			x->tv_usec -= MICROSECONDS;
			x->tv_sec  += 1;
		} while (x->tv_usec >= MICROSECONDS);
}

/* x = a, normalised */
void
timeval_copy(
	struct timeval *	x,
	const struct timeval *	a
	)
{
	COPYNORM(x, a);
}

/* x = a + b */
void
timeval_add(
	struct timeval *	x,
	const struct timeval *	a,
	const struct timeval *	b
	)
{	
	struct timeval c;

	c.tv_sec  = a->tv_sec  + b->tv_sec;
	c.tv_usec = a->tv_usec + b->tv_usec;
	COPYNORM(x, &c);
}

/* x = a + b, b is fraction only */
void
timeval_addus(
	struct timeval *	x,
	const struct timeval *	a,
	long			b
	)
{	
	struct timeval c;

	c.tv_sec  = a->tv_sec;
	c.tv_usec = a->tv_usec + b;
	COPYNORM(x, &c);
}

/* x = a - b */
void
timeval_sub(
	struct timeval *	x,
	const struct timeval *	a,
	const struct timeval *	b
	)
{	
	struct timeval c;

	c.tv_sec  = a->tv_sec  - b->tv_sec;
	c.tv_usec = a->tv_usec - b->tv_usec;
	COPYNORM(x, &c);
}

/* x = a - b, b is fraction only */
void
timeval_subus(
	struct timeval *	x,
	const struct timeval *	a,
	long			b
	)
{	
	struct timeval c;

	c.tv_sec  = a->tv_sec;
	c.tv_usec = a->tv_usec - b;
	COPYNORM(x, &c);
}

/* x = -a */
void
timeval_neg(
	struct timeval *	x,
	const struct timeval *	a
	)
{	
	struct timeval c;

	c.tv_sec  = -a->tv_sec;
	c.tv_usec = -a->tv_usec;
	COPYNORM(x, &c);
}

/*
 * x = abs(a)
 * return value is nonzero if negation was needed
 */
int
timeval_abs(
	struct timeval *	x,
	const struct timeval *	a
	)
{	
	struct timeval	c;
	int		r;

	COPYNORM(&c, a);
	r = (c.tv_sec < 0);
	if (r != 0) {
		c.tv_sec  = - c.tv_sec;
		c.tv_usec = - c.tv_usec;
		if (c.tv_usec < 0) {
			c.tv_sec  -= 1;
			c.tv_usec += MICROSECONDS;
		}
	}
	*x = c;

	return r;
}

/* compare a <--> b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
int
timeval_cmp_fast(
	const struct timeval *a,
	const struct timeval *b
	)
{
	int r;

	r = (a->tv_sec > b->tv_sec) - (a->tv_sec < b->tv_sec);
	if (r == 0)
		r = (a->tv_usec > b->tv_usec) -
		    (a->tv_usec < b->tv_usec);
	
	return r;
}

int
timeval_cmp(
	const struct timeval *a,
	const struct timeval *b
	)
{
	int	       r;
	struct timeval A;
	struct timeval B;

	COPYNORM(&A, a);
	COPYNORM(&B, b);
	r = (A.tv_sec > B.tv_sec) - (A.tv_sec < B.tv_sec);
	if (r == 0)
		r = (A.tv_usec > B.tv_usec) -
		    (A.tv_usec < B.tv_usec);
	
	return r;
}

/*
 * test previously-normalised a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
int
timeval_test_fast(
	const struct timeval *a
	)
{
	int r;

	r = (a->tv_sec > 0) - (a->tv_sec < 0);
	if (r == 0)
		r = (a->tv_usec > 0);
	
	return r;
}

/*
 * test possibly denormal a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
int
timeval_test(
	const struct timeval *a
	)
{
	struct timeval A;
	int	       r;

	COPYNORM(&A, a);
	r = (A.tv_sec > 0) - (A.tv_sec < 0);
	if (r == 0)
		r = (A.tv_usec > 0);
	
	return r;
}

/* return LIB buffer ptr to string rep */
const char*
timeval_tostr(
	const struct timeval *x
	)
{
	/* see timespecops.c for rationale -- this needs refactoring
	 *
	 * Even with 64 bit time_t, 32 chars will suffice. Hopefully,
	 * LIB_BUFLENGTH is big enough; the current definiton checks
	 * this by the preprocessor just at the top of this file. */
	static const char *fmt = "-%" TIMEFMT "u.%06lu";
	
	struct timeval v;
	char *		cp;
	int	       notneg;
	u_time	       itmp;
	u_long	       ftmp;
	
	/* normalise and get absolute value into unsigned values. Since
	 * the negation of TIME_T_MIN (if it existed) is implementation
	 * defined, we try to avoid it. */
	COPYNORM(&v, x);
	notneg = v.tv_sec >= 0;
	if (notneg != 0) {
		itmp = (u_time)v.tv_sec;
		ftmp = (u_long)v.tv_usec;
	} else if (v.tv_usec != 0) {
		itmp = (u_time)-(v.tv_sec + 1);
		ftmp = (u_long)(MICROSECONDS - v.tv_usec);
	} else {
		itmp = ((u_time) -(v.tv_sec + 1)) + 1;
		ftmp = 0;
	}

	/* get buffer and format data */
	LIB_GETBUF(cp);
	snprintf(cp, LIB_BUFLENGTH, fmt + notneg, itmp, ftmp);
	
	return cp;
}

void
timeval_abstolfp(
	l_fp *			y,
	const struct timeval *	x
	)
{
	struct timeval v;

	COPYNORM(&v, x);
	MYTVUTOF(v.tv_usec, y->l_uf);
	y->l_ui = (u_int32)v.tv_sec + JAN_1970;
}

void
timeval_reltolfp(
	l_fp *			y,
	const struct timeval *	x
	)
{
	struct timeval v;
	
	COPYNORM(&v, x);
	MYTVUTOF(v.tv_usec, y->l_uf);
	y->l_i = (int32)v.tv_sec;
}

void
timeval_relfromlfp(
	struct timeval *y,
	const l_fp *	x
	)
{
	struct timeval	out;
	l_fp		tmp;
	int		neg;
	
	tmp = *x;
	neg = L_ISNEG(&tmp);
	if (neg != 0)
		L_NEG(&tmp);	
	MYFTOTVU(x->l_uf, out.tv_usec);
	out.tv_sec = x->l_ui;
	if (neg != 0) {
		out.tv_sec = -out.tv_sec;
		out.tv_usec = -out.tv_usec;
	}
	COPYNORM(y, &out);
}

void
timeval_urelfromlfp(
	struct timeval *y,
	const l_fp *	x
	)
{
	struct timeval out;
	
	MYFTOTVU(x->l_uf, out.tv_usec);
	out.tv_sec = x->l_ui;
	COPYNORM(y, &out);
}

void
timeval_absfromlfp(
	struct timeval *y,
	const l_fp *	x,
	const time_t *	p
	)
{
	struct timeval	out;
	vint64		sec;

	sec = ntpcal_ntp_to_time(x->l_ui, p);
	MYFTOTVU(x->l_uf, out.tv_usec);

	/* copying a vint64 to a time_t needs some care... */
#if SIZEOF_TIME_T == 4
	out.tv_sec = (time_t)sec.d_s.lo;
#elif defined(HAVE_INT64)
	out.tv_sec = (time_t)sec.q_s;
#else
	out.tv_sec = ((time_t)sec.d_s.hi << 32) + sec.d_s.lo;
#endif
	
	COPYNORM(y, &out);
}

/* -*- EOF -*- */
