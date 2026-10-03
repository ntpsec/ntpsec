/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Known-answer tests for the AES-SIV wrapper in libntp/aes_siv_evp.c.
 *
 * The expected values were not produced by the code under test.  Each
 * was computed with two independent implementations, which agree byte
 * for byte:
 *   - the libaes_siv library NTPsec vendored before it was replaced by
 *     aes_siv_evp.c (libaes_siv/aes_siv.c at commit c7461cb7)
 *   - pycryptodome AES.new(key, AES.MODE_SIV, nonce=nonce), update(ad),
 *     then encrypt_and_digest(plaintext); the nonce is the last S2V
 *     component, matching the AD-then-nonce order used here
 * This wrapper skips an empty AD or nonce (NULL or zero length).
 * libaes_siv's one-shot AES_SIV_Encrypt() adds an empty AD component,
 * and also an empty nonce component when the nonce is non-NULL with
 * length 0.  Those expected tags therefore came from its incremental
 * API (AES_SIV_Init, AES_SIV_AssociateData for each non-empty
 * component only, AES_SIV_EncryptFinal) and from pycryptodome given
 * only the non-empty components.
 * RFC5297_A1 is RFC 5297 Appendix A.1.
 */

#include "config.h"
#include "aes_siv_evp.h"

#include <string.h>

#include "unity.h"
#include "unity_fixture.h"

#define TAG_LEN 16
#define PT_LEN 40
#define CT_LEN (TAG_LEN + PT_LEN)

TEST_GROUP(aes_siv_evp);

// 256-bit key, ad, nonce and pt as filled in by TEST_SETUP.
static const unsigned char kat256[CT_LEN] = {
        0xb8, 0x5e, 0xbd, 0xe2, 0xb4, 0x3f, 0xb4, 0x1b,
        0xe2, 0x77, 0x34, 0x15, 0x64, 0xf6, 0xd0, 0x0d,
        0xfc, 0xdb, 0xd3, 0x6c, 0x7b, 0x63, 0xad, 0x8c,
        0xe1, 0x3b, 0x72, 0x44, 0x31, 0x3d, 0xba, 0x5d,
        0x0a, 0xd8, 0xaf, 0xf8, 0x9d, 0x09, 0xeb, 0xf6,
        0xd6, 0xcb, 0x72, 0x1d, 0xe9, 0xe5, 0x4a, 0xe0,
        0xa3, 0xf7, 0xb2, 0xfa, 0xe5, 0x3a, 0xee, 0x7b
};

// 256-bit key, empty plaintext, ad and nonce as filled in by TEST_SETUP.
static const unsigned char kat_empty256[TAG_LEN] = {
        0xb6, 0xc8, 0x28, 0x92, 0x52, 0x30, 0xfc, 0xe6,
        0xdb, 0x8d, 0x74, 0xa4, 0x2c, 0x96, 0x01, 0xdc
};

static AES_SIV_CTX *ctx;
static unsigned char key[64];      // key[i] = i; first key_len bytes used
static unsigned char ad[48];       // ad[i] = 0x80 + i
static unsigned char nonce[16];    // nonce[i] = 0xc0 + i
static unsigned char pt[PT_LEN];   // pt[i] = 0x40 + i

TEST_SETUP(aes_siv_evp) {
        size_t i;

        for (i = 0; i < sizeof(key); i++) {
                key[i] = (unsigned char)i;
        }
        for (i = 0; i < sizeof(ad); i++) {
                ad[i] = (unsigned char)(0x80 + i);
        }
        for (i = 0; i < sizeof(nonce); i++) {
                nonce[i] = (unsigned char)(0xc0 + i);
        }
        for (i = 0; i < sizeof(pt); i++) {
                pt[i] = (unsigned char)(0x40 + i);
        }
        ctx = AES_SIV_CTX_new();
        TEST_ASSERT_NOT_NULL(ctx);
}

TEST_TEAR_DOWN(aes_siv_evp) {
        AES_SIV_CTX_free(ctx);
}

// Decrypt that must fail authentication: returns 0, leaves *out_len
// alone and zeroes the output.
static void check_auth_fail(size_t key_len,
                            unsigned char const *n, size_t n_len,
                            unsigned char const *ct,
                            unsigned char const *a, size_t a_len) {
        static const unsigned char zero[PT_LEN];
        unsigned char out[PT_LEN];
        size_t out_len = sizeof(out);

        memset(out, 0xa5, sizeof(out));
        TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, out, &out_len,
                              key, key_len, n, n_len,
                              ct, CT_LEN, a, a_len));
        TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero, out, sizeof(out));
}

// Non-empty plaintext with AD and nonce: round trip, then a single
// flipped byte in the ciphertext, the AD or the nonce must be rejected.
static void check_nonempty(size_t key_len,
                           const unsigned char expected[CT_LEN]) {
        unsigned char out[CT_LEN];
        unsigned char dec[PT_LEN];
        unsigned char bad_ct[CT_LEN];
        unsigned char bad_ad[sizeof(ad)];
        unsigned char bad_nonce[sizeof(nonce)];
        size_t out_len = sizeof(out);

        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Encrypt(ctx, out, &out_len,
                              key, key_len, nonce, sizeof(nonce),
                              pt, sizeof(pt), ad, sizeof(ad)));
        TEST_ASSERT_EQUAL_UINT(CT_LEN, out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, out, CT_LEN);

        out_len = sizeof(dec);
        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Decrypt(ctx, dec, &out_len,
                              key, key_len, nonce, sizeof(nonce),
                              expected, CT_LEN, ad, sizeof(ad)));
        TEST_ASSERT_EQUAL_UINT(PT_LEN, out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(pt, dec, PT_LEN);

        memcpy(bad_ct, expected, CT_LEN);
        bad_ct[TAG_LEN + 7] ^= 0x01;
        check_auth_fail(key_len, nonce, sizeof(nonce), bad_ct,
                        ad, sizeof(ad));

        memcpy(bad_ad, ad, sizeof(ad));
        bad_ad[20] ^= 0x01;
        check_auth_fail(key_len, nonce, sizeof(nonce), expected,
                        bad_ad, sizeof(bad_ad));

        memcpy(bad_nonce, nonce, sizeof(nonce));
        bad_nonce[5] ^= 0x01;
        check_auth_fail(key_len, bad_nonce, sizeof(bad_nonce), expected,
                        ad, sizeof(ad));
}

TEST(aes_siv_evp, NonEmpty256) {
        check_nonempty(32, kat256);
}

TEST(aes_siv_evp, NonEmpty384) {
        static const unsigned char expected[CT_LEN] = {
                0x14, 0xc5, 0x60, 0x48, 0x50, 0xba, 0x1e, 0x4a,
                0x13, 0x5b, 0x85, 0x51, 0x1c, 0x91, 0x90, 0x98,
                0x4d, 0xf0, 0x07, 0x45, 0x72, 0xe9, 0x11, 0x7c,
                0xfd, 0xc5, 0x6f, 0x0b, 0xe6, 0x92, 0x93, 0x5a,
                0x24, 0x02, 0xbd, 0xcd, 0xe8, 0x1c, 0x8d, 0xf1,
                0x07, 0x1b, 0x27, 0xc7, 0x90, 0xed, 0x8e, 0x50,
                0x4e, 0xac, 0xb2, 0xc1, 0x56, 0x27, 0x27, 0x48
        };
        check_nonempty(48, expected);
}

TEST(aes_siv_evp, NonEmpty512) {
        static const unsigned char expected[CT_LEN] = {
                0x45, 0xa6, 0xd5, 0x43, 0xba, 0xb7, 0x60, 0x99,
                0x23, 0xa0, 0x98, 0x02, 0xb2, 0x34, 0x60, 0x48,
                0x37, 0xd9, 0xfd, 0xc7, 0xfe, 0x61, 0x14, 0x40,
                0xe9, 0xa8, 0xb4, 0x73, 0x14, 0x7f, 0x42, 0xec,
                0x9f, 0x53, 0xfc, 0xe0, 0xb2, 0x0a, 0x16, 0x29,
                0xb3, 0x67, 0x03, 0xee, 0xb0, 0x5a, 0x86, 0xe7,
                0xb0, 0xf9, 0xb8, 0x7b, 0x7e, 0x48, 0x1e, 0x84
        };
        check_nonempty(64, expected);
}

// Empty plaintext: the output is the 16-byte S2V tag alone.  Decrypt
// must accept it without touching out (callers pass NULL) and must
// reject it with any single bit flipped.
static void check_empty(size_t key_len,
                        unsigned char const *n, size_t n_len,
                        unsigned char const *a, size_t a_len,
                        const unsigned char expected[TAG_LEN]) {
        unsigned char tag[TAG_LEN];
        size_t out_len = sizeof(tag);
        size_t bit;

        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Encrypt(ctx, tag, &out_len,
                              key, key_len, n, n_len,
                              NULL, 0, a, a_len));
        TEST_ASSERT_EQUAL_UINT(TAG_LEN, out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tag, TAG_LEN);

        out_len = 1234;
        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Decrypt(ctx, NULL, &out_len,
                              key, key_len, n, n_len,
                              expected, TAG_LEN, a, a_len));
        TEST_ASSERT_EQUAL_UINT(0, out_len);

        for (bit = 0; bit < 8 * TAG_LEN; bit++) {
                memcpy(tag, expected, TAG_LEN);
                tag[bit / 8] ^= (unsigned char)(1U << (bit % 8));
                out_len = 1234;
                TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, NULL,
                                      &out_len, key, key_len, n, n_len,
                                      tag, TAG_LEN, a, a_len));
                TEST_ASSERT_EQUAL_UINT(1234, out_len);
        }
}

TEST(aes_siv_evp, EmptyPlaintext256) {
        check_empty(32, nonce, sizeof(nonce), ad, sizeof(ad), kat_empty256);
}

TEST(aes_siv_evp, EmptyPlaintext384) {
        static const unsigned char expected[TAG_LEN] = {
                0xea, 0x2a, 0x49, 0x06, 0x43, 0x80, 0x99, 0xe1,
                0x98, 0xd4, 0xbc, 0x9e, 0xcc, 0x83, 0x4b, 0x8a
        };
        check_empty(48, nonce, sizeof(nonce), ad, sizeof(ad), expected);
}

TEST(aes_siv_evp, EmptyPlaintext512) {
        static const unsigned char expected[TAG_LEN] = {
                0x2e, 0x41, 0xac, 0x72, 0x83, 0xef, 0x68, 0x48,
                0xf7, 0xb3, 0xac, 0x67, 0xbe, 0xc1, 0x67, 0x4f
        };
        check_empty(64, nonce, sizeof(nonce), ad, sizeof(ad), expected);
}

// Empty plaintext and no nonce: S2V over the AD alone.  A NULL nonce
// and a zero-length one are both skipped.
TEST(aes_siv_evp, EmptyPlaintextNoNonce) {
        static const unsigned char expected[TAG_LEN] = {
                0xce, 0x3a, 0xfd, 0xa1, 0x2e, 0x45, 0x59, 0x7f,
                0xf1, 0x7d, 0xa4, 0x81, 0x83, 0x0b, 0x74, 0x79
        };
        check_empty(32, NULL, 0, ad, sizeof(ad), expected);
        check_empty(32, nonce, 0, ad, sizeof(ad), expected);
}

// Empty plaintext and no AD: S2V over the nonce alone.  A NULL AD and a
// zero-length one are both skipped.
TEST(aes_siv_evp, EmptyPlaintextNoAD) {
        static const unsigned char expected[TAG_LEN] = {
                0xb6, 0xbd, 0x49, 0x15, 0xdc, 0x37, 0x0e, 0xeb,
                0xea, 0x25, 0x29, 0xf3, 0x93, 0xdf, 0xb5, 0x7b
        };
        check_empty(32, nonce, sizeof(nonce), NULL, 0, expected);
        check_empty(32, nonce, sizeof(nonce), ad, 0, expected);
}

// RFC 5297 Appendix A.1: deterministic AEAD, AD but no nonce.
TEST(aes_siv_evp, RFC5297_A1) {
        static const unsigned char k[32] = {
                0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
                0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
                0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        };
        static const unsigned char a[24] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
        };
        static const unsigned char p[14] = {
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
        };
        static const unsigned char expected[30] = {
                0x85, 0x63, 0x2d, 0x07, 0xc6, 0xe8, 0xf3, 0x7f,
                0x95, 0x0a, 0xcd, 0x32, 0x0a, 0x2e, 0xcc, 0x93,
                0x40, 0xc0, 0x2b, 0x96, 0x90, 0xc4, 0xdc, 0x04,
                0xda, 0xef, 0x7f, 0x6a, 0xfe, 0x5c
        };
        unsigned char out[sizeof(expected)];
        unsigned char dec[sizeof(p)];
        size_t out_len = sizeof(out);

        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Encrypt(ctx, out, &out_len,
                              k, sizeof(k), NULL, 0,
                              p, sizeof(p), a, sizeof(a)));
        TEST_ASSERT_EQUAL_UINT(sizeof(expected), out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, out, sizeof(expected));

        out_len = sizeof(dec);
        TEST_ASSERT_EQUAL_INT(1, AES_SIV_Decrypt(ctx, dec, &out_len,
                              k, sizeof(k), NULL, 0,
                              expected, sizeof(expected), a, sizeof(a)));
        TEST_ASSERT_EQUAL_UINT(sizeof(p), out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(p, dec, sizeof(p));
}

// Key lengths other than 32, 48 and 64 are rejected on both the empty
// and the non-empty paths, and *out_len is left alone.
TEST(aes_siv_evp, BadKeyLength) {
        static const size_t bad[] = {16, 33};
        unsigned char out[CT_LEN];
        size_t out_len;
        size_t i;

        // The decrypt inputs authenticate under the first 32 key bytes,
        // so only the key-length check can reject them.
        for (i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
                out_len = sizeof(out);
                TEST_ASSERT_EQUAL_INT(0, AES_SIV_Encrypt(ctx, out, &out_len,
                                      key, bad[i], nonce, sizeof(nonce),
                                      pt, sizeof(pt), ad, sizeof(ad)));
                TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);

                out_len = sizeof(out);
                TEST_ASSERT_EQUAL_INT(0, AES_SIV_Encrypt(ctx, out, &out_len,
                                      key, bad[i], nonce, sizeof(nonce),
                                      NULL, 0, ad, sizeof(ad)));
                TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);

                out_len = sizeof(out);
                TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, out, &out_len,
                                      key, bad[i], nonce, sizeof(nonce),
                                      kat256, CT_LEN, ad, sizeof(ad)));
                TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);

                out_len = sizeof(out);
                TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, out, &out_len,
                                      key, bad[i], nonce, sizeof(nonce),
                                      kat_empty256, TAG_LEN, ad,
                                      sizeof(ad)));
                TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);
        }
}

// An output buffer one byte too small is rejected.  The decrypt input
// authenticates, so only the size check can make that call fail.
TEST(aes_siv_evp, ShortOutput) {
        unsigned char out[CT_LEN];
        size_t out_len;

        out_len = CT_LEN - 1;
        TEST_ASSERT_EQUAL_INT(0, AES_SIV_Encrypt(ctx, out, &out_len,
                              key, 32, nonce, sizeof(nonce),
                              pt, sizeof(pt), ad, sizeof(ad)));
        TEST_ASSERT_EQUAL_UINT(CT_LEN - 1, out_len);

        out_len = PT_LEN - 1;
        TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, out, &out_len,
                              key, 32, nonce, sizeof(nonce),
                              kat256, CT_LEN, ad, sizeof(ad)));
        TEST_ASSERT_EQUAL_UINT(PT_LEN - 1, out_len);
}

// Input shorter than the tag cannot be a valid ciphertext.
TEST(aes_siv_evp, ShortCiphertext) {
        unsigned char out[TAG_LEN];
        unsigned char ct[TAG_LEN - 1];
        size_t out_len = sizeof(out);

        memset(ct, 0, sizeof(ct));
        TEST_ASSERT_EQUAL_INT(0, AES_SIV_Decrypt(ctx, out, &out_len,
                              key, 32, nonce, sizeof(nonce),
                              ct, sizeof(ct), ad, sizeof(ad)));
        TEST_ASSERT_EQUAL_UINT(sizeof(out), out_len);
}

TEST_GROUP_RUNNER(aes_siv_evp) {
        RUN_TEST_CASE(aes_siv_evp, NonEmpty256);
        RUN_TEST_CASE(aes_siv_evp, NonEmpty384);
        RUN_TEST_CASE(aes_siv_evp, NonEmpty512);
        RUN_TEST_CASE(aes_siv_evp, EmptyPlaintext256);
        RUN_TEST_CASE(aes_siv_evp, EmptyPlaintext384);
        RUN_TEST_CASE(aes_siv_evp, EmptyPlaintext512);
        RUN_TEST_CASE(aes_siv_evp, EmptyPlaintextNoNonce);
        RUN_TEST_CASE(aes_siv_evp, EmptyPlaintextNoAD);
        RUN_TEST_CASE(aes_siv_evp, RFC5297_A1);
        RUN_TEST_CASE(aes_siv_evp, BadKeyLength);
        RUN_TEST_CASE(aes_siv_evp, ShortOutput);
        RUN_TEST_CASE(aes_siv_evp, ShortCiphertext);
}
