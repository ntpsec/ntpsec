/*
 * ntp_crypto.h - definitions for cryptographic operations
 */
#ifdef AUTOKEY
#include "global.h"
#include "md5.h"
#ifdef PUBKEY
#include "rsaref.h"
#include "rsa.h"
#endif /* PUBKEY */

/*
 * Cryptostatus word
 */
#define CRYPTO_FLAG_ENAB  0x01	/* crypto enable */
#define CRYPTO_FLAG_RSA	  0x02	/* public/private keys */
#define CRYPTO_FLAG_DH	  0x04	/* agreement parameters */
#define CRYPTO_FLAG_TAI	  0x08	/* leapseconds table */

/*
 * Extension field definitions
 */
#define CRYPTO_VN	1	/* current protocol version number */

#define CRYPTO_NULL	((CRYPTO_VN << 8) | 0) /* no operation */
#define CRYPTO_STAT	((CRYPTO_VN << 8) | 1) /* status */
#define CRYPTO_ASSOC	((CRYPTO_VN << 8) | 2) /* association ID */
#define CRYPTO_AUTO	((CRYPTO_VN << 8) | 3) /* autokey values */
#define CRYPTO_PRIV	((CRYPTO_VN << 8) | 4) /* cookie value */
#define CRYPTO_DHPAR	((CRYPTO_VN << 8) | 5) /* agreement params */
#define CRYPTO_DH	((CRYPTO_VN << 8) | 6) /* public value */
#define CRYPTO_NAME	((CRYPTO_VN << 8) | 7) /* host name/pub key */
#define CRYPTO_TAI	((CRYPTO_VN << 8) | 8) /* leapseconds table */
#define CRYPTO_RESP	0x8000			/* response */
#define CRYPTO_ERROR	0x4000			/* error */

#ifdef PUBKEY
/*
 * Configuration codes
 */
#define CRYPTO_CONF_NONE  0	/* nothing doing */
#define CRYPTO_CONF_FLAGS 1	/* initialize flags */
#define CRYPTO_CONF_PRIV  2	/* load private key from file */
#define CRYPTO_CONF_PUBL  3	/* load public key from file */
#define CRYPTO_CONF_DH    4	/* load Diffie_Hellman pars from file */
#define CRYPTO_CONF_LEAP  5	/* load leapsecond table */
#define CRYPTO_CONF_KEYS  6	/* set keys directory path */
#endif /* PUBKEY */

/*
 * Function prototypes
 */
extern	void	crypto_recv	P((struct peer *, struct recvbuf *));
extern	int	crypto_xmit	P((u_int32 *, int, u_int, keyid_t,
				    u_int));
extern	keyid_t	session_key	P((struct sockaddr_in *, struct
				    sockaddr_in *, keyid_t, keyid_t,
				    u_long));
extern	void	make_keylist	P((struct peer *, struct interface *));
extern	void	key_expire	P((struct peer *));
extern	void	crypto_agree	P((void));
#ifdef PUBKEY
extern	void	crypto_config	P((int, char *));
extern	void	crypto_setup	P((void));
extern	int	crypto_public	P((struct peer *, u_char *, u_int));

/*
 * Cryptographic values
 */
extern	R_DH_PARAMS dh_params;
extern	struct value host;	/* host name/public key */
extern	struct value dhparam;	/* agreement parameters */
extern	struct value dhpub;	/* public value */
extern	struct value tai_leap;	/* leapseconds table */
extern	u_int	crypto_flags;	/* status word */	
extern	u_int	sys_tai;	/* current UTC offset from TAI */
#endif /* PUBKEY */
#endif /* AUTOKEY */
