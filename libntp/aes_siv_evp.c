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
	const unsigned char *tag_in;
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
	tag_in = ciphertext;
	ct_in = ciphertext + SIV_TAG_LEN;

	/* Set the expected tag for verification */
	if (EVP_CIPHER_CTX_ctrl(ctx->cipher_ctx, EVP_CTRL_AEAD_SET_TAG,
	                        SIV_TAG_LEN, (void *)tag_in) != 1) {
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
		if (out != NULL && plaintext_len > 0) {
			OPENSSL_cleanse(out, plaintext_len);
		}
		goto cleanup;
	}

	*out_len = plaintext_len;
	ret = 1;

cleanup:
	EVP_CIPHER_free(cipher);
	return ret;
}
