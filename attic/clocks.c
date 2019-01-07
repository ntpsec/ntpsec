/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

struct table {
  const int type;
  const char* name;
};

#define BATCHSIZE 1000000
#define BILLION 1000000000
#define HISTSIZE 250

struct table clocks [] = {
  {CLOCK_REALTIME, "CLOCK_REALTIME"},
#ifdef CLOCK_REALTIME_COARSE
  {CLOCK_REALTIME_COARSE, "CLOCK_REALTIME_COARSE"},
#endif
#ifdef CLOCK_MONOTONIC
  {CLOCK_MONOTONIC, "CLOCK_MONOTONIC"},
#endif
#ifdef CLOCK_MONOTONIC_RAW
  {CLOCK_MONOTONIC_RAW, "CLOCK_MONOTONIC_RAW"},
#endif
#ifdef CLOCK_BOOTTIME
  {CLOCK_BOOTTIME, "CLOCK_BOOTTIME"},
#endif
  {0, NULL}
};

/* This is the number of paired reads with no change in time. */
/* Hack: Making this global avoids returning more than one item.  */
int dups;

int do_res(int type, const char* name);
int do_average(int type, const char* name);
int do_fastest(int type, const char* name);
void do_hist(int type, int fastest);

int do_res(int type, const char* name) {
  int err;
  struct timespec ts;

  err = clock_getres(type, &ts);
  if (-1 == err) {
    printf("clock_getres(%s) didn't work, err %d\n", name, errno);
    return -1;
  }
  return ts.tv_nsec;
}

int do_average(int type, const char* name) {
  int err, i;
  struct timespec start, stop;
  uint64_t sec, nanos;

  err = clock_gettime(type, &start);
  if (-1 == err) {
    printf("clock_gettime(%s) didn't work, err %d\n", name, errno);
    return -1;
  }

  clock_gettime(CLOCK_REALTIME, &start);
  for (i = 0; i < BATCHSIZE; i++) {
    clock_gettime(type, &stop);
  }
  clock_gettime(CLOCK_REALTIME, &stop);

  /* Beware of overflowing 32 bits. */
  sec = (stop.tv_sec-start.tv_sec);
  nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
  return nanos/BATCHSIZE;

}

int do_fastest(int type, const char* name) {
  int i;
  struct timespec start, stop;
  uint64_t sec, nanos, fastest;

  (void)name;  /* Squash unused warnings */

  dups = 0;
  fastest = 999999999;
  for (i = 0; i < BATCHSIZE; i++) {
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

void do_hist(int type, int fastest) {
  int nsPerBucket = 5;
  int i;
  int delta, lines, toobig, hits, miss;
  struct timespec start, stop;
  uint64_t sec, nanos;
  unsigned int hist[HISTSIZE];

  dups = 0;
  toobig = 0;
  for (i = 0; i < HISTSIZE; i++) hist[i] = 0;

  for (i = 0; i < BATCHSIZE; i++) {
    clock_gettime(type, &start);
    clock_gettime(type, &stop);
    sec = (stop.tv_sec-start.tv_sec);
    nanos = sec*BILLION + (stop.tv_nsec-start.tv_nsec);
    if (0 == nanos) {
      /* I've seen this on Pi 1. */
      dups++;
      continue;
    }
    delta = (nanos-fastest) / nsPerBucket;
    if (0 > delta) {
      /* fastest wasn't fast enough. */
      printf("Too small: %ld, %d\n", (long)nanos, fastest);
      continue;
    }
    if (delta < HISTSIZE) {
      hist[delta]++;
    } else {
      toobig++;
    }
  }
  printf("\n");
  printf("Histogram: CLOCK_REALTIME, %d ns per bucket, %d samples.\n",
    nsPerBucket, BATCHSIZE);
  printf("        ns      hits\n");
  lines = 0;
  hits = 0;
  for (i=0; i<HISTSIZE; i++) {
    if ((5 <= lines) && (0.98*BATCHSIZE<hits)) break;  /* minimal printout */
    if (hist[i]) {
      printf("%10d  %8u\n", i*nsPerBucket+fastest, hist[i]);
      lines++;
      hits += hist[i];
    }
  }
  miss = i;
  for (  ; i<HISTSIZE; i++) {
    toobig += hist[i];
  }
  
  if (toobig) printf("%d samples were bigger than %d.\n",
                toobig, miss*nsPerBucket+fastest);
}


int main(int argc, char *argv[])
{
  int i;
  int res, average, fastest;

  (void)argc;  /* Squash unused warnings */
  (void)argv;

printf("      res   avg     min  dups  CLOCK\n");
for (i=0; (NULL != clocks[i].name); i++) {
  res = do_res(clocks[i].type, clocks[i].name);
  average = do_average(clocks[i].type, clocks[i].name);
  fastest = do_fastest(clocks[i].type, clocks[i].name);
  printf("%9d %5d %7d", res, average, fastest);
  if (0.9*BATCHSIZE < dups)
    /* Hack: negative to show good if close to all are bad */
    printf(" %5d", dups-BATCHSIZE);
  else if (0 < dups)
    printf(" %5d", dups);
  else
    printf("      ");
  printf("  %s\n", clocks[i].name);
  fflush(stdout);
}

if (1) {
  fastest = do_fastest(CLOCK_REALTIME, "CLOCK_REALTIME");
  do_hist(CLOCK_REALTIME, fastest);
}

return 0;
  
}

