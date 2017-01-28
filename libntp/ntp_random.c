/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include <stdint.h>

#include <openssl/rand.h>

#include "ntp_endian.h"

int32_t
ntp_random(void)
{
	unsigned char rnd[sizeof(uint32_t)];
	RAND_bytes(rnd, sizeof(rnd));
	return (int32_t)ntp_be32dec(rnd);
}
