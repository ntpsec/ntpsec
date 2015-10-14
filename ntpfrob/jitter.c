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

#include <config.h>

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ntp_fp.h"

#define NBUF	800002
#define JAN_1970 2208988800UL		/* Unix base epoch */
#define NSAMPLES 10

char progname[10];
double sys_residual;
double average;

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
	now->l_i = ts.tv_sec + JAN_1970;
	dtemp = ts.tv_nsec / 1e9;

	/*
	 * Renormalize to seconds past 1900 and fraction.
	 */
	dtemp += sys_residual;
	if (dtemp >= 1) {
		dtemp -= 1;
		now->l_i++;
	} else if (dtemp < -1) {
		dtemp += 1;
		now->l_i--;
	}
	dtemp *= FRAC;
	now->l_uf = (uint32_t)dtemp;
}

void jitter(const bool json)
{
	l_fp tr;
	int i, j;
	double dtemp, gtod[NBUF];

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
		LFPTOD(&tr, gtod[i]);
	}

	/*
	 * Write out gtod array for later processing with Matlab
	 */
	average = 0;
	for (i = 0; i < NBUF - 2; i++) {
		gtod[i] = gtod[i + 1] - gtod[i];
		printf("%13.9f\n", gtod[i]);
		average += gtod[i];
	}

	/*
	 * Sort the gtod array and display deciles
	 */
	for (i = 0; i < NBUF - 2; i++) {
		for (j = 0; j <= i; j++) {
			if (gtod[j] > gtod[i]) {
				dtemp = gtod[j];
				gtod[j] = gtod[i];
				gtod[i] = dtemp;
			}
		}
	}
	average = average / (NBUF - 2);
	if (json) {
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
	else
	{
		fprintf(stdout, "Average %13.9f\n", average);
		fprintf(stdout, "First rank\n");
		for (i = 0; i < NSAMPLES; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
		fprintf(stdout, "Last rank\n");
		for (i = NBUF - 12; i < NBUF - 2; i++)
		    fprintf(stdout, "%2d %13.9f\n", i, gtod[i]);
	}
}

/* end */
