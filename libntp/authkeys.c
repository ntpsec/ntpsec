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
#include "ntp_auth.h"


/* define the payload region of auth_data beyond the list pointers */
#define auth_info_payload	keyid

#define	KEY_TRUSTED	0x001	/* this key is trusted */

#ifdef DEBUG
typedef struct auth_alloc_tag auth_alloc;

struct auth_alloc_tag {
	auth_alloc *	link;
	void *		mem;		/* enable free() atexit */
};

auth_alloc *	auth_allocs;
#endif	/* DEBUG */

static inline unsigned short	auth_log2(double x);
static void	auth_moremem	(int);
static void	auth_resize_hashtable(void);
static void	alloc_auth_info(auth_info **, keyid_t,	AUTH_Type,
				    const char *,
				    unsigned short, unsigned short, uint8_t *);
static void	free_auth_info(auth_info *, auth_info **);
#ifdef DEBUG
static void	free_auth_mem(void);
#endif

static auth_info key_listhead;		/* list of all in-use keys */
/*
 * The hash table. This is indexed by the low order bits of the
 * keyid. This gets updated in auth_resize_hashtable
 */
#define KEYHASH(keyid)	((keyid) & authhashmask)
#define INIT_AUTHHASHSIZE 64
static unsigned short authhashbuckets = INIT_AUTHHASHSIZE;
static unsigned short authhashmask = INIT_AUTHHASHSIZE - 1;
static auth_info **key_hash;

unsigned int authnumkeys;	/* number of active keys */
unsigned int authnumfreekeys;	/* number of free keys */
unsigned long authkeylookups;	/* calls to lookup keys */
unsigned long authkeynotfound;	/* keys not found */
unsigned long authencryptions;	/* calls to authencrypt */
unsigned long authdigestencrypt;/* calls to digest_encrypt */
unsigned long authcmacencrypt;	/* calls to cmac_encrypt */
unsigned long authdecryptions;	/* calls to authdecrypt */
unsigned long authdigestdecrypt;/* calls to digest_decrypt */
unsigned long authdigestfail;	/* fails from digest_decrypt */
unsigned long authcmacdecrypt;	/* calls to cmac_decrypt*/
unsigned long authcmacfail;	/* fails from cmac_decrypt*/
uptime_t auth_timereset;	/* current_time when stats reset */

/*
 * Storage for free auth_info structures.  We malloc() such things but
 * never free them.
 */
static auth_info *authfreekeys;

#define	MEMINC	16		/* number of new free ones to get */

/*
 * There used to be a cache for the last key we used.
 * It was also a kludge to pass arguments.
 * We now pass a pointer to auth_data.
 * A cache isn't all that useful:
 *   On a busy server, we can use the same auth_data for the reply.
 *   On a client, where the reply might hit the cache from the request,
 *     the extra cost of a lookup isn't significant.
 */


/*
 * auth_init - initialize internal data
 */
void
auth_init(void)
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
 * auth_reset_stats - reset the authentication stat counters.
 * can't use global current_time - not in library.
 */
void
auth_reset_stats(uptime_t reset_time)
{
	authkeylookups = 0;
	authkeynotfound = 0;
	authencryptions = 0;
	authdigestencrypt = 0;
	authcmacencrypt = 0;
	authdecryptions = 0;
	authdigestdecrypt = 0;
	authdigestfail = 0;
	authcmacdecrypt = 0;
	authcmacfail = 0;
	auth_timereset = reset_time;
}



/*
 * free_auth_mem - assist in leak detection by freeing all dynamic
 *		   allocations from this module.
 */
#ifdef DEBUG
static void
free_auth_mem(void)
{
	auth_info *	sk;
	auth_alloc *	alloc;
	auth_alloc *	next_alloc;

	while (NULL != (sk = HEAD_DLIST(key_listhead, llink))) {
		free_auth_info(sk, &key_hash[KEYHASH(sk->keyid)]);
	}
	free(key_hash);
	key_hash = NULL;
	for (alloc = auth_allocs; NULL != alloc; alloc = next_alloc) {
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
	auth_info *	auth;
	int		i;
#ifdef DEBUG
	void *		base;
	auth_alloc *	allocrec;
# define MOREMEM_EXTRA_ALLOC	(sizeof(*allocrec))
#else
# define MOREMEM_EXTRA_ALLOC	(0)
#endif

	i = (keycount > 0)
		? keycount
		: MEMINC;
	auth = emalloc_zero((unsigned int)i * sizeof(*auth) + MOREMEM_EXTRA_ALLOC);
#ifdef DEBUG
	base = auth;
#endif
	authnumfreekeys += i;

	for (; i > 0; i--, auth++) {
		LINK_SLIST(authfreekeys, auth, llink.f);
	}

#ifdef DEBUG
	allocrec = (void *)auth;
	allocrec->mem = base;
	LINK_SLIST(auth_allocs, allocrec, link);
#endif
}


/*
 * auth_prealloc
 */
void
auth_prealloc(
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
	auth_info *	auth;

	totalkeys = authnumkeys + (unsigned int)authnumfreekeys;
	hashbits = auth_log2(totalkeys / 4.0) + 1;
	hashbits = max(4, hashbits);
	hashbits = min(15, hashbits);

	authhashbuckets = 1 << hashbits;
	authhashmask = authhashbuckets - 1;
	newalloc = authhashbuckets * sizeof(key_hash[0]);

	key_hash = erealloc(key_hash, newalloc);
	memset(key_hash, '\0', newalloc);

	ITER_DLIST_BEGIN(key_listhead, auth, llink, auth_info)
		hash = KEYHASH(auth->keyid);
		LINK_SLIST(key_hash[hash], auth, hlink);
	ITER_DLIST_END()
}


/*
 * alloc_auth_info - allocate and link in an auth_info slot.
 * secret must be allocated with a free-compatible allocator.
 * It is owned by the new auth_info and will be free()d by
 * free_auth_info().
 */
static void
alloc_auth_info(
	auth_info **	bucket,
	keyid_t		keyid,
	AUTH_Type	type,
	const char *	name,
	unsigned short	flags,
	unsigned short	key_size,
	uint8_t *	key
	)
{
	auth_info *	auth;

	if (authnumfreekeys < 1)
		auth_moremem(-1);
	UNLINK_HEAD_SLIST(auth, authfreekeys, llink.f);
	//ENSURE(sk != NULL);
	auth->keyid = keyid;
	auth->type = type;
	auth->flags = flags;
	auth->key_size = key_size;
	auth->key = key;
	switch (type) {
	  case AUTH_NONE:
		auth->digest = NULL;
		auth->cipher = NULL;
		break;
	  case AUTH_DIGEST:
		auth->digest = EVP_get_digestbyname(name);
		auth->cipher = NULL;
		break;
	  case AUTH_CMAC:
		auth->digest = NULL;
		auth->cipher = EVP_get_cipherbyname(name);
		break;
	  default:
		msyslog(LOG_ERR, "BUG: alloc_auth_info: bogus type %u", type);
		exit(1);
	}
	LINK_SLIST(*bucket, auth, hlink);
	LINK_TAIL_DLIST(key_listhead, auth, llink);
	authnumfreekeys--;
	authnumkeys++;
}


/*
 * free_auth_info - common code to remove a auth_info and recycle its entry.
 */
static void
free_auth_info(
	auth_info *	auth,
	auth_info **	bucket
	)
{
	auth_info *	unlinked;

	if (NULL != auth->key) {
		memset(auth->key, '\0', auth->key_size);
		free(auth->key);
                auth->key = NULL;
	}
	UNLINK_SLIST(unlinked, *bucket, auth, hlink, auth_info);
	//ENSURE(sk == unlinked);
	UNLINK_DLIST(auth, llink);
	memset((char *)auth + offsetof(auth_info, auth_info_payload), '\0',
	       sizeof(*auth) - offsetof(auth_info, auth_info_payload));
	LINK_SLIST(authfreekeys, auth, llink.f);
	authnumkeys--;
	authnumfreekeys++;
}


/*
 * authtrust - declare a key to be trusted/untrusted
 * untrusted case not used (except for test code)  2018-Jun
 */
void
authtrust(
	keyid_t		id,
	bool		trust
	)
{
	auth_info **	bucket;
	auth_info *	auth;

	/*
	 * Search bin for key; if it does not exist and is untrusted,
	 * forget it.
	 */
	bucket = &key_hash[KEYHASH(id)];
	for (auth = *bucket; NULL != auth; auth = auth->hlink) {
		if (id == auth->keyid)
			break;
	}
	if (!trust && NULL == auth)
		return;

	/*
	 * There are two conditions remaining. Either it does not
	 * exist and is to be trusted or it does exist and is or is
	 * not to be trusted.
	 */
	if (NULL != auth) {
		/* Key exists. Leave it around so we can trust it again. */
		if (trust) {
			auth->flags |= KEY_TRUSTED;
		} else {
			auth->flags &= ~KEY_TRUSTED;
		}
		return;

	}

	/* Create empty slot to hold trusted flag.  No key.  */
	alloc_auth_info(bucket, id, AUTH_NONE, 0, KEY_TRUSTED, 0, NULL);
}

/*
 * authlookup - find key, check trust
 */
auth_info *
authlookup(
        keyid_t	keyno,
        bool	needtrust
        )
{
        auth_info *     auth;
        auth_info **    bucket;

	authkeylookups++;
        bucket = &key_hash[KEYHASH(keyno)];
        for (auth = *bucket; NULL != auth; auth = auth->hlink) {
                if (keyno == auth->keyid)
                        break;
        }
        if (NULL == auth ||
	   (AUTH_NONE == auth->type) ||
	   (needtrust && !(KEY_TRUSTED & auth->flags))) {
		if (0) msyslog(LOG_INFO, "DEBUG: authlookup fail: key %u, %s, auth: %s",
			keyno, needtrust? "T" : "F",
			(NULL == auth)? "NULL:" : "Ok");
                authkeynotfound++;
                return NULL;
        }
        return auth;
}

void
auth_setkey(
	keyid_t keyno,
	AUTH_Type type,
	const char * name,
	const uint8_t *key,
	size_t key_size
	)
{
	auth_info **	bucket;
	uint8_t *	newkey;

	if (0) msyslog(LOG_INFO, "DEBUG: auth_setkey: key %u, %s, length %zu",
	    keyno, name, key_size);

	//ENSURE(??? <= USHRT_MAX);
	//ENSURE(len < 4 * 1024);
	/*
	 * See if we already have the key.  If so just stick in the
	 * new value.
	 */
	bucket = &key_hash[KEYHASH(keyno)];
	for (auth_info * auth = *bucket; NULL != auth; auth = auth->hlink) {
		if (keyno == auth->keyid) {
			auth->type = type;
			switch (type) {
			  case AUTH_NONE:
				auth->digest = NULL;
				auth->cipher = NULL;
				break;
			  case AUTH_DIGEST:
				auth->digest = EVP_get_digestbyname(name);
				auth->cipher = NULL;
				break;
			  case AUTH_CMAC:
				auth->digest = NULL;
				auth->cipher = EVP_get_cipherbyname(name);
				break;
			  default:
				msyslog(LOG_ERR, "BUG: auth_setkey: bogus type %u", type);
				exit(1);
			}
			if (NULL != auth->key) {
				memset(auth->key, '\0', auth->key_size);
                        	free(auth->key);
			}
			auth->key_size = (unsigned short)key_size;
                        auth->key = emalloc(key_size);
			memcpy(auth->key, key, key_size);
			return;
		}
	}

	/*
	 * Need to allocate new structure.  Do it.
	 */
	newkey = emalloc(key_size);
	memcpy(newkey, key, key_size);
	alloc_auth_info(bucket, keyno, type, name, 0,
		    (unsigned short)key_size, newkey);
#ifdef DEBUG
	if (debug >= 4) { /* SPECIAL DEBUG */
		printf("auth_setkey: key %d type %s len %d ", (int)keyno,
		    name, (int)key_size);
		for (size_t j = 0; j < key_size; j++)
			printf("%02x", key[j]);
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
	auth_info * auth;

	ITER_DLIST_BEGIN(key_listhead, auth, llink, auth_info)
		/*
		 * Don't lose info as to which keys are trusted.
		 */
		if (KEY_TRUSTED & auth->flags) {
			if (NULL != auth->key) {
				memset(auth->key, '\0', auth->key_size);
				free(auth->key);
				auth->key = NULL;
			}
			auth->key_size = 0;
			auth->type = AUTH_NONE;
			auth->digest = NULL;
			auth->cipher = NULL;
		} else {
			free_auth_info(auth, &key_hash[KEYHASH(auth->keyid)]);
		}
	ITER_DLIST_END()
}


/*
 * authencrypt - generate message authenticator
 * fills in keyid in packet
 * Returns length of authenticator field
 */
int
authencrypt(
	auth_info *	auth,	/* assumed setup correctly */
	uint32_t *	pkt,
	int		length
	)
{

	/* Pre-CMAC versions of this code had checking here.
	 * That logic has been pushed up a layer.  2018-June
	 */

	authencryptions++;
	pkt[length / 4] = htonl(auth->keyid);
	switch (auth->type) {
	    case AUTH_DIGEST:
		authdigestencrypt++;
		return digest_encrypt(auth, pkt, length);
	    case AUTH_CMAC:
		authcmacencrypt++;
		return cmac_encrypt(auth, pkt, length);
	    default:
		msyslog(LOG_ERR, "BUG: authencrypt: bogus type %u", auth->type);
		exit(1);
	}
	return 0;
}


/*
 * authdecrypt - verify message authenticator
 *
 * Returns true if authenticator valid.
 */
bool
authdecrypt(
	auth_info *	auth,	/* assumed setup correctly */
	uint32_t *	pkt,
	int		length,
	int		size
	)
{
	bool answer;

	/* Pre-CMAC versions of this code had checking here.
	 * That logic has been pushed up a layer.  2018-June
	 */

	authdecryptions++;
	switch (auth->type) {
	    case AUTH_DIGEST:
		authdigestdecrypt++;
		answer = digest_decrypt(auth, pkt, length, size);
		if (!answer) authdigestfail++;
		return answer;
	    case AUTH_CMAC:
		authcmacdecrypt++;
		answer = cmac_decrypt(auth, pkt, length, size);
		if (!answer) authcmacfail++;
		return answer;
	    default:
		msyslog(LOG_ERR, "BUG: authdecrypt: bogus type %u", auth->type);
		exit(1);
	}
	return false;
}
