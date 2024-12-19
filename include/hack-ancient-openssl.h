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

#ifndef HAVE_EVP_MD_CTX_NEW
  #define EVP_MD_CTX_create()     EVP_MD_CTX_new()
  #define EVP_MD_CTX_init(ctx)    EVP_MD_CTX_reset((ctx)) 
  #define EVP_MD_CTX_destroy(ctx) EVP_MD_CTX_free((ctx))
#endif

#endif /* GUARD_HACK_ANCIENT_OPENSSL_H */
