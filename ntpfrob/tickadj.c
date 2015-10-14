/*
 * tickadj - read, and possibly modify, the kernel `tick' and
 *	     `tickadj' variables', using adjtimex(2).  This is
 *	     included only for compatibility with old scripts.
 *	     and its former support for unsafe /dev/kmem operations
 *	     has been removed.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <config.h>

#include "ntp_types.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef HAVE_ADJTIMEX
#error This program requires the adjtimex(2) system call.
#else
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>

static struct timex txc;

void tickadj(const int newtick)
{
	if (newtick != 0)
	{
#ifdef STRUCT_TIMEX_HAS_TIME_TICK
		if ( (txc.time_tick = newtick) < 1 )
#else
		if ( (txc.tick = newtick) < 1 )
#endif
		{
			fprintf(stderr, "ntpfrob: silly value for tick: %d\n", newtick);
			exit(1);
		}
#ifdef ADJ_TIMETICK
		txc.modes = ADJ_TIMETICK;
#else
#ifdef STRUCT_TIMEX_HAS_MODES
		txc.modes = ADJ_TICK;
#else
		txc.mode = ADJ_TICK;
#endif
#endif
	}
	else
	{
#ifdef ADJ_TIMETICK
		txc.modes = 0;
#else
#ifdef STRUCT_TIMEX_HAS_MODES
		txc.modes = 0;
#else
		txc.mode = 0;
#endif
#endif
	}

	if (adjtimex(&txc) < 0)
	{
		perror("adjtimex");
	}
	else
	{
#ifdef STRUCT_TIMEX_HAS_TIME_TICK
		printf("tick     = %ld\ntick_adj = %ld\n", txc.time_tick, txc.tickadj);
#else
		printf("tick = %ld\n", txc.tick);
#endif
	}

}
#endif
