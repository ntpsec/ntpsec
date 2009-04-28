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

#ifndef CRYPTO_H
#define CRYPTO_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ntp_fp.h>
#include <ntp.h>
#include <ntp_md5.h>
#include <ntp_stdlib.h>

#include "utilities.h"
#include "sntp-opts.h"


/* #include "sntp-opts.h" */

struct key {
	int key_id;
	int key_len;
	char type;
	char key_seq[16];
	struct key *next;
};

int auth_md5(char *pkt_data, int mac_size, struct key *cmp_key);
int auth_init(const char *keyfile, struct key **keys);
void get_key(int key_id, struct key **d_key);


#endif
