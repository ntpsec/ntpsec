/* ssl_init.c	Common OpenSSL initialization code
 * This is needed for crypto as well as NTS
 */
#define OPENSSL_SUPPRESS_DEPRECATED 1

#include "config.h"
#include "ntp_stdlib.h"
#include "ntp.h"

#include <stdbool.h>
#include <openssl/ssl.h>
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
	unsigned char dummy;

	if (ssl_init_done) {
		return;
	}

#ifndef DISABLE_NTS
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS|OPENSSL_INIT_LOAD_CRYPTO_STRINGS|OPENSSL_INIT_ADD_ALL_CIPHERS|OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	OpenSSL_add_all_digests();
	OpenSSL_add_all_ciphers();
	atexit(&atexit_ssl_cleanup);
#endif

	/* More initialization help for seccomp */
	/* RAND_poll in OpenSSL on Raspbian needs get{u,g,eu,eg}id() */
	ntp_RAND_bytes(&dummy, 1);

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
