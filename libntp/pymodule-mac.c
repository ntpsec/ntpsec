/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include "config.h"

#include <string.h>
#include <ctype.h>

#include "ntp_types.h"
#include "ntp_stdlib.h"

#include "pymodule-mac.h"

/* Don't include Python.h */

#define OPENSSL_SUPPRESS_DEPRECATED 1
#include <openssl/evp.h>
#include <openssl/cmac.h>

/* Slightly older version of OpenSSL */
/* Similar hack in ssl_init.c and attic/digest-timing.c */
#ifndef EVP_MD_CTX_new
#define EVP_MD_CTX_new() EVP_MD_CTX_create()
#endif
#ifndef EVP_MD_CTX_reset
#define EVP_MD_CTX_reset(ctx) EVP_MD_CTX_init(ctx)
#endif

/* Needed on OpenSSL < 1.1.0 */
static void init_ssl(void) {
	static bool init_done = false;
	if (init_done)
		return;
	init_done = true;
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
}

/* xx = ntp.ntpc.checkname(name)
 * returns false if algorithm name is invalid. */

int do_checkname(const char *name)
{
	char upcase[100];
	const EVP_MD *digest;
	const EVP_CIPHER *cipher;

	init_ssl();

        strlcpy(upcase, name, sizeof(upcase));
	for (int i=0; upcase[i]!=0; i++) {
		upcase[i] = toupper(upcase[i]);
	}

        digest = EVP_get_digestbyname(upcase);
	if (NULL != digest) {
		return true;
        }

        if ((strcmp(upcase, "AES") == 0) || (strcmp(upcase, "AES128CMAC") == 0)) {
                strlcpy(upcase, "AES-128", sizeof(upcase));
        }
        strlcat(upcase, "-CBC", sizeof(upcase));
	cipher = EVP_get_cipherbyname(upcase);
	if (NULL != cipher) {
		int length = EVP_CIPHER_key_length(cipher);
		return length;
	}

	return false;
}


/* mac = ntp.ntpc.mac(data, key, name) */

#if EVP_MAX_MD_SIZE > MAX_MAC_LENGTH
#error "MAX_MAC_LENGTH isn't big enough"
/* FIXME: Does this cover CMAC ?? */
#endif

void do_mac(char *name,
	uint8_t *data, size_t datalen,
	uint8_t *key, size_t keylen,
	uint8_t mac[MAX_MAC_LENGTH], size_t *maclen)
{
	char upcase[100];
	static EVP_MD_CTX *digest_ctx = NULL;
	static CMAC_CTX *cmac_ctx = NULL;
	const EVP_MD *digest;
	const EVP_CIPHER *cipher;
	size_t cipherlen;
	uint8_t newkey[EVP_MAX_KEY_LENGTH];

	init_ssl();

        strlcpy(upcase, name, sizeof(upcase));
	for (int i=0; upcase[i]!=0; i++) {
		upcase[i] = toupper(upcase[i]);
	}

        digest = EVP_get_digestbyname(upcase);
	if (NULL != digest) {
		/* Old digest case, MD5, SHA1 */
		unsigned int maclenint;
		if (NULL == digest_ctx)
			digest_ctx = EVP_MD_CTX_new();
		EVP_MD_CTX_reset(digest_ctx);
		if (!EVP_DigestInit_ex(digest_ctx, digest, NULL)) {
			*maclen = 0;
			return;
		}
		EVP_DigestUpdate(digest_ctx, key, keylen);
		EVP_DigestUpdate(digest_ctx, data, (unsigned int)datalen);
		EVP_DigestFinal_ex(digest_ctx, mac, &maclenint);
		if (MAX_MAC_LENGTH < maclenint)
			maclenint = MAX_MAC_LENGTH;
		*maclen = maclenint;
		return;
	}

        if ((strcmp(upcase, "AES") == 0) || (strcmp(upcase, "AES128CMAC") == 0)) {
                strlcpy(upcase, "AES-128", sizeof(upcase));
        }
        strlcat(upcase, "-CBC", sizeof(upcase));

	cipher = EVP_get_cipherbyname(upcase);
	if (NULL == cipher) {
		*maclen = 0;
		return;
	}
	cipherlen = EVP_CIPHER_key_length(cipher);
	if (cipherlen < keylen) {
		keylen = cipherlen;		/* truncate */
	} else if (cipherlen > keylen) {
		memcpy(newkey, key, keylen);
		while (cipherlen > keylen)
			key[keylen++] = 0;	/* pad with 0s */
		key = newkey;
	}
	if (NULL == cmac_ctx)
		cmac_ctx = CMAC_CTX_new();
        if (!CMAC_Init(cmac_ctx, key, keylen, cipher, NULL)) {
                /* Shouldn't happen.  Does if wrong key_size. */
		*maclen = 0;
		return;
        }
        CMAC_Update(cmac_ctx, data, (unsigned int)datalen);
        CMAC_Final(cmac_ctx, mac, maclen);
        if (MAX_MAC_LENGTH < *maclen)
                *maclen = MAX_MAC_LENGTH;
	return;
}

