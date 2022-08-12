/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>

#include <openssl/opensslv.h>
#include <openssl/rand.h>

#include "config.h"
#include "ntp.h"

/* NB: RAND_bytes comes from OpenSSL
 * Starting in version 1.1.1, it reseeds itself occasionally.
 * That may need access to /dev/urandom which may be blocked by chroot jails.
 * getrandom(2) is used when available.  It was added to Linux kernel 3.17
 * so this won't be a problem on newer Linux systems.
 */

void ntp_RAND_bytes(unsigned char *buf, int num) {
	int err;
	err = RAND_bytes(buf, num);
	if (1 != err) {
		msyslog(LOG_ERR, "ERR: RAND_bytes failed");
		exit(1);
	}
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
