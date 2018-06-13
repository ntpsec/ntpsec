/*
 *	CMAC and digest support for NTP
 */
#include "config.h"

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <openssl/evp.h>	/* provides OpenSSL digest API */
#include <openssl/md5.h>

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"
#include "ntp.h"

#ifndef EVP_MD_CTX_reset
/* Slightly older version of OpenSSL */
/* Similar hack in ssl_init.c and attic/digest-timing.c */
#define EVP_MD_CTX_reset(ctx) EVP_MD_CTX_init(ctx)
#endif

/* Need one per thread. */
extern EVP_MD_CTX *digest_ctx;
extern CMAC_CTX *cmac_ctx;

/* ctmemeq - test two blocks memory for equality without leaking
 * timing information.
 *
 * Return value: true if the two blocks of memory are equal, false
 * otherwise.
 *
 * TODO: find out if this is useful elsewhere and if so move
 * it to a more appropriate place and give it a prototype in a
 * header file.
 */
static bool ctmemeq(const void *s1, const void *s2, size_t n) {
	const uint8_t *a = s1;
	const uint8_t *b = s2;
	uint8_t accum = 0;

	for(size_t i = 0; i < n; i++) {
		accum |= a[i] ^ b[i];
	}

	return accum == 0;
}

/*
 * cmac_encrypt - generate CMAC authenticator
 *
 * Returns length of MAC including key ID and digest.
 */
int
cmac_encrypt(
	auth_info* auth,
	uint32_t *pkt,		/* packet pointer */
	int	length		/* packet length */
	)
{
	uint8_t	mac[CMAC_MAX_MAC_LENGTH];
	size_t	len;
	CMAC_CTX *ctx = cmac_ctx;

	CMAC_resume(ctx);
	if (!CMAC_Init(ctx, auth->key, auth->key_size, auth->cipher, NULL)) {
		/* Shouldn't happen.  Does if wrong key_size. */
		msyslog(LOG_ERR,
		    "CMAC: encrypt: CMAC init failed, %u, %u",
			auth->keyid, auth->key_size);
		return (0);
	}
	CMAC_Update(ctx, (uint8_t *)pkt, (unsigned int)length);
	CMAC_Final(ctx, mac, &len);
	if (MAX_BARE_MAC_LENGTH < len)
		len = MAX_BARE_MAC_LENGTH;
	memmove((uint8_t *)pkt + length + 4, mac, len);
	return (int)(len + 4);
}


/*
 * cmac_decrypt - verify CMAC authenticator
 *
 * Returns true if valid, false if invalid.
 */
bool
cmac_decrypt(
	auth_info*	auth,
	uint32_t	*pkt,	/* packet pointer */
	int	length,	 	/* packet length */
	int	size		/* MAC size */
	)
{
	uint8_t	mac[CMAC_MAX_MAC_LENGTH];
	size_t	len;
	CMAC_CTX *ctx = cmac_ctx;

	CMAC_resume(ctx);
	if (!CMAC_Init(ctx, auth->key, auth->key_size, auth->cipher, NULL)) {
		/* Shouldn't happen.  Does if wrong key_size. */
		msyslog(LOG_ERR,
		    "CMAC: decrypt: CMAC init failed, %u, %u",
			auth->keyid, auth->key_size);
		return false;
	}
	CMAC_Update(ctx, (uint8_t *)pkt, (unsigned int)length);
	CMAC_Final(ctx, mac, &len);
	if (MAX_BARE_MAC_LENGTH < len)
		len = MAX_BARE_MAC_LENGTH;
	if ((unsigned int)size != len + 4) {
		/* Beware of DoS attack.
		 * This indicates either the sender is broken
		 * or some admin fatfingered things.
		 * Similar code at digest_decrypt.
		 */
		if (0) msyslog(LOG_ERR,
		    "CMAC: decrypt: MAC length error");
		return false;
	}
	return ctmemeq(mac, (char *)pkt + length + 4, len);
}

/*
 * digest_encrypt - generate message digest
 *
 * Returns length of MAC including key ID and digest.
 */
int
digest_encrypt(
	auth_info* auth,
	uint32_t *pkt,		/* packet pointer */
	int	length		/* packet length */
	)
{
	uint8_t	digest[EVP_MAX_MD_SIZE];
	unsigned int	len;
	EVP_MD_CTX *ctx = digest_ctx;

	/*
	 * Compute digest of key concatenated with packet. Note: the
	 * key type and digest type have been verified when the key
	 * was created.
	 */
	EVP_MD_CTX_reset(ctx);
	if (!EVP_DigestInit_ex(ctx, auth->digest, NULL)) {
		msyslog(LOG_ERR,
		    "MAC: encrypt: digest init failed");
		return (0);
	}
	EVP_DigestUpdate(ctx, auth->key, auth->key_size);
	EVP_DigestUpdate(ctx, (uint8_t *)pkt, (unsigned int)length);
	EVP_DigestFinal_ex(ctx, digest, &len);
	if (MAX_BARE_MAC_LENGTH < len)
		len = MAX_BARE_MAC_LENGTH;
	memmove((uint8_t *)pkt + length + 4, digest, len);
	return (int)(len + 4);
}


/*
 * digest_decrypt - verify message authenticator
 *
 * Returns true if digest valid, false if invalid.
 */
bool
digest_decrypt(
	auth_info*	auth,
	uint32_t	*pkt,	/* packet pointer */
	int	length,	 	/* packet length */
	int	size		/* MAC size */
	)
{
	uint8_t	digest[EVP_MAX_MD_SIZE];
	unsigned int	len;
	EVP_MD_CTX *ctx = digest_ctx;

	/*
	 * Compute digest of key concatenated with packet. Note: the
	 * key type and digest type have been verified when the key
	 * was created.
	 */
	EVP_MD_CTX_reset(ctx);
	if (!EVP_DigestInit_ex(ctx, auth->digest, NULL)) {
		msyslog(LOG_ERR,
		    "MAC: decrypt: digest init failed");
		return false;
	}
	EVP_DigestUpdate(ctx, auth->key, auth->key_size);
	EVP_DigestUpdate(ctx, (uint8_t *)pkt, (unsigned int)length);
	EVP_DigestFinal_ex(ctx, digest, &len);
	if (MAX_BARE_MAC_LENGTH < len)
		len = MAX_BARE_MAC_LENGTH;
	if ((unsigned int)size != len + 4) {
		/* Beware of DoS attack.
		 * This indicates either the sender is broken
		 * or some admin fatfingered things.
		 * Similar code at cmac_decrypt.
		 */
		if (0) msyslog(LOG_ERR,
		    "ERR: decrypt: digest length error");
		return false;
	}
	return ctmemeq(digest, (char *)pkt + length + 4, len);
}

/*
 * Calculate the reference id from the address. If it is an IPv4
 * address, use it as is. If it is an IPv6 address, do a md5 on
 * it and use the top 4 bytes. (Note: NTP Classic claimed it used
 * the *bottom* 4 bytes, but that didn't match the code or the test.)
 * The result is in network byte order.
 */
uint32_t
addr2refid(sockaddr_u *addr)
{
	uint8_t		digest[MD5_DIGEST_LENGTH];
	uint32_t	addr_refid;
	EVP_MD_CTX	*ctx;
	unsigned int	len;

	if (IS_IPV4(addr))
		return (NSRCADR(addr));

	ctx = EVP_MD_CTX_create();
#ifdef EVP_MD_CTX_FLAG_NON_FIPS_ALLOW
	/* MD5 is not used as a crypto hash here. */
	EVP_MD_CTX_set_flags(ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
#endif
	if (!EVP_DigestInit_ex(ctx, EVP_md5(), NULL)) {
		msyslog(LOG_ERR, "MAC: MD5 init failed");
		exit(1);
	}

	EVP_DigestUpdate(ctx, (uint8_t *)PSOCK_ADDR6(addr),
	    sizeof(struct in6_addr));
	EVP_DigestFinal_ex(ctx, digest, &len);
	EVP_MD_CTX_destroy(ctx);
	memcpy(&addr_refid, digest, sizeof(addr_refid));
	return (addr_refid);
}
