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
	 * follows with the standard normalisation loops. */
	long z;

	z = (x->tv_nsec < 0) ? -x->tv_nsec : x->tv_nsec;
	if (z >> 32) {
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
	struct timespec A;
	struct timespec B;

	COPYNORM(&A, a);
	COPYNORM(&B, b);
	return timespec_cmp_fast(&A, &B);
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
	struct timespec A;

	COPYNORM(&A, a);
	return timespec_test_fast(&A);
}

const char*
timespec_tostr(
	const struct timespec *x
	)
{
	struct timespec v;
	int		s;
	char	       *cp;

	LIB_GETBUF(cp);
	s = timespec_abs(&v, x);
	if (v.tv_sec >= 0)
		snprintf(cp, LIB_BUFLENGTH, "%s%ld.%09ld",
			 "-"+(s==0), (long)v.tv_sec, (long)v.tv_nsec);
	else if (v.tv_nsec == 0)
		snprintf(cp, LIB_BUFLENGTH, "%ld.000000000",
			 (long)v.tv_sec);
	else
		cp = "#OVERFLOW#";

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

	MYFTOTVN(x->l_uf, out.tv_nsec);
	out.tv_sec = x->l_i;
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
#   ifdef HAVE_INT64
	out.tv_sec = (time_t)sec.q_s;
#   elif SIZEOF_TIME_T > 4
	out.tv_sec = ((time_t)sec.d_s.hi << 32) + sec.d_s.lo;
#   else
	out.tv_sec = (time_t)sec.d_s.lo;
#   endif
	
	COPYNORM(y, &out);
}
/* -*- EOF -*- */
