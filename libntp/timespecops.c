/*
 * timespecops.c -- calculations on 'struct timespec' values
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 */

#include <config.h>
#include <math.h>

#include "lib_strbuf.h"
#include "ntp_calendar.h"

#include "timespecops.h"

/* formatting to string needs at max 31 bytes (even with 64 bit time_t),
 * so we check LIB_BUFLENGTH is big enough for our pupose.
 */
#if LIB_BUFLENGTH < 32
#error LIB_BUFLENGTH not big enough
#endif

/* make sure we have the right definition for NANOSECONDS */
#undef NANOSECONDS
#define NANOSECONDS 1000000000

/* conversion between l_fp fractions and nanoseconds */
#if SIZEOF_LONG >= 8
# define MYFTOTVN(tsf,tvu)						\
	((tvu) = (int32)						\
		(((u_long)(tsf) * NANOSECONDS + 0x80000000) >> 32))
# define MYTVNTOF(tvu, tsf)						\
	((tsf) = (u_int32)						\
		((((u_long)(tvu) << 32) + NANOSECONDS / 2) /		\
		 NANOSECONDS))
#else
# define MYFTOTVN(tsf, tvu)						\
	((tvu) = (int32)floor((tsf) / 4.294967296 + 0.5))
# define MYTVNTOF(tvu, tsf)						\
	((tsf) = (u_int32)floor((tvu) * 4.294967296 + 0.5))
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
#define COPYNORM(dst, src)						\
	do {								\
		*(dst) = *(src);					\
		if (timespec_isdenormal(dst))				\
			timespec_norm(dst);				\
	} while (0)


void
timespec_norm(
	struct timespec *x
	)
{
#if SIZEOF_LONG > 4
	/* tv_nsec is of type 'long', and on a 64 bit machine using only
	 * loops becomes prohibitive once the upper 32 bits get
	 * involved. On the other hand, division by constant should be
	 * fast enough; so we do a division of the nanoseconds in that
	 * case. The floor adjustment step follows with the standard
	 * normalisation loops.  And 'labs' is intentinally not used
	 * here: its has implementation defined behaviour when applied
	 * to LONG_MIN. */
	if (x->tv_nsec < -3l * NANOSECONDS ||
	    x->tv_nsec >  3l * NANOSECONDS  ) {
		long z;
		z = x->tv_nsec / NANOSECONDS;
		x->tv_nsec -= z * NANOSECONDS;
		x->tv_sec  += z;
	}
#endif
	/* since 10**9 is close to 2**32, we don't divide but do a
	 * normalisation in a loop; this takes 3 steps max, and should
	 * outperform a division even if the mul-by-inverse trick is
	 * employed. */
	if (x->tv_nsec < 0)
		do {
			x->tv_nsec += NANOSECONDS;
			x->tv_sec  -= 1;
		} while (x->tv_nsec < 0);
	else if (x->tv_nsec >= NANOSECONDS)
		do {
			x->tv_nsec -= NANOSECONDS;
			x->tv_sec  += 1;
		} while (x->tv_nsec >= NANOSECONDS);
}

/* x = a, normalised */
void
timespec_copy(
	struct timespec *	x,
	const struct timespec *	a
	)
{
	COPYNORM(x, a);
}

/* x = a + b */
void
timespec_add(
	struct timespec *	x,
	const struct timespec *	a,
	const struct timespec *	b
	)
{	
	struct timespec c;

	c.tv_sec  = a->tv_sec  + b->tv_sec;
	c.tv_nsec = a->tv_nsec + b->tv_nsec;
	COPYNORM(x, &c);
}

/* x = a + b, b is fraction only */
void
timespec_addns(
	struct timespec *	x,
	const struct timespec *	a,
	long			b
	)
{	
	struct timespec c;

	c.tv_sec  = a->tv_sec;
	c.tv_nsec = a->tv_nsec + b;
	COPYNORM(x, &c);
}

/* x = a - b */
void
timespec_sub(
	struct timespec *	x,
	const struct timespec *	a,
	const struct timespec *	b
	)
{	
	struct timespec c;

	c.tv_sec  = a->tv_sec  - b->tv_sec;
	c.tv_nsec = a->tv_nsec - b->tv_nsec;
	COPYNORM(x, &c);
}

/* x = a - b, b is fraction only */
void
timespec_subns(
	struct timespec *	x,
	const struct timespec *	a,
	long			b
	)
{	
	struct timespec c;

	c.tv_sec  = a->tv_sec;
	c.tv_nsec = a->tv_nsec - b;
	COPYNORM(x, &c);
}

/* x = -a */
void
timespec_neg(
	struct timespec *	x,
	const struct timespec *	a
	)
{	
	struct timespec c;

	c.tv_sec  = - a->tv_sec;
	c.tv_nsec = - a->tv_nsec;
	COPYNORM(x, &c);
}

/*
 * x = abs(a)
 * returns nonzero if negation was needed
 */
int
timespec_abs(
	struct timespec *	x,
	const struct timespec *	a
	)
{	
	struct timespec c;
	int		r;

	COPYNORM(&c, a);
	r = (c.tv_sec < 0);
	if (r != 0) {
		c.tv_sec  = - c.tv_sec;
		c.tv_nsec = - c.tv_nsec;
		if (c.tv_nsec < 0) {
			c.tv_sec  -= 1;
			c.tv_nsec += NANOSECONDS;
		}
	}
	*x = c;

	return r;
}

/*
 * compare previously-normalised a and b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
int
timespec_cmp_fast(
	const struct timespec *a,
	const struct timespec *b
	)
{
	int r;

	r = (a->tv_sec > b->tv_sec) - (a->tv_sec < b->tv_sec);
	if (r == 0)
		r = (a->tv_nsec > b->tv_nsec) -
		    (a->tv_nsec < b->tv_nsec);
	
	return r;
}

/*
 * compare possibly denormal a and b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
int
timespec_cmp(
	const struct timespec *a,
	const struct timespec *b
	)
{
	int		r;
	struct timespec	A;
	struct timespec	B;

	COPYNORM(&A, a);
	COPYNORM(&B, b);
	r = (A.tv_sec > B.tv_sec) - (A.tv_sec < B.tv_sec);
	if (r == 0)
		r = (A.tv_nsec > B.tv_nsec) -
		    (A.tv_nsec < B.tv_nsec);

	return r;
}

/*
 * test previously-normalised a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
int
timespec_test_fast(
	const struct timespec *a
	)
{
	int r;

	r = (a->tv_sec > 0) - (a->tv_sec < 0);
	if (r == 0)
		r = (a->tv_nsec > 0);
	
	return r;
}

/*
 * test possibly denormal a
 * return 1 / 0 / -1 if	 a < / == / > 0
 */
int
timespec_test(
	const struct timespec *a
	)
{
	int		r;
	struct timespec A;

	COPYNORM(&A, a);
	r = (A.tv_sec > 0) - (A.tv_sec < 0);
	if (r == 0)
		r = (A.tv_nsec > 0);

	return r;
}

const char*
timespec_tostr(
	const struct timespec *x
	)
{
	/*
	 * Formatting a 'time_t' to a string of decimal digits poses
	 * some interesting portability problems regarding the size of a
	 * time_t and the handling of signed integer overflow, which is
	 * undefined and must be avoided.
	 *
	 * Even with 64 bit time_t, 32 chars will suffice. Hopefully,
	 * LIB_BUFLENGTH is big enough; the current definiton checks
	 * this by the preprocessor just at the top of this file. */
	static const char *fmt = "-%" TIMEFMT "u.%09lu";
	
	struct timespec v;
	char	       *cp;
	int		notneg;
	u_time		itmp;
	u_long		ftmp;
	
	/* normalise and get absolute value into unsigned values. Since
	 * the negation of TIME_T_MIN (if it existed) is implementation
	 * defined, we try to avoid it. */
	COPYNORM(&v, x);
	notneg = v.tv_sec >= 0;
	if (notneg != 0) {
		itmp = (u_time)v.tv_sec;
		ftmp = (u_long)v.tv_nsec;
	} else if (v.tv_nsec != 0) {
		itmp = (u_time)-(v.tv_sec + 1);
		ftmp = (u_long)(NANOSECONDS - v.tv_nsec);
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
timespec_abstolfp(
	l_fp *			y,
	const struct timespec *	x
	)
{
	struct timespec v;

	COPYNORM(&v, x);
	MYTVNTOF(v.tv_nsec, y->l_uf);
	y->l_ui = (u_int32)v.tv_sec + JAN_1970;
}

void
timespec_reltolfp(
	l_fp *			y,
	const struct timespec *	x
	)
{
	struct timespec v;
	
	COPYNORM(&v, x);
	MYTVNTOF(v.tv_nsec, y->l_uf);
	y->l_i = (int32)v.tv_sec;
}

void
timespec_relfromlfp(
	struct timespec *y,
	const l_fp	*x
	)
{
	struct timespec out;
	l_fp		tmp;
	int		neg;
	
	tmp = *x;
	neg = L_ISNEG(&tmp);
	if (neg != 0)
		L_NEG(&tmp);	
	MYFTOTVN(x->l_uf, out.tv_nsec);
	out.tv_sec = x->l_ui;
	if (neg != 0) {
		out.tv_sec  = -out.tv_sec;
		out.tv_nsec = -out.tv_nsec;
	}
	COPYNORM(y, &out);
}

void
timespec_urelfromlfp(
	struct timespec *y,
	const l_fp	*x
	)
{
	struct timespec out;
	
	MYFTOTVN(x->l_uf, out.tv_nsec);
	out.tv_sec = x->l_ui;
	COPYNORM(y, &out);
}

void
timespec_absfromlfp(
	struct timespec *y,
	const l_fp	*x,
	const time_t	*p
	)
{
	struct timespec out;
	vint64		sec;

	sec = ntpcal_ntp_to_time(x->l_ui, p);
	MYFTOTVN(x->l_uf, out.tv_nsec);

	/* copying a vint64 to a time_t needs some care... */
#if SIZEOF_TIME_T <= 4
	out.tv_sec = (time_t)sec.d_s.lo;
#elif defined(HAVE_INT64)
	out.tv_sec = (time_t)sec.q_s;
#else
	out.tv_sec = ((time_t)sec.d_s.hi << 32) + sec.d_s.lo;
#endif
	
	COPYNORM(y, &out);
}
/* -*- EOF -*- */
