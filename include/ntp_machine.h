/*
 * ntp_machine.h
 *
 * Collect all machine dependent idiosyncrasies in one place.
 *
 */

#ifndef GUARD_NTP_MACHINE_H
#define GUARD_NTP_MACHINE_H

#include <sys/time.h>
#include <time.h>

#ifndef CLOCK_REALTIME
/*
 * Pacify platforms that don't have a real clock_gettime(2),
 * notably Mac OS X.
 */
#define CLOCK_REALTIME	0
#define CLOCK_MONOTONIC	1
typedef int clockid_t;
int clock_gettime(clockid_t clock_id, struct timespec *tp);
#endif

int ntp_set_tod (struct timespec *tvs);

#endif	/* GUARD_NTP_MACHINE_H */
