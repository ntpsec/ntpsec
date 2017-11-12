/*
 *	digest support for NTP
 */
#include "config.h"

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <openssl/evp.h>	/* provides OpenSSL digest API */

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp.h"

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
        size_t i;

	for(i=0; i<n; i++) {
		accum |= a[i] ^ b[i];
	}

	return accum == 0;
}

/*
 * mac_authencrypt - generate message digest
 *
 * Returns length of MAC including key ID and digest.
 */
int
mac_authencrypt(
	int	type,		/* hash algorithm */
	uint8_t	*key,		/* key pointer */
	uint32_t *pkt,		/* packet pointer */
	int	length		/* packet length */
	)
{
	uint8_t	digest[EVP_MAX_MD_SIZE];
	unsigned int	len;
	EVP_MD_CTX *ctx;

	/*
	 * Compute digest of key concatenated with packet. Note: the
	 * key type and digest type have been verified when the key
	 * was created.
	 */
	ssl_init();
	ctx = EVP_MD_CTX_create();
	if (!EVP_DigestInit_ex(ctx, EVP_get_digestbynid(type), NULL)) {
		msyslog(LOG_ERR,
		    "MAC: encrypt: digest init failed");
		return (0);
	}
	EVP_DigestUpdate(ctx, key, cache_secretsize);
	EVP_DigestUpdate(ctx, (uint8_t *)pkt, (unsigned int)length);
	EVP_DigestFinal_ex(ctx, digest, &len);
	EVP_MD_CTX_destroy(ctx);
	memmove((uint8_t *)pkt + length + 4, digest, len);
	return (int)(len + 4);
}


/*
 * mac_authdecrypt - verify message authenticator
 *
 * Returns true if digest valid, false if invalid.
 */
bool
mac_authdecrypt(
	int	type,		/* hash algorithm */
	uint8_t	*key,		/* key pointer */
	uint32_t	*pkt,		/* packet pointer */
	int	length,	 	/* packet length */
	int	size		/* MAC size */
	)
{
	uint8_t	digest[EVP_MAX_MD_SIZE];
	unsigned int	len;
	EVP_MD_CTX *ctx;

	/*
	 * Compute digest of key concatenated with packet. Note: the
	 * key type and digest type have been verified when the key
	 * was created.
	 */
	ssl_init();
	ctx = EVP_MD_CTX_create();
	if (!EVP_DigestInit_ex(ctx, EVP_get_digestbynid(type), NULL)) {
		msyslog(LOG_ERR,
		    "MAC: decrypt: digest init failed");
		return false;
	}
	EVP_DigestUpdate(ctx, key, cache_secretsize);
	EVP_DigestUpdate(ctx, (uint8_t *)pkt, (unsigned int)length);
	EVP_DigestFinal_ex(ctx, digest, &len);
	EVP_MD_CTX_destroy(ctx);
	if ((unsigned int)size != len + 4) {
		msyslog(LOG_ERR,
		    "MAC: decrypt: MAC length error");
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
	uint8_t		digest[20];
	uint32_t	addr_refid;
	EVP_MD_CTX	*ctx;
	unsigned int	len;

	if (IS_IPV4(addr))
		return (NSRCADR(addr));

	ssl_init();

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
