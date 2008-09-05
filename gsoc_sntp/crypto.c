#include "crypto.h"

int key_cnt = 0;

int
auth_md5 (
		void *pkt_data,
		int mac_size,
		char *cmp_key,
		int cmp_size
	 )
{
	register int a;
	MD5Context ctx;
	char digest[16];
	
	MD5Init(&ctx);
	unsigned char *digest_data = (unsigned char *) malloc(sizeof(char) * (LEN_PKT_NOMAC + cmp_size));

	for(a=0; a<LEN_PKT_NOMAC; a++)
		digest_data[a] = (unsigned char) pkt_data[a];

	for(a=0; a<cmp_size; a++)
		digest_data[LEN_PKT_NOMAC + a] = (unsigned char) cmp_key[a];


	MD5Update(&ctx, digest_data, LEN_PKT_NOMAC + cmp_size);
	MD5Final(digest, &ctx);

	free(digest_data);

	if(strncmp(cmp_key, digest, cmp_size) != 0) {
		return 0;
	}
	else {
		return 1;
	}
}

int
auth_init (
		char *keyfile,
		struct key *keys
		)
{
	char kbuf[96];
	FILE *keyf = fopen(keyfile, "r"); 
	register int a, line_cnt;
	struct key *prev;

	if(keyf == NULL) {
		/* Do something about it */
	}

	line_cnt = 0;

	while(!feof(keyf)) {
		struct key *act = (struct key *) malloc(sizeof(struct key));

		fgets(kbuf, 96, keyf);

		sscanf(fbuf, "%i %c %16s", act->key_id, act->type, act->key_seq);

#ifdef DEBUG
		printf("auth_init: key_id %i type %c with key %s\n", act->key_id, act->type, act->key_seq);
#endif

		if(line_cnt == 0) {
			keys = act;
			prev = act;
		}
		else {
			prev->next = act;
			prev = act;
		}

		line_cnt++;
	}

	fclose(keyf);

	key_cnt = line_cnt;

	return line_cnt;
}






	}
	
	rewind(keyf);



