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
#endif /* OPENSSL *.

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
	char *token;
	u_long keyno;
	int keytype;
	char buf[512];		/* lots of room for line */

	/*
	 * Open file.  Complain and return if it can't be opened.
	 */
	fp = fopen(file, "r");
	if (fp == NULL) {
		msyslog(LOG_ERR, "can't open key file %s: %m", file);
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
			    "cannot change key %s", token);
			continue;
		}

		if (keyno > NTP_MAXKEY) {
			msyslog(LOG_ERR,
			    "key %s > %d reserved for autokeyd", token,
			    NTP_MAXKEY);
			continue;
		}

		/*
		 * Next is keytype.  See if that is all right.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			msyslog(LOG_ERR,
			    "no key type for key %ld", keyno);
			continue;
		}
#ifdef OPENSSL

		/*
		 * If the key type is 'M' or 'm', it is replaced by 'MD5".
		 * In any case, it must be one of the algorithms supported
		 * by OpenSSL. The key type is the NID used by the message
		 * digest algorithm.
		 */
		if (*token == 'M' || *token == 'm')
			token  = "MD5";
		keytype = OBJ_sn2nid(token);
		if (keytype == 0) {
			msyslog(LOG_ERR,
			    "invalid key type for key %ld", keyno);
			continue;
		}
#else	/* !OPENSSL */

		/*
		 * The key type is unused, but is required to be 'M' or
		 * 'm' for compatibility.
		 */
		if (!(*token == 'M' || *token == 'm')) {
			msyslog(LOG_ERR,
			    "invalid key type for key %ld", keyno);
			continue;
		}
		keytype = NID_md5;
#endif /* !OPENSSL */

		/*
		 * Finally, get key and insert it
		 */
		token = nexttok(&line);
		if (token == NULL)
			msyslog(LOG_ERR,
			    "no key for key %ld", keyno);
		else
			MD5auth_setkey(keyno, keytype, token,
			    strlen(token));
	}
	fclose(fp);
	return (1);
}
