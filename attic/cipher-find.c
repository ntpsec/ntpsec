/*
 * digest.c - Hack to test various digest types.
 *
 * Fedora mentions blake2b and friends.  They are in the man page
 * and header files, but not available via OBJ_sn2n so I assume they
 * are not interesting and dropped them to reduce clutter.
 *
 * If the type column is blank, the OpenSSL package doesn't know
 * about that digest, maybe because it isn't spelled correctly.
 *
 * If the type column is non-blank but the length column is empty,
 * the library recognizes the type but doesn't support it.
 *
 * If the length column is filled in, that's the length of the digest.
 *
 */

#include <stdio.h>
#include <string.h>

#include <openssl/objects.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define UNUSED_ARG(arg)         ((void)(arg))

const char* ciphers[] = {
    "AES-128", "AES-192", "AES-256",
    "DES", "DES3",
    "ARIA-128", "CAMELLIA-128",
    NULL };

unsigned char packet[100];
int
main (
	int argc,
	char *argv[]
	)
{
    UNUSED_ARG(argc);
    UNUSED_ARG(argv);

#if OPENSSL_VERSION_NUMBER > 0x20000000L
    EVP_MAC *mac;
    EVP_MAC_CTX *ctx;

    unsigned int versionNumber = OPENSSL_VERSION_NUMBER;
    const char *versionText = OPENSSL_VERSION_TEXT;
    printf("OpenSSL Version is %x, %s\n", versionNumber, versionText);

#if OPENSSL_VERSION_NUMBER > 0x20000000L
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS|OPENSSL_INIT_LOAD_CRYPTO_STRINGS|OPENSSL_INIT_ADD_ALL_CIPHERS|OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);
#else
    OpenSSL_add_all_ciphers();
#endif

    mac = EVP_MAC_fetch(NULL, "cmac", NULL);
    if (NULL == mac) {
      printf("Barf, no mac\n");
      exit(1);
    }
    ctx = EVP_MAC_CTX_new(mac);
    if (NULL == ctx) {
      printf("Barf, no ctx\n");
      exit(2);
    }


    printf("            name keylen maclen\n");
    for (int i = 0; NULL != ciphers[i]; i++) {
	unsigned char key[256];
	char name[100];
	EVP_CIPHER *cipher;
        OSSL_PARAM params[2];
	unsigned int keylen;
	unsigned int maclen = 0;
        memset(key, 0, sizeof(key));
	/* Grump: strlcpy/strlcat don't exist on some systems. */
	strncpy(name, ciphers[i], sizeof(name)-5);
	strcat(name, "-CBC");
	name[sizeof(name)-1] = 0;
        cipher = EVP_CIPHER_fetch(NULL, name, NULL);
	if (NULL == cipher) {
	    printf("%16s (no cipher)\n", name);
	    continue;
	}
	keylen = EVP_CIPHER_get_key_length(cipher);
        EVP_CIPHER_free(cipher);

        params[0] = OSSL_PARAM_construct_utf8_string("cipher", name, 0);
        params[1] = OSSL_PARAM_construct_end();
        if (0 == EVP_MAC_CTX_set_params(ctx, params)) {
            printf("%16s (params didn't work)\n", name);
            continue;
        }

	/* need to kick it */
        if (0 == EVP_MAC_init(ctx, key, keylen-3+i, NULL)) {
            unsigned long err = ERR_get_error();
            char * str = ERR_error_string(err, NULL);
            printf("EVP_MAC_init() failed: %s.\n", str);
            continue;
        }


        maclen = EVP_MAC_CTX_get_mac_size(ctx);
	printf("%16s %6u %6u\n", name, keylen, maclen);
   }
#endif /* OPENSSL_VERSION_NUMBER */

    return 0;
}
