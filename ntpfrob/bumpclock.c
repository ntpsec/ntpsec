/*
 * Hack to bump the system time for testing ntpd response.
 * Must be run as root.
 * Arg is microseconds.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "ntpfrob.h"
#include "ntp_machine.h"

#define NS_PER_S	1000000000

/* FIXME: Make use of clockwork.c instead of duplicating code. */

#ifndef HAVE_CLOCK_GETTIME
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    struct timeval tv;

    switch (clk_id) {
    case CLOCK_REALTIME:
	/*
	 * On OSX, it's tempting to use clock_get_time() for its apparent
	 * nanosecond resolution, but it really only has microsecond
	 * resolution, and is substantially slower than gettimeofday().
	 */
	if (gettimeofday(&tv, NULL))
	    return -1;
	tp->tv_sec = tv.tv_sec;
	tp->tv_nsec = tv.tv_usec * 1000;
	return 0;
    case CLOCK_MONOTONIC:
#ifdef __MACH__
	/*
	 * mach_absolute_time() with proper scaling would be faster, but is
	 * more directly analogous to CLOCK_MONOTONIC_RAW, and constancy of
	 * the scale factors would need to be investigated.
	 */
	 {
	    mach_timespec_t mts;
	    mach_port_t mach_host;
	    static clock_serv_t sclock = 0; // Cached SYSTEM_CLOCK port

	    if (!sclock) {
		mach_host= mach_host_self();
		host_get_clock_service(mach_host, SYSTEM_CLOCK, &sclock);
		mach_port_deallocate(mach_task_self(), mach_host);
	    }
	    clock_get_time(sclock, &mts);
	    tp->tv_sec = mts.tv_sec;
	    tp->tv_nsec = mts.tv_nsec;
	    return 0;
	}
#else /* !__MACH__ */
#error POSIX clock_gettime(2) is required for CLOCK_MONOTONIC
#endif /* !__MACH__ */
    default:
	errno = EINVAL;
	return -1;
    }
}
#endif /* !HAVE_CLOCK_GETTIME */

#ifndef HAVE_CLOCK_SETTIME

int clock_settime(clockid_t clk_id, const struct timespec *tp)
{
    struct timeval tv;

    switch (clk_id) {
    case CLOCK_REALTIME:
	tv.tv_sec = tp->tv_sec;
	tv.tv_usec = (tp->tv_nsec + 500) / 1000;
	return settimeofday(&tv, NULL);
    default:
	errno = EINVAL;
	return -1;
    }
}
#endif /* !HAVE_CLOCK_SETTIME */

void bumpclock(int bump)
{
    struct timespec was, set, now;
    int rc1, rc2, rc3;
    int er1, er2, er3;

    printf("Bumping clock by %d microseconds.\n", bump);

    rc1 = clock_gettime(CLOCK_REALTIME, &was);
    er1 = errno;

    set = was;
    bump *= 1000;
    /* coverity[tainted_data] */
    set.tv_nsec += bump;
    while (set.tv_nsec >= NS_PER_S ) {
	set.tv_nsec -= NS_PER_S;
	set.tv_sec +=1;
    }
    while (set.tv_nsec <= 0 ) {
	set.tv_nsec += NS_PER_S;
	set.tv_sec -=1;
    }
    rc2 = clock_settime(CLOCK_REALTIME, &set);
    er2 = errno;

    rc3 = clock_gettime(CLOCK_REALTIME, &now);
    er3 = errno;

    /* Defer printing so it doesn't distort timing. */
    if (rc1)
	printf("Couldn't get time: %s\n", strerror(er1));
    else
	printf("Was: %ld.%09ld\n", (long)was.tv_sec, was.tv_nsec);

    if (rc2) {
	printf("Couldn't set time: %s\n", strerror(er2));
	printf("Try: %ld.%09ld\n", (long)set.tv_sec, set.tv_nsec);
    } else
	printf("Set: %ld.%09ld\n", (long)set.tv_sec, set.tv_nsec);
 
   if (rc3)
	printf("Couldn't set time: %s\n", strerror(er3));
    else
	printf("Now: %ld.%09ld\n", (long)now.tv_sec, now.tv_nsec);
}
