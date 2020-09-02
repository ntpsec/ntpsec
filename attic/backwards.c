/* backwards.c - check for time going backwards
 * Started from Artturi Alm on port-arm@netbsd.org
 * Fri, 25 Jan 2019 09:54:42 +0200
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNUSED_ARG(arg)         ((void)(arg))

static void ts_print(struct timespec *ts0, struct timespec *ts1);


void ts_print(struct timespec *ts0, struct timespec *ts1)
{
	printf("ts0=(%ld,%ld)\nts1=(%ld,%ld)\n",
	    (long)ts0->tv_sec, ts0->tv_nsec,
	    (long)ts1->tv_sec, ts1->tv_nsec);
}

int
main(int argc, char *argv[])
{
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	struct timespec ts0, ts1;


	/* Some systems return equal.
	 * Print first 5 equal examples. */
	for(int equal = 0; equal < 5; ) {
		clock_gettime(CLOCK_REALTIME, &ts0);  /* warm up cache */
		clock_gettime(CLOCK_REALTIME, &ts1);
		clock_gettime(CLOCK_REALTIME, &ts0);
		clock_gettime(CLOCK_REALTIME, &ts1);

		if ((ts0.tv_sec < ts1.tv_sec) ||
		    ((ts0.tv_sec == ts1.tv_sec) &&
		     (ts0.tv_nsec <= ts1.tv_nsec))) {
		    continue;
		} else {
			ts_print(&ts0, &ts1);
			if (ts0.tv_sec == ts1.tv_sec
				&& ts0.tv_nsec == ts1.tv_nsec) {
			    equal++;
			}
		}
	}

	for(;;) {
		clock_gettime(CLOCK_REALTIME, &ts0);  /* warm up cache */
		clock_gettime(CLOCK_REALTIME, &ts1);
		clock_gettime(CLOCK_REALTIME, &ts0);
		clock_gettime(CLOCK_REALTIME, &ts1);

		if ((ts0.tv_sec < ts1.tv_sec) ||
		    ((ts0.tv_sec == ts1.tv_sec) &&
		     (ts0.tv_nsec < ts1.tv_nsec))) {
		} else {
			ts_print(&ts0, &ts1);
		}
	}
}

