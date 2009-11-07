/*
 * authreadkeys.c - routines to support the reading of the key file
 */
#include <config.h>
#include <stdio.h>
#include <ctype.h>

#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#ifdef OPENSSL
#include "openssl/objects.h"
#endif /* OPENSSL */

/* Forwards */
static char *nexttok (char **);

/*
 * nexttok - basic internal tokenizing routine
 */
static char *
nexttok(
	char **str
	)
{
	register char *cp;
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
	    return (NULL);
	
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
int
authreadkeys(
	const char *file
	)
{
	FILE *fp;
	char *line;
	char *token, *keystr;
	keyid_t keyno;
	int keytype;
	char buf[512];		/* lots of room for line */

	/*
	 * Open file.  Complain and return if it can't be opened.
	 */
	fp = fopen(file, "r");
	if (fp == NULL) {
		msyslog(LOG_ERR, "authreadkeys: file %s: %m",
		    file);
		return (0);
	}
#ifdef OPENSSL
	OpenSSL_add_all_algorithms();
#endif /* OPENSSL */


	/*
	 * Remove all existing keys
	 */
	auth_delkeys();

	/*
	 * Now read lines from the file, looking for key entries
	 */
	while ((line = fgets(buf, sizeof buf, fp)) != NULL) {
		token = nexttok(&line);
		if (token == NULL)
			continue;
		
		/*
		 * First is key number.  See if it is okay.
		 */
		keyno = atoi(token);
		if (keyno == 0) {
			msyslog(LOG_ERR,
			    "authreadkeys: cannot change key %s", token);
			continue;
		}

		if (keyno > NTP_MAXKEY) {
			msyslog(LOG_ERR,
			    "authreadkeys: key %s > %d reserved for Autokey",
			    token, NTP_MAXKEY);
			continue;
		}

		/*
		 * Next is keytype. See if that is all right.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "authreadkeys: no key type for key %d", keyno);
			continue;
		}
#ifdef OPENSSL

		/*
		 * If the key type is 'M' or 'm', it is replaced by 'MD5".
		 * In any case, it must be one of the algorithms supported
		 * by OpenSSL. The key type is the NID used by the message
		 * digest algorithm. Ther are a number of inconsistencies in
		 * the OpenSSL database. We attempt to discover them here
		 * and prevent use of inconsistent data.
		 */
		if (strcmp(token, "M") == 0 || strcmp(token, "m") == 0)
			token  = "MD5";
		keytype = OBJ_sn2nid(token);
		if (keytype == 0 || keytype > 255) {
			msyslog(LOG_ERR,
			    "authreadkeys: invalid type for key %d", keyno);
			continue;
		}
		if (EVP_get_digestbynid(keytype) == NULL) {
			msyslog(LOG_ERR,
			    "authreadkeys: no algorithm for key %d", keyno);
			continue;
		}
#else /* OPENSSL */

		/*
		 * The key type is unused, but is required to be 'M' or
		 * 'm' for compatibility.
		 */
		if (!(*token == 'M' || *token == 'm')) {
			msyslog(LOG_ERR,
			    "authreadkeys: invalid type for key %d", keyno);
			continue;
		}
		keytype = KEY_TYPE_MD5;
#endif /* OPENSSL */
		keystr = token;

		/*
		 * Finally, get key and insert it
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "authreadkeys: no key for key %d", keyno);
			continue;
		}
		MD5auth_setkey(keyno, keytype, token, strlen(token));
	}
	fclose(fp);
	return (1);
}
