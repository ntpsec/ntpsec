/* ntp_endian.h - BSD-style big-endian encoding/decoding functions
 *
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * These are portable reimplementations of what BSD provides
 * in <sys/endian.h>, but using uint8_t instead of void *.
 */
#ifndef GUARD_NTP_ENDIAN_H
#define GUARD_NTP_ENDIAN_H

#include <stdint.h>

static inline uint16_t ntp_be16dec(const uint8_t *buf) {
	return (uint16_t)(((uint16_t)buf[0] << 8) | buf[1]);
}

static inline uint32_t ntp_be32dec(const uint8_t *buf) {
	return ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
	    ((uint32_t)buf[2] << 8) | buf[3];
}

static inline uint64_t ntp_be64dec(const uint8_t *buf) {
	return ((uint64_t)ntp_be32dec(buf) << 32) | ntp_be32dec(buf + 4);
}

#endif
