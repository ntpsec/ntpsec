/*
 * timetoa.c -- time_t related string formatting
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 *
 * Printing a 'time_t' has a lot of portability pitfalls, due to it's
 * opaque base type. The only requirement imposed by the standard is
 * that it must be a numeric type. For all practical purposes it's a
 * signed int, and 32 bits are common.
 *
 * Since the UN*X time epoch will cause a signed integer overflow for
 * 32-bit signed int in the year 2038, implementations slowly move to
 * 64bit base types for time_t, even in 32-bit environments.
 *
 * As the printf() family has no standardised type specifier for time_t,
 * guessing the right output format specifier is a bit troublesome and
 * best done with the help of the preprocessor and "config.h".
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */

#include "config.h"

#include <math.h>
#include <stdio.h>

#include "timetoa.h"
#include "ntp_assert.h"
#include "lib_strbuf.h"

/*
 * Given the size of time_t, guess what can be used as an unsigned value
 * to hold a time_t and the printf() format specifcation.
 *
 * These should be used with the string constant concatenation feature
 * of the compiler like this:
 *
 * printf("a time stamp: %" TIME_FORMAT " and more\n", a_time_t_value);
 *
 * It's not exactly nice, but there's not much leeway once we want to
 * use the printf() family on time_t values.
 */

#if SIZEOF_TIME_T <= SIZEOF_INT

typedef unsigned int u_time;
#define TIME_FORMAT "d"
#define UTIME_FORMAT "u"

#elif SIZEOF_TIME_T <= SIZEOF_LONG

typedef unsigned long u_time;
#define TIME_FORMAT "ld"
#define UTIME_FORMAT "lu"

#elif defined(SIZEOF_LONG_LONG) && SIZEOF_TIME_T <= SIZEOF_LONG_LONG

typedef unsigned long long u_time;
#define TIME_FORMAT "lld"
#define UTIME_FORMAT "llu"

#else
#include "GRONK: what size has a time_t here?"
#endif

/*
 * Formatting to string needs at max 40 bytes (even with 64 bit time_t),
 * so we check LIB_BUFLENGTH is big enough for our purpose.
 */
#if LIB_BUFLENGTH < 40
# include "GRONK: LIB_BUFLENGTH is not sufficient"
#endif

/*
 * general fractional timestamp formatting
 *
 * Many pieces of ntpd require a machine with two's complement
 * representation of signed integers, so we don't go through the whole
 * rigamarole of creating fully portable code here. But we have to stay
 * away from signed integer overflow, as this might cause trouble even
 * with two's complement representation.
 */
const char *
format_time_fraction(
	time_t	secs,
	long	frac,
	int	prec
	)
{
	char *		cp;
	u_int		prec_u;
	u_time		secs_u;
	u_int		u;
	long		fraclimit;
	int		notneg;	/* flag for non-negative value	*/
	ldiv_t		qr;

	DEBUG_REQUIRE(prec != 0);

	LIB_GETBUF(cp);
	secs_u = (u_time)secs;
	
	/* check if we need signed or unsigned mode */
	notneg = (prec < 0);
	prec_u = abs(prec);
	/* fraclimit = (long)pow(10, prec_u); */
	for (fraclimit = 10, u = 1; u < prec_u; u++) {
		DEBUG_INSIST(fraclimit < fraclimit * 10);
		fraclimit *= 10;
	}

	/*
	 * Since conversion to string uses lots of divisions anyway,
	 * there's no big extra penalty for normalisation. We do it for
	 * consistency.
	 */
	if (frac < 0 || frac >= fraclimit) {
		qr = ldiv(frac, fraclimit);
		if (qr.rem < 0) {
			qr.quot--;
			qr.rem += fraclimit;
		}
		secs_u += (time_t)qr.quot;
		frac = qr.rem;
	}

	/* Get the absolute value of the split representation time. */
	notneg = notneg || ((time_t)secs_u >= 0);
	if (!notneg) {
		secs_u = ~secs_u;
		if (0 == frac)
			secs_u++;
		else
			frac = fraclimit - frac;
	}

	/* finally format the data and return the result */
	snprintf(cp, LIB_BUFLENGTH, "%s%" UTIME_FORMAT ".%0*ld",
	    notneg? "" : "-", secs_u, prec_u, frac);
	
	return cp;
}
