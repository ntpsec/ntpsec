/*
 * authkeys.c - routines to manage the storage of authentication keys
 */
#include "config.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "ntp.h"
#include "ntpd.h"
#include "ntp_lists.h"
#include "ntp_malloc.h"
#include "ntp_stdlib.h"

/*
 * Structure to store keys in in the hash table.
 */
typedef struct savekey symkey;

struct savekey {
	symkey *	hlink;		/* next in hash bucket */
	DECL_DLIST_LINK(symkey, llink);	/* for overall & free lists */
	uint8_t *	secret;		/* shared secret */
	keyid_t		keyid;		/* key identifier */
	unsigned short	type;		/* OpenSSL digest NID */
	unsigned short	secretsize;	/* secret octets */
	unsigned short	flags;		/* KEY_ flags that wave */
};

/* define the payload region of symkey beyond the list pointers */
#define symkey_payload	secret

#define	KEY_TRUSTED	0x001	/* this key is trusted */

#ifdef DEBUG
typedef struct symkey_alloc_tag symkey_alloc;

struct symkey_alloc_tag {
	symkey_alloc *	link;
	void *		mem;		/* enable free() atexit */
};

symkey_alloc *	authallocs;
#endif	/* DEBUG */

static inline unsigned short	auth_log2(double x);
static void	auth_moremem	(int);
static void		auth_resize_hashtable(void);
static void		allocsymkey(symkey **, keyid_t,	unsigned short,
				    unsigned short, unsigned short, uint8_t *);
static void		freesymkey(symkey *, symkey **);
#ifdef DEBUG
static void		free_auth_mem(void);
#endif

static symkey key_listhead;		/* list of all in-use keys */
/*
 * The hash table. This is indexed by the low order bits of the
 * keyid. This gets updated in auth_resize_hashtable
 */
#define KEYHASH(keyid)	((keyid) & authhashmask)
#define INIT_AUTHHASHSIZE 64
static unsigned short authhashbuckets = INIT_AUTHHASHSIZE;
static unsigned short authhashmask = INIT_AUTHHASHSIZE - 1;
static symkey **key_hash;

unsigned int authkeynotfound;		/* keys not found */
unsigned int authkeylookups;		/* calls to lookup keys */
unsigned int authnumkeys;		/* number of active keys */
unsigned int authkeyuncached;		/* cache misses */
static unsigned int authnokey;		/* calls to encrypt with no key */
unsigned int authencryptions;		/* calls to encrypt */
unsigned int authdecryptions;		/* calls to decrypt */

/*
 * Storage for free symkey structures.  We malloc() such things but
 * never free them.
 */
static symkey *authfreekeys;
int authnumfreekeys;

#define	MEMINC	16		/* number of new free ones to get */

/*
 * The key cache. We cache the last key we looked at here.
 */
keyid_t	cache_keyid;			/* key identifier */
uint8_t *cache_secret;			/* secret */
unsigned short cache_secretsize;	/* secret length */
int cache_type;				/* OpenSSL digest NID */
unsigned short cache_flags;		/* flags that wave */


/*
 * init_auth - initialize internal data
 */
void
init_auth(void)
{
	size_t newalloc;

	/*
	 * Initialize hash table and free list
	 */
	newalloc = authhashbuckets * sizeof(key_hash[0]);

	key_hash = erealloc(key_hash, newalloc);
	memset(key_hash, '\0', newalloc);

	INIT_DLIST(key_listhead, llink);

#ifdef DEBUG
	atexit(&free_auth_mem);
#endif
}


/*
 * free_auth_mem - assist in leak detection by freeing all dynamic
 *		   allocations from this module.
 */
#ifdef DEBUG
static void
free_auth_mem(void)
{
	symkey *	sk;
	symkey_alloc *	alloc;
	symkey_alloc *	next_alloc;

	while (NULL != (sk = HEAD_DLIST(key_listhead, llink))) {
		freesymkey(sk, &key_hash[KEYHASH(sk->keyid)]);
	}
	free(key_hash);
	key_hash = NULL;
	cache_keyid = 0;
	cache_flags = 0;
	for (alloc = authallocs; alloc != NULL; alloc = next_alloc) {
		next_alloc = alloc->link;
		free(alloc->mem);	
	}
	authfreekeys = NULL;
	authnumfreekeys = 0;
}
#endif	/* DEBUG */


/*
 * auth_moremem - get some more free key structures
 */
static void
auth_moremem(
	int	keycount
	)
{
	symkey *	sk;
	int		i;
#ifdef DEBUG
	void *		base;
	symkey_alloc *	allocrec;
# define MOREMEM_EXTRA_ALLOC	(sizeof(*allocrec))
#else
# define MOREMEM_EXTRA_ALLOC	(0)
#endif

	i = (keycount > 0)
		? keycount
		: MEMINC;
	sk = emalloc_zero((unsigned int)i * sizeof(*sk) + MOREMEM_EXTRA_ALLOC);
#ifdef DEBUG
	base = sk;
#endif
	authnumfreekeys += i;

	for (; i > 0; i--, sk++) {
		LINK_SLIST(authfreekeys, sk, llink.f);
	}

#ifdef DEBUG
	allocrec = (void *)sk;
	allocrec->mem = base;
	LINK_SLIST(authallocs, allocrec, link);
#endif
}


/*
 * auth_prealloc_symkeys
 */
void
auth_prealloc_symkeys(
	int	keycount
	)
{
	int	allocated;
	int	additional;

	allocated = (int)authnumkeys + authnumfreekeys;
	additional = keycount - allocated;
	if (additional > 0)
		auth_moremem(additional);
	auth_resize_hashtable();
}


static inline unsigned short
auth_log2(double x)
{
	return (unsigned short)(log10(x) / log10(2));
}


/*
 * auth_resize_hashtable
 *
 * Size hash table to average 4 or fewer entries per bucket initially,
 * within the bounds of at least 4 and no more than 15 bits for the hash
 * table index.  Populate the hash table.
 */
static void
auth_resize_hashtable(void)
{
	unsigned int	totalkeys;
	unsigned short	hashbits;
	unsigned short	hash;
	size_t		newalloc;
	symkey *	sk;

	totalkeys = authnumkeys + (unsigned int)authnumfreekeys;
	hashbits = auth_log2(totalkeys / 4.0) + 1;
	hashbits = max(4, hashbits);
	hashbits = min(15, hashbits);

	authhashbuckets = 1 << hashbits;
	authhashmask = authhashbuckets - 1;
	newalloc = authhashbuckets * sizeof(key_hash[0]);

	key_hash = erealloc(key_hash, newalloc);
	memset(key_hash, '\0', newalloc);

	ITER_DLIST_BEGIN(key_listhead, sk, llink, symkey)
		hash = KEYHASH(sk->keyid);
		LINK_SLIST(key_hash[hash], sk, hlink);
	ITER_DLIST_END()
}


/*
 * allocsymkey - common code to allocate and link in symkey
 *
 * secret must be allocated with a free-compatible allocator.  It is
 * owned by the referring symkey structure, and will be free()d by
 * freesymkey().
 */
static void
allocsymkey(
	symkey **	bucket,
	keyid_t		id,
	unsigned short	flags,
	unsigned short	type,
	unsigned short	secretsize,
	uint8_t *	secret
	)
{
	symkey *	sk;

	if (authnumfreekeys < 1)
		auth_moremem(-1);
	UNLINK_HEAD_SLIST(sk, authfreekeys, llink.f);
	//ENSURE(sk != NULL);
	sk->keyid = id;
	sk->flags = flags;
	sk->type = type;
	sk->secretsize = secretsize;
	sk->secret = secret;
	LINK_SLIST(*bucket, sk, hlink);
	LINK_TAIL_DLIST(key_listhead, sk, llink);
	authnumfreekeys--;
	authnumkeys++;
}


/*
 * freesymkey - common code to remove a symkey and recycle its entry.
 */
static void
freesymkey(
	symkey *	sk,
	symkey **	bucket
	)
{
	symkey *	unlinked;

	if (sk->secret != NULL) {
		memset(sk->secret, '\0', sk->secretsize);
		free(sk->secret);
                sk->secret = NULL;
	}
	UNLINK_SLIST(unlinked, *bucket, sk, hlink, symkey);
	//ENSURE(sk == unlinked);
	UNLINK_DLIST(sk, llink);
	memset((char *)sk + offsetof(symkey, symkey_payload), '\0',
	       sizeof(*sk) - offsetof(symkey, symkey_payload));
	LINK_SLIST(authfreekeys, sk, llink.f);
	authnumkeys--;
	authnumfreekeys++;
}


/*
 * authhavekey - return true and cache the key, if zero or both known
 *		 and trusted.
 */
int
authhavekey(
	keyid_t		id
	)
{
	symkey *	sk;

	authkeylookups++;
	if (0 == id || cache_keyid == id) {
		return true;
	}

	/*
	 * Search the bin for the key. If found and the key type
	 * is zero, somebody marked it trusted without specifying
	 * a key or key type. In this case consider the key missing.
	 */
	authkeyuncached++;
	for (sk = key_hash[KEYHASH(id)]; sk != NULL; sk = sk->hlink) {
		if (id == sk->keyid) {
			if (0 == sk->type) {
				authkeynotfound++;
				return false;
			}
			break;
		}
	}

	/*
	 * If the key is not found, or if it is found but not trusted,
	 * the key is not considered found.
	 */
	if (NULL == sk) {
		authkeynotfound++;
		return false;
	}
	if (!(KEY_TRUSTED & sk->flags)) {
		authnokey++;
		return false;
	}

	/*
	 * The key is found and trusted. Initialize the key cache.
	 */
	cache_keyid = sk->keyid;
	cache_type = sk->type;
	cache_flags = sk->flags;
	cache_secret = sk->secret;
	cache_secretsize = sk->secretsize;

	return true;
}


/*
 * authtrust - declare a key to be trusted/untrusted
 */
void
authtrust(
	keyid_t		id,
	bool		trust
	)
{
	symkey **	bucket;
	symkey *	sk;

	/*
	 * Search bin for key; if it does not exist and is untrusted,
	 * forget it.
	 */
	bucket = &key_hash[KEYHASH(id)];
	for (sk = *bucket; sk != NULL; sk = sk->hlink) {
		if (id == sk->keyid)
			break;
	}
	if (!trust && NULL == sk)
		return;

	/*
	 * There are two conditions remaining. Either it does not
	 * exist and is to be trusted or it does exist and is or is
	 * not to be trusted.
	 */	
	if (sk != NULL) {
		if (cache_keyid == id) {
			cache_flags = 0;
			cache_keyid = 0;
		}

		/*
		 * Key exists. If it is to be trusted, say so.
		 */
		if (trust) {
			sk->flags |= KEY_TRUSTED;
			return;
		}

		/* No longer trusted, return it to the free list. */
		freesymkey(sk, bucket);
		return;
	}

	allocsymkey(bucket, id, KEY_TRUSTED, 0, 0, NULL);
}


/*
 * authistrusted - determine whether a key is trusted
 */
int
authistrusted(
	keyid_t		keyno
	)
{
	symkey *	sk;
	symkey **	bucket;

	if (keyno == cache_keyid)
		return !!(KEY_TRUSTED & cache_flags);

	authkeyuncached++;
	bucket = &key_hash[KEYHASH(keyno)];
	for (sk = *bucket; sk != NULL; sk = sk->hlink) {
		if (keyno == sk->keyid)
			break;
	}
	if (NULL == sk || !(KEY_TRUSTED & sk->flags)) {
		authkeynotfound++;
		return false;
	}
	return true;
}


void
mac_setkey(
	keyid_t keyno,
	int	keytype,
	const uint8_t *key,
	size_t len
	)
{
	symkey **	bucket;
	uint8_t *	secret;
	size_t		secretsize;
	
	//ENSURE(keytype <= USHRT_MAX);
	//ENSURE(len < 4 * 1024);
	/*
	 * See if we already have the key.  If so just stick in the
	 * new value.
	 */
	bucket = &key_hash[KEYHASH(keyno)];
	for (symkey * sk = *bucket; sk != NULL; sk = sk->hlink) {
		if (keyno == sk->keyid) {
			sk->type = (unsigned short)keytype;
			secretsize = len;
			sk->secretsize = (unsigned short)secretsize;
                        free(sk->secret);
                        sk->secret = emalloc(secretsize);
			memcpy(sk->secret, key, secretsize);
			if (cache_keyid == keyno) {
				cache_flags = 0;
				cache_keyid = 0;
			}
			return;
		}
	}

	/*
	 * Need to allocate new structure.  Do it.
	 */
	secretsize = len;
	secret = emalloc(secretsize);
	memcpy(secret, key, secretsize);
	allocsymkey(bucket, keyno, 0, (unsigned short)keytype,
		    (unsigned short)secretsize, secret);
#ifdef DEBUG
	if (debug >= 4) { /* SPECIAL DEBUG */
		printf("auth_setkey: key %d type %d len %d ", (int)keyno,
		    keytype, (int)secretsize);
		for (size_t j = 0; j < secretsize; j++)
			printf("%02x", secret[j]);
		printf("\n");
	}	
#endif
}


/*
 * auth_delkeys - delete untrusted keys, and clear all info
 *                except the trusted bit of trusted keys, in
 *		  preparation for rereading the keys file.
 */
void
auth_delkeys(void)
{
	symkey *	sk;

	ITER_DLIST_BEGIN(key_listhead, sk, llink, symkey)
		/*
		 * Don't lose info as to which keys are trusted.
		 */
		if (KEY_TRUSTED & sk->flags) {
			if (sk->secret != NULL) {
				memset(sk->secret, '\0', sk->secretsize);
				free(sk->secret);
				sk->secret = NULL;
			}
			sk->secretsize = 0;
		} else {
			freesymkey(sk, &key_hash[KEYHASH(sk->keyid)]);
		}
	ITER_DLIST_END()
}


/*
 * authencrypt - generate message authenticator
 *
 * Returns length of authenticator field, zero if key not found.
 */
int
authencrypt(
	keyid_t		keyno,
	uint32_t *	pkt,
	int		length
	)
{\
	/*
	 * A zero key identifier means the sender has not verified
	 * the last message was correctly authenticated. The MAC
	 * consists of a single word with value zero.
	 */
	authencryptions++;
	pkt[length / 4] = htonl(keyno);
	if (0 == keyno) {
		return 4;
	}
	if (!authhavekey(keyno)) {
		return 0;
	}

	return mac_authencrypt(cache_type, cache_secret, pkt, length);
}


/*
 * authdecrypt - verify message authenticator
 *
 * Returns true if authenticator valid, false if invalid or not found.
 */
int
authdecrypt(
	keyid_t		keyno,
	uint32_t *	pkt,
	int		length,
	int		size
	)
{
	/*
	 * A zero key identifier means the sender has not verified
	 * the last message was correctly authenticated.  For our
	 * purpose this is an invalid authenticator.
	 */
	authdecryptions++;
	if (0 == keyno || !authhavekey(keyno) || size < 4) {
		return false;
	}

	return mac_authdecrypt(cache_type, cache_secret, pkt, length, size);
}
