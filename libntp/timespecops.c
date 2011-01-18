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

#if SIZEOF_LONG >= 8
# define MYFTOTVN(tsf,tvu) \
	(tvu) = (int32)(((u_long)(tsf) * NANOSECONDS + 0x80000000) >> 32)
# define MYTVNTOF(tvu,tsf) \
	(tsf) = (u_int32)((((u_long)(tvu)<<32)+NANOSECONDS/2) / NANOSECONDS)
#else
# define MYFTOTVN(tsf,tvu) \
	(tvu) = (int32)floor((tsf) / 4.294967296 + 0.5)
# define MYTVNTOF(tvu,tsf) \
	(tsf) = (u_int32)floor((tvu) * 4.294967296 + 0.5)
#endif

#define COPYNORM(dst,src) \
  do {	  *(dst) = *(src);		\
	  if (timespec_isdenormal((dst)))	\
		  timespec_norm((dst));	\
  } while (0)
	

void
timespec_norm(
	struct timespec *x
	)
{
#if SIZEOF_LONG > 4
	/* tv_nsec is of type 'long', and on a 64 bit machine
	 * normalisation by loop becomes prohibitive, once the upper 32
	 * bits become involved. On the other hand, division by constant
	 * should be fast enough; so we do a division of the nanoseconds
	 * if the high dword becomes involved. The floor adjustment step
	 * follows with the standard normalisation loops.  Also note
	 * that 'abs' returns int, which might not be good here. */
	long z;
	z = x->tv_nsec;
	if (((z < 0 ? -z : z) >> 32) > 0) {
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

/* x = a, normlised */
void
timespec_copy(
	      struct timespec *x,
	const struct timespec *a
	)
{
	COPYNORM(x,a);
}

/* x = a + b */
void
timespec_add(
	      struct timespec *x,
	const struct timespec *a,
	const struct timespec *b
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
	      struct timespec *x,
	const struct timespec *a,
	long		       b
	)
{	
	struct timespec c;

	c.tv_sec  = a->tv_sec;
	c.tv_nsec = a->tv_nsec + b;
	COPYNORM(x, &c);
}

/* x = a + b */
void
timespec_sub(
	      struct timespec *x,
	const struct timespec *a,
	const struct timespec *b
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
	      struct timespec *x,
	const struct timespec *a,
	long		       b
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
	      struct timespec *x,
	const struct timespec *a
	)
{	
	struct timespec c;

	c.tv_sec  = - a->tv_sec;
	c.tv_nsec = - a->tv_nsec;
	COPYNORM(x, &c);
}

/* x = ( a < 0) ? -a : a
 * return if negation was needed
 */
int
timespec_abs(
	      struct timespec *x,
	const struct timespec *a
	)
{	
	struct timespec c;
	int		r;

	COPYNORM(&c, a);
	if ((r = (c.tv_sec < 0)) != 0) {
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

/* compare a <--> b
 * return 1 / 0 / -1 if	 a < / == / > b
 */
int
timespec_cmp_fast(
	const struct timespec *a,
	const struct timespec *b
	)
{
	int r;

	r = (a->tv_sec > b->tv_sec)
	  - (a->tv_sec < b->tv_sec);
	if (r == 0)
		r = (a->tv_nsec > b->tv_nsec)
		  - (a->tv_nsec < b->tv_nsec);
	
	return r;
}

int
timespec_cmp(
	const struct timespec *a,
	const struct timespec *b
	)
{
	int		r;
	struct timespec A;
	struct timespec B;

	COPYNORM(&A, a);
	COPYNORM(&B, b);
	r = (A.tv_sec > B.tv_sec)
	  - (A.tv_sec < B.tv_sec);
	if (r == 0)
		r = (A.tv_nsec > B.tv_nsec)
		  - (A.tv_nsec < B.tv_nsec);
	
	return r;
}

/* test a
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
	 * some interesting portability problems:
	 *
	 * 1.) The size of 'time_t' is not defined: 32, 64, or arbitray
	 * bits (though 32 and 64 are most common, of course)
	 *
	 * 2.) It's not easy to find an unsigned type with the same width of
	 * time_t in a portable way. (int / long / long long / __int64
	 * /...)
	 *
	 * 3.) Possible 'printf()' format specs are also difficult to find.
	 *
	 * Therefore, this implementation chooses to do the conversion
	 * manually, doing divisions on a possible negative time_t value
	 * even after a normalisation step. (-TIME_T_MIN == TIME_T_MIN
	 * on many systems, though there is no actual definition for
	 * TIME_T_MIN, but I guess you get my meaning...)
	 *
	 * And since we have to do the integer part manually, we do the
	 * fractional part in the same way, though after the
	 * normalisation we can be sure that at least the fraction is in
	 * range.
	 *
	 * Even with 64 bit time_t, 32 chars will suffice. Hopefully,
	 * LIB_BUFLENGTH is big enough; the current definiton does this
	 * check by the preprocessor just at the top of this file.
	 */
	struct timespec v;
	int		s;
	int		digits;
	int		dig;
	char	       *cp;
	time_t		itmp;
	long		ftmp;

	/* normalise to positive value and get sign flag */
	s = timespec_abs(&v, x);

	/* get buffer, position to end and insert terminating NUL */
	LIB_GETBUF(cp);
	cp += LIB_BUFLENGTH - 1;
	*cp = '\0';

	/* convert fraction to decimal digits */
	for (digits = 9; digits;  digits--) {
		ftmp = v.tv_nsec / 10;		
		dig  = (int)(v.tv_nsec - ftmp * 10);
		v.tv_nsec = ftmp;
		*--cp = '0' + dig;
	}
	*--cp = '.';
	
	/* convert integer part to decimal digits
	 *
	 * -*- convert at least one digit, make sure remaining value
	 * is not negative. This assumes that integer division
	 * truncates towards zero, as required by the standard. */
	itmp = v.tv_sec / 10;		
	dig  = (int)(v.tv_sec - itmp * 10);
	v.tv_sec = (itmp < 0) ? -itmp : itmp;
	*--cp = '0' + ((dig < 0) ? -dig : dig);
	/* -*- convert remaining digits */
	while (v.tv_sec != 0) {
		itmp = v.tv_sec / 10;		
		dig  = (int)(v.tv_sec - itmp * 10);
		v.tv_sec = itmp;
		*--cp = '0' + dig;
	}
	/* add minus sign for negative integer part */
	if (s)
		*--cp = '-';

	return cp;
}

void
timespec_abstolfp(
	l_fp		      *y,
	const struct timespec *x
	)
{
	struct timespec v;

	COPYNORM(&v, x);
	MYTVNTOF(v.tv_nsec, y->l_uf);
	y->l_ui = (u_int32)v.tv_sec + JAN_1970;
}

void
timespec_reltolfp(
	l_fp		      *y,
	const struct timespec *x
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
	const l_fp	*x)
{
	struct timespec out;
	l_fp		tmp;
	int		neg;
	
	tmp = *x;
	if ((neg = L_ISNEG(&tmp)) != 0)
		L_NEG(&tmp);	
	MYFTOTVN(x->l_uf, out.tv_nsec);
	out.tv_sec = x->l_ui;
	if (neg) {
		out.tv_sec  = - out.tv_sec;
		out.tv_nsec = - out.tv_nsec;
	}
	COPYNORM(y, &out);
}

void
timespec_urelfromlfp(
	struct timespec *y,
	const l_fp	*x)
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
#   if SIZEOF_TIME_T <= 4
	out.tv_sec = (time_t)sec.d_s.lo;
#   elif defined(HAVE_INT64)
	out.tv_sec = (time_t)sec.q_s;
#   else
	out.tv_sec = ((time_t)sec.d_s.hi << 32) + sec.d_s.lo;
#   endif
	
	COPYNORM(y, &out);
}
/* -*- EOF -*- */
