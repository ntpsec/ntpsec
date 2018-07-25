/*
 * authreadkeys.c - routines to support the reading of the key file
 */
#include "config.h"
#include <stdio.h>
#include <ctype.h>

#include "ntp.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"
#include "ntp_auth.h"

#include <openssl/objects.h>
#include <openssl/evp.h>

#include <openssl/cmac.h>

#define NAMEBUFSIZE 100

/* Forwards */
static char *nexttok (char **);

/*
 * nexttok - basic internal tokenizing routine
 */
static char *
nexttok(
	char	**str
	)
{
	char *cp;
	char *starttok;

	cp = *str;

	/*
	 * Space past white space
	 */
	while (*cp == ' ' || *cp == '\t')
		cp++;

	/*
	 * Save this and space to end of token
	 */
	starttok = cp;
	while (*cp != '\0' && *cp != '\n' && *cp != ' '
	       && *cp != '\t' && *cp != '#')
		cp++;

	/*
	 * If token length is zero return an error, else set end of
	 * token to zero and return start.
	 */
	if (starttok == cp)
		return NULL;

	if (*cp == ' ' || *cp == '\t')
		*cp++ = '\0';
	else
		*cp = '\0';

	*str = cp;
	return starttok;
}

static char*
try_cmac(const char *upcased, char* namebuf) {
	strncpy(namebuf, upcased, NAMEBUFSIZE);
	if ((strcmp(namebuf, "AES") == 0) || (strcmp(namebuf, "AES128CMAC") == 0))
		strncpy(namebuf, "AES-128", NAMEBUFSIZE);
	strlcat(namebuf, "-CBC", NAMEBUFSIZE);
	namebuf[NAMEBUFSIZE-1] = '\0';
	if (0) msyslog(LOG_INFO, "DEBUG try_cmac: %s=>%s", upcased, namebuf);
	if (EVP_get_cipherbyname(namebuf) == NULL)
		return NULL;
	return namebuf;
}

static char*
try_digest(char *upcased, char *namebuf) {
	strncpy(namebuf, upcased, NAMEBUFSIZE);
	if (EVP_get_digestbyname(namebuf) != NULL)
	  return namebuf;
	if ('M' == upcased[0]) {
		/* hack for backward compatibility */
		strncpy(namebuf, "MD5", NAMEBUFSIZE);
		if (EVP_get_digestbyname(namebuf) != NULL)
	 		return namebuf;
	}
	return NULL;
}

static void
check_digest_mac_length(
	keyid_t keyno,
	char *name) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    EVP_MD_CTX *ctx;
    const EVP_MD *md;

    md = EVP_get_digestbyname(name);
    ctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestFinal_ex(ctx, digest, &length);
    EVP_MD_CTX_destroy(ctx);

    if (MAX_BARE_MAC_LENGTH < length) {
	msyslog(LOG_ERR, "AUTH: authreadkeys: digest for key %u, %s will be truncated.", keyno, name);
    }
}

static void
check_cmac_mac_length(
	keyid_t keyno,
	char *name) {
    unsigned char mac[CMAC_MAX_MAC_LENGTH+1024];
    size_t length = 0;
    char key[EVP_MAX_KEY_LENGTH];  /* garbage is OK */
    CMAC_CTX *ctx;
    const EVP_CIPHER *cipher = EVP_get_cipherbyname(name);

    ctx = CMAC_CTX_new();
    CMAC_Init(ctx, key, EVP_CIPHER_key_length(cipher), cipher, NULL);
    CMAC_Final(ctx, mac, &length);
    CMAC_CTX_free(ctx);

    /* CMAC_MAX_MAC_LENGTH isn't in API
     * Check here to avoid buffer overrun in cmac_decrypt and cmac_encrypt
     */
    if (CMAC_MAX_MAC_LENGTH < length) {
	msyslog(LOG_ERR,
		"AUTH: authreadkeys: CMAC for key %u, %s is too big: %lu",
		keyno, name, (long unsigned int)length);
	exit(1);
    }

    if (MAX_BARE_MAC_LENGTH < length) {
	msyslog(LOG_ERR, "AUTH: authreadkeys: CMAC for key %u, %s will be truncated.", keyno, name);
    }
}

/* check_mac_length - Check for CMAC/digest too long.
 * maybe should check for too short.
 */
static void
check_mac_length(
	keyid_t keyno,
	AUTH_Type type,
	char * name,
	char *upcased) {
    switch (type) {
	case AUTH_CMAC:
	    check_cmac_mac_length(keyno, name);
	    break;
    	case AUTH_DIGEST:
	    check_digest_mac_length(keyno, name);
	    break;
    	default:
	    msyslog(LOG_ERR, "BUG: authreadkeys: unknown AUTH type for key %u, %s", keyno, upcased);
    }
}

/* check_cmac_key_length: check and fix CMAC key length
 * Ciphers require a specific key length.
 * Truncate or pad if necessary.
 * This may modify the key string - assumes enough storage.
 * AES-128 is 128 bits or 16 bytes.
 */
static int
check_cmac_key_length(
	keyid_t keyno,
	char *name,
	char *key,
	int keylength) {
    const EVP_CIPHER *cipher = EVP_get_cipherbyname(name);
    int len = EVP_CIPHER_key_length(cipher);
    int i;

    if (len < keylength) {
	    msyslog(LOG_ERR, "AUTH: CMAC key %u will be truncated %d=>%d",
		keyno, keylength, len);
    } else if ( len > keylength) {
	    msyslog(LOG_ERR, "AUTH: CMAC key %u will be padded %d=>%d",
		keyno, keylength, len);
	    for (i=keylength; i<len; i++) key[i] = 0;
    } else {
	    if (0) msyslog(LOG_ERR, "AUTH: CMAC key %u is right size", keyno);
    }

    return len;
}

static int
check_key_length(
	keyid_t keyno,
	AUTH_Type type,
	char *name,
	char *key,
	int keylength) {
    int length = keylength;
    switch (type) {
	case AUTH_CMAC:
	    length = check_cmac_key_length(keyno, name, key, keylength);
	    break;
    	case AUTH_DIGEST:
	    /* any length key works */
	    break;
    	default:
	    msyslog(LOG_ERR, "BUG: authreadkeys: unknown AUTH type for key %u", keyno);
    }
    return length;
}


/*
 * authreadkeys - (re)read keys from a file.
 */
bool
authreadkeys(
	const char *file
	)
{
	FILE	*fp;
	char	*line;
	keyid_t	keyno;
	AUTH_Type type;
	char	buf[512];		/* lots of room for line */
	uint8_t	keystr[32];		/* Bug 2537 */
	char *	name;
	char	namebuf[NAMEBUFSIZE];
	size_t	len;
	size_t	j;
	int	keys = 0;

	/*
	 * Open file.  Complain and return if it can't be opened.
	 */
	fp = fopen(file, "r");
	if (fp == NULL) {
		msyslog(LOG_ERR, "AUTH: authreadkeys: file %s: %m",
		    file);
		return false;
	}
	ssl_init();
msyslog(LOG_ERR, "AUTH: authreadkeys: reading %s", file);

	/*
	 * Remove all existing keys
	 */
	auth_delkeys();

	/*
	 * Now read lines from the file, looking for key entries
	 */
	while ((line = fgets(buf, sizeof(buf), fp)) != NULL) {
		char *token = nexttok(&line);
		if (token == NULL)
			continue;

		/*
		 * First is key number.  See if it is okay.
		 */
		keyno = (keyid_t)atoi(token);
		if (keyno == 0) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: cannot change key %s", token);
			continue;
		}

		if (keyno > NTP_MAXKEY) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: key %s > %d reserved",
			    token, NTP_MAXKEY);
			continue;
		}

		/*
		 * Next is CMAC or Digest type. See if that is all right.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: no key type for key %u", keyno);
			continue;
		}
		/*
		 * This area used to pass around nids.
		 * There is a bug in older versions of libcrypt
		 * where some nid routine doesn't work so AES
		 * didn't work on old CentOS, NetBSD, or FreeBSD.
		 * 2018-Jun-08
		 *
		 * OpenSSL names are capitalized.
		 * So uppercase the name before passing to
		 * EVP_get_digestbyname() or EVP_get_cipherbyname().
		 *
		 * AES is short for AES-128.
		 * CMAC names get "-CBC" appended.
		 * ntp classic uses AES128CMAC, so we support that too.
		 *
		 * If it is not recognized but begins with 'M' use
		 * NID_md5 digest to be consistent with past behavior.
		 * Try CMAC names first to dodge this hack in case future
		 * cipher names begin with M.
		 */
		char *upcased;
		char *pch;
		upcased = lib_getbuf();
		strlcpy(upcased, token, LIB_BUFLENGTH);
		for (pch = upcased; '\0' != *pch; pch++)
			*pch = (char)toupper((unsigned char)*pch);

		name = NULL;
		if (NULL == name) {
			name = try_cmac(upcased, namebuf);
			if (NULL != name)
				type = AUTH_CMAC;
		}
		if (NULL == name) {
			name = try_digest(upcased, namebuf);
			if (NULL != name)
				type = AUTH_DIGEST;
		}
                if (NULL == name) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: unknown auth type %u, %s",
				keyno, token);
                        continue;
                }



		/*
		 * Finally, get key and insert it.
		 * If it is longer than 20 characters, it is a binary
		 * string encoded in hex; otherwise, it is a text string
		 * of printable ASCII characters.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: no key for key %u", keyno);
			continue;
		}
		len = strlen(token);
		if (len <= 20) {	/* Bug 2537 */
			len = check_key_length(keyno, type, name, upcased, len);
			check_mac_length(keyno, type, name, upcased);
			auth_setkey(keyno, type, name, (uint8_t *)token, len);
			keys++;
		} else {
			char	hex[] = "0123456789abcdef";
			size_t	jlim;

			jlim = len;
			if ((2*sizeof(keystr)) < jlim) {
			  jlim =  2 * sizeof(keystr);
			  msyslog(LOG_ERR,
			    "AUTH: authreadkeys: key %u trucated to %u bytes",
			    keyno, (unsigned int)jlim);

			}
			for (j = 0; j < jlim; j++) {
				char *ptr = strchr(hex, tolower((unsigned char)token[j]));
				if (ptr == NULL)
					break;	/* abort decoding */
				uint8_t temp = (uint8_t)(ptr - hex);
				if (j & 1)
					keystr[j / 2] |= temp;
				else
					keystr[j / 2] = (uint8_t)(temp << 4);
			}
			if (j < jlim) {
			    msyslog(LOG_ERR,
				"AUTH: authreadkeys: invalid hex digit for key %u",
				keyno);
			    continue;
			}
			len = jlim / 2;
			len = check_key_length(keyno, type, name, upcased, len);
			check_mac_length(keyno, type, name, upcased);
			auth_setkey(keyno, type, name, keystr, len);
			keys++;
		}
	}
	fclose(fp);
	msyslog(LOG_ERR, "AUTH: authreadkeys: added %d keys", keys);
	return true;
}
