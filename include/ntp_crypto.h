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
 * Extension field definitions
 */
#define CRYPTO_VN	1	/* current protocol version number */

#define CRYPTO_NULL	((CRYPTO_VN << 8) | 0) /* no operation */
#define CRYPTO_STAT	((CRYPTO_VN << 8) | 1) /* reserved */
#define CRYPTO_ASSOC	((CRYPTO_VN << 8) | 2) /* association ID */
#define CRYPTO_AUTO	((CRYPTO_VN << 8) | 3) /* autokey values */
#define CRYPTO_PRIV	((CRYPTO_VN << 8) | 4) /* cookie value */
#define CRYPTO_DHPAR	((CRYPTO_VN << 8) | 5) /* DH parameters */
#define CRYPTO_DH	((CRYPTO_VN << 8) | 6) /* DH public value */
#define CRYPTO_NAME	((CRYPTO_VN << 8) | 7) /* RSA public key */
#define CRYPTO_TAI	((CRYPTO_VN << 8) | 8) /* TAI offset */
#define CRYPTO_RESP	0x8000			/* response */
#define CRYPTO_ERROR	0x4000			/* error */

/*
 * Cryptoflags
 */
#define CRYPTO_FLAG_ENAB  0x01	/* enable public key */
#define CRYPTO_FLAG_TAI	  0x02	/* request TAI leapseconds */

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
				    int));
extern	keyid_t	session_key	P((struct sockaddr_in *, struct
				    sockaddr_in *, keyid_t, keyid_t,
				    u_long));
extern	void	make_keylist	P((struct peer *));
extern	void	key_expire	P((struct peer *));
extern	void	crypto_agree	P((void));
#ifdef PUBKEY
extern	void	crypto_config	P((int, char *));
extern	void	crypto_setup	P((void));
extern	int	crypto_public	P((struct peer *, u_char *, u_int));

/*
 * Cryptographic values
 */
extern	int	crypto_flags;
extern	R_DH_PARAMS dh_params;
extern	struct value host;	/* public key/host name */
extern	struct value dhparam;	/* Diffie-Hellman parameters */
extern	struct value dhpub;	/* Diffie-Hellman public value */
extern	struct value tai_leap;	/* TAI leapseconds table */
extern	u_int	sys_tai;	/* current UTC offset from TAI */
#endif /* PUBKEY */
#endif /* AUTOKEY */
