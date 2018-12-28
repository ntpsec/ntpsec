/*
 *
 * Try to run this code to see what the PPS-API finds. You give it the
 * device as argument and you may have to modify the pp.mode = BLA assignment.
 *
 * Code originally by Poul-Henning Kemp.
 *
 * Copyright 2015 by the NTPsec project contributors
 *  SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "timespecops.h"
#include "ntpfrob.h"

#ifdef HAVE_SYS_TIMEPPS_H
#include <sys/timepps.h>

#ifdef timespecsub
  /* It's defined on NetBSD */
# undef timespecsub
#endif
#define timespecsub(vvp, uvp)                                           \
        do {                                                            \
                (vvp)->tv_sec -= (uvp)->tv_sec;                         \
                (vvp)->tv_nsec -= (uvp)->tv_nsec;                       \
                if ((vvp)->tv_nsec < 0) {                               \
                        (vvp)->tv_sec--;                                \
                        (vvp)->tv_nsec += NS_PER_S;                   \
                }                                                       \
        } while (0)

static void Chew(struct timespec *, struct timespec *, unsigned, unsigned);

static void
Chew(struct timespec *tsa, struct timespec *tsc, unsigned sa, unsigned sc)
{
	struct timespec ts;

	printf("%ld.%09ld ", (long)tsa->tv_sec, tsa->tv_nsec);
	printf("%ld.%09ld ", (long)tsc->tv_sec, tsc->tv_nsec);
	printf("%u %u ", sa, sc);

	ts = *tsc;
	timespecsub(&ts,tsa);
	printf("%.9f ", ts.tv_sec + ts.tv_nsec * S_PER_NS);
	printf("\n");
	fflush(stdout);
}

void err(const char *legend)
{
    fflush(stdout);
    fprintf(stderr, "ntpfrob: %s: %s\n", legend, strerror(errno));
    fflush(stderr);
    exit(1);
}
#endif /* HAVE_SYS_TIMEPPS_H */

void ppscheck(const char *device)
{
#ifndef HAVE_SYS_TIMEPPS_H
	(void)device;
 	fputs("ntpfrob: PPS is not available.\n", stderr);
	exit(1);
#else
	int fd;
	pps_info_t pi;
	pps_params_t pp;
	pps_handle_t ph = 0;    /* 0 to prevent spurious uninialized warning */
	int mode;               /* PPS capabilities */
	unsigned int olda = 0;  /* old assert sequence */
	unsigned int oldc = 0;  /* old clear sequence */
	struct timespec to;

	if (device == NULL)
		device = "/dev/cuaa1";
	setbuf(stdout, 0);
	fd = open(device, O_RDONLY);
	if (fd < 0)
		err("Trying to open() PPS device");
	if (time_pps_create(fd, &ph) < 0)
		err("return handle to time_pps_create() device");

	if (time_pps_getcap(ph, &mode) < 0)
		err("return time_pps_getcap() implementation capabilities");

        memset(&pp, 0, sizeof(pp));
	/* pp.mode = PPS_CAPTUREASSERT | PPS_ECHOASSERT; */
	pp.mode = PPS_CAPTUREBOTH;
	/* pp.mode = PPS_CAPTUREASSERT; */

#ifdef PPS_API_VERS
        pp.api_version = PPS_API_VERS;
#else
        /* FreeBSD, NetBSD do not publicly define PPS_ABI_VERS, assume 1 */
        pp.api_version = 1;
#endif

	if (time_pps_setparams(ph, &pp) < 0)
		err("return time_pps_setparams() parameters to interface");

	while (1) {
		to.tv_nsec = 0;
		to.tv_sec = 0;
		if (time_pps_fetch(ph, PPS_TSFMT_TSPEC, &pi, &to) < 0)
			err("return timestamp associated with time_pps_fetch() instance");
		if (olda == pi.assert_sequence &&
		    oldc == pi.clear_sequence) {
			/* used to be usleep(10000) - 0.1 sec */
			const struct timespec tenth = {0, 100000};
			nanosleep(&tenth, NULL);
			continue;
		}

		Chew(&pi.assert_timestamp, &pi.clear_timestamp,
			pi.assert_sequence, pi.clear_sequence);
		olda = pi.assert_sequence;
		oldc = pi.clear_sequence;
	}
#endif /* HAVE_SYS_TIMEPPS_H */
        /* either way, never returns */
}

/* end */
