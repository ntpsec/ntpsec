/*
 * ntp_crypto.h - definitions for cryptographic operations
 */
#ifndef GUARD_NTP_CRYPTO_H
#define GUARD_NTP_CRYPTO_H

/*
 * Configuration codes
 */
#define CRYPTO_CONF_NONE  0	/* nothing doing */
#define CRYPTO_CONF_PRIV  1	/* host name */
#define CRYPTO_CONF_IDENT 2	/* group name */
#define CRYPTO_CONF_CERT  3	/* certificate file name */
#define CRYPTO_CONF_RAND  4	/* random seed file name */
#define CRYPTO_CONF_IFFPAR 5	/* IFF parameters file name */
#define CRYPTO_CONF_GQPAR 6	/* GQ parameters file name */
#define	CRYPTO_CONF_MVPAR 7	/* MV parameters file name */
#define CRYPTO_CONF_PW	  8	/* private key password */
#define	CRYPTO_CONF_NID   9	/* specify digest name */

#endif /* GUARD_NTP_CRYPTO_H */
