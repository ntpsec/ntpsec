/*
 * timespecops.h -- calculations on 'struct timespec' values
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */
#ifndef GUARD_TIMESPECOPS_H
#define GUARD_TIMESPECOPS_H

#include <sys/types.h>
#include "ntp.h"

/* milliseconds per second */
#define MS_PER_S 1000
/* seconds per millisecond */
#define S_PER_MS 1.0e-3

/* microseconds per second */
#define US_PER_S 1000000
/* seconds per microsecond */
#define S_PER_US 1.0e-6

/* nanoseconds per second */
#define NS_PER_S 1000000000
/* seconds per nanosecond */
#define S_PER_NS 1.0e-9

/* nano seconds per millisecond */
#define NS_PER_MS 1000000

/* returns true if a time stored as a double is close to zero */
#define D_ISZERO_NS(t) (fabs(t) < S_PER_NS/10.0)

/* predicate: returns true if the nanoseconds are in nominal range */
#define timespec_isnormal(x) ((x)->tv_nsec >= 0 && (x)->tv_nsec < NS_PER_S)

/* predicate: returns true if the nanoseconds are out-of-bounds */
#define timespec_isdenormal(x)	(!timespec_isnormal(x))

/* conversion between l_fp fractions and nanoseconds */
#define FTOTVN(tsf) \
    ((int32_t)(((uint64_t)(tsf) * NS_PER_S + 0x80000000) >> 32))
#define TVNTOF(tvu) \
    ((uint32_t) ((((uint64_t)(tvu) << 32) + NS_PER_S / 2) / NS_PER_S))


extern struct timespec normalize_tspec(struct timespec);
extern struct timespec d_to_tspec(double);
extern struct timespec add_tspec(struct timespec, struct timespec);
extern struct timespec add_tspec_ns(struct timespec, long);
extern struct timespec sub_tspec(struct timespec, struct timespec);
extern struct timespec sub_tspec_ns(struct timespec, long);
extern struct timespec neg_tspec(struct timespec);
extern struct timespec abs_tspec(struct timespec);
extern int cmp_tspec(struct timespec a, struct timespec b);
extern int cmp_tspec_denorm(struct timespec, struct timespec);
extern int test_tspec( struct timespec);
extern int test_tspec_denorm( struct timespec);
extern const char * tspectoa( struct timespec);
extern l_fp tspec_intv_to_lfp( struct timespec);
extern l_fp tspec_stamp_to_lfp( struct timespec);
extern struct timespec lfp_intv_to_tspec(l_fp);
extern struct timespec lfp_uintv_to_tspec(l_fp);
extern struct timespec lfp_stamp_to_tspec(l_fp, time_t);
extern struct timespec tval_to_tspec(struct timeval);

#endif	/* GUARD_TIMESPECOPS_H */
