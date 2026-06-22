/*
 * aes_siv_evp.h - AES-SIV wrapper using OpenSSL 3.x EVP
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This provides the same API as libaes_siv but uses OpenSSL 3.x native
 * EVP_CIPHER AES-SIV support instead of the bundled libaes_siv library.
 *
 * API-compatible replacement for libaes_siv/aes_siv.h
 */

#ifndef AES_SIV_EVP_H_
#define AES_SIV_EVP_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Opaque context structure for AES-SIV operations.
 * The context can be reused for multiple encrypt/decrypt operations.
 */
typedef struct AES_SIV_CTX_st AES_SIV_CTX;

/*
 * Allocate and initialize a new AES-SIV context.
 *
 * Returns: pointer to new context on success, NULL on failure
 */
AES_SIV_CTX *AES_SIV_CTX_new(void);

/*
 * Free an AES-SIV context and securely clear any sensitive data.
 *
 * ctx: context to free (may be NULL)
 */
void AES_SIV_CTX_free(AES_SIV_CTX *ctx);

/*
 * Clean up a context for reuse without freeing it.
 * Securely clears any sensitive data from the context.
 *
 * ctx: context to clean up
 */
void AES_SIV_CTX_cleanup(AES_SIV_CTX *ctx);

/*
 * Copy an AES-SIV context.
 *
 * dst: destination context (must be allocated)
 * src: source context
 *
 * Returns: 1 on success, 0 on failure
 */
int AES_SIV_CTX_copy(AES_SIV_CTX *dst, AES_SIV_CTX const *src);

/*
 * One-shot AES-SIV encryption.
 *
 * This function encrypts plaintext using AES-SIV mode (RFC 5297).
 * The output consists of a 16-byte SIV tag followed by the ciphertext.
 *
 * ctx:           AES-SIV context (used as scratch space)
 * out:           output buffer for tag + ciphertext
 * out_len:       [in] size of output buffer, [out] actual output length
 * key:           encryption key (32, 48, or 64 bytes)
 * key_len:       key length in bytes
 * nonce:         optional nonce (treated as additional AD)
 * nonce_len:     nonce length in bytes (may be 0)
 * plaintext:     data to encrypt
 * plaintext_len: plaintext length in bytes
 * ad:            additional authenticated data
 * ad_len:        AD length in bytes
 *
 * Returns: 1 on success, 0 on failure
 *
 * Key length determines the SIV variant:
 *   32 bytes -> AEAD_AES_SIV_CMAC_256 (AES-128)
 *   48 bytes -> AEAD_AES_SIV_CMAC_384 (AES-192)
 *   64 bytes -> AEAD_AES_SIV_CMAC_512 (AES-256)
 */
int AES_SIV_Encrypt(AES_SIV_CTX *ctx, unsigned char *out, size_t *out_len,
                    unsigned char const *key, size_t key_len,
                    unsigned char const *nonce, size_t nonce_len,
                    unsigned char const *plaintext, size_t plaintext_len,
                    unsigned char const *ad, size_t ad_len);

/*
 * One-shot AES-SIV decryption.
 *
 * This function decrypts ciphertext encrypted with AES-SIV mode (RFC 5297).
 * The input consists of a 16-byte SIV tag followed by the ciphertext.
 * Decryption includes authentication; if authentication fails, the output
 * is zeroed and the function returns 0.
 *
 * ctx:            AES-SIV context (used as scratch space)
 * out:            output buffer for plaintext
 * out_len:        [in] size of output buffer, [out] actual output length
 * key:            encryption key (32, 48, or 64 bytes)
 * key_len:        key length in bytes
 * nonce:          optional nonce (treated as additional AD)
 * nonce_len:      nonce length in bytes (may be 0)
 * ciphertext:     tag + ciphertext to decrypt
 * ciphertext_len: total ciphertext length (including 16-byte tag)
 * ad:             additional authenticated data
 * ad_len:         AD length in bytes
 *
 * Returns: 1 on success (authentication passed), 0 on failure
 *
 * Key length determines the SIV variant:
 *   32 bytes -> AEAD_AES_SIV_CMAC_256 (AES-128)
 *   48 bytes -> AEAD_AES_SIV_CMAC_384 (AES-192)
 *   64 bytes -> AEAD_AES_SIV_CMAC_512 (AES-256)
 */
int AES_SIV_Decrypt(AES_SIV_CTX *ctx, unsigned char *out, size_t *out_len,
                    unsigned char const *key, size_t key_len,
                    unsigned char const *nonce, size_t nonce_len,
                    unsigned char const *ciphertext, size_t ciphertext_len,
                    unsigned char const *ad, size_t ad_len);

#ifdef __cplusplus
}
#endif

#endif /* AES_SIV_EVP_H_ */
