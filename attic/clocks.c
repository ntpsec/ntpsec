/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


struct table {
  const int type;
  const char* name;
};

#define BATCHSIZE 1000000
#define BILLION 1000000000
#define HISTSIZE 2500
#define NSPERBUCKET 1
#define NSPERBUCKET2 250
#define MAXHISTLINES 15

struct table clocks [] = {
	{CLOCK_REALTIME, "CLOCK_REALTIME"},
#ifdef CLOCK_REALTIME_COARSE
	{CLOCK_REALTIME_COARSE, "CLOCK_REALTIME_COARSE"},
#endif
	{CLOCK_MONOTONIC, "CLOCK_MONOTONIC"},
#ifdef CLOCK_MONOTONIC_RAW
	{CLOCK_MONOTONIC_RAW, "CLOCK_MONOTONIC_RAW"},
#endif
#ifdef CLOCK_BOOTTIME
	{CLOCK_BOOTTIME, "CLOCK_BOOTTIME"},
#endif
#ifdef CLOCK_TAI
	{CLOCK_TAI, "CLOCK_TAI"},
#endif
	{0, NULL}
};

static int getpid_average(void) {
        int err;
        struct timespec start, stop;
        uint64_t sec, nanos;

        err = clock_gettime(CLOCK_MONOTONIC, &start);
        if (-1 == err) {
                printf("clock_gettime(CLOCK_MONOTONIC) didn't work, err %d\n", errno);
                return -1;
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < BATCHSIZE; i++) {
                getpid();
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);

        /* Beware of overflowing 32 bits. */
        sec = (stop.tv_sec-start.tv_sec);
        nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
        return nanos/BATCHSIZE;

}


static int gettimeofday_average(void) {
        int err;
        struct timespec start, stop;
        uint64_t sec, nanos;
	struct timeval now;

        err = clock_gettime(CLOCK_MONOTONIC, &start);
        if (-1 == err) {
                printf("clock_gettime(CLOCK_MONOTONIC) didn't work, err %d\n", errno);
                return -1;
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < BATCHSIZE; i++) {
                gettimeofday(&now, NULL);
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);

        /* Beware of overflowing 32 bits. */
        sec = (stop.tv_sec-start.tv_sec);
        nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
        return nanos/BATCHSIZE;

}


/* This is the number of paired reads with no change in time. */
/* Hack: Making this global avoids returning more than one item.  */
int dups;

static int do_res(int type, const char* name) {
	int err;
	struct timespec ts;

	err = clock_getres(type, &ts);
	if (-1 == err) {
		printf("clock_getres(%s) didn't work, err %d\n", name, errno);
		return -1;
	}
	return ts.tv_nsec;
}

static int do_average(int type, const char* name) {
	int err;
	struct timespec start, stop;
	uint64_t sec, nanos;

	err = clock_gettime(type, &start);
	if (-1 == err) {
		printf("clock_gettime(%s) didn't work, err %d\n", name, errno);
		return -1;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
		clock_gettime(type, &stop);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;

}

static int do_fastest(int type) {
	struct timespec start, stop;
	uint64_t sec, nanos, fastest;

	dups = 0;
	fastest = 999999999;
	for (int i = 0; i < BATCHSIZE; i++) {
		clock_gettime(type, &start);  /* warm up caches */
		clock_gettime(type, &start);
		clock_gettime(type, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (0 == nanos) {
			/* I've seen this on Pi 1. */
			dups++;
			continue;
		}
		if (nanos < fastest) fastest = nanos;
	}

	return fastest;
}
static int do_slowest(int type) {
	struct timespec start, stop;
	uint64_t sec, nanos, slowest;

	dups = 0;
	slowest = 0;
	for (int i = 0; i < BATCHSIZE; i++) {
		clock_gettime(type, &start);  /* warm up caches */
		clock_gettime(type, &start);
		clock_gettime(type, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (0 == nanos) {
			/* I've seen this on Pi 1. */
			dups++;
			continue;
		}
		if (nanos > slowest) slowest = nanos;
	}

	return slowest;
}

/* The second hist shows the tail. */
static void do_hist(int type) {
	int nsPerBucket = NSPERBUCKET;
	int nsPerBucket2 = NSPERBUCKET2;
	int i;
	int index, lines, toobig, hits;
	int last = 0;
	struct timespec start, stop;
	int64_t sec, nanos, slowest;
	unsigned int hist[HISTSIZE];
	unsigned int hist2[HISTSIZE];

	dups = 0;
	for (i = 0; i < HISTSIZE; i++) {
		hist[i] = 0;
		hist2[i] = 0;
	}

	slowest = 0;
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
			/* This happens on Pi 1. */
			dups++;
			continue;
		}
		index = nanos / nsPerBucket;
		if (index < HISTSIZE) {
			hist[index]++;
		}
		index = nanos / nsPerBucket2;
		if (index < HISTSIZE) {
			hist2[index]++;
		}
		if (slowest < nanos) slowest = nanos;
	}
	if (dups) {
		printf("%d samples were duplicated.\n", dups);
	}

	printf("\n");
	printf("Histogram: CLOCK_REALTIME, %d ns per bucket, %d samples.\n",
	       nsPerBucket, BATCHSIZE);
	printf("        ns      hits\n");
	toobig = 0;
	lines = 0;
	hits = 0;
	for (i=0; i<HISTSIZE; i++) {
		if ((MAXHISTLINES <= lines) && (0.98*BATCHSIZE < hits)) {
			break;  /* Avoid clutter of printing long tail */
		}
		if (hist[i]) {
			last = i*nsPerBucket;
			printf("%10d  %8u\n", i*nsPerBucket, hist[i]);
			lines++;
			hits += hist[i];
		}
	}
	toobig = BATCHSIZE-hits;
	if (toobig) { printf("%d samples were bigger than %d.\n",
			     toobig, last);
	}

	printf("\n");
	printf("Histogram: CLOCK_REALTIME, %d ns per bucket, %d samples.\n",
	       nsPerBucket2, BATCHSIZE);
	printf("        ns      hits\n");
	lines = 0;
	hits = 0;
	for (i=0; i<HISTSIZE; i++) {
		if ((MAXHISTLINES <= lines) && (0.98*BATCHSIZE < hits)) {
			break;  /* Avoid clutter of printing long tail */
		}
		if (hist2[i]) {
			last = i*nsPerBucket2;
			printf("%10d  %8u\n", i*nsPerBucket2, hist2[i]);
			lines++;
			hits += hist2[i];
		}
	}
	toobig = BATCHSIZE-hits;
	if (toobig) {
		printf("%d samples were bigger than %d.\n", toobig, last);
		printf("Slowest was %ld.\n", (long)slowest);
	}
}


int main(int argc, char *argv[]) {
	int res, average, fastest;
	int slowestest = 0;

	(void)argc;  /* Squash unused warnings */
	(void)argv;

	printf("      res   avg      min  dups  CLOCK\n");
	average = getpid_average();
	printf("          %5d                 %s\n",
		average, "getpid() [kernel call]");
	average = gettimeofday_average();
	printf("     1000 %5d                 %s\n",
		average, "gettimeofday() [usec]");
	for (int i=0; (NULL != clocks[i].name); i++) {
		res = do_res(clocks[i].type, clocks[i].name);
		average = do_average(clocks[i].type, clocks[i].name);
		fastest = do_fastest(clocks[i].type);
		printf("%9d %5d %8d", res, average, fastest);
		if (0.9*BATCHSIZE < dups) {
			/* Hack: negative to show good if close to all are bad */
			printf(" %5d", dups-BATCHSIZE);
		} else if (0 < dups) {
			printf(" %5d", dups);
		} else {
			printf("      ");
		}
		printf("  %s\n", clocks[i].name);
		fflush(stdout);
	}

	if (1) {
		fastest = do_fastest(CLOCK_REALTIME);
		do_hist(CLOCK_REALTIME);
	}

	
	printf("\nSlowest from each batch of %d...\n", BATCHSIZE);
        for (int i=0; i<50; i++) {
		int slowest;
		slowest = do_slowest(CLOCK_REALTIME);
		printf(" %6ld", (long)slowest);
		if ((i%10)==9) printf("\n");
		if (dups) {
		  printf("  %d samples were duplicated.\n", dups);
		}
		if (slowest > slowestest) slowestest = slowest;

	}
	printf("Slowest was %ld\n", (long)slowestest);

	return 0;

}

