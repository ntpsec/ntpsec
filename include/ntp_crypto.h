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
#define CRYPTO_PUBL	((CRYPTO_VN << 8) | 1) /* public key */
#define CRYPTO_ASSOC	((CRYPTO_VN << 8) | 2) /* association ID */
#define CRYPTO_AUTO	((CRYPTO_VN << 8) | 3) /* autokey values */
#define CRYPTO_PRIV	((CRYPTO_VN << 8) | 4) /* cookie value */
#define CRYPTO_DHPAR	((CRYPTO_VN << 8) | 5) /* DH parameters */
#define CRYPTO_DH	((CRYPTO_VN << 8) | 6) /* DH public value */
#define CRYPTO_NAME	((CRYPTO_VN << 8) | 7) /* host name */
#define CRYPTO_TAI	((CRYPTO_VN << 8) | 8) /* TAI offset */
#define CRYPTO_RESP	0x8000	/* response */
#define CRYPTO_ERROR	0x4000	/* error */

/*
 * Cryptoflags
 */
#define CRYPTO_FLAG_ENAB  0x01	/* enable public key */
#define CRYPTO_FLAG_PUBL  0x02	/* fetch peer public key from server */

#ifdef PUBKEY

#define MAX_DH_LEN (DH_PRIME_LEN(1024)) /* max agreed key length */

/*
 * Configuration codes
 */
#define CRYPTO_CONF_NONE  0	/* nothing doing */
#define CRYPTO_CONF_FLAGS 1	/* initialize flags */
#define CRYPTO_CONF_PRIV  2	/* load private key from file */
#define CRYPTO_CONF_PUBL  3	/* load public key from file */
#define CRYPTO_CONF_DH    4	/* load Diffie_Hellman pars from file */
#define CRYPTO_CONF_KEYS  5 	/* set keys directory path */
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
extern	char *	private_key_file;
extern	u_int	private_key_fstamp;
extern	char *	public_key_file;
extern	u_int	public_key_fstamp;
extern	char *	dh_params_file;
extern	u_int	dh_params_fstamp;
#endif /* PUBKEY */
#endif /* AUTOKEY */
