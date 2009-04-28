/*
 * Copyright (C) 2008  Johannes Maximilian Kühn
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "crypto.h"

struct key *key_ptr;
int key_cnt = 0;

/* Generates a md5 digest of the ntp packet (exluding the MAC) concatinated
 * with the key specified in keyid and compares this digest to the digest in
 * the packet's MAC. If they're equal this function returns 1 (packet is 
 * authentic) or else 0 (not authentic).
 */
int
auth_md5 (
		char *pkt_data,
		int mac_size,
		struct key *cmp_key
	 )
{
	register int a;
	char digest[16];
	MD5_CTX ctx;

	if(cmp_key->type != 'M')
		return -1;
	
	MD5Init(&ctx);

	char *digest_data = (char *) malloc(sizeof(char) * (LEN_PKT_NOMAC + cmp_key->key_len));

	for(a=0; a<LEN_PKT_NOMAC; a++)
		digest_data[a] = pkt_data[a];

	for(a=0; a<cmp_key->key_len; a++)
		digest_data[LEN_PKT_NOMAC + a] = cmp_key->key_seq[a];

	MD5Update(&ctx, (unsigned char *)digest_data, LEN_PKT_NOMAC + cmp_key->key_len);
	MD5Final((unsigned char *)digest, &ctx);

	free(digest_data);

	for(a=0; a<16; a++)
		if(digest[a] != pkt_data[LEN_PKT_MAC + a])
			return 0;

	return 1;
}

/* Load keys from the specified keyfile into the key structures.
 * Returns -1 if the reading failed, otherwise it returns the 
 * number of keys it read
 */
int
auth_init (
		const char *keyfile,
		struct key **keys
		)
{
	FILE *keyf = fopen(keyfile, "r"); 
	struct key *prev = NULL;

	register int a, line_limit;
	int scan_cnt, line_cnt = 0;
	char kbuf[96];

	if(keyf == NULL) {
		if(ENABLED_OPT(NORMALVERBOSE))
			printf("sntp auth_init: Couldn't open key file %s for reading!\n", keyfile);

		return -1;
	}

	line_cnt = 0;
	
	if(feof(keyf)) {
		if(ENABLED_OPT(NORMALVERBOSE))
			printf("sntp auth_init: Key file %s is empty!\n", keyfile);

		return -1;
	}


	while(!feof(keyf)) {
		struct key *act = (struct key *) malloc(sizeof(struct key));
		line_limit = 0;

		fgets(kbuf, 96, keyf);
		
		for(a=0; a<strlen(kbuf) && a < 96; a++) {
			if(kbuf[a] == '#') {
				line_limit = a;
				break;
			}
		}

		if(line_limit != 0)
			kbuf[line_limit] = '\0';

#ifdef DEBUG
		printf("sntp auth_init: fgets: %s", kbuf);
#endif
		

		if((scan_cnt = sscanf(kbuf, "%i %c %16s", &act->key_id, &act->type, act->key_seq)) == 3) {
			act->key_len = strlen(act->key_seq);
	
			if(act->type != 0) {
				*keys = act;
				prev = act;
			}
			else {
				prev->next = act;
				act->next = NULL;
				prev = act;
			}

			key_cnt++;

#ifdef DEBUG
			printf("sntp auth_init: key_id %i type %c with key %s\n", act->key_id, act->type, act->key_seq);
#endif
		}
		else {
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

	struct key *kptr = *keys;
	for(a=0; a<key_cnt; a++) {
		printf("key_id %i type %c with key %s (key length: %i)\n", kptr->key_id, 
				kptr->type, kptr->key_seq, kptr->key_len);
		kptr = kptr->next;
	}
	STDLINE
#endif

	key_cnt = line_cnt;
	key_ptr = *keys;

	return line_cnt;
}

/* Looks for the key with keyid key_id and sets the d_key pointer to the 
 * address of the key. If no matching key is found the pointer is not touched.
 */
void
get_key (
		int key_id,
		struct key **d_key
		)
{
	register int a;
	struct key *itr_key = key_ptr;

	if(key_cnt == 0)
		return;
	
	for(a=0; a<key_cnt && itr_key != NULL; a++) {
		if(itr_key->key_id == key_id) {
			*d_key = itr_key;
			return;
		}
	}
	
	return;
}
