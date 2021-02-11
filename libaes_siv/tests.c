/* Copyright Akamai Technologies, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#define _POSIX_C_SOURCE 200112L
#define _ISOC99_SOURCE 1

#include "config.h"
#include "aes_siv.h"

#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/opensslv.h>

static void debug(const char *label, const unsigned char *hex, size_t len) {
        size_t i;
        printf("%16s: ", label);
        for (i = 0; i < len; i++) {
                if (i > 0 && i % 16 == 0) {
                        printf("\n                  ");
                }
                printf("%.2x", (int)hex[i]);
                if (i > 0 && i % 4 == 3) {
                        printf(" ");
                }
        }
        printf("\n");
}

static int fail_allocation_counter = -1;

static void* mock_malloc(size_t num) {
        if(fail_allocation_counter < 0) {
                return malloc(num);
        }
        if(fail_allocation_counter-- == 0) {
                return NULL;
        }
        return malloc(num);
}

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
static void* mock_malloc_ex(size_t num, const char *file, int line) {
	(void)file;
	(void)line;
	return mock_malloc(num);
}

static void *mock_realloc_ex(void *mem, size_t num, const char *file, int line) {
	(void)file;
	(void)line;
	return realloc(mem, num);
}

static void mock_free_ex(void *mem, const char* file, int line) {
	(void)file;
	(void)line;
	free(mem);
}
#endif

/* This needs to be the first test case because CRYPTO_set_mem_functions()
   will fail once any allocations have happened.
*/
static void test_malloc_failure(void) {
        int ret, i=0;
        AES_SIV_CTX *ctx;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
        ret = CRYPTO_set_mem_functions(mock_malloc, realloc, free);
#else
	ret = CRYPTO_set_mem_functions(mock_malloc_ex, mock_realloc_ex, mock_free_ex);
#endif
        assert(ret == 1);

        printf("Test allocation failure cases:\n" );

        do {
                fail_allocation_counter = i++;
        } while((ctx = AES_SIV_CTX_new()) == NULL);
        assert(i > 1);
        printf("AES_SIV_CTX_new() succeeds after %d successful allocations.\n", i-1);
        AES_SIV_CTX_free(ctx);
        fail_allocation_counter = -1;
}

static void test_cleanup_before_free(void) {
	printf("Test cleanup before free: ");
	AES_SIV_CTX *ctx = AES_SIV_CTX_new();
	assert(ctx != NULL);
	AES_SIV_CTX_cleanup(ctx);
	AES_SIV_CTX_free(ctx);
	printf("OK\n");
}

static void test_vector_1(void) {
        const unsigned char key[] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        };

        const unsigned char ad[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };

        const unsigned char plaintext[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};

        const unsigned char ciphertext[] = {
                0x85, 0x63, 0x2d, 0x07, 0xc6, 0xe8, 0xf3, 0x7f,
                0x95, 0x0a, 0xcd, 0x32, 0x0a, 0x2e, 0xcc, 0x93,
                0x40, 0xc0, 0x2b, 0x96, 0x90, 0xc4, 0xdc, 0x04,
                0xda, 0xef, 0x7f, 0x6a, 0xfe, 0x5c
        };

        unsigned char ciphertext_out[256];
        unsigned char plaintext_out[256];

        size_t plaintext_len = sizeof plaintext_out;
        size_t ciphertext_len = sizeof ciphertext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("Test vector 1:\n");
        debug("key", key, sizeof key);
        debug("AD", ad, sizeof ad);
        debug("plaintext", plaintext, sizeof plaintext);
        debug("exp. ciphertext", ciphertext, sizeof ciphertext);

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        printf("Encryption:\n");
        ret = AES_SIV_Encrypt(ctx, ciphertext_out, &ciphertext_len, key,
                              sizeof key, NULL, 0, plaintext, sizeof plaintext,
                              ad, sizeof ad);
        assert(ret == 1);
        assert(ciphertext_len == sizeof ciphertext);
        assert(!memcmp(ciphertext, ciphertext_out, ciphertext_len));

        printf("Decryption:\n");
        ret = AES_SIV_Decrypt(ctx, plaintext_out, &plaintext_len, key,
                              sizeof key, NULL, 0, ciphertext_out,
                              ciphertext_len, ad, sizeof ad);
        assert(ret == 1);
        assert(plaintext_len == sizeof plaintext);
        assert(!memcmp(plaintext, plaintext_out, plaintext_len));
        AES_SIV_CTX_free(ctx);
}

static void test_vector_2(void) {
        const unsigned char key[] = {
                0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78,
                0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70,
                0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
        };

        const unsigned char ad1[] = {
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                0xde, 0xad, 0xda, 0xda, 0xde, 0xad, 0xda, 0xda,
                0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
                0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
        };

        const unsigned char ad2[] = {
                0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
                0x90, 0xa0
        };

        const unsigned char nonce[] = {
                0x09, 0xf9, 0x11, 0x02, 0x9d, 0x74, 0xe3, 0x5b,
                0xd8, 0x41, 0x56, 0xc5, 0x63, 0x56, 0x88, 0xc0
        };

        const unsigned char plaintext[] = {
                0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
                0x73, 0x6f, 0x6d, 0x65, 0x20, 0x70, 0x6c, 0x61,
                0x69, 0x6e, 0x74, 0x65, 0x78, 0x74, 0x20, 0x74,
                0x6f, 0x20, 0x65, 0x6e, 0x63, 0x72, 0x79, 0x70,
                0x74, 0x20, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20,
                0x53, 0x49, 0x56, 0x2d, 0x41, 0x45, 0x53
        };

        const unsigned char ciphertext[] = {
                0x7b, 0xdb, 0x6e, 0x3b, 0x43, 0x26, 0x67, 0xeb,
                0x06, 0xf4, 0xd1, 0x4b, 0xff, 0x2f, 0xbd, 0x0f,
                0xcb, 0x90, 0x0f, 0x2f, 0xdd, 0xbe, 0x40, 0x43,
                0x26, 0x60, 0x19, 0x65, 0xc8, 0x89, 0xbf, 0x17,
                0xdb, 0xa7, 0x7c, 0xeb, 0x09, 0x4f, 0xa6, 0x63,
                0xb7, 0xa3, 0xf7, 0x48, 0xba, 0x8a, 0xf8, 0x29,
                0xea, 0x64, 0xad, 0x54, 0x4a, 0x27, 0x2e, 0x9c,
                0x48, 0x5b, 0x62, 0xa3, 0xfd, 0x5c, 0x0d
        };

        unsigned char ciphertext_out[256];
        unsigned char plaintext_out[256];

        AES_SIV_CTX *ctx;
        int ret;

        printf("Test vector 2:\n");
        debug("key", key, sizeof key);
        debug("AD1", ad1, sizeof ad1);
        debug("AD2", ad2, sizeof ad2);
        debug("nonce", nonce, sizeof nonce);
        debug("plaintext", plaintext, sizeof plaintext);
        debug("exp. ciphertext", ciphertext, sizeof ciphertext);

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        printf("Encryption:\n");
        ret = AES_SIV_Init(ctx, key, sizeof key);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, ad1, sizeof ad1);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, ad2, sizeof ad2);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, nonce, sizeof nonce);
        assert(ret == 1);
        ret = AES_SIV_EncryptFinal(ctx, ciphertext_out, ciphertext_out + 16,
                                   plaintext, sizeof plaintext);
        assert(ret == 1);
        debug("IV || C", ciphertext_out, sizeof plaintext + 16);
        assert(!memcmp(ciphertext_out, ciphertext, sizeof ciphertext));

        printf("Decryption:\n");
        ret = AES_SIV_Init(ctx, key, sizeof key);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, ad1, sizeof ad1);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, ad2, sizeof ad2);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx, nonce, sizeof nonce);
        assert(ret == 1);
        ret = AES_SIV_DecryptFinal(ctx, plaintext_out, ciphertext_out,
                                   ciphertext_out + 16, sizeof plaintext);
        assert(ret == 1);
        debug("plaintext", plaintext_out, sizeof plaintext);
        assert(!memcmp(plaintext_out, plaintext, sizeof plaintext));
        AES_SIV_CTX_free(ctx);
}

static void test_384bit(void) {
        const unsigned char key[] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
        };

        const unsigned char ad[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };

        const unsigned char plaintext[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
        };

        const unsigned char ciphertext[] = {
                0x89, 0xe8, 0x69, 0xb9, 0x32, 0x56, 0x78, 0x51,
                0x54, 0xf0, 0x96, 0x39, 0x62, 0xfe, 0x07, 0x40,
                0xef, 0xf3, 0x56, 0xe4, 0x2d, 0xec, 0x1f, 0x4f,
                0xeb, 0xde, 0xd3, 0x66, 0x42, 0xf2
        };

        unsigned char ciphertext_out[256];
        unsigned char plaintext_out[256];

        size_t plaintext_len = sizeof plaintext_out;
        size_t ciphertext_len = sizeof ciphertext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("384-bit key test:\n");
        debug("key", key, sizeof key);
        debug("AD", ad, sizeof ad);
        debug("plaintext", plaintext, sizeof plaintext);
        debug("exp. ciphertext", ciphertext, sizeof ciphertext);

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        printf("Encryption:\n");
        ret = AES_SIV_Encrypt(ctx, ciphertext_out, &ciphertext_len, key,
                              sizeof key, NULL, 0, plaintext, sizeof plaintext,
                              ad, sizeof ad);
        assert(ret == 1);
        assert(ciphertext_len == sizeof ciphertext);
        assert(!memcmp(ciphertext, ciphertext_out, ciphertext_len));

        printf("Decryption:\n");
        ret = AES_SIV_Decrypt(ctx, plaintext_out, &plaintext_len, key,
                              sizeof key, NULL, 0, ciphertext_out,
                              ciphertext_len, ad, sizeof ad);
        assert(ret == 1);
        assert(plaintext_len == sizeof plaintext);
        assert(!memcmp(plaintext, plaintext_out, plaintext_len));
        AES_SIV_CTX_free(ctx);
}

static void test_512bit(void) {
        const unsigned char key[] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
        };

        const unsigned char ad[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };

        const unsigned char plaintext[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
        };

        const unsigned char ciphertext[] = {
                0x72, 0x4d, 0xfb, 0x2e, 0xaf, 0x94, 0xdb, 0xb1,
                0x9b, 0x0b, 0xa3, 0xa2, 0x99, 0xa0, 0x80, 0x1e,
                0xf3, 0xb0, 0x5a, 0x55, 0x49, 0x8e, 0xc2, 0x55,
                0x26, 0x90, 0xb8, 0x98, 0x10, 0xe4
        };

        unsigned char ciphertext_out[256];
        unsigned char plaintext_out[256];

        size_t plaintext_len = sizeof plaintext_out;
        size_t ciphertext_len = sizeof ciphertext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("512-bit key test:\n");
        debug("key", key, sizeof key);
        debug("AD", ad, sizeof ad);
        debug("plaintext", plaintext, sizeof plaintext);
        debug("exp. ciphertext", ciphertext, sizeof ciphertext);

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        printf("Encryption:\n");
        ret = AES_SIV_Encrypt(ctx, ciphertext_out, &ciphertext_len, key,
                              sizeof key, NULL, 0, plaintext, sizeof plaintext,
                              ad, sizeof ad);
        assert(ret == 1);
        assert(ciphertext_len == sizeof ciphertext);
        assert(!memcmp(ciphertext, ciphertext_out, ciphertext_len));

        printf("Decryption:\n");
        ret = AES_SIV_Decrypt(ctx, plaintext_out, &plaintext_len, key,
                              sizeof key, NULL, 0, ciphertext_out,
                              ciphertext_len, ad, sizeof ad);
        assert(ret == 1);
        assert(plaintext_len == sizeof plaintext);
        assert(!memcmp(plaintext, plaintext_out, plaintext_len));
        AES_SIV_CTX_free(ctx);
}

static void test_highlevel_with_nonce(void) {
        const unsigned char key[] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        };

        const unsigned char ad[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };

        const unsigned char nonce[] = {
                0x09, 0xf9, 0x11, 0x02, 0x9d, 0x74, 0xe3, 0x5b,
                0xd8, 0x41, 0x56, 0xc5, 0x63, 0x56, 0x88, 0xc0
        };

        const unsigned char plaintext[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
        };

        unsigned char ciphertext_out[256];
        unsigned char plaintext_out[256];

        size_t plaintext_len = sizeof plaintext_out;
        size_t ciphertext_len = sizeof ciphertext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("Test high-level interface with non-NULL nonce:\n");
        debug("key", key, sizeof key);
        debug("AD", ad, sizeof ad);
        debug("nonce", nonce, sizeof nonce);
        debug("plaintext", plaintext, sizeof plaintext);

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        printf("Encryption:\n");
        ret = AES_SIV_Encrypt(ctx, ciphertext_out, &ciphertext_len, key,
                              sizeof key, nonce, sizeof nonce, plaintext,
                              sizeof plaintext, ad, sizeof ad);
        assert(ret == 1);

        printf("Decryption:\n");
        ret = AES_SIV_Decrypt(ctx, plaintext_out, &plaintext_len, key,
                              sizeof key, nonce, sizeof nonce, ciphertext_out,
                              ciphertext_len, ad, sizeof ad);
        assert(ret == 1);
        assert(plaintext_len == sizeof plaintext);
        assert(!memcmp(plaintext, plaintext_out, plaintext_len));
        AES_SIV_CTX_free(ctx);
}

static void test_copy(void) {
        const unsigned char key[] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        };

        const unsigned char ad[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };

        const unsigned char plaintext1[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
        };

        const unsigned char plaintext2[] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xef
        };

        const unsigned char ciphertext[] = {
                0x85, 0x63, 0x2d, 0x07, 0xc6, 0xe8, 0xf3, 0x7f,
                0x95, 0x0a, 0xcd, 0x32, 0x0a, 0x2e, 0xcc, 0x93,
                0x40, 0xc0, 0x2b, 0x96, 0x90, 0xc4, 0xdc, 0x04,
                0xda, 0xef, 0x7f, 0x6a, 0xfe, 0x5c
        };

        unsigned char ciphertext1_out[256], ciphertext2_out[256];

        AES_SIV_CTX *ctx1, *ctx2, *ctx3;
        int ret;

        ctx1 = AES_SIV_CTX_new();
        assert(ctx1 != NULL);
        ctx2 = AES_SIV_CTX_new();
        assert(ctx2 != NULL);
        ctx3 = AES_SIV_CTX_new();
        assert(ctx3 != NULL);

        ret = AES_SIV_Init(ctx1, key, sizeof key);
        assert(ret == 1);
        ret = AES_SIV_CTX_copy(ctx2, ctx1);
        assert(ret == 1);

        ret = AES_SIV_AssociateData(ctx1, ad, sizeof ad);
        assert(ret == 1);
        ret = AES_SIV_AssociateData(ctx2, ad, sizeof ad);
        assert(ret == 1);

        ret = AES_SIV_CTX_copy(ctx3, ctx1);
        assert(ret == 1);

        ret = AES_SIV_EncryptFinal(ctx1, ciphertext1_out, ciphertext1_out + 16,
                                   plaintext1, sizeof plaintext1);
        assert(ret == 1);
        assert(!memcmp(ciphertext, ciphertext1_out, sizeof ciphertext));

        ret = AES_SIV_EncryptFinal(ctx2, ciphertext2_out, ciphertext2_out + 16,
                                   plaintext1, sizeof plaintext1);
        assert(ret == 1);
        assert(!memcmp(ciphertext, ciphertext2_out, sizeof ciphertext));

        ret = AES_SIV_EncryptFinal(ctx3, ciphertext2_out, ciphertext2_out + 16,
                                   plaintext2, sizeof plaintext2);
        assert(ret == 1);
        assert(memcmp(ciphertext, ciphertext2_out, sizeof ciphertext));

        AES_SIV_CTX_free(ctx1);
        AES_SIV_CTX_free(ctx2);
        AES_SIV_CTX_free(ctx3);
}

static void test_bad_key(void) {
        static const unsigned char key[40];
        static const unsigned char ad[16];
        static const unsigned char plaintext[16];

        unsigned char ciphertext_out[256];
        size_t ciphertext_len = sizeof ciphertext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("Test bad key size: ");

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        ret = AES_SIV_Encrypt(ctx, ciphertext_out, &ciphertext_len, key,
                              sizeof key, NULL, 0, plaintext, sizeof plaintext,
                              ad, sizeof ad);
        assert(ret == 0);

        ret = AES_SIV_Init(ctx, key, sizeof key);
        assert(ret == 0);

        AES_SIV_CTX_free(ctx);
        printf("OK\n");
}

static void test_decrypt_failure(void) {
        static const unsigned char key[32];
        static const unsigned char ad[16];
        static const unsigned char ciphertext[32];

        unsigned char plaintext_out[256];
        size_t plaintext_len = sizeof plaintext_out;

        AES_SIV_CTX *ctx;
        int ret;

        printf("Test decryption failure:\n");

        ctx = AES_SIV_CTX_new();
        assert(ctx != NULL);

        ret = AES_SIV_Decrypt(ctx, plaintext_out, &plaintext_len, key,
                              sizeof key, NULL, 0, ciphertext,
                              sizeof ciphertext, ad, sizeof ad);
        assert(ret == 0);

        AES_SIV_CTX_free(ctx);
}

int main(void) {
        test_malloc_failure();
	test_cleanup_before_free();
        test_vector_1();
        test_vector_2();
        test_384bit();
        test_512bit();
        test_highlevel_with_nonce();
        test_copy();
        test_bad_key();
        test_decrypt_failure();
        return 0;
}
