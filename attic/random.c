/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "ntp.h"

#define BATCHSIZE 1000000
#define BILLION 1000000000
#define HISTSIZE 2500
#define NSPERBUCKET 1
#define MAXHISTLINES 10

/* hack for libntp */
const char *progname = "foo";

int do_average(void);
int do_average(void) {
	long int sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos;

	clock_gettime(CLOCK_REALTIME, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
		sum += random();
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;
}

int do_average32(void);
int do_average32(void) {
	int32_t sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos;

	clock_gettime(CLOCK_REALTIME, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
		sum += ntp_random();
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;
}

int do_average64(void);
int do_average64(void) {
	uint64_t sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos;

	clock_gettime(CLOCK_REALTIME, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
		sum += ntp_random64();
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;
}

int do_fastest(void);
int do_fastest(void) {
	int sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest;

	fastest = 999999999;
	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
		sum += random();
		clock_gettime(CLOCK_REALTIME, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (nanos < fastest) {
			fastest = nanos;
		}
	}

	return fastest;
}

int do_fastest32(void);
int do_fastest32(void) {
	int sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest;

	fastest = 999999999;
	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
		sum += ntp_random();
		clock_gettime(CLOCK_REALTIME, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (nanos < fastest) {
			fastest = nanos;
		}
	}

	return fastest;
}

int do_fastest64(void);
int do_fastest64(void) {
	uint64_t sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest;

	fastest = 999999999;
	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
		sum += ntp_random64();
		clock_gettime(CLOCK_REALTIME, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (nanos < fastest) {
			fastest = nanos;
		}
	}

	return fastest;
}

int dups = 0;
int do_hist(int type, int fastest);
int do_hist(int type, int fastest) {
	int nsPerBucket = NSPERBUCKET;
	int i;
	int delta, lines, toobig, hits, miss;
	struct timespec start, stop;
	int64_t sec, nanos;
	unsigned int hist[HISTSIZE];
	int faster = 0;

	toobig = 0;
	for (i = 0; i < HISTSIZE; i++) {
		hist[i] = 0;
	}

	for (i = 0; i < BATCHSIZE; i++) {
		clock_gettime(type, &start);  /* warm up cache */
		clock_gettime(type, &stop);
		clock_gettime(type, &start);
		clock_gettime(type, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (0 > nanos) {
			printf("## Time went backwards: %ld.%9ld-%ld.%9ld=>%ld\n",
			       (long)stop.tv_sec, stop.tv_nsec,
			       (long)start.tv_sec, start.tv_nsec,
			       (long)nanos);
			continue;
		}
		if (0 == nanos) {
			/* I've seen this on Pi 1. */
			dups++;
			continue;
		}
		delta = (nanos-fastest) / nsPerBucket;
		if (0 > delta) {
			/* fastest wasn't fast enough */
			if (0 == faster) {
				faster = delta;
			}
			if (faster > delta) {
				faster = delta;
			}
			continue;
		}
		if (delta < HISTSIZE) {
			hist[delta]++;
		} else {
			toobig++;
		}
	}
	if (faster) { return faster;
	}
	printf("\n");
	printf("Histogram: CLOCK_REALTIME, %d ns per bucket, %d samples.\n",
	       nsPerBucket, BATCHSIZE);
	printf("        ns      hits\n");
	lines = 0;
	hits = 0;
	for (i=0; i<HISTSIZE; i++) {
		if ((MAXHISTLINES <= lines) && (0.98*BATCHSIZE < hits)) {
			break;  /* Avoid clutter of printing long tail */
		}
		if (hist[i]) {
			printf("%10d  %8u\n", i*nsPerBucket+fastest, hist[i]);
			lines++;
			hits += hist[i];
		}
	}
	miss = i-1;
	for (  ; i<HISTSIZE; i++) {
		toobig += hist[i];
	}

	if (dups) {
		printf("%d samples were duplicated.\n", dups);
	}
	if (toobig) { printf("%d samples were bigger than %d.\n",
			     toobig, miss*nsPerBucket+fastest);
	}
	return faster;
}


int main(int argc, char *argv[]) {
	int average, fastest;

	(void)argc;  /* Squash unused warnings */
	(void)argv;

	printf("                  avg  fastest\n");

	average = do_average();
	fastest = do_fastest();
	printf("random():       %5d %8d\n", average, fastest);
	fflush(stdout);

	average = do_average32();
	fastest = do_fastest32();
	printf("ntp_random():   %5d %8d\n", average, fastest);
	fflush(stdout);

	average = do_average64();
	fastest = do_fastest64();
	printf("ntp_random64(): %5d %8d\n", average, fastest);
	fflush(stdout);

	return 0;

}

