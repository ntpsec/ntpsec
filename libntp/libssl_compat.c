/*
 * libssl_compat.c -- OpenSSL v1.1 compatibility functions
 *
 * ---------------------------------------------------------------------
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project
 *
 * Based on an idea by Kurt Roeckx <kurt@roeckx.be>
 *
 * ---------------------------------------------------------------------
 * This is a clean room implementation of shim functions that have
 * counterparts in the OpenSSL v1.1 API but not in earlier versions. So
 * while OpenSSL broke binary compatibility with v1.1, this shim module
 * should provide the necessary source code compatibility with older
 * versions of OpenSSL.
 * ---------------------------------------------------------------------
 */
#include "config.h"
#include "ntp_types.h"

#include <openssl/evp.h>

#include "libssl_compat.h"

/* ----------------------------------------------------------------- */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* ----------------------------------------------------------------- */

/* --------------------------------------------------------------------
 * allocation and deallocation of message digests
 */
EVP_MD_CTX*
sslshim_EVP_MD_CTX_new(void)
{
	return calloc(1, sizeof(EVP_MD_CTX));
}

void
sslshim_EVP_MD_CTX_free(
	EVP_MD_CTX *	pctx
	)
{
	free(pctx);
}

/* ----------------------------------------------------------------- */
#endif
/* ----------------------------------------------------------------- */
