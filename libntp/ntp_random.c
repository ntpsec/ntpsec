/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>
#include <string.h>

#include <openssl/opensslv.h>
#include <openssl/rand.h>

#include "config.h"
#include "ntp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"

#ifndef PRAND_BUF_LEN
#define PRAND_BUF_LEN 4096l
#endif
static uint8_t prand_buffer[PRAND_BUF_LEN];
static size_t prand_burned = PRAND_BUF_LEN;

/* NB: RAND_bytes comes from OpenSSL
 * Starting in version 1.1.1, it reseeds itself occasionally.
 * That may need access to /dev/urandom which may be blocked by chroot jails.
 * getrandom(2) is used when available.  It was added to Linux kernel 3.17
 * so this won't be a problem on newer Linux systems.
 */

/* Use psuedorandom pool to get entropy from */
static void ntp_pool_rand_fill(uint8_t *buf, int num) {
	if (0 > num) {
		return;
	}
	if (1 != RAND_bytes(buf, num)) {
		msyslog(LOG_ERR, "ERR: RAND_bytes failed");
		exit(1);
	}
}

void ntp_RAND_bytes(unsigned char *buf, int num) {
	if (0 > num) {
		return;
	}
	if (PRAND_BUF_LEN < num) {  // This should never happen
		return ntp_pool_rand_fill(buf, num);
	}
	if (PRAND_BUF_LEN < num + prand_burned) {
		ntp_pool_rand_fill(&prand_buffer[0], prand_burned);
		prand_burned = 0;
	}
	memcpy(buf, &prand_buffer[0] + prand_burned, num);
	prand_burned += num;
}

void ntp_RAND_priv_bytes(unsigned char *buf, int num) {
	int err;
#if (OPENSSL_VERSION_NUMBER > 0x1010100fL) && !defined(LIBRESSL_VERSION_NUMBER)
	err = RAND_priv_bytes(buf, num);
#else
	err = RAND_bytes(buf, num);
#endif
	if (1 != err) {
		msyslog(LOG_ERR, "ERR: RAND_priv_bytes failed");
		exit(1);
	}
}

/* Return four byes of entropy. This differs from the ~31bits from random() */

uint32_t ntp_random(void) {
	uint32_t ret;
	ntp_RAND_bytes((uint8_t*)&ret, sizeof(ret));
	return ret;
}
