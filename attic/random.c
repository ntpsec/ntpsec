/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <openssl/opensslv.h>    /* for OPENSSL_VERSION_NUMBER */
#include <openssl/rand.h>

#define BATCHSIZE 1000000
#define BILLION 1000000000
#define HISTSIZE 2500
#define NSPERBUCKET 1
#define MAXHISTLINES 10

/* hack for libntp */
const char *progname = "foo";

static int getpid_average(void) {
        int err;
        struct timespec start, stop;
        uint64_t sec, nanos;

        err = clock_gettime(CLOCK_REALTIME, &start);
        if (-1 == err) {
                printf("clock_gettime(CLOCK_REALTIME) didn't work, err %d\n", errno);
                return -1;
        }

        clock_gettime(CLOCK_REALTIME, &start);
        for (int i = 0; i < BATCHSIZE; i++) {
                getpid();
        }
        clock_gettime(CLOCK_REALTIME, &stop);

        /* Beware of overflowing 32 bits. */
        sec = (stop.tv_sec-start.tv_sec);
        nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
        return nanos/BATCHSIZE;

}

static int clock_average(void) {
        int err;
        struct timespec start, stop;
        uint64_t sec, nanos;

        err = clock_gettime(CLOCK_REALTIME, &start);
        if (-1 == err) {
                printf("clock_gettime(CLOCK_REALTIME) didn't work, err %d\n", errno);
                return -1;
        }

        clock_gettime(CLOCK_REALTIME, &start);
        for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &stop);
        }
        clock_gettime(CLOCK_REALTIME, &stop);

        /* Beware of overflowing 32 bits. */
        sec = (stop.tv_sec-start.tv_sec);
        nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
        return nanos/BATCHSIZE;

}

static int do_average(void) {
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

static int do_avg_bytes(unsigned int bytes) {
	int err = 0;
	struct timespec start, stop;
	uint64_t sec, nanos;
	unsigned char rnd[512];

	if (bytes > sizeof(rnd)) {
		printf("## do_avg_bytes - too big\n");
		exit(1);
	}

	clock_gettime(CLOCK_REALTIME, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
            err += RAND_bytes((unsigned char *)&rnd, bytes);
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	if (BATCHSIZE != err) {
		printf("## do_avg_bytes: troubles from RAND_bytes, %d\n",
			BATCHSIZE-err);
		exit(1);
	}

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;
}

#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
static int do_avg_priv(unsigned int bytes) {
	int err = 0;
	struct timespec start, stop;
	uint64_t sec, nanos;
	unsigned char rnd[512];

	if (bytes > sizeof(rnd)) {
		printf("## do_avg_priv - too big\n");
		exit(1);
	}

	clock_gettime(CLOCK_REALTIME, &start);
	for (int i = 0; i < BATCHSIZE; i++) {
            err += RAND_priv_bytes((unsigned char *)&rnd, bytes);
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	if (BATCHSIZE != err) {
		printf("## do_avg_priv: troubles from RAND_bytes, %d\n",
			BATCHSIZE-err);
		exit(1);
	}

	/* Beware of overflowing 32 bits. */
	sec = (stop.tv_sec-start.tv_sec);
	nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
	return nanos/BATCHSIZE;
}
#endif

static int do_fastest(void) {
	int sum = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest = 999999999;

	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
		/* coverity[dc,weak_crypto] */
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

static int do_fast_bytes(unsigned bytes) {
	int err = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest = 999999999;
	unsigned char rnd[512];

	if (bytes > sizeof(rnd)) {
		printf("## do_fast_bytes - too big\n");
		exit(1);
	}

	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
                err += RAND_bytes((unsigned char *)&rnd, bytes);
		clock_gettime(CLOCK_REALTIME, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (nanos < fastest) {
			fastest = nanos;
		}
	}

	if (BATCHSIZE != err) {
		printf("## do_fast_bytes: troubles from RAND_bytes, %d\n",
			BATCHSIZE-err);
		exit(1);
	}

	return fastest;
}

#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
static int do_fast_priv(unsigned bytes) {
	int err = 0;
	struct timespec start, stop;
	uint64_t sec, nanos, fastest = 999999999;
	unsigned char rnd[512];

	if (bytes > sizeof(rnd)) {
		printf("## do_fast_priv - too big\n");
		exit(1);
	}

	for (int i = 0; i < BATCHSIZE; i++) {
                clock_gettime(CLOCK_REALTIME, &start);
                err += RAND_priv_bytes((unsigned char *)&rnd, bytes);
		clock_gettime(CLOCK_REALTIME, &stop);
		sec = (stop.tv_sec-start.tv_sec);
		nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
		if (nanos < fastest) {
			fastest = nanos;
		}
	}

	if (BATCHSIZE != err) {
		printf("## do_fast_priv: troubles from RAND_bytes, %d\n",
			BATCHSIZE-err);
		exit(1);
	}

	return fastest;
}
#endif

static void do_bytes(int bytes) {
	int average = do_avg_bytes(bytes);
	int fastest = do_fast_bytes(bytes);
	printf("RAND_bytes():      %5d %8d %4d\n", average, fastest, bytes);
	fflush(stdout);
}

#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
static void do_priv(int bytes) {
	int average = do_avg_priv(bytes);
	int fastest = do_fast_priv(bytes);
	printf("RAND_priv_bytes(): %5d %8d %4d\n", average, fastest, bytes);
	fflush(stdout);
}
#endif

int main(int argc, char *argv[]) {
	int average, fastest;

	(void)argc;  /* Squash unused warnings */
	(void)argv;

	printf("Running on %s\n", OPENSSL_VERSION_TEXT);
	printf(" times in ns         avg  fastest  lng\n");

	average = getpid_average();
	printf("getpid():          %5d            (simple kernel call)\n",
		average);

	average = clock_average();
	printf("clock_gettime:     %5d            (overhead of fastest)\n",
		average);

	average = do_average();
	fastest = do_fastest();
	printf("random():          %5d %8d\n", average, fastest);
	fflush(stdout);

	do_bytes(4);
	do_bytes(16);
	do_bytes(32);
	do_bytes(160);
	do_bytes(320);

#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
	do_priv(4);
	do_priv(16);
	do_priv(32);
	do_priv(160);
	do_priv(320);
#endif

	return 0;

}

