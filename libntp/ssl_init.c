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

#include "libssl_compat.h"

void	atexit_ssl_cleanup(void);

bool ssl_init_done;

void
ssl_init(void)
{
	init_lib();

	if (ssl_init_done)
		return;

	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
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
 * Used by ntpd authreadkeys(), ntpq keytype()
 */
int
keytype_from_text(
	const char *text,
	size_t *pdigest_len
	)
{
	int		key_type;
	u_int		digest_len;
	const u_long	max_digest_len = MAX_MAC_LEN - sizeof(keyid_t);
	uint8_t		digest[EVP_MAX_MD_SIZE];
	char *		upcased;
	char *		pch;
	EVP_MD_CTX	*ctx;

	/*
	 * OpenSSL digest short names are capitalized, so uppercase the
	 * digest name before passing to OBJ_sn2nid().  If it is not
	 * recognized but begins with 'M' use NID_md5 to be consistent
	 * with past behavior.
	 */
	INIT_SSL();
	LIB_GETBUF(upcased);
	strlcpy(upcased, text, LIB_BUFLENGTH);
	for (pch = upcased; '\0' != *pch; pch++)
		*pch = (char)toupper((unsigned char)*pch);
	key_type = OBJ_sn2nid(upcased);

	if (!key_type && 'm' == tolower((unsigned char)text[0]))
		key_type = NID_md5;

	if (!key_type)
		return 0;

	if (NULL != pdigest_len) {
		ctx = EVP_MD_CTX_new();
		EVP_DigestInit(ctx, EVP_get_digestbynid(key_type));
		EVP_DigestFinal(ctx, digest, &digest_len);
		EVP_MD_CTX_free(ctx);
		if (digest_len > max_digest_len) {
			fprintf(stderr,
				"key type %s %u octet digests are too big, max %lu\n",
				keytype_name(key_type), digest_len,
				max_digest_len);
			msyslog(LOG_ERR,
				"key type %s %u octet digests are too big, max %lu",
				keytype_name(key_type), digest_len,
				max_digest_len);
			return 0;
		}
		*pdigest_len = digest_len;
	}

	return key_type;
}


/*
 * keytype_name		returns OpenSSL short name for digest by NID.
 *
 * Used by ntpq keytype()
 */
const char *
keytype_name(
	int nid
	)
{
	static const char unknown_type[] = "(unknown key type)";
	const char *name;

	INIT_SSL();
	name = OBJ_nid2sn(nid);
	if (NULL == name)
		name = unknown_type;
	return name;
}

