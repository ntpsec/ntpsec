/*
 * DES interface for rsaref2.0
 *
 * These routines implement an interface for the RSA Laboratories
 * implementation of the Data Encryption Standard (DES) algorithm
 * operating in Cipher-Block Chaining (CBC) mode. This algorithm is
 * included in the rsaref2.0 package available from RSA in the US and
 * foreign countries. Further information is available at www.rsa.com.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include "ntp_types.h"
#include "ntp_string.h"
#include "ntp_stdlib.h"

#ifdef DES

/*
 * DESauthencrypt - generate DES-CBC message authenticator
 *
 * Returns length of authenticator field.
 */
int
DESauthencrypt(
	u_char *key,		/* key pointer */
	u_int32 *pkt,		/* packet pointer */
	int length		/* packet length */
	)
{
}


/*
 * DESauthdecrypt - verify DES message authenticator
 *
 * Returns one if authenticator valid, zero if invalid.
 */
int
DESauthdecrypt(
	u_char *key,		/* key pointer */
	u_int32 *pkt,		/* packet pointer */
	int length, 		/* packet length */
	int size		/* size of MAC field */
	)
{
}
#else
int authencrypt_bs;
#endif /* DES */
