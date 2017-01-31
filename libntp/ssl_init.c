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
#include <lib_strbuf.h>

#include <openssl/err.h>
#include <openssl/evp.h>

void	atexit_ssl_cleanup(void);

static bool ssl_init_done;

void
ssl_init(void)
{
	if (ssl_init_done)
		return;

	init_lib();

	ERR_load_crypto_strings();
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
	ERR_free_strings();
}


/*
 * keytype_from_text	returns OpenSSL NID for digest by name, and
 *			optionally the associated digest length.
 *
 * Used by ntpd authreadkeys()
 */
int
keytype_from_text(
	const char *text
	)
{
	int		key_type;
	char *		upcased;
	char *		pch;

	/*
	 * OpenSSL digest short names are capitalized, so uppercase the
	 * digest name before passing to OBJ_sn2nid().  If it is not
	 * recognized but begins with 'M' use NID_md5 to be consistent
	 * with past behavior.
	 */
	ssl_init();
	LIB_GETBUF(upcased);
	strlcpy(upcased, text, LIB_BUFLENGTH);
	for (pch = upcased; '\0' != *pch; pch++)
		*pch = (char)toupper((unsigned char)*pch);
	key_type = OBJ_sn2nid(upcased);

	if (!key_type && 'm' == tolower((unsigned char)text[0]))
		key_type = NID_md5;

	if (!key_type)
		return 0;

	return key_type;
}
