#ifndef GUARD_AUTH_H
#define GUARD_AUTH_H

#include "ntp_types.h"
#include "ntp_lists.h"

#include <openssl/evp.h>

typedef enum {AUTH_NONE, AUTH_DIGEST, AUTH_CMAC, AUTH_HMAC} AUTH_Type;

/*
 * Structure to store auth data in the hash table.
 */
typedef struct auth_data auth_info;

struct auth_data {
	auth_info *	hlink;			/* next in hash bucket */
	DECL_DLIST_LINK(auth_info, llink);	/* for overall & free lists */
	keyid_t		keyid;			/* key identifier */
	AUTH_Type	type;			/* CMAC or old digest */
	unsigned short	flags;			/* KEY_ flags that wave */
	uint8_t *	key;			/* shared secret */
	unsigned short	key_size;		/* secret length */
	const EVP_MD *	digest;			/* Digest mode only */
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	EVP_MAC_CTX *cmac_ctx;			/* EVP CMAC mode */
	EVP_MAC_CTX *hmac_ctx;			/* EVP HMAC mode */
#else
	const EVP_CIPHER *cipher;		/* Old CMAC mode only */
#endif
};

extern  void    auth_init       (void);
extern  void    auth_prealloc	(int);
extern  void    auth_reset_stats(uptime_t reset_time);


extern  void	auth_setkey	(keyid_t, AUTH_Type, const char *, const uint8_t *, size_t);
extern  void    auth_delkeys    (void);
extern  bool    authreadkeys    (const char *);
extern  void    authtrust       (keyid_t, bool);

extern  auth_info *    authlookup   (keyid_t, bool);

extern  bool    authdecrypt     (auth_info*, uint32_t *, int, int);
extern  int     authencrypt     (auth_info*, uint32_t *, int);

extern   bool    digest_decrypt (auth_info*, uint32_t *, int, int);
extern   int     digest_encrypt (auth_info*, uint32_t *, int);

extern   bool    cmac_decrypt (auth_info*, uint32_t *, int, int);
extern   int     cmac_encrypt (auth_info*, uint32_t *, int);

extern   bool    hmac_decrypt (auth_info*, uint32_t *, int, int);
extern   int     hmac_encrypt (auth_info*, uint32_t *, int);


extern	unsigned int authnumkeys;	/* number of active keys */
extern	unsigned int authnumfreekeys;	/* number of free keys */
extern	unsigned long authkeylookups;	/* calls to lookup keys */
extern	unsigned long authkeynotfound;	/* keys not found */
extern	unsigned long authencryptions;	/* calls to authencrypt */
extern	unsigned long authdigestencrypt;/* calls to digest_encrypt */
extern	unsigned long authcmacencrypt;	/* calls to cmac_encrypt */
extern	unsigned long authhmacencrypt;	/* calls to hmac_encrypt */
extern	unsigned long authdecryptions;	/* calls to authdecrypt */
extern	unsigned long authdigestdecrypt;/* calls to digest_decrypt */
extern	unsigned long authdigestfail;	/* fails from digest_decrypt */
extern	unsigned long authcmacdecrypt;	/* calls to cmac_decrypt*/
extern	unsigned long authcmacfail;	/* fails from cmac_decrypt*/
extern	unsigned long authhmacdecrypt;	/* calls to hmac_decrypt*/
extern	unsigned long authhmacfail;	/* fails from hmac_decrypt*/
extern	uptime_t auth_timereset;	/* current_time when stats reset */


#if OPENSSL_VERSION_NUMBER > 0x20000000L
extern EVP_MAC_CTX *evpc_ctx;   /* used by authreadkeys and authkeys */
extern EVP_MAC_CTX *evph_ctx;   /* used by authreadkeys and authkeys */
/* For testing */
extern EVP_MAC_CTX* Setup_CMAC_CTX(const char *name, uint8_t *key, int keylen);
extern EVP_MAC_CTX* Setup_HMAC_CTX(const char *name, uint8_t *key, int keylen);
#endif

#endif	/* GUARD_AUTH_H */
