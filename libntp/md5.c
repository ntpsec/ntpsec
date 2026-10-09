/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * MD5 (RFC 1321), only for the IPv6 refid.  RFC 5905 fixes the
 * algorithm there and it protects nothing, so it is computed here
 * rather than asked of OpenSSL, whose providers may not offer MD5
 * (FIPS).  Do not use this for anything that needs a secure hash.
 */

#include "config.h"

#include <stdint.h>
#include <string.h>

#include "ntp_stdlib.h"

static const uint32_t md5_k[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const uint8_t md5_s[64] = {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static void
md5_block(
	uint32_t h[4],
	const uint8_t *p
	)
{
	uint32_t m[16];
	uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
	uint32_t f, t;
	unsigned int i, g;

	for (i = 0; i < 16; i++) {
		m[i] = (uint32_t)p[4*i] | (uint32_t)p[4*i + 1] << 8 |
		    (uint32_t)p[4*i + 2] << 16 | (uint32_t)p[4*i + 3] << 24;
	}
	for (i = 0; i < 64; i++) {
		if (i < 16) {
			f = (b & c) | (~b & d);
			g = i;
		} else if (i < 32) {
			f = (d & b) | (~d & c);
			g = (5*i + 1) % 16;
		} else if (i < 48) {
			f = b ^ c ^ d;
			g = (3*i + 5) % 16;
		} else {
			f = c ^ (b | ~d);
			g = (7*i) % 16;
		}
		t = a + f + md5_k[i] + m[g];
		a = d;
		d = c;
		c = b;
		b += (t << md5_s[i]) | (t >> (32 - md5_s[i]));
	}
	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
}

void
ntp_md5(
	const void *data,
	size_t len,
	uint8_t *digest
	)
{
	uint32_t h[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
	const uint8_t *p = data;
	uint64_t bits = (uint64_t)len * 8;
	uint8_t tail[128];
	size_t tlen;
	unsigned int i;

	for (; len >= 64; len -= 64, p += 64) {
		md5_block(h, p);
	}
	memset(tail, 0, sizeof(tail));
	memcpy(tail, p, len);
	tail[len] = 0x80;
	tlen = (len < 56) ? 64 : 128;
	for (i = 0; i < 8; i++) {
		tail[tlen - 8 + i] = (uint8_t)(bits >> (8*i));
	}
	md5_block(h, tail);
	if (128 == tlen) {
		md5_block(h, tail + 64);
	}
	for (i = 0; i < NTP_MD5_LENGTH; i++) {
		digest[i] = (uint8_t)(h[i/4] >> (8*(i%4)));
	}
}
