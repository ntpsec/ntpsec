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

#include "ntpfrob.h"

#include "ntp_fp.h"
#include "ntp_calendar.h"
#include "timespecops.h"

#define NBUF	800002
#define NSAMPLES 10

static doubletime_t sys_residual;
static doubletime_t average;

/*
 * get_clocktime - return system time in NTP timestamp format.
 */
void
get_clocktime(
	l_fp *now		/* system time */
	)
{
	doubletime_t dtemp;

	struct timespec ts;	/* seconds and nanoseconds */

	/*
	 * Convert Unix clock from seconds and nanoseconds to seconds.
	 */
	clock_gettime(CLOCK_REALTIME, &ts);
	*now = lfptouint(ts.tv_sec + (long)JAN_1970);  /* no fraction, yet */
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

// modified from https://stackoverflow.com/questions/11931547/qsort-does-not-work-for-double-array
static int doublecmp(const void * a, const void * b)
{
  if (*(const double*)a > *(const double*)b) return -1;
  else if (*(const double*)a < *(const double*)b) return 1;
  else return 0;
}

void jitter(const iomode mode)
{
	l_fp tr;
	int i;
	double gtod[NBUF];
	doubletime_t jitterx;	/* shadow warning on some old systems */

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
	for (i = 0; i < (NBUF - 1); i++) {
		gtod[i] = gtod[i + 1] - gtod[i];
		if (mode == raw)
			printf("%13.9f\n", gtod[i]);
		average += gtod[i];
	}

	if (mode == raw)
	    exit(0);

	average = average / (NBUF - 1);

	// calculate 'variance' and call it jitter
	// and scale everything up a million time for clarity
	jitterx = 0;
	for (i = 0; i < (NBUF - 1); i ++) {
		gtod[i] = (gtod[i] - average) * 1000000;
		jitterx += gtod[i] * gtod[i];
	}
	jitterx = jitterx / (NBUF - 1);

	/*
	 * Sort the gtod array and display deciles
	 */
	qsort(gtod, (NBUF - 1), sizeof(gtod[0]), doublecmp);

	if (mode == json) {
		fprintf(stdout, "{\"Mean\": %.9Lf, \"High\": [", average);
		for (i = 0; i < NSAMPLES; i++) {
		    fprintf(stdout, "%.9f", gtod[i]);
		    if (i < NSAMPLES - 1)
			fputs(", ", stdout);
		}
		fputs("], \"Low\": [", stdout);
		for (i = NBUF - NSAMPLES - 2; i < NBUF - 2; i++) {
		    fprintf(stdout, "%.9f", gtod[i]);
		    if (i < NBUF - 3)
			fputs(", ", stdout);
		}
		fprintf(stdout, "], \"Jitter\": %.9Lf}\n", jitterx);
	}
	else if (mode != raw)
	{
		fprintf(stdout, "Mean %13.9Lf\n", average);
		fprintf(stdout, "Jitter %13.9Lf\n", jitterx);
		fprintf(stdout, "High\n");
		for (i = 0; i < NSAMPLES; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
		fprintf(stdout, "Low\n");
		for (i = NBUF - NSAMPLES - 2; i < NBUF - 2; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
	}

	exit(0);
        /* never returns */
}

/* end */
