/*
 * aes_siv_evp.c - AES-SIV wrapper using OpenSSL 3.x EVP
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This provides the same API as libaes_siv but uses OpenSSL 3.x native
 * EVP_CIPHER AES-SIV support instead of the bundled libaes_siv library.
 *
 * OpenSSL 3.x provides AES-SIV via EVP_CIPHER with names:
 *   "AES-128-SIV" for 256-bit key (32 bytes)
 *   "AES-192-SIV" for 384-bit key (48 bytes)
 *   "AES-256-SIV" for 512-bit key (64 bytes)
 *
 * Note: The key length for SIV is double the AES variant because SIV
 * uses two keys internally (one for S2V/CMAC, one for CTR encryption).
 */

#include "config.h"
#include "aes_siv_evp.h"

#include <string.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

/* SIV tag length is always 16 bytes */
#define SIV_TAG_LEN 16

/*
 * Context structure for AES-SIV operations.
 * We cache the EVP_CIPHER_CTX for potential reuse, though the one-shot
 * functions reset it on each call anyway.
 */
struct AES_SIV_CTX_st {
	EVP_CIPHER_CTX *cipher_ctx;
};

AES_SIV_CTX *AES_SIV_CTX_new(void) {
	AES_SIV_CTX *ctx;

	ctx = OPENSSL_zalloc(sizeof(struct AES_SIV_CTX_st));
	if (ctx == NULL) {
		return NULL;
	}

	ctx->cipher_ctx = EVP_CIPHER_CTX_new();
	if (ctx->cipher_ctx == NULL) {
		OPENSSL_free(ctx);
		return NULL;
	}

	return ctx;
}

void AES_SIV_CTX_cleanup(AES_SIV_CTX *ctx) {
	if (ctx != NULL && ctx->cipher_ctx != NULL) {
		EVP_CIPHER_CTX_reset(ctx->cipher_ctx);
	}
}

void AES_SIV_CTX_free(AES_SIV_CTX *ctx) {
	if (ctx != NULL) {
		EVP_CIPHER_CTX_free(ctx->cipher_ctx);
		OPENSSL_free(ctx);
	}
}

int AES_SIV_CTX_copy(AES_SIV_CTX *dst, AES_SIV_CTX const *src) {
	if (dst == NULL || src == NULL) {
		return 0;
	}
	return EVP_CIPHER_CTX_copy(dst->cipher_ctx, src->cipher_ctx);
}

/*
 * Get the cipher name for a given key length.
 * Returns NULL for invalid key lengths.
 */
static const char *get_cipher_name(size_t key_len) {
	switch (key_len) {
	case 32:
		return "AES-128-SIV";
	case 48:
		return "AES-192-SIV";
	case 64:
		return "AES-256-SIV";
	default:
		return NULL;
	}
}

/*
 * OpenSSL before 3.5.0 cannot produce an AES-SIV tag for an empty
 * plaintext: its SIV provider returns early on zero-length input, so
 * the final step fails.  NTS authenticates packets with an empty
 * plaintext, so for that case we compute the tag ourselves with the
 * RFC 5297 S2V construction over AES-CMAC.  The ciphertext is empty,
 * so the tag is the whole output.
 *
 * Remove this once the distributions we support ship OpenSSL >= 3.5.0
 * (Debian oldstable and Ubuntu 24.04 LTS were still on 3.0.x in 2026).
 */

/* RFC 5297 doubling in GF(2^128) */
static void siv_dbl(unsigned char block[SIV_TAG_LEN]) {
	unsigned char carry = block[0] >> 7;
	int i;

	for (i = 0; i < SIV_TAG_LEN - 1; i++) {
		block[i] = (unsigned char)((block[i] << 1) | (block[i + 1] >> 7));
	}
	block[SIV_TAG_LEN - 1] = (unsigned char)((block[SIV_TAG_LEN - 1] << 1) ^
	                                         (carry * 0x87));
}

static int siv_cmac(EVP_MAC_CTX *mctx, const unsigned char *key, size_t key_len,
                    const OSSL_PARAM *params,
                    const unsigned char *in, size_t in_len,
                    unsigned char out[SIV_TAG_LEN]) {
	size_t out_len;

	return EVP_MAC_init(mctx, key, key_len, params) == 1 &&
	       EVP_MAC_update(mctx, in, in_len) == 1 &&
	       EVP_MAC_final(mctx, out, &out_len, SIV_TAG_LEN) == 1;
}

/*
 * S2V(K1, ad, nonce, "") with K1 the first half of the SIV key.
 * Components are included under the same rules AES_SIV_Encrypt()
 * uses when it feeds them to OpenSSL.
 */
static int siv_empty_tag(unsigned char tag[SIV_TAG_LEN],
                         unsigned char const *key, size_t key_len,
                         unsigned char const *nonce, size_t nonce_len,
                         unsigned char const *ad, size_t ad_len) {
	static const unsigned char zero[SIV_TAG_LEN];
	static char cbc128[] = "AES-128-CBC";
	static char cbc192[] = "AES-192-CBC";
	static char cbc256[] = "AES-256-CBC";
	char *cbc_name;
	EVP_MAC *mac;
	EVP_MAC_CTX *mctx = NULL;
	OSSL_PARAM params[2];
	unsigned char d[SIV_TAG_LEN], t[SIV_TAG_LEN];
	size_t k1_len = key_len / 2;
	int ret = 0;
	int i;

	switch (key_len) {
	case 32:
		cbc_name = cbc128;
		break;
	case 48:
		cbc_name = cbc192;
		break;
	case 64:
		cbc_name = cbc256;
		break;
	default:
		return 0;
	}

	mac = EVP_MAC_fetch(NULL, "CMAC", NULL);
	if (mac == NULL) {
		return 0;
	}
	mctx = EVP_MAC_CTX_new(mac);
	if (mctx == NULL) {
		goto cleanup;
	}
	params[0] = OSSL_PARAM_construct_utf8_string(OSSL_MAC_PARAM_CIPHER,
	                                             cbc_name, 0);
	params[1] = OSSL_PARAM_construct_end();

	/* D = CMAC(K1, <zero>) */
	if (!siv_cmac(mctx, key, k1_len, params, zero, sizeof(zero), d)) {
		goto cleanup;
	}

	/* D = dbl(D) xor CMAC(K1, S_i) for each associated-data component */
	if (ad != NULL && ad_len > 0) {
		if (!siv_cmac(mctx, key, k1_len, params, ad, ad_len, t)) {
			goto cleanup;
		}
		siv_dbl(d);
		for (i = 0; i < SIV_TAG_LEN; i++) {
			d[i] ^= t[i];
		}
	}
	if (nonce != NULL && nonce_len > 0) {
		if (!siv_cmac(mctx, key, k1_len, params, nonce, nonce_len, t)) {
			goto cleanup;
		}
		siv_dbl(d);
		for (i = 0; i < SIV_TAG_LEN; i++) {
			d[i] ^= t[i];
		}
	}

	/* Empty final component: T = dbl(D) xor pad(""), tag = CMAC(K1, T) */
	siv_dbl(d);
	d[0] ^= 0x80;
	ret = siv_cmac(mctx, key, k1_len, params, d, SIV_TAG_LEN, tag);

cleanup:
	EVP_MAC_CTX_free(mctx);
	EVP_MAC_free(mac);
	return ret;
}

int AES_SIV_Encrypt(AES_SIV_CTX *ctx, unsigned char *out, size_t *out_len,
                    unsigned char const *key, size_t key_len,
                    unsigned char const *nonce, size_t nonce_len,
                    unsigned char const *plaintext, size_t plaintext_len,
                    unsigned char const *ad, size_t ad_len) {
	EVP_CIPHER *cipher = NULL;
	const char *cipher_name;
	int len;
	int ret = 0;
	size_t required_len;
	unsigned char *tag_out;
	unsigned char *ct_out;

	if (ctx == NULL || ctx->cipher_ctx == NULL) {
		return 0;
	}

	/* Output must have room for tag + ciphertext */
	required_len = plaintext_len + SIV_TAG_LEN;
	if (*out_len < required_len) {
		return 0;
	}

	if (plaintext_len == 0) {
		if (!siv_empty_tag(out, key, key_len, nonce, nonce_len, ad, ad_len)) {
			OPENSSL_cleanse(out, SIV_TAG_LEN);
			return 0;
		}
		*out_len = SIV_TAG_LEN;
		return 1;
	}

	cipher_name = get_cipher_name(key_len);
	if (cipher_name == NULL) {
		return 0;
	}

	/* Fetch the cipher */
	cipher = EVP_CIPHER_fetch(NULL, cipher_name, NULL);
	if (cipher == NULL) {
		return 0;
	}

	/* Reset context for new operation */
	EVP_CIPHER_CTX_reset(ctx->cipher_ctx);

	/* Initialize encryption */
	if (EVP_EncryptInit_ex2(ctx->cipher_ctx, cipher, key, NULL, NULL) != 1) {
		goto cleanup;
	}

	/*
	 * In OpenSSL's AES-SIV implementation:
	 * - Pass AD components via EVP_EncryptUpdate with NULL output
	 * - The original libaes_siv treats nonce as an additional AD component
	 *   (called after the main AD in the S2V computation)
	 * - OpenSSL processes AD components in the order they are provided
	 *
	 * From RFC 5297: S2V takes a sequence of strings (AD1, AD2, ..., ADn, plaintext)
	 * The original code does: S2V(AD, nonce, plaintext)
	 */

	/* Add main associated data */
	if (ad != NULL && ad_len > 0) {
		if (EVP_EncryptUpdate(ctx->cipher_ctx, NULL, &len, ad, (int)ad_len) != 1) {
			goto cleanup;
		}
	}

	/* Add nonce as additional associated data */
	if (nonce != NULL && nonce_len > 0) {
		if (EVP_EncryptUpdate(ctx->cipher_ctx, NULL, &len, nonce, (int)nonce_len) != 1) {
			goto cleanup;
		}
	}

	/*
	 * SIV output format: tag (16 bytes) || ciphertext
	 * We need to write ciphertext after the tag position
	 */
	tag_out = out;
	ct_out = out + SIV_TAG_LEN;

	/* Encrypt the plaintext */
	if (EVP_EncryptUpdate(ctx->cipher_ctx, ct_out, &len, plaintext, (int)plaintext_len) != 1) {
		goto cleanup;
	}

	/* Finalize - for SIV this doesn't produce additional output */
	if (EVP_EncryptFinal_ex(ctx->cipher_ctx, ct_out + len, &len) != 1) {
		goto cleanup;
	}

	/* Get the SIV tag */
	if (EVP_CIPHER_CTX_ctrl(ctx->cipher_ctx, EVP_CTRL_AEAD_GET_TAG, SIV_TAG_LEN, tag_out) != 1) {
		goto cleanup;
	}

	*out_len = required_len;
	ret = 1;

cleanup:
	EVP_CIPHER_free(cipher);
	if (ret != 1) {
		/* Clear output on failure */
		OPENSSL_cleanse(out, required_len);
	}
	return ret;
}

int AES_SIV_Decrypt(AES_SIV_CTX *ctx, unsigned char *out, size_t *out_len,
                    unsigned char const *key, size_t key_len,
                    unsigned char const *nonce, size_t nonce_len,
                    unsigned char const *ciphertext, size_t ciphertext_len,
                    unsigned char const *ad, size_t ad_len) {
	EVP_CIPHER *cipher = NULL;
	const char *cipher_name;
	int len;
	int ret = 0;
	size_t plaintext_len;
	unsigned char tag_in[SIV_TAG_LEN];
	const unsigned char *ct_in;

	if (ctx == NULL || ctx->cipher_ctx == NULL) {
		return 0;
	}

	/* Ciphertext must be at least the tag length */
	if (ciphertext_len < SIV_TAG_LEN) {
		return 0;
	}

	plaintext_len = ciphertext_len - SIV_TAG_LEN;

	/* Output must have room for plaintext */
	if (*out_len < plaintext_len) {
		return 0;
	}

	if (plaintext_len == 0) {
		unsigned char expected[SIV_TAG_LEN];

		if (!siv_empty_tag(expected, key, key_len, nonce, nonce_len, ad, ad_len) ||
		    CRYPTO_memcmp(expected, ciphertext, SIV_TAG_LEN) != 0) {
			return 0;
		}
		*out_len = 0;
		return 1;
	}

	cipher_name = get_cipher_name(key_len);
	if (cipher_name == NULL) {
		return 0;
	}

	/* Fetch the cipher */
	cipher = EVP_CIPHER_fetch(NULL, cipher_name, NULL);
	if (cipher == NULL) {
		return 0;
	}

	/* Reset context for new operation */
	EVP_CIPHER_CTX_reset(ctx->cipher_ctx);

	/* Initialize decryption */
	if (EVP_DecryptInit_ex2(ctx->cipher_ctx, cipher, key, NULL, NULL) != 1) {
		goto cleanup;
	}

	/*
	 * SIV input format: tag (16 bytes) || ciphertext
	 */
	/* Copy the tag: EVP_CIPHER_CTX_ctrl() takes a non-const pointer. */
	memcpy(tag_in, ciphertext, SIV_TAG_LEN);
	ct_in = ciphertext + SIV_TAG_LEN;

	/* Set the expected tag for verification */
	if (EVP_CIPHER_CTX_ctrl(ctx->cipher_ctx, EVP_CTRL_AEAD_SET_TAG,
	                        SIV_TAG_LEN, tag_in) != 1) {
		goto cleanup;
	}

	/* Add main associated data */
	if (ad != NULL && ad_len > 0) {
		if (EVP_DecryptUpdate(ctx->cipher_ctx, NULL, &len, ad, (int)ad_len) != 1) {
			goto cleanup;
		}
	}

	/* Add nonce as additional associated data */
	if (nonce != NULL && nonce_len > 0) {
		if (EVP_DecryptUpdate(ctx->cipher_ctx, NULL, &len, nonce, (int)nonce_len) != 1) {
			goto cleanup;
		}
	}

	/* Decrypt the ciphertext */
	if (EVP_DecryptUpdate(ctx->cipher_ctx, out, &len, ct_in, (int)plaintext_len) != 1) {
		goto cleanup;
	}

	/* Finalize and verify authentication tag */
	if (EVP_DecryptFinal_ex(ctx->cipher_ctx, out + len, &len) != 1) {
		/* Authentication failed - clear output */
		OPENSSL_cleanse(out, plaintext_len);
		goto cleanup;
	}

	*out_len = plaintext_len;
	ret = 1;

cleanup:
	EVP_CIPHER_free(cipher);
	return ret;
}
