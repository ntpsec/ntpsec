/*
 *	MD5 interface for rsaref2.0
 *
 * These routines implement an interface for the RSA Laboratories
 * implementation of the Message Digest 5 (MD5) algorithm. This
 * algorithm is included in the rsaref2.0 package available from RSA in
 * the US and foreign countries. Further information is available at
 * www.rsa.com.
 */

#include "ntp_machine.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include "ntp_types.h"
#include "ntp_string.h"
#include "ntp_stdlib.h"
#include "global.h"
#include "md5.h"

/*
 * MD5authencrypt - generate MD5 message authenticator
 *
 * Returns length of authenticator field.
 */
int
MD5authencrypt(
	u_char *key,		/* key pointer */
	u_int32 *pkt,		/* packet pointer */
	int length		/* packet length */
	)
{
	MD5_CTX md5;
	u_char digest[16];

	/*
	 * MD5 with key identifier concatenated with packet.
	 */
	MD5Init(&md5);
	MD5Update(&md5, key, (u_int)cache_keylen);
	MD5Update(&md5, (u_char *)pkt, (u_int)length);
	MD5Final(digest, &md5);
	memmove((u_char *)pkt + length + 4, digest, 16);
	return (16 + 4);
}


/*
 * MD5authdecrypt - verify MD5 message authenticator
 *
 * Returns one if authenticator valid, zero if invalid.
 */
int
MD5authdecrypt(
	u_char *key,		/* key pointer */
	u_int32 *pkt,		/* packet pointer */
	int length,	 	/* packet length */
	int size		/* MAC size */
	)
{
	MD5_CTX md5;
	u_char digest[16];

	/*
	 * MD5 with key identifier concatenated with packet.
	 */
	MD5Init(&md5);
	MD5Update(&md5, key, (u_int)cache_keylen);
	MD5Update(&md5, (u_char *)pkt, (u_int)length);
	MD5Final(digest, &md5);
	if (size != 16 + 4)
		return (0);
	return (!memcmp(digest, (char *)pkt + length + 4, 16));
}
