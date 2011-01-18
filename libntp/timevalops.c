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

#undef MICROSECONDS
#define MICROSECONDS 1000000

#if SIZEOF_LONG >= 8
# define MYFTOTVU(tsf,tvu) \
	(tvu) = (int32)(((u_long)(tsf) * MICROSECONDS + 0x80000000) >> 32)
# define MYTVUTOF(tvu,tsf) \
	(tsf) = (u_int32)((((u_long)(tvu)<<32)+MICROSECONDS/2) / MICROSECONDS)
#else
# define MYFTOTVU(tsf,tvu) TSFTOTVU(tsf, tvu)
# define MYTVUTOF(tvu,tsf) TVUTOTSF(tvu, tsf)
#endif

#define COPYNORM(dst,src) \
  do {	  *(dst) = *(src);		\
	  if (timeval_isdenormal((dst)))	\
		  timeval_norm((dst));	\
  } while (0)
	

void
timeval_norm(
	struct timeval *x
	)
{
	/* If the fraction becomes excessive denormal, we use division
	 * to do first partial normalisation. The normalisation loops
	 * following will do the remaining cleanup.
	 */
	if (abs(x->tv_usec) >= 4*MICROSECONDS) {
		long z;
		z = x->tv_usec / MICROSECONDS;
		x->tv_usec -= z * MICROSECONDS;
		x->tv_sec  += z;
	}

	/* since 10**9 is close to 2**32, we don't divide but do a
	 * normalisation in a loop; this takes 3 steps max, and should
	 * outperform a division even if the mul-by-inverse trick is
	 * employed. */
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

/* x = a, normlised */
void
timeval_copy(
	      struct timeval *x,
	const struct timeval *a
	)
{
	COPYNORM(x,a);
}

/* x = a + b */
void
timeval_add(
	      struct timeval *x,
	const struct timeval *a,
	const struct timeval *b
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
	      struct timeval *x,
	const struct timeval *a,
	long		       b
	)
{	
	struct timeval c;

	c.tv_sec  = a->tv_sec;
	c.tv_usec = a->tv_usec + b;
	COPYNORM(x, &c);
}

/* x = a + b */
void
timeval_sub(
	      struct timeval *x,
	const struct timeval *a,
	const struct timeval *b
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
	      struct timeval *x,
	const struct timeval *a,
	long		       b
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
	      struct timeval *x,
	const struct timeval *a
	)
{	
	struct timeval c;

	c.tv_sec  = - a->tv_sec;
	c.tv_usec = - a->tv_usec;
	COPYNORM(x, &c);
}

/* x = ( a < 0) ? -a : a
 * return if negation was needed
 */
int
timeval_abs(
	      struct timeval *x,
	const struct timeval *a
	)
{	
	struct timeval c;
	int		r;

	COPYNORM(&c, a);
	if ((r = (c.tv_sec < 0)) != 0) {
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

	r = (a->tv_sec > b->tv_sec)
	  - (a->tv_sec < b->tv_sec);
	if (r == 0)
		r = (a->tv_usec > b->tv_usec)
		  - (a->tv_usec < b->tv_usec);
	
	return r;
}

int
timeval_cmp(
	const struct timeval *a,
	const struct timeval *b
	)
{
	struct timeval A;
	struct timeval B;

	COPYNORM(&A, a);
	COPYNORM(&B, b);
	return timeval_cmp_fast(&A, &B);
}

/* test a
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
	struct timeval v;
	int	       s;
	char	      *cp;

	LIB_GETBUF(cp);
	s = timeval_abs(&v, x);
	if (v.tv_sec >= 0)
		snprintf(cp, LIB_BUFLENGTH, "%s%ld.%06ld",
			 "-"+(s==0), (long)v.tv_sec, (long)v.tv_usec);
	else if (v.tv_usec == 0)
		snprintf(cp, LIB_BUFLENGTH, "%ld.000000",
			 (long)v.tv_sec);
	else
		cp = "#OVERFLOW#";

	return cp;
}

void
timeval_abstolfp(
	l_fp		     *y,
	const struct timeval *x
	)
{
	struct timeval v;

	COPYNORM(&v, x);
	MYTVUTOF(v.tv_usec, y->l_uf);
	y->l_ui = (u_int32)v.tv_sec + JAN_1970;
}

void
timeval_reltolfp(
	l_fp		     *y,
	const struct timeval *x
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
	const l_fp     *x)
{
	struct timeval out;

	MYFTOTVU(x->l_uf, out.tv_usec);
	out.tv_sec = x->l_i;
	COPYNORM(y, &out);
}

void
timeval_urelfromlfp(
	struct timeval *y,
	const l_fp     *x)
{
	struct timeval out;
	
	MYFTOTVU(x->l_uf, out.tv_usec);
	out.tv_sec = x->l_ui;
	COPYNORM(y, &out);
}

void
timeval_absfromlfp(
	struct timeval *y,
	const l_fp     *x,
	const time_t   *p
	)
{
	struct timeval out;
	vint64	       sec;

	sec = ntpcal_ntp_to_time(x->l_ui, p);
	MYFTOTVU(x->l_uf, out.tv_usec);

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
