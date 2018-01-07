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

#include <openssl/objects.h>
#include <openssl/evp.h>

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
	int	keytype;
	char	buf[512];		/* lots of room for line */
	uint8_t	keystr[32];		/* Bug 2537 */
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
	while ((line = fgets(buf, sizeof buf, fp)) != NULL) {
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
		 * Next is keytype. See if that is all right.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: no key type for key %u", keyno);
			continue;
		}
		/*
		 * The key type is the NID used by the message digest 
		 * algorithm. There are a number of inconsistencies in
		 * the OpenSSL database. We attempt to discover them
		 * here and prevent use of inconsistent data later.
		 *
		 * OpenSSL digest short names are capitalized, so uppercase the
		 * digest name before passing to OBJ_sn2nid().  If it is not
		 * recognized but begins with 'M' use NID_md5 to be consistent
		 * with past behavior.
		 */
		char *upcased;
		char *pch;
		upcased = lib_getbuf();
		strlcpy(upcased, token, LIB_BUFLENGTH);
		for (pch = upcased; '\0' != *pch; pch++)
			*pch = (char)toupper((unsigned char)*pch);

		keytype = OBJ_sn2nid(upcased);
		if ((NID_undef == keytype) && ('M' == upcased[0]))
			keytype = NID_md5;
		if (NID_undef == keytype) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: invalid type for key %u, %s",
				keyno, token);
			continue;
		}
		if (EVP_get_digestbynid(keytype) == NULL) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: no algorithm for key %u, %s",
				keyno, token);
			continue;
		}

		/*
		 * Finally, get key and insert it. If it is longer than 20
		 * characters, it is a binary string encoded in hex;
		 * otherwise, it is a text string of printable ASCII
		 * characters.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "AUTH: authreadkeys: no key for key %u", keyno);
			continue;
		}
		len = strlen(token);
		if (len <= 20) {	/* Bug 2537 */
			mac_setkey(keyno, keytype, (uint8_t *)token, len);
			keys++;
		} else {
			char	hex[] = "0123456789abcdef";
			size_t	jlim;

			jlim = min(len, 2 * sizeof(keystr));
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
			mac_setkey(keyno, keytype, keystr, jlim / 2);
			keys++;
		}
	}
	fclose(fp);
	msyslog(LOG_ERR, "AUTH: authreadkeys: added %d keys", keys);
	return true;
}
