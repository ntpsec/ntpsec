/*
 * timetoa.h -- time_t related string formatting
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 *
 * Printing a 'time_t' has some portability pitfalls, due to it's opaque
 * base type. The only requirement imposed by the standard is that it
 * must be a numeric type. For all practical purposes it's a signed int,
 * and 32 bits are common.
 *
 * Since the UN*X time epoch will cause a signed integer overflow for
 * 32-bit signed int values in the year 2038, implementations slowly
 * move to 64bit base types for time_t, even in 32-bit environments. In
 * such an environment sizeof(time_t) could be bigger than sizeof(long)
 * and the commonly used idiom of casting to long leads to truncation.
 *
 * As the printf() family has no standardised type specifier for time_t,
 * guessing the right output format specifier is a bit troublesome and
 * best done with the help of the preprocessor and "config.h".
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */
#ifndef GUARD_TIMETOA_H
#define GUARD_TIMETOA_H

#include "ntp_fp.h"
#include "ntp_stdlib.h"

/*
 * general fractional time stamp formatting.
 *
 * secs - integral seconds of time stamp
 * frac - fractional units
 * prec - log10 of units per second (3=milliseconds, 6=microseconds,..)
 *	  or in other words: the count of decimal digits required.
 *	  If prec is < 0, abs(prec) is taken for the precision and secs
 *	  is treated as an unsigned value.
 *
 * The function will eventually normalise the fraction and adjust the
 * seconds accordingly.
 *
 * This function uses the string buffer library for the return value,
 * so do not keep the resulting pointers around.
 */
extern const char *
format_time_fraction(time_t secs, long frac, int prec);

#endif /* GUARD_TIMETOA_H */
