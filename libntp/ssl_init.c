/*
 * ssl_init.c	Common OpenSSL initialization code for the various
 *		programs which use it.
 *
 * Moved from ntpd/ntp_crypto.c crypto_setup()
 */

#include <config.h>

#include <unistd.h>
#include <ctype.h>
#include <ntp.h>
#include <ntp_debug.h>

#include <openssl/evp.h>

void	atexit_ssl_cleanup(void);

static bool ssl_init_done;

void
ssl_init(void)
{
	if (ssl_init_done)
		return;

	init_lib();

	OpenSSL_add_all_digests();
	atexit(&atexit_ssl_cleanup);

	ssl_init_done = true;
}


void
atexit_ssl_cleanup(void)
{
	if (!ssl_init_done)
		return;

	ssl_init_done = false;
	EVP_cleanup();
}
