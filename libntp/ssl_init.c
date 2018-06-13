/*
 * ssl_init.c	Common OpenSSL initialization code for the various
 *		programs which use it.
 *
 * Moved from ntpd/ntp_crypto.c crypto_setup()
 */

#include "config.h"
#include "ntp_stdlib.h"

#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/cmac.h>

#ifndef EVP_MD_CTX_new
/* Slightly older version of OpenSSL */
/* Similar hack in macencrypt.c and attic/digest-timing.c */
#define EVP_MD_CTX_new() EVP_MD_CTX_create()
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
static void	atexit_ssl_cleanup(void);
#endif

static bool ssl_init_done;
EVP_MD_CTX *digest_ctx;
CMAC_CTX *cmac_ctx;

void
ssl_init(void)
{
	if (ssl_init_done)
		return;

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	OpenSSL_add_all_digests();
	OpenSSL_add_all_ciphers();
	atexit(&atexit_ssl_cleanup);
#endif

	digest_ctx = EVP_MD_CTX_new();
	cmac_ctx = CMAC_CTX_new();
	ssl_init_done = true;
}


#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
static void
atexit_ssl_cleanup(void)
{
	if (!ssl_init_done)
		return;

	ssl_init_done = false;
	EVP_cleanup();
}
#endif
