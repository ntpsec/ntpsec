#ifndef CRYPTO_H
#define CRYPTO_H

#include <ntp_md5.h>
#include <string.h>

/* #include "sntp-opts.h" */

int auth_md5(void *pkt_data, int mac_size, char *cmp_key, int cmp_size);
int auth_init(char *keyfile, struct key *keys);

struct key {
	unsigned int key_id;
	char type;
	char key_seq[16];
	struct key *next;
};

#endif
