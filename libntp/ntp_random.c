/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include "config.h"
#include <stdint.h>
#include <sodium.h>

long
ntp_random( void )
{
	uint32_t rnd;
	randombytes_buf(&rnd, sizeof rnd);
	return (long)(rnd & 0x7fffffff);
}
