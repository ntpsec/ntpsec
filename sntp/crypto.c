#include "crypto.h"
#include <ctype.h>

struct key *key_ptr;
int key_cnt = 0;

static int
s_keytype_from_text(
	const char *text
	)
{
	int		key_type;
#ifdef OPENSSL
	char		upcased[EVP_MAX_MD_SIZE];
	char *		pch;

	/*
	 * OpenSSL digest short names are capitalized, so uppercase the
	 * digest name before passing to OBJ_sn2nid().  If it is not
	 * recognized but begins with 'M' use NID_md5 to be consistent
	 * with past behavior.
	 */
	strncpy(upcased, text, sizeof upcased);
	for (pch = upcased; '\0' != *pch; pch++)
		*pch = (char)toupper(*pch);
	key_type = OBJ_sn2nid(upcased);
#else
	key_type = 0;
#endif

	if (!key_type && 'm' == tolower(text[0]))
		key_type = NID_md5;
	return key_type;
}

int
make_mac(
	char *pkt_data,
	int pkt_size,
	int mac_size,
	struct key *cmp_key,
	char * digest
	)
{
	unsigned int len = mac_size;
#ifdef OPENSSL
	int key_type;
	EVP_MD_CTX ctx;
#else
	MD5_CTX ctx;
#endif /* OPENSSL */
	
	if (cmp_key->key_len > 64)
		return 0;
	if (pkt_size % 4 != 0)
		return 0;
#ifdef OPENSSL
	INIT_SSL();
	key_type = s_keytype_from_text(cmp_key->type);
	EVP_DigestInit(&ctx, EVP_get_digestbynid(key_type));
	EVP_DigestUpdate(&ctx, (u_char *)cmp_key->key_seq, (u_int)cmp_key->key_len);
	EVP_DigestUpdate(&ctx, (u_char *)pkt_data, (u_int)pkt_size);
	EVP_DigestFinal(&ctx, (u_char *)digest, &len);
#else /* OPENSSL */
	if ((cmp_key->type[0] | 0x20) != 'm')
		return 0;
	if (mac_size < 16)
		return 0;
	MD5Init(&ctx);
	MD5Update(&ctx, (u_char *)cmp_key->key_seq, (u_int)cmp_key->key_len); 
	MD5Update(&ctx, (u_char *)pkt_data, (u_int)pkt_size);
	MD5Final((u_char *)digest, &ctx);
	len = 16;
#endif /* OPENSSL */

	return (int)len;
}


/* Generates a md5 digest of the key specified in keyid concatinated with the 
 * ntp packet (exluding the MAC) and compares this digest to the digest in
 * the packet's MAC. If they're equal this function returns 1 (packet is 
 * authentic) or else 0 (not authentic).
 */
int
auth_md5(
	char *pkt_data,
	int pkt_size,
	int mac_size,
	struct key *cmp_key
	)
{
	int rv;
	char digest[20];

	if (mac_size > sizeof digest)
		return 0;
	rv = make_mac(pkt_data, pkt_size, sizeof digest, cmp_key, digest);
	return rv ? !memcmp(digest, pkt_data + pkt_size + 4, rv) : rv;
}

static int
hex_val( unsigned char x)
{
	static const short vals [] = 
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
	  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	  -1, 10, 11, 12, 13, 14, 15 };

	return (x < '0' || x > 'f') ? -1 : vals[x - '0'];
}

/* Load keys from the specified keyfile into the key structures.
 * Returns -1 if the reading failed, otherwise it returns the 
 * number of keys it read
 */
int
auth_init(
	const char *keyfile,
	struct key **keys
	)
{
	FILE *keyf = fopen(keyfile, "r"); 
	struct key *prev = NULL;
	int scan_cnt, line_cnt = 0;
	char kbuf[200];
	char keystring[129];

	if (keyf == NULL) {
		if (ENABLED_OPT(NORMALVERBOSE))
			printf("sntp auth_init: Couldn't open key file %s for reading!\n", keyfile);
		return -1;
	}
	if (feof(keyf)) {
		if (ENABLED_OPT(NORMALVERBOSE))
			printf("sntp auth_init: Key file %s is empty!\n", keyfile);
		fclose(keyf);
		return -1;
	}
	key_cnt = 0;
	while (!feof(keyf)) {
		char * octothorpe;
		struct key *act = emalloc(sizeof(struct key));
		int goodline = 0;

		fgets(kbuf, sizeof(kbuf), keyf);
		kbuf[199] = '\0';
		octothorpe = strchr(kbuf, '#');
		if (octothorpe)
			*octothorpe = '\0';
#ifdef DEBUG
		printf("sntp auth_init: fgets: %s", kbuf);
#endif
		scan_cnt = sscanf(kbuf, "%i %10s %128s", &act->key_id, act->type, keystring);
		if (scan_cnt == 3) {
			int len = strlen(keystring);
			if (len <= 20) {
				act->key_len = len;
				memcpy(act->key_seq, keystring, len+1);
				goodline = 1;
			} else if ((len & 1) != 0) {
				goodline = 0; /* it's bad */
			} else {
                                int j;
				goodline = 1;
				act->key_len = len >> 1;
				for (j = 0; j < len; j+=2) {
					int val;
					val = (hex_val(keystring[j]) << 4) |
					       hex_val(keystring[j+1]);
					if (val < 0) {
						goodline = 0; /* it's bad */
						break;
					}
					act->key_seq[j>>1] = (char)val;
				}
			}
		}
		if (goodline) {
			act->next = NULL;
			if (NULL == prev)
				*keys = act;
			else
				prev->next = act;
			prev = act;
			key_cnt++;
#ifdef DEBUG
			printf("sntp auth_init: key_id %i type %s with key %s\n", act->key_id, act->type, act->key_seq);
#endif
		} else {
#ifdef DEBUG
			printf("sntp auth_init: scanf read %i items, doesn't look good, skipping line %i.\n", scan_cnt, line_cnt);
#endif
			free(act);
		}
		line_cnt++;
	}
	fclose(keyf);
	
#ifdef DEBUG
	STDLINE
	printf("sntp auth_init: Read %i keys from file %s:\n", line_cnt, keyfile);
	{
		struct key *kptr = *keys;
		register int a;

		for (a = 0; a < key_cnt; a++) {
			printf("key_id %i type %s with key %s (key length: %i)\n",
			       kptr->key_id, kptr->type, kptr->key_seq, kptr->key_len);
			kptr = kptr->next;
		}
	}
	STDLINE
#endif
	key_ptr = *keys;
	return key_cnt;
}

/* Looks for the key with keyid key_id and sets the d_key pointer to the 
 * address of the key. If no matching key is found the pointer is not touched.
 */
void
get_key(
	int key_id,
	struct key **d_key
	)
{
	struct key *itr_key;

	if (key_cnt == 0)
		return;
	for (itr_key = key_ptr; itr_key; itr_key = itr_key->next) {
		if (itr_key->key_id == key_id) {
			*d_key = itr_key;
			break;
		}
	}
	return;
}
