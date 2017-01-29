/*
 * libssl_compat.h -- OpenSSL v1.1 compatibility shims
 *
 * ---------------------------------------------------------------------
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project
 *
 * Based on an idea by Kurt Roeckx <kurt@roeckx.be>
 *
 * ---------------------------------------------------------------------
 * This is a clean room implementation of shim functions that have
 * counterparts in the OpenSSL v1.1 API but not in earlier versions.
 *
 * If the OpenSSL version used for compilation needs the shims (that is,
 * does not provide the new functions) the names of these functions are
 * redirected to our shims.
 * ---------------------------------------------------------------------
 */

#ifndef NTP_LIBSSL_COMPAT_H
#define NTP_LIBSSL_COMPAT_H

#include <openssl/evp.h>

/* ----------------------------------------------------------------- */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* ----------------------------------------------------------------- */

/* shim the new-style API on an old-style OpenSSL */

extern EVP_MD_CTX	*sslshim_EVP_MD_CTX_new(void);
extern void		sslshim_EVP_MD_CTX_free(EVP_MD_CTX *ctx);

#define EVP_MD_CTX_new		sslshim_EVP_MD_CTX_new
#define EVP_MD_CTX_free		sslshim_EVP_MD_CTX_free

/* ----------------------------------------------------------------- */
#endif /* OPENSSL_VERSION_NUMBER < v1.1.0 */
/* ----------------------------------------------------------------- */

#endif /* NTP_LIBSSL_COMPAT_H */
