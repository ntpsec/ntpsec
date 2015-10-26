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

#include "ntpfrob.h"

#ifdef HAVE_ADJTIMEX
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>

static struct timex txc;
#endif /* HAVE_ADJTIMEX */

void tickadj(const iomode mode, const int newtick)
{
#ifndef HAVE_ADJTIMEX
	fputs("ntpfrob: \n", stderr);
	exit(1);
#else
	if (newtick != 0)
	{
#ifdef STRUCT_TIMEX_HAS_TIME_TICK
		if ( (txc.time_tick = newtick) < 1 )
#else
		if ( (txc.tick = newtick) < 1 )
#endif /* STRUCT_TIMEX_HAS_TIME_TICK */
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
#endif /* STRUCT_TIMEX_HAS_MODES */
#endif /* ADJ_TIMETICK */
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
#endif /* STRUCT_TIMEX_HAS_MODES */
#endif /* ADJ_TIMETICK */
	}

	if (adjtimex(&txc) < 0)
	{
		perror("adjtimex");
	}
	else
	{
#ifdef STRUCT_TIMEX_HAS_TIME_TICK
		if (json)
			printf("{\"tick\":%ld,\"tick_adj\":%ld}\n",
			       txc.time_tick, txc.tickadj);
		else
			printf("tick = %ld\ntick_adj = %ld\n",
			       txc.time_tick, txc.tickadj);
#else
		if (json)
			printf("{\"tick\":%ld}\n", txc.tick);
		else
			printf("tick = %ld\n", txc.tick);
#endif /* STRUCT_TIMEX_HAS_TIME_TICK */
	}

#endif /* HAVE_ADJTIMEX */
}

/* end */
