/*
 * Copyright Frank Kardel
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GUARD_NTP_DEBUG_H
#define GUARD_NTP_DEBUG_H

/*
 * macro for debugging output - cut down on #ifdef pollution.
 *
 * DPRINT() is the new one debug logger to rule them all.
 *
 * The calling convention is not attractive:
 *     DPRINT(debuglevel, (fmt, ...));
 *     DPRINT(2, ("this will appear on stdout if debug >= %d\n", 2));
 */
#ifdef DEBUG
extern int debug;
#define DPRINT(lvl, arg)					\
	do { 						\
		if (debug >= (lvl))			\
			printf arg;			\
	} while (0)
#else
#define DPRINT(lvl, arg)	do {} while (0)
#endif  /* DEBUG */

#endif	/* GUARD_NTP_DEBUG_H */
