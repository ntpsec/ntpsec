/*
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"

#include <stdio.h>
#include <stdbool.h>

#include "ntp_types.h"
#include "ntp_calendar.h"
#include "ntpfrob.h"

#define	DEFAULT_SYS_PRECISION	-99

int default_get_resolution(void);
int default_get_precision(void);

void precision(const iomode mode)
{
	if (mode == json)
		printf("{\"log2 of resolution\":%d, \"log2 of precision\":%d}\n",
		       default_get_resolution(),
		       default_get_precision());
	else
		printf("log2(resolution) = %d, log2(precision) = %d\n",
			default_get_resolution(),
			default_get_precision());
}

/* Find the resolution of the system clock by watching how the current time
 * changes as we read it repeatedly.
 *
 * If a machine has resolution (i.e. accurate timing info) > 1us, then it will
 * probably use the "unused" low order bits as a counter (to force time to be
 * a strictly increaing variable), incrementing it each time any process
 * requests the time [[ or maybe time will stand still ? ]].
 *
 * SO: the logic goes:
 *
 *      IF      the difference from the last time is "small" (< MINSTEP)
 *      THEN    this machine is "counting" with the low order bits
 *      ELIF    this is not the first time round the loop
 *      THEN    this machine *WAS* counting, and has now stepped
 *      ELSE    this machine has resolution < time to read clock
 *
 * SO: if it exits on the first loop, assume "full accuracy" (1us)
 *     otherwise, take the log2(observed difference, rounded UP)
 *
 * MINLOOPS > 1 ensures that even if there is a STEP between the initial call
 * and the first loop, it doesn't stop too early.
 * Making it even greater allows MINSTEP to be reduced, assuming that the
 * chance of MINSTEP-1 other processes getting in and calling clock_gettime()
 * between this processes's calls.
 * Reducing MINSTEP may be necessary as this sets an upper bound for the time
 * to actually call clock_gettime().
 */

#define	DNSECS	1000000000L
#define	HUSECS	(1024 * 1024)
#define	MINSTEP	200	/* assume no system returns less than 200 nansec */
/* Don't use "1" as some *other* process may read too */
/* We assume no system actually *ANSWERS* in this time */
#define MAXSTEP 20000000   /* maximum clock increment (ns) */
#define MINLOOPS 5      /* minimum number of step samples */
#define	MAXLOOPS (HUSECS * 1024)	/* Assume precision < .1s ! */

int
default_get_resolution(void)
{
	struct timespec tp;
	long last;
	int i;
	long diff;
	long val;
	int minsteps = MINLOOPS;	/* need at least this many steps */

	clock_gettime(CLOCK_REALTIME, &tp);
	last = tp.tv_nsec;
	for (i = - --minsteps; i< MAXLOOPS; i++) {
		clock_gettime(CLOCK_REALTIME, &tp);
		diff = tp.tv_nsec - last;
		if (diff < 0) diff += DNSECS;
		if (diff > MINSTEP && minsteps-- <= 0)
			break;
		last = tp.tv_nsec;
	}
	diff /= 1000;	/* step down to milliseconds */

	fprintf(stderr, "resolution = %ld usec after %d loop%s\n",
	       diff, i, (i==1) ? "" : "s");

	diff = (diff *3)/2;
	if (i >= MAXLOOPS) {
	    fprintf(stderr,
		"     (Boy this machine is fast ! %d loops without a step)\n",
		MAXLOOPS);
	    diff = 1; /* No STEP, so FAST machine */
	}
	if (i == 0) {
	    fprintf(stderr,
		"     (The resolution is less than the time to read the clock -- Assume 1us)\n");
	    diff = 1; /* time to read clock >= resolution */
	}
	for (i=0, val=HUSECS; val>0; i--, val >>= 1) if (diff >= val) return i;
	fprintf(stderr,
		"     (Oh dear -- that wasn't expected ! I'll guess !)\n");
	return DEFAULT_SYS_PRECISION /* Something's BUST, so lie ! */;
}

/* ===== Rest of this code lifted straight from xntpd/ntp_proto.c ! ===== */

/*
 * This routine calculates the differences between successive calls to
 * clock_gettime(REALTIME). If a difference is less than zero, the ns field
 * has rolled over to the next second, so we add a second in ns. If
 * the difference is greater than zero and less than MINSTEP, the
 * clock has been advanced by a small amount to avoid standing still.
 * If the clock has advanced by a greater amount, then a timer interrupt
 * has occurred and this amount represents the precision of the clock.
 * In order to guard against spurious values, which could occur if we
 * happen to hit a fat interrupt, we do this for MINLOOPS times and
 * keep the minimum value obtained.
 */
int
default_get_precision(void)
{
	struct timespec tp;
	long last;
	int i;
	long diff;
	long val;
	long nsec;

	nsec = 0;
	val = MAXSTEP;
	clock_gettime(CLOCK_REALTIME, &tp);
	last = tp.tv_nsec;
	for (i = 0; i < MINLOOPS && nsec < HUSECS * 1024;) {
	    clock_gettime(CLOCK_REALTIME, &tp);
		diff = tp.tv_nsec - last;
		last = tp.tv_nsec;
		if (diff < 0)
		    diff += DNSECS;
		nsec += diff;
		if (diff > MINSTEP) {
			i++;
			if (diff < val)
			    val = diff;
		}
	}
	val /= 1000;	/* step down to milliseconds */
	fprintf(stderr, "precision  = %ld usec after %d loop%s\n",
	       val, i, (i == 1) ? "" : "s");
	if (nsec >= HUSECS * 1024) {
	    fprintf(stderr, "     (Boy this machine is fast! nsec was %ld)\n",
		       nsec);
		val = MINSTEP;	/* val <= MINSTEP; fast machine */
	}
	diff = HUSECS;
	for (i = 0; diff > val; i--)
	    diff >>= 1;
	return (i);
}

/* end */
