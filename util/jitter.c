/*
 * This program can be used to calibrate the clock reading jitter of a
 * particular CPU and operating system. It first tickles every element
 * of an array, in order to force pages into memory, then repeatedly calls
 * gettimeofday() and, finally, writes out the time values for later
 * analysis. From this you can determine the jitter and if the clock ever
 * runs backwards.
 */

#include <stdio.h>
#include <sys/time.h>
#include "jitter.h"

#define NBUF 20002

int debug;
char progname[10];

int
main(
	int argc,
	char *argv[]
	)
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
		get_systime(&tr);
		LFPTOD(&tr, gtod[i]);
	}

	/*
	 * Write out gtod array for later processing with Matlab
	 */
	for (i = 0; i < NBUF - 2; i++) {
		gtod[i] = gtod[i + 1] - gtod[i];
		printf("%13.9lf\n", gtod[i]);
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
	fprintf(stderr, "First rank\n");
	for (i = 0; i < 10; i++)
		fprintf(stderr, "%2d %13.9lf\n", i, gtod[i]);
	fprintf(stderr, "Last rank\n");
	for (i = NBUF - 12; i < NBUF - 2; i++)
		fprintf(stderr, "%2d %13.9lf\n", i, gtod[i]);
	exit(0);
}
