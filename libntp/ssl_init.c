/*
 * ssl_init.c	Common OpenSSL initialization code for the various
 *		programs which use it.
 *
 * Moved from ntpd/ntp_crypto.c crypto_setup()
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <ntp.h>
#include <ntp_debug.h>

#ifndef OPENSSL
int ssl_init__non_empty_compliation_unit;
#else	/* OPENSSL follows */
#include "openssl/err.h"
#include "openssl/rand.h"


int ssl_init_done;

void
ssl_init(void)
{
	if (ssl_init_done)
		return;

	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();

	ssl_init_done = 1;
}


void
ssl_check_version(void)
{
	if ((SSLeay() ^ OPENSSL_VERSION_NUMBER) & ~0xff0L) {
		msyslog(LOG_ERR,
		    "OpenSSL version mismatch. Built against %lx, you have %lx",
		    OPENSSL_VERSION_NUMBER, SSLeay());
		fprintf(stderr,
		    "OpenSSL version mismatch. Built against %lx, you have %lx\n",
		    OPENSSL_VERSION_NUMBER, SSLeay());
		exit (-1);
	}

	INIT_SSL();
}
#endif	/* OPENSSL */
