/*
 * ntp_crypto.h - definitions for cryptographic operations
 */
#ifdef OPENSSL
#include "openssl/evp.h"

/*
 * The following bits are set by the CRYPTO_ASSOC message from
 * the server and are not modified by the client.
 */
#define CRYPTO_FLAG_ENAB  0x0001 /* crypto enable */
#define CRYPTO_FLAG_KEY	  0x0002 /* host keys */
#define CRYPTO_FLAG_SIGN  0x0004 /* signature keys */
#define CRYPTO_FLAG_TAI	  0x0008 /* leapseconds table */
#define CRYPTO_FLAG_HOST  (CRYPTO_FLAG_ENAB | CRYPTO_FLAG_KEY |\
			  CRYPTO_FLAG_SIGN | CRYPTO_FLAG_TAI)

/*
 * The following bits are used by the client during the protocol
 * exchange.
 */
#define CRYPTO_FLAG_PROV  0x0100 /* certificate verified */
#define CRYPTO_FLAG_AGREE 0x0200 /* cookie verifed */
#define CRYPTO_FLAG_AUTO  0x0400 /* autokey verified */
#define CRYPTO_FLAG_LEAP  0x0800 /* leapseconds table verified */
#define	CRYPTO_FLAG_VRFY  0x1000 /* signed certificate verified */
#define CRYPTO_FLAG_TRUST 0x2000 /* someone set the trust bit */

/*
 * The following flags are used for certificate management.
 */
#define CERT_SIGN       0x0001  /* certificate is signed */
#define CERT_VALID      0x0002  /* certificate is valid */

/*
 * Extension field definitions
 */
#define	CRYPTO_MAXLEN	1024	/* max extension field length */
#define CRYPTO_VN	2	/* current protocol version number */
#define CRYPTO_CMD(x)	(((CRYPTO_VN << 8) | (x)) << 16)
#define CRYPTO_NULL	CRYPTO_CMD(0) /* no operation */
#define CRYPTO_ASSOC	CRYPTO_CMD(1) /* association */
#define CRYPTO_CERT	CRYPTO_CMD(2) /* certificate */
#define CRYPTO_COOK	CRYPTO_CMD(3) /* cookie value */
#define CRYPTO_AUTO	CRYPTO_CMD(4) /* autokey values */
#define CRYPTO_TAI	CRYPTO_CMD(5) /* leapseconds table */
#define	CRYPTO_SIGN	CRYPTO_CMD(6) /* certificate sign */
#define CRYPTO_RESP	0x80000000 /* response */
#define CRYPTO_ERROR	0x40000000 /* error */

/*
 * Autokey event codes
 */
#define XEVNT_CMD(x)	(CRPT_EVENT | (x))
#define XEVNT_OK	XEVNT_CMD(0) /* success */
#define XEVNT_LEN	XEVNT_CMD(1) /* bad field format or length */
#define XEVNT_TSP	XEVNT_CMD(2) /* bad timestamp */
#define XEVNT_FSP	XEVNT_CMD(3) /* bad filestamp */
#define XEVNT_PUB	XEVNT_CMD(4) /* bad or missing public key */
#define XEVNT_MD	XEVNT_CMD(5) /* unsupported digest type */
#define XEVNT_KEY	XEVNT_CMD(6) /* mismatched digest types */
#define XEVNT_SGL	XEVNT_CMD(7) /* bad signature length */
#define XEVNT_SIG	XEVNT_CMD(8) /* signature not verified */
#define XEVNT_VFY	XEVNT_CMD(9) /* certificate not verified */
#define XEVNT_PER	XEVNT_CMD(10) /* certificate expired */
#define XEVNT_CKY	XEVNT_CMD(11) /* bad or missing cookie */
#define XEVNT_DAT	XEVNT_CMD(12) /* bad or missing leapseconds table */
#define XEVNT_CRT	XEVNT_CMD(13) /* bad or missing certificate */
/*
 * Configuration codes
 */
#define CRYPTO_CONF_NONE  0	/* nothing doing */
#define CRYPTO_CONF_PRIV  1	/* host keys file name */
#define CRYPTO_CONF_SIGN  2	/* signature keys file name */
#define CRYPTO_CONF_LEAP  3	/* leapseconds table file name */
#define CRYPTO_CONF_KEYS  4	/* keys directory path */
#define CRYPTO_CONF_CERT  5	/* certificate file name */
#define CRYPTO_CONF_RAND  6	/* random seed file name */
#define	CRYPTO_CONF_TRST  7	/* specify trust */

/*
 * Cryptographic values
 */
extern	u_int	crypto_flags;	/* status word */
extern	struct value hostval;	/* host name/value */
extern	struct cert_info *cinfo; /* host certificate information */
extern	struct value dhparam;	/* agreement parameters */
extern	struct value dhpub;	/* public value */
extern	struct value tai_leap;	/* leapseconds table */
extern	u_int	sys_tai;	/* current UTC offset from TAI */
#endif /* OPENSSL */
