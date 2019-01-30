/* ntp_endian.h - BSD-style big-endian encoding/decoding functions
 *
 * Copyright 2016 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-clause
 *
 * These are portable reimplementations of what BSD provides
 * in <sys/endian.h>.
 */
#ifndef GUARD_NTP_ENDIAN_H
#define GUARD_NTP_ENDIAN_H

#include <stdint.h>

uint16_t ntp_be16dec(const void *buf) __attribute__((pure));
uint32_t ntp_be32dec(const void *buf) __attribute__((pure));
uint64_t ntp_be64dec(const void *buf) __attribute__((pure));

#endif
