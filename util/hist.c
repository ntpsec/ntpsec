/*
 * This program can be used to calibrate the clock reading jitter of a
 * particular CPU and operating system. It first tickles every element
 * of an array, in order to force pages into memory, then repeatedly calls
 * clock_gettime() and, finally, writes out the time values for later
 * analysis. From this you can determine the jitter and if the clock ever
 * runs backwards.
 */

#include <config.h>

#include "ntp_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NANOSECONDS	1000000000L

#define NBUF 100001			/* size of basic histogram */
#define NSRT 20000			/* size of overflow histogram */
#define NCNT (600L * NANOSECONDS)	/* sample interval (ns) */

int col (const void *, const void *);

int
main(
	int argc,
	char *argv[]
	)
{
	struct timespec ts, tr;
	int i, j, n;
	long t, u, v, w, gtod[NBUF], ovfl[NSRT];

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	/*
	 * Force pages into memory
	 */
	for (i = 0; i < NBUF; i++)
		gtod[i] = 0;
	for (i = 0; i < NSRT; i++)
		ovfl[i] = 0;

	/*
	 * Construct histogram
	 */
	n = 0;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	t = ts.tv_sec * NANOSECONDS + ts.tv_nsec;
	v = t;
	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &tr);
		u = tr.tv_sec * NANOSECONDS + tr.tv_nsec; 
		if (u - v > NCNT)
			break;
		w = u - t;
		if (w <= 0) {
/*
			printf("error <= 0 %ld %ld %ld, %ld %ld\n", w,
			       (long)ts.tv_sec, (long)ts.tv_nsec,
			       (long)tr.tv_sec, (long)tr.tv_nsec);
*/
		} else if (w > NBUF - 1) {
			ovfl[n] = w;
			if (n < NSRT - 1)
				n++;
		} else {
			gtod[w]++;
		}
		ts = tr;
		t = u;
	}

	/*
	 * Write out histogram
	 */
	for (i = 0; i < NBUF - 1; i++) {
		if (gtod[i] > 0)
			printf("%d %ld\n", i, gtod[i]);
	}
	if (n == 0)
		exit(0);
	qsort(&ovfl, (size_t)n, sizeof(ovfl[0]), col);
	w = 0;
	j = 0;
	for (i = 0; i < n; i++) {
		if (ovfl[i] != w) {
			if (j > 0)
				printf("%ld %d\n", w, j);
			w = ovfl[i];
			j = 1;
		} else
			j++;
	}
	if (j > 0)
		printf("%ld %d\n", w, j);
 
	exit(0);
}

int
col(
	const void *vx,
	const void *vy
	)
{
	const long *x = vx;
	const long *y = vy;

	return (*x - *y);
}
