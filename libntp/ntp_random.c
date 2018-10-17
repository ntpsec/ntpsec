/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include <stdint.h>

#include <openssl/rand.h>

#include "config.h"
#include "ntp.h"

/* NB: RAND_bytes comes from OpenSSL
 * Starting in version 1.1.1, it reseeds itself occasionally.
 * That needs access to /dev/urandom which may be blocked by chroot jails.
 */

int32_t
ntp_random(void)
{
	int err;
	uint32_t rnd = 0;
	err = RAND_bytes((unsigned char *)&rnd, sizeof(rnd));
	if (1 != err) {
		msyslog(LOG_ERR, "ERR: ntp_random - RAND_bytes failed");
	  exit(1);
	}
	return rnd;
}

uint64_t
ntp_random64(void)
{
        int err;
        uint64_t rnd = 0;
        err = RAND_bytes((unsigned char *)&rnd, sizeof(rnd));
        if (1 != err) {
                msyslog(LOG_ERR, "ERR: ntp_random - RAND_bytes failed");
          exit(1);
        }
        return rnd;
}

