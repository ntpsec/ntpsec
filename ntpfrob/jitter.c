/*
 * This program can be used to calibrate the clock reading jitter of a
 * particular CPU and operating system. It first tickles every element
 * of an array, in order to force pages into memory, then repeatedly
 * reads the system clock and, finally, writes out the time values for
 * later analysis. From this you can determine the jitter and if the
 * clock ever runs backwards.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ntpfrob.h"

#include "ntp_fp.h"
#include "ntp_calendar.h"
#include "timespecops.h"

#define NBUF	800002
#define NSAMPLES 10

static double sys_residual;
static double average;

/*
 * get_clocktime - return system time in NTP timestamp format.
 */
void
get_clocktime(
	l_fp *now		/* system time */
	)
{
	double dtemp;

	struct timespec ts;	/* seconds and nanoseconds */

	/*
	 * Convert Unix clock from seconds and nanoseconds to seconds.
	 */
	clock_gettime(CLOCK_REALTIME, &ts);
	setlfpsint(*now, ts.tv_sec + JAN_1970);
	dtemp = ts.tv_nsec * S_PER_NS;

	/*
	 * Renormalize to seconds past 1900 and fraction.
	 */
	dtemp += sys_residual;
	if (dtemp >= 1) {
		dtemp -= 1;
		bumplfpsint(*now, 1);
	} else if (dtemp < -1) {
		dtemp += 1;
		bumplfpsint(*now, -1);
	}
	dtemp *= FRAC;
	setlfpfrac(*now, (uint32_t)dtemp);
}

static int doublecmp(const void *a, const void *b)
{
    return (int)(*((double *)a) - *((double *)b));
}

void jitter(const iomode mode)
{
	l_fp tr;
	int i;
	double gtod[NBUF];

	/*
	 * Force pages into memory
	 */
	for (i = 0; i < NBUF; i ++)
	    gtod[i] = 0;

	/*
	 * Construct gtod array
	 */
	for (i = 0; i < NBUF; i ++) {
		get_clocktime(&tr);
		gtod[i] = lfptod(tr);
	}

	/*
	 * Write out gtod array for later processing with Matlab
	 */
	average = 0;
	for (i = 0; i < NBUF - 2; i++) {
		gtod[i] = gtod[i + 1] - gtod[i];
		if (mode == raw)
			printf("%13.9f\n", gtod[i]);
		average += gtod[i];
	}

	if (mode == raw)
	    exit(0);
	
	/*
	 * Sort the gtod array and display deciles
	 */
	qsort(gtod, NBUF, sizeof(gtod[0]), doublecmp);
	average = average / (NBUF - 2);
	if (mode == json) {
		fprintf(stdout, "{\"Average\":%13.9f,", average);
		fprintf(stdout, "\"First rank\":[");
		for (i = 0; i < NSAMPLES; i++) {
		    fprintf(stdout, "%13.9f", gtod[i]);
		    if (i < NSAMPLES - 1)
			fputc(',', stdout);
		    fputs("],", stdout);
		}
		fprintf(stdout, "\"Last rank\":");
		for (i = NBUF - 12; i < NBUF - 2; i++) {
		    fprintf(stdout, "%13.9f\n", gtod[i]);
		    if (i < NSAMPLES - 1)
			fputc(',', stdout);
		    fputs("]}\n", stdout);
		}
	}
	else if (mode != raw)
	{
		fprintf(stdout, "Average %13.9f\n", average);
		fprintf(stdout, "First rank\n");
		for (i = 0; i < NSAMPLES; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
		fprintf(stdout, "Last rank\n");
		for (i = NBUF - 12; i < NBUF - 2; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
	}

	exit(0);
}

/* end */
