/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

/*
 * Hack to time aead routines
 *
 * We use it in 6 places, 3 pairs of encrypt/decrypt
 *   cookies
 *   client to server
 *   server to client
 *
 *   This code is the minimal code to get timing of the crypto functions
 *   so we can compare libaes_siv with OpenSSL
 *   Then round up so we can test things.
 *
 *   For cookies, there are 2 key lengths involved:
 *     the length of the keys inside the cookie: c2s and s2c
 *     the length of the key used to make the cookie
 *
 * RFC5297, October 2008
 * Synthetic Initialization Vector (SIV) Authenticated Encryption
 *           Using the Advanced Encryption Standard (AES)
 *
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include "aes_siv.h"

#define UNUSED_ARG(arg)         ((void)(arg))
#define INSIST(x)       assert(x)

#if OPENSSL_VERSION_NUMBER > 0x20000000L

int samplesize = 1000000;

/* Some copied from nts.h */
#define AEAD_AES_SIV_CMAC_256 15
#define AEAD_AES_SIV_CMAC_384 16
#define AEAD_AES_SIV_CMAC_512 17
#define AEAD_AES_SIV_CMAC_256_KEYLEN 32
#define AEAD_AES_SIV_CMAC_384_KEYLEN 48
#define AEAD_AES_SIV_CMAC_512_KEYLEN 64
#define NTS_MAX_KEYLEN 64
#define NTS_MAX_COOKIELEN 192
#define NONCE_LENGTH 16
#define AD_LENGTH 20
#define AEAD_LENGTH 4
#define TAG_LENGTH 16

AES_SIV_CTX* lib_ctx;
EVP_CIPHER_CTX *ssl_ctx;

uint32_t key_I = 12345;  /* keep 4 byte alignment */
uint32_t aead = 54321;
uint8_t nonce[NONCE_LENGTH];
uint8_t key[NTS_MAX_KEYLEN];
uint8_t c2s[NTS_MAX_KEYLEN];
uint8_t s2c[NTS_MAX_KEYLEN];

uint8_t cookie[NTS_MAX_COOKIELEN];
uint8_t cookie2[NTS_MAX_COOKIELEN];
int cookielen;
uint32_t key_I_out;
uint8_t nonce_out[NONCE_LENGTH];
uint32_t aead_out;
uint8_t c2s_out[NTS_MAX_KEYLEN];
uint8_t s2c_out[NTS_MAX_KEYLEN];

static void nts_log_ssl_error(void);

static void CookieEncryptLib(int cookie_keylen, int wire_keylen, const char* name) {
        uint8_t plaintext[NTS_MAX_COOKIELEN];
        int used, plainlength;
        uint8_t * finger;
        size_t left;
        struct timespec start, stop;
        double fast;

	memset(cookie, 0, sizeof(cookie));

        finger = plaintext;
        memcpy(finger, &aead, sizeof(aead));
        finger += sizeof(aead);
        memcpy(finger, c2s, wire_keylen);
        finger += wire_keylen;
        memcpy(finger, s2c, wire_keylen);
        finger += wire_keylen;
        plainlength = finger-plaintext;

        /* collect associated data */
        finger = cookie;

        memcpy(finger, &key_I, sizeof(key_I));
        finger += sizeof(key_I);

        memcpy(finger, nonce, NONCE_LENGTH);
        finger += NONCE_LENGTH;

        used = finger-cookie;
        left = sizeof(cookie)-used;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < samplesize; i++) {
            bool ok;
            left = sizeof(cookie)-used;
	    ok = AES_SIV_Encrypt(lib_ctx,
		/* left: in: max out length, out: length used */
		finger, &left,
		key, cookie_keylen,
		nonce, NONCE_LENGTH,
		plaintext, plainlength,
		cookie, AD_LENGTH);
            if (!ok) {
                printf("CookieEncryptLib: Error from AES_SIV_Encrypt\n");
                exit(1);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
        used += left;
	cookielen = used;
        INSIST(used <= NTS_MAX_COOKIELEN);
        fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
        printf("LIB Encrypt  %3d %3d %3d %6.0f %7.3f  %s\n",
               cookie_keylen, wire_keylen, used, fast/samplesize,  fast/1E9, name);
}


static void CookieDecryptLib(int cookie_keylen, int wire_keylen) {
        uint8_t plaintext[NTS_MAX_COOKIELEN];
        int used;
        size_t  plainlength, cipherlength;
        uint8_t * finger;
        uint8_t * mynonce;
        struct timespec start, stop;
        double fast;

        /* associated data */
        finger = cookie;
        memcpy(&key_I_out, finger, sizeof(key_I_out));
        finger += sizeof(key_I_out);
	mynonce = finger;
        memcpy(nonce_out, mynonce, NONCE_LENGTH);
        finger += NONCE_LENGTH;

        used = finger-cookie;

        cipherlength = cookielen - AD_LENGTH;
        plainlength = NTS_MAX_COOKIELEN;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < samplesize; i++) {
            bool ok;
            plainlength = NTS_MAX_COOKIELEN;  /* Gets updated in loop */
	    ok = AES_SIV_Decrypt(lib_ctx,
		/* plainlength: in: max out length, out: length used */
		plaintext, &plainlength,
		key, cookie_keylen,
		mynonce, NONCE_LENGTH,
		finger, cipherlength,
		cookie, AD_LENGTH);
            if (!ok) {
                printf("CookieDecryptLib: Error from AES_SIV_Decrypt\n");
                exit(1);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
	used += plainlength;
	used += TAG_LENGTH;
        INSIST(used <= NTS_MAX_COOKIELEN);
        fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
        printf("LIB Decrypt  %3d %3d %3d %6.0f %7.3f\n",
               cookie_keylen, wire_keylen, used, fast/samplesize,  fast/1E9);

        finger = plaintext;
        memcpy(&aead_out, finger, sizeof(aead_out));
        finger += sizeof(aead);
        memcpy(c2s_out, finger, wire_keylen);
        finger += wire_keylen;
        memcpy(s2c_out, finger, wire_keylen);
        finger += wire_keylen;
}


static void CookieCheck(int length, int wire_keylen) {

  if (length != cookielen)
    printf("Length mismatch: %d, %d\n", length, cookielen);
  if (key_I != key_I_out)
    printf("key_I mismatch: %u, %u\n", key_I, key_I_out);
  if (memcmp(nonce, nonce_out, NONCE_LENGTH))
    printf("nonce mismatch\n");

/* Encryypted stuff */
  if (aead != aead_out)
    printf("aead mismatch: %u, %u\n", aead, aead_out);
  if (memcmp(c2s, c2s_out, wire_keylen))
    printf("c2s, mismatch\n");
  if (memcmp(s2c, s2c_out, wire_keylen))
    printf("s2c, mismatch\n");

}


static void CookieCompare(int length) {
  uint32_t *c1 = (uint32_t*)cookie;
  uint32_t *c2 = (uint32_t*)cookie2;

  if (0 == memcmp(cookie, cookie2, length)) return;

  for (int i = 0; i*4 < length+8; i++) {
    printf("%3d: %08x %08x\n", i*4, c1[i], c2[i]);
  }

}

static void CookieEncryptSSL(int cookie_keylen, int wire_keylen, const char* name) {
	const EVP_CIPHER *cipher;
        uint8_t plaintext[NTS_MAX_COOKIELEN];
        int used, plainlength;
        uint8_t * finger;
        uint8_t * tag;
        int left, adlen;
        struct timespec start, stop;
        double fast;

	memset(cookie, 0, sizeof(cookie));

	cipher = EVP_CIPHER_fetch(NULL, name, NULL);
	if (NULL==cipher) {
		/* This happens on a FIPS box. */
		printf("CookieEncryptSSL: Can't find cipher %s.\n", name);
		exit(1);
	}
        finger = plaintext;
        memcpy(finger, &aead, sizeof(aead));
        finger += sizeof(aead);
        memcpy(finger, c2s, wire_keylen);
        finger += wire_keylen;
        memcpy(finger, s2c, wire_keylen);
        finger += wire_keylen;
        plainlength = finger-plaintext;

        /* collect associated data */
        finger = cookie;
        memcpy(finger, &key_I, sizeof(key_I));
        finger += sizeof(key_I);
        memcpy(finger, nonce, NONCE_LENGTH);
        finger += NONCE_LENGTH;
	adlen = finger-cookie;

	tag = finger;                  /* prepend tag */
	finger += TAG_LENGTH;

        used = finger-cookie;
        left = sizeof(cookie)-used;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < samplesize; i++) {
            used = finger-cookie;
            left = sizeof(cookie)-used;
	    /* FIXME: move cipher init out of loop */
            if(1 != EVP_EncryptInit_ex2(ssl_ctx, cipher, key, NULL, NULL)) {
                printf("CookieEncryptSSL: Error from EVP_EncryptInit_ex2\n");
                exit(1);
            }
            if(1 != EVP_EncryptUpdate(ssl_ctx, NULL, &left, cookie, adlen)) {
                printf("CookieEncryptSSL: EVP_EncryptUpdate--AD\n");
                exit(1);
            }
	    /* bug in real cookie code: nonce used twice */
            left = sizeof(cookie)-used;
            if(1 != EVP_EncryptUpdate(ssl_ctx, NULL, &left, nonce, NONCE_LENGTH)) {
                printf("CookieEncryptSSL: EVP_EncryptUpdate--nonce\n");
                exit(1);
            }
            left = sizeof(cookie)-used;
            if(1 != EVP_EncryptUpdate(ssl_ctx, finger, &left, plaintext, plainlength)) {
                printf("CookieEncryptSSL: EVP_EncryptUpdate--AEAD\n");
                exit(1);
            }
	    used += left;
            if(1 != EVP_EncryptFinal_ex(ssl_ctx, finger+used, &left)) {
                printf("CookieEncryptSSL: EVP_EncryptFinal_ex\n");
                exit(1);
            }
            if(1 != EVP_CIPHER_CTX_ctrl(ssl_ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag)) {
                printf("CookieEncryptSSL: EVP_CIPHER_CTX_ctrl\n");
                exit(1);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
        used += left;
	cookielen = used;
        INSIST(used <= NTS_MAX_COOKIELEN);
        fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
        printf("SSL Encrypt  %3d %3d %3d %6.0f %7.3f  %s\n",
               cookie_keylen, wire_keylen, used, fast/samplesize,  fast/1E9, name);
}

static void CookieDecryptSSL(int cookie_keylen, int wire_keylen, const char* name) {
        const EVP_CIPHER *cipher;
        uint8_t plaintext[NTS_MAX_COOKIELEN];
        int used, left;
        size_t  cipherlength;
        uint8_t * finger;
        uint8_t * tag;
        uint8_t * mynonce;
        struct timespec start, stop;
        double fast;

        cipher = EVP_CIPHER_fetch(NULL, name, NULL);
        if (NULL==cipher) {
                /* This happens on a FIPS box. */
                printf("CookieDecryptSSL: Can't find cipheri %s.\n", name);
                exit(1);
        }

        /* associated data */
        finger = cookie;
        memcpy(&key_I_out, finger, sizeof(key_I_out));
        finger += sizeof(key_I_out);
	mynonce = finger;
        memcpy(nonce_out, mynonce, NONCE_LENGTH);
        finger += NONCE_LENGTH;
	tag = finger;
	finger += TAG_LENGTH;

        used = finger-cookie;
        left = sizeof(cookie)-used;

	cipherlength = cookielen -AD_LENGTH -TAG_LENGTH;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < samplesize; i++) {
            used = finger-cookie;
            left = sizeof(cookie)-used;
            /* FIXME: move cipher init out of loop */
            if(1 != EVP_DecryptInit_ex2(ssl_ctx, cipher, key, NULL, NULL)) {
                printf("CookieDecryptSSL: Error from EVP_DecryptInit_ex2\n");
                exit(1);
            }
            if(1 != EVP_CIPHER_CTX_ctrl(ssl_ctx, EVP_CTRL_AEAD_SET_TAG, 16, tag)) {
                printf("CookieDencryptSSL: EVP_CIPHER_CTX_ctrl\n");
                exit(1);
            }
            left = sizeof(cookie)-used;
            if(1 != EVP_DecryptUpdate(ssl_ctx, NULL, &left, cookie, AD_LENGTH)) {
                printf("CookieDecryptSSL: EVP_DecryptUpdate--AD\n");
                exit(1);
            }
            /* bug in real cookie code: nonce used twice */
            if(1 != EVP_DecryptUpdate(ssl_ctx, NULL, &left, mynonce, NONCE_LENGTH)) {
                printf("CookieDecryptSSL: EVP_DecryptUpdate--nonce\n");
                exit(1);
            }
            left = sizeof(plaintext);
            if(1 != EVP_DecryptUpdate(ssl_ctx, plaintext, &left, finger, cipherlength)) {
                printf("CookieDecryptSSL: EVP_DecryptUpdate--AEAD\n");
		nts_log_ssl_error();
                exit(1);
            }
            used += left;
            left = sizeof(cookie)-used;
            if(1 != EVP_DecryptFinal_ex(ssl_ctx, finger+used, &left)) {
                printf("CookieDecryptSSL: EVP_DecryptFinal_ex\n");
                exit(1);
            }
            left = sizeof(cookie)-used;
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
        INSIST(used <= NTS_MAX_COOKIELEN);
        fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
        printf("SSL Decrypt  %3d %3d %3d %6.0f %7.3f\n",
               cookie_keylen, wire_keylen, used, fast/samplesize,  fast/1E9);

        finger = plaintext;
        memcpy(&aead_out, finger, sizeof(aead_out));
        finger += sizeof(aead);
        memcpy(c2s_out, finger, wire_keylen);
        finger += wire_keylen;
        memcpy(s2c_out, finger, wire_keylen);
        finger += wire_keylen;
}

static void nts_log_ssl_error(void) {
        char buff[256];
        int err = ERR_get_error();
        SSL_load_error_strings();        /* Needed on NetBSD */
        while (0 != err) {
                ERR_error_string_n(err, buff, sizeof(buff));
                printf("NTS err: %s\n", buff);
                err = ERR_get_error();
        }
}


int main(int argc, char *argv[])
{
	char *ctimetxt;
	time_t now;
	char buff[256];

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	setlinebuf(stdout);

        lib_ctx = AES_SIV_CTX_new();
        ssl_ctx = EVP_CIPHER_CTX_new();
	nts_log_ssl_error();

        RAND_bytes(nonce, NONCE_LENGTH);
        RAND_bytes(key, NTS_MAX_KEYLEN);
        RAND_bytes(c2s, NTS_MAX_KEYLEN);
        RAND_bytes(s2c, NTS_MAX_KEYLEN);

	now = time(NULL);
	ctimetxt = ctime(&now);
	ctimetxt[24] = 0;	/* Hack: smash return */
	gethostname(buff, sizeof(buff));
	printf("# %s on %s\n", ctimetxt, buff);
	printf("# %s\n", OPENSSL_VERSION_TEXT);
	printf("\n");

	printf("#     cKL=cookieKeyLen, wKL=wireKeyLen, CL=cookieLen\n");
	printf("#            cKL wKL  CL  ns/op sec/run  Cookie Alg\n");

	/* 1st arg is length of cookie key */
	/* 2nd arg is length of c2s and s2c */

	/* AES-128-SIV, RFC 5297 */
	/* default minimal lengths */
	CookieEncryptLib(32, 32, "AES_SIV_CMAC_256");
	CookieDecryptLib(32, 32);
	CookieCheck(104, 32);
	memcpy(cookie2, cookie, NTS_MAX_COOKIELEN);
	CookieEncryptSSL(32, 32, "AES-128-SIV");
	CookieCompare(104);
	CookieDecryptSSL(32, 32, "AES-128-SIV");
	CookieCheck(104, 32);
	printf("\n");

	/* longer cookie key, same size cookie */
	CookieEncryptLib(64, 32, "AES_SIV_CMAC_512");
	CookieDecryptLib(64, 32);
	CookieCheck(104, 32);
	memcpy(cookie2, cookie, NTS_MAX_COOKIELEN);
	CookieEncryptSSL(64, 32, "AES-256-SIV");
	CookieCompare(104);
	CookieDecryptSSL(64, 32, "AES-256-SIV");
	CookieCheck(104, 32);
	printf("\n");

	/* Longer wire key, bigger cookie */
	CookieEncryptLib(32, 64, "AES_SIV_CMAC_256");
	CookieDecryptLib(32, 64);
	CookieCheck(168, 32);
	memcpy(cookie2, cookie, NTS_MAX_COOKIELEN);
	CookieEncryptSSL(32, 64, "AES-128-SIV");
	CookieCompare(168);
	CookieDecryptSSL(32, 64, "AES-128-SIV");
	CookieCheck(168, 64);
	printf("\n");

	/* AES-128-GCM-SIV, RFC 8452 */
	/* FIXME: need to set ??? length */
	CookieEncryptSSL(16, 32, "AES-128-GCM-SIV");
	CookieDecryptSSL(16, 32, "AES-128-GCM-SIV");
	CookieCheck(104, 32);
	CookieEncryptSSL(32, 32, "AES-256-GCM-SIV");
	CookieDecryptSSL(32, 32, "AES-256-GCM-SIV");
	CookieCheck(104, 32);

	return 0;
}
#else
int main(int argc, char *argv[])
{
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);
	return 1;
}
#endif  // OPENSSL_VERSION_NUMBER
