/*
 * hack-ancient-openssl.h - hack to run on ancient versions of OpenSSL
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This is needed only on ancient systems that
 * are using OpenSSL older than OpenSSL 1.1.0.
 *
 * From the man page:
 *  The EVP_MD_CTX_create() and EVP_MD_CTX_destroy() functions were renamed
 *  to EVP_MD_CTX_new() and EVP_MD_CTX_free() in OpenSSL 1.1.0,
 *  respectively.
 *
 * See:
 *   https://lists.ntpsec.org/pipermail/devel/2024-December/010502.html
 *   https://lists.ntpsec.org/pipermail/devel/2024-June/010451.html
 */

#ifndef GUARD_HACK_ANCIENT_OPENSSL_H
#define GUARD_HACK_ANCIENT_OPENSSL_H

#include "config.h"

#ifndef HAVE_EVP_MD_CTX_NEW

#include <openssl/evp.h>

static inline EVP_MD_CTX *
EVP_MD_CTX_new(void) {
	(void) EVP_MD_CTX_new;
	return EVP_MD_CTX_create();
}

// Note the difference in the return types.
// The return value for EVP_MD_CTX_reset isn't documented in the manpage,
// but since other int-returning functions return 0 for failure and 1 for
// success, we assume the same here.
static inline int
EVP_MD_CTX_reset(EVP_MD_CTX *ctx) {
	(void) EVP_MD_CTX_reset;
	EVP_MD_CTX_init(ctx);
	return 1;
}

static inline void
EVP_MD_CTX_free(EVP_MD_CTX *ctx) {
	(void) EVP_MD_CTX_free;
	EVP_MD_CTX_destroy(ctx);
}

#endif /* !HAVE_EVP_MD_CTX_NEW */

#endif /* GUARD_HACK_ANCIENT_OPENSSL_H */
