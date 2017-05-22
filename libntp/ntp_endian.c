/* ntp_endian.c - BSD-style big-endian encoding/decoding functions
 *
 * Copyright 2016 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-clause
 * 
 * These are portable reimplementations of what BSD provides
 * in <sys/endian.h>.
 */

#include <stdint.h>
#include "ntp_endian.h"

uint16_t ntp_be16dec(const void *buf) {
	const uint8_t *b = (const uint8_t*)buf;
	return ((uint16_t)(b[0]) << 8) +
          (uint16_t)(b[1]);
}

uint32_t ntp_be32dec(const void *buf) {
	const uint8_t *b = (const uint8_t*)buf;
	return ((uint32_t)(b[0]) << 24) +
	    ((uint32_t)(b[1]) << 16) +
	    ((uint32_t)(b[2]) << 8) +
	    (uint32_t)(b[3]);
}

void ntp_be32enc(void *buf, uint32_t x) {
	uint8_t *b = (uint8_t*)buf;
	b[0] = (x >> 24) & 0xff;
	b[1] = (x >> 16) & 0xff;
	b[2] = (x >> 8) & 0xff;
	b[3] = x & 0xff;
}

uint64_t ntp_be64dec(const void *buf) {
	const uint8_t *b = (const uint8_t*)buf;
	return ((uint64_t)(b[0]) << 56) +
	    ((uint64_t)(b[1]) << 48) +
	    ((uint64_t)(b[2]) << 40) +
	    ((uint64_t)(b[3]) << 32) +
	    ((uint64_t)(b[4]) << 24) +
	    ((uint64_t)(b[5]) << 16) +
	    ((uint64_t)(b[6]) << 8) +
	    (uint64_t)(b[7]);
}

void ntp_be64enc(void *buf, uint64_t x) {
	uint8_t *b = (uint8_t*)buf;
	b[0] = (x >> 56) & 0xff;
	b[1] = (x >> 48) & 0xff;
	b[2] = (x >> 40) & 0xff;
	b[3] = (x >> 32) & 0xff;
	b[4] = (x >> 24) & 0xff;
	b[5] = (x >> 16) & 0xff;
	b[6] = (x >> 8) & 0xff;
	b[7] = x & 0xff;
}
