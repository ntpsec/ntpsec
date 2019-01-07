#ifndef GUARD_AUTH_H
#define GUARD_AUTH_H

#include "ntp_types.h"
#include "ntp_lists.h"

#include <openssl/evp.h>
#include <openssl/cmac.h>

typedef enum {AUTH_NONE, AUTH_CMAC, AUTH_DIGEST} AUTH_Type;

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
	const EVP_CIPHER *cipher;		/* CMAC mode only */
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


extern	unsigned int authnumkeys;	/* number of active keys */
extern	unsigned int authnumfreekeys;	/* number of free keys */
extern	unsigned long authkeylookups;	/* calls to lookup keys */
extern	unsigned long authkeynotfound;	/* keys not found */
extern	unsigned long authencryptions;	/* calls to authencrypt */
extern	unsigned long authdigestencrypt;/* calls to digest_encrypt */
extern	unsigned long authcmacencrypt;	/* calls to cmac_encrypt */
extern	unsigned long authdecryptions;	/* calls to authdecrypt */
extern	unsigned long authdigestdecrypt;/* calls to digest_decrypt */
extern	unsigned long authdigestfail;	/* fails from digest_decrypt */
extern	unsigned long authcmacdecrypt;	/* calls to cmac_decrypt*/
extern	unsigned long authcmacfail;	/* fails from cmac_decrypt*/
extern	uptime_t auth_timereset;	/* current_time when stats reset */


/* Not in CMAC API */
#define CMAC_MAX_MAC_LENGTH 64

#endif	/* GUARD_AUTH_H */
