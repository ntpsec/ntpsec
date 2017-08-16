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

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
static void	atexit_ssl_cleanup(void);
#endif

static bool ssl_init_done;

void
ssl_init(void)
{
	if (ssl_init_done)
		return;

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	OpenSSL_add_all_digests();
	atexit(&atexit_ssl_cleanup);
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
