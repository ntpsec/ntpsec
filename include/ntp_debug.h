/*
 * Created: Sat Aug 20 14:23:01 2005
 *
 * Copyright (C) 2005 by Frank Kardel
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-clause
 */
#ifndef GUARD_NTP_DEBUG_H
#define GUARD_NTP_DEBUG_H

/*
 * macro for debugging output - cut down on #ifdef pollution.
 *
 * DPRINT() is the new one debug logger to rule them all.
 * Uses mprintf() and so supports %m, replaced by strerror(errno).
 *
 * The calling convention is not attractive:
 *     DPRINT(debuglevel, (fmt, ...));
 *     DPRINT(2, ("this will appear on stdout if debug >= %d\n", 2));
 *
 * TPRINT is used where the logger needs to remain even when DEBUG is off.
 */
#ifdef DEBUG
#define DPRINT(lvl, arg)					\
	do { 						\
		if (debug >= (lvl))			\
			mprintf arg;			\
	} while (0)
#else
#define DPRINT(lvl, arg)	do {} while (0)
#endif  /* DEBUG */

#define TPRINT(lvl, arg)				\
	do { 						\
		if (debug >= (lvl))			\
			mprintf arg;			\
	} while (0)

#endif	/* GUARD_NTP_DEBUG_H */
