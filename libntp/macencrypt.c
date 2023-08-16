/*
 *	CMAC and digest support for NTP
 */

/*  Notes:
 *
 * This module covers the simple shared-key authentication.
 * This is the working code that has to go fast.
 * The setup code in authreadkeys is not time critical.
 *
 * There are 3 main options: MD5, SHA1, and AES.
 * MD5 and SHA1 are digests.
 *   https://en.wikipedia.org/wiki/Message_digest
 * AES is a MAC.
 *   https://en.wikipedia.org/wiki/Message_authentication_code
 * OpenSSL has different APIs for them.
 *
 * Before OpenSSL 3, we (and many others) used the undocumented
 * CMAC interface via openssl/cmac.h which is now (loudly) deprecated.
 *
 * The per packet operations all have this form:
 *   EVP_MAC_init()
 *   EVP_MAC_update()
 *   EVP_MAC_final()
 *
 * The init step involves setting things up for the desired algorithm
 * and key.  This can be an expensive step.  Some or much of the work
 * can be pushed back to the one-time setup routines at the cost of
 * more memory.  I call that preloading.
 *
 * This code now expects both the cipher and key to be preloaded.
 * Just preloading the cipher will save a lot of memory if you
 * are using a lot of keys.  The edit in this code is simple.
 *
 * Play with attic/cmac-timing for numbers.
 *
 *
 * Modern CPUs come with support to speed up AES operations.
 * On Intel, it's the aes capabilitiy.  You can see them
 * under flags in /proc/cpuinfo
 *
 * Maybe more info in attic/cmac-timing and friends.
 */

#include "config.h"

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <openssl/err.h>
#include <openssl/evp.h>	/* provides OpenSSL digest API */
#include <openssl/md5.h>

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"
#include "ntp.h"

/* Need one per thread. */
extern EVP_MD_CTX *digest_ctx;

#if OPENSSL_VERSION_NUMBER > 0x20000000L
#include <openssl/params.h>
#else
#include <openssl/cmac.h>
extern CMAC_CTX *cmac_ctx;
#endif


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
#if OPENSSL_VERSION_NUMBER > 0x20000000L
        EVP_MAC_CTX *ctx = auth->mac_ctx;

#if OPENSSL_VERSION_NUMBER > 0x30000020L
        if (0 == EVP_MAC_init(ctx, NULL, 0, NULL)) {
#else
// Bug in OpenSSL 3.0.2
// Need to reload key which is slow.  See attic/cmac-timing
        if (0 == EVP_MAC_init(ctx, auth->key, auth->key_size, NULL)) {
#endif
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "encrypt: EVP_MAC_init() failed: %s.", str);
                exit(1);
        }
        if (0 == EVP_MAC_update(ctx, (unsigned char *)pkt, length)) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "encrypt: EVP_MAC_update() failed: %s.", str);
                exit(1);
        }
        if (0 == EVP_MAC_final(ctx, mac, &len, sizeof(mac))) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "encrypt: EVP_MAC_final() failed: %s.", str);
                exit(1);
        }
#else
	CMAC_CTX *ctx = cmac_ctx;
	if (!CMAC_Init(ctx, auth->key, auth->key_size, auth->cipher, NULL)) {
		/* Shouldn't happen.  Does if wrong key_size. */
		msyslog(LOG_ERR,
		    "encrypt: CMAC init failed, %u, %u",
			auth->keyid, auth->key_size);
		return (0);
	}
	CMAC_Update(ctx, (uint8_t *)pkt, (unsigned int)length);
	CMAC_Final(ctx, mac, &len);
#endif
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
#if OPENSSL_VERSION_NUMBER > 0x20000000L
        EVP_MAC_CTX *ctx = auth->mac_ctx;

#if OPENSSL_VERSION_NUMBER > 0x30000020L
        if (0 == EVP_MAC_init(ctx, NULL, 0, NULL)) {
#else
// Bug in OpenSSL 3.0.2
// Need to reload key which is slow.  See attic/cmac-timing
        if (0 == EVP_MAC_init(ctx, auth->key, auth->key_size, NULL)) {
#endif
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "decrypt: EVP_MAC_init() failed: %s.", str);
                return false;
        }
        if (0 == EVP_MAC_update(ctx, (unsigned char *)pkt, length)) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "decrypt: EVP_MAC_update() failed: %s.", str);
                return false;
        }
        if (0 == EVP_MAC_final(ctx, mac, &len, sizeof(mac))) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                msyslog(LOG_ERR, "decrypt: EVP_MAC_final() failed: %s.", str);
                return false;
        }
#else
	CMAC_CTX *ctx = cmac_ctx;
	if (!CMAC_Init(ctx, auth->key, auth->key_size, auth->cipher, NULL)) {
		/* Shouldn't happen.  Does if wrong key_size. */
		msyslog(LOG_ERR,
		    "decrypt: CMAC init failed, %u, %u",
			auth->keyid, auth->key_size);
		return false;
	}
	CMAC_Update(ctx, (uint8_t *)pkt, (unsigned int)length);
	CMAC_Final(ctx, mac, &len);
#endif
	if (MAX_BARE_MAC_LENGTH < len)
		len = MAX_BARE_MAC_LENGTH;

	if ((unsigned int)size != len + 4) {
		/* Beware of DoS attack.
		 * This indicates either the sender is broken
		 * or some admin fatfingered things.
		 * Similar code at digest_decrypt.
		 */
		if (0) msyslog(LOG_ERR,
		    "MAC: decrypt: MAC length error");
		return false;
	}
	return !CRYPTO_memcmp(mac, (char *)pkt + length + 4, len);
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
	return !CRYPTO_memcmp(digest, (char *)pkt + length + 4, len);
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

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	EVP_MD *md5;
	ctx = EVP_MD_CTX_new();
	/* See section FIPS Provider:
	 * https://www.openssl.org/docs/man3.0/man7/crypto.html
	 * for property query strings
	 */
	md5 = EVP_MD_fetch(NULL, "MD5", "fips=no");
	if(!EVP_DigestInit_ex(ctx, md5, NULL)) {
		msyslog(LOG_ERR, "MAC: MD5 init failed");
		exit(1);
	}
	EVP_MD_free(md5);
#else
	ctx = EVP_MD_CTX_create();
#ifdef EVP_MD_CTX_FLAG_NON_FIPS_ALLOW
	/* MD5 is not used as a crypto hash here. */
	EVP_MD_CTX_set_flags(ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
#endif
	if (!EVP_DigestInit_ex(ctx, EVP_md5(), NULL)) {
		msyslog(LOG_ERR, "MAC: MD5 init failed");
		exit(1);
	}
#endif
	EVP_DigestUpdate(ctx, (uint8_t *)PSOCK_ADDR6(addr),
	    sizeof(struct in6_addr));
	EVP_DigestFinal_ex(ctx, digest, &len);
	EVP_MD_CTX_destroy(ctx);
	memcpy(&addr_refid, digest, sizeof(addr_refid));
	return (addr_refid);
}
