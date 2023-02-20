/* ssl_init.c	Common OpenSSL initialization code
 * This is needed for crypto as well as NTS
 */

#include "config.h"
#include "ntp_stdlib.h"
#include "ntp.h"

#include <stdbool.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>

#if OPENSSL_VERSION_NUMBER > 0x20000000L
#include <openssl/params.h>
#include <openssl/err.h>
#else
#include <openssl/cmac.h>
#endif

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
#if OPENSSL_VERSION_NUMBER > 0x20000000L
EVP_MAC_CTX *evp_ctx;
#else
CMAC_CTX *cmac_ctx;
#endif

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
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	{
	EVP_MAC *mac = EVP_MAC_fetch(NULL, "cmac", NULL);
	if (NULL == mac) {
		msyslog(LOG_ERR, "ssl_init: EVP_MAC_fetch failed");
		exit(1);
	}
	evp_ctx = EVP_MAC_CTX_new(mac);
	if (NULL == evp_ctx) {
		msyslog(LOG_ERR, "ssl_init: EVP_MAC_CTX_new failed");
		exit(1);
	}
	// EVP_MAC_free(mac);
	}
/* Barf: EVP_MAC_CTX_dup doesn't work until src gets cipher and key */
	{
	EVP_MAC_CTX *ctx;
	OSSL_PARAM params[3];
	char key[16];
	char copy[100];		/* OSSL_PARAM vs const */

	ctx = EVP_MAC_CTX_dup(evp_ctx);
	if (NULL != ctx ) {
		/* Was failing: 2023-feb-20, OpenSSL 3.0.8 */
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		msyslog(LOG_ERR, "ssl_init: first dup worked: %s", str);
	}

	strlcpy(copy, "AES-128-CBC", sizeof(copy));
	params[0] = OSSL_PARAM_construct_utf8_string("cipher", copy, 0);
	params[1] = OSSL_PARAM_construct_octet_string("key", key, 16);
	params[2] = OSSL_PARAM_construct_end();
	if (0 == EVP_MAC_CTX_set_params(evp_ctx, params)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		msyslog(LOG_ERR, "ssl_init: EVP_MAC_CTX_set_params() failed: %s", str);
	}


	ctx = EVP_MAC_CTX_dup(evp_ctx);
	if (NULL == ctx ) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		msyslog(LOG_ERR, "ssl_init: second dup failed: %s", str);
	}
	}
#else
	cmac_ctx = CMAC_CTX_new();
#endif
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
