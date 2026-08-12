/*
 * Copyright the NTPsec project contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/* Hack to time various implementations of HMAC.
 *
 * Build with: cc hmac-timing.c -o hmac-timing -lcrypto
 *
 * This is just the HMAC timing.
 * It doesn't include the copy or compare or finding the right key.
 *
 * Beware of overflows in the timing computations.
 *
 * Disable AES-NI (Intel hardware: NI == New Instruction) with:
 *    OPENSSL_ia32cap="~0x200000200000000"
 * Check /proc/cpuinfo flags for "aes" to see if you have it.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Silence warnings from CMAC routines in OpenSSL 3.0.0 */
#define OPENSSL_SUPPRESS_DEPRECATED 1

#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/objects.h>
#if OPENSSL_VERSION_NUMBER > 0x20000000L && !defined(LIBRESSL_VERSION_NUMBER)
#include <openssl/params.h> 
#endif

#define UNUSED_ARG(arg)         ((void)(arg))

#if OPENSSL_VERSION_NUMBER > 0x20000000L && !defined(LIBRESSL_VERSION_NUMBER)

int SAMPLESIZE = 1000000;

#define PACKET_LENGTH 48
#define MAX_PACKET_LENGTH 4096
#define MAX_KEY_LENGTH 64

EVP_MAC_CTX *evp;

unsigned char answer[EVP_MAX_MD_SIZE];

static void ssl_init(void)
{
  EVP_MAC *mac;
  mac = EVP_MAC_fetch(NULL, "hmac", NULL);
  if (NULL == mac) {
    printf("## Oops, EVP_MAC_fetch() failed.\n");
    exit(1);
  }
  evp = EVP_MAC_CTX_new(mac);
  if (NULL == evp) {
    printf("## Oops, EVP_MAC_CTX_new() failed.\n");
    exit(1);
  }
  EVP_MAC_free(mac);
}


static const EVP_MD *CheckDigest(const char *name) {
  const EVP_MD *digest;
  digest = EVP_get_digestbyname(name);
  if (NULL == digest) {
    /* no error available */
    unsigned long err = ERR_get_error();
    char *str = ERR_error_string(err, NULL);
    printf("## Oops: EVP_get_digestbyname() failed: %s\n    %s\n", name, str);
    return NULL;
  }
  return digest;
}

static void PrintHex(const unsigned char* bytes, int length) {
  printf("  ");
  for (int i=0; i<length; i++) {
    if (16 == i) {
      printf("...");  break;
    }
    printf("%02x", bytes[i]);
  }
}



static size_t One_HMAC(
  const EVP_MD *digest,     /* digest  */
  uint8_t *key,             /* key pointer */
  int     keylength,        /* key length */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
  unsigned int len = EVP_MAX_MD_SIZE;

  if (0 == HMAC(digest,
      key, keylength, pkt, pktlength, answer, &len)) {
      unsigned long err = ERR_get_error();
      char * str = ERR_error_string(err, NULL);
      printf("## Oops, HMAC() failed: %s.\n", str);
      return 0;
    }
    return len;
}


static void Do_HMAC(
  const char *name,       /* name of digest */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	const EVP_MD *digest = CheckDigest(name);
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;
	int samplesize = SAMPLESIZE;

	if (NULL == digest) {
		return;
	}
	if (pktlength > 1000) samplesize /= 10;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
		digestlength = One_HMAC(digest, key, keylength, pkt, pktlength);
		if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %2lu %6.0f %7.3f",
	       name, keylength, pktlength, digestlength, fast/samplesize,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}


static size_t One_EVP_MAC(
  EVP_MAC_CTX *ctx,         /* context  */
  char *name,               /* name of cipher */
  uint8_t *key,             /* key pointer */
  int     keylength,        /* key length */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	OSSL_PARAM params[2];
	size_t len = EVP_MAX_MD_SIZE;

	params[0] =
          OSSL_PARAM_construct_utf8_string("digest", name, 0);
	params[1] = OSSL_PARAM_construct_end();
	if (0 == EVP_MAC_CTX_set_params(evp, params)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_CTX_set_params() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_init(ctx, key, keylength, NULL)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_init() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_update(ctx, pkt, pktlength)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_update() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_final(ctx, answer, &len, sizeof(answer))) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_final() failed: %s.\n", str);
		return 0;
	}
	return len;
}


static void Do_EVP_MAC(
  const char *name,       /* name of digest */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	const EVP_MD *digest = CheckDigest(name);
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;
	int samplesize = SAMPLESIZE;
	char stupid[100];			/* OpenSSL rejects const */

	if (NULL == digest) {
		return;
	}
	if (pktlength > 1000) samplesize /= 10;
	strncpy(stupid, name, sizeof(stupid));

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
		digestlength = One_EVP_MAC(evp, stupid, key, keylength, pkt, pktlength);
		if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %2lu %6.0f %7.3f",
	       name, keylength, pktlength, digestlength, fast/samplesize,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}


static size_t One_EVP_MAC2(
  EVP_MAC_CTX *ctx,         /* context */
  uint8_t *key,             /* key pointer */
  int     keylength,        /* key length */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	size_t len = EVP_MAX_MD_SIZE;

	if (0 == EVP_MAC_init(ctx, key, keylength, NULL)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_init() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_update(ctx, pkt, pktlength)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_update() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_final(ctx, answer, &len, sizeof(answer))) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_final() failed: %s.\n", str);
		return 0;
	}
	return len;
}


static void Do_EVP_MAC2(
  const char *name,       /* name of digest */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;
	int samplesize = SAMPLESIZE;
	const EVP_MD *digest = CheckDigest(name);
	OSSL_PARAM params[2];
	char stupid[100];			/* OpenSSL rejects const */

	if (NULL == digest) {
		return;
	}
	if (pktlength > 1000) samplesize /= 10;
	strncpy(stupid, name, sizeof(stupid));

	params[0] =
          OSSL_PARAM_construct_utf8_string("digest", stupid, 0);
	params[1] = OSSL_PARAM_construct_end();
	if (0 == EVP_MAC_CTX_set_params(evp, params)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_CTX_set_params() failed: %s.\n", str);
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
		digestlength = One_EVP_MAC2(evp, key, keylength, pkt, pktlength);
if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %2lu %6.0f %7.3f",
	       name, keylength, pktlength, digestlength, fast/samplesize,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}


static size_t One_EVP_MAC3(
  EVP_MAC_CTX *ctx,         /* context  */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	size_t len = EVP_MAX_MD_SIZE;

	if (0 == EVP_MAC_init(ctx, NULL, 0, NULL)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_init() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_update(ctx, pkt, pktlength)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_update() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_final(ctx, answer, &len, sizeof(answer))) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_final() failed: %s.\n", str);
		return 0;
	}
	return len;
}


static void Do_EVP_MAC3(
  const char *name,       /* name of cipher */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;
	int samplesize = SAMPLESIZE;
	const EVP_MD *digest = CheckDigest(name);
	OSSL_PARAM params[3];
	char stupid[100];			/* OpenSSL rejects const */

	if (NULL == digest) {
		return;
	}
	if (pktlength > 1000) samplesize /= 10;
	strncpy(stupid, name, sizeof(stupid));

	params[0] =
          OSSL_PARAM_construct_utf8_string("digest", stupid, 0);
	params[1] =
          OSSL_PARAM_construct_octet_string("key", key, keylength);
	params[2] = OSSL_PARAM_construct_end();
	if (0 == EVP_MAC_CTX_set_params(evp, params)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_CTX_set_params() failed: %s.\n", str);
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
		digestlength = One_EVP_MAC3(evp, pkt, pktlength);
if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %2lu %6.0f %7.3f",
	       name, keylength, pktlength, digestlength, fast/samplesize,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}

int main(int argc, char *argv[])
{
	uint8_t key[MAX_KEY_LENGTH];
	uint8_t packet[MAX_PACKET_LENGTH];
	char buff[256];
	char *ctimetxt;
	time_t now;

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	setlinebuf(stdout);

	ssl_init();
	RAND_bytes((unsigned char *)&key, MAX_KEY_LENGTH);
	RAND_bytes((unsigned char *)&packet, MAX_PACKET_LENGTH);
	/* make things deterministic */
	for (int i=0; i< MAX_KEY_LENGTH; i++) key[i]=i*i+0x23;
	for (int i=0; i< MAX_PACKET_LENGTH; i++) packet[i]=i*i+0x31;

	now = time(NULL);
	ctimetxt = ctime(&now);
	ctimetxt[24] = 0;	/* Hack: smash return */
	gethostname(buff, sizeof(buff));
	printf("# %s on %s\n", ctimetxt, buff);
	printf("# %s\n", OPENSSL_VERSION_TEXT);

	printf("\n");
	printf("# KL=key length, PL=packet length, HL=HMAC length\n");
	printf("# HMAC        KL   PL HL  ns/op sec/run\n");

	Do_HMAC("SHA256",       key, 16, packet, PACKET_LENGTH);
	Do_HMAC("SHA256",       key, 20, packet, PACKET_LENGTH);
	Do_HMAC("SHA384",       key, 20, packet, PACKET_LENGTH);
	Do_HMAC("SHA512",       key, 20, packet, PACKET_LENGTH);

	printf("\n");
	printf("# KL=key length, PL=packet length, HL=HMAC length\n");
	printf("# EVP_MAC     KL   PL HL  ns/op sec/run\n");

	Do_EVP_MAC("SHA256",    key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC("SHA256",    key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC("SHA384",    key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC("SHA512",    key, 20, packet, PACKET_LENGTH);

	printf("\n");
	printf("EVP_MAC Preload cipher.\n");
	Do_EVP_MAC2("SHA256",   key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC2("SHA256",   key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC2("SHA384",   key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC2("SHA512",   key, 20, packet, PACKET_LENGTH);

	printf("\n");
	printf("EVP_MAC Preload cipher and key.\n");
	Do_EVP_MAC3("SHA256",   key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC3("SHA256",   key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC3("SHA384",   key, 20, packet, PACKET_LENGTH);
	Do_EVP_MAC3("SHA512",   key, 20, packet, PACKET_LENGTH);

	return 0;
}
#else
int main(int argc, char *argv[])
{
        UNUSED_ARG(argc);
        UNUSED_ARG(argv);
        printf("This program doesn't work on really old versions of OpenSSL\n");
        return 1; 
}
#endif  // OPENSSL_VERSION_NUMBER

