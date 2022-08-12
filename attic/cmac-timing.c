/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

/* Hack to time various implementations of CMAC.
 *
 * This is just the CMAC timing.
 * It doesn't include the copy or compare or finding the right key.
 *
 * Beware of overflows in the timing computations.
 *
 * Disable AES-NI (Intel hardware: NI == New Instruction) with:
 *    OPENSSL_ia32cap="~0x200000200000000"
 * Check /proc/cpuinfo flags for "aes" to see if you have it.
 */

#define CMAC_VERSION_CUTOFF 0x10000003

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Silence warnings from CMAC routines in OpenSSL 3.0.0 */
#define OPENSSL_SUPPRESS_DEPRECATED 1

#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/cmac.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/objects.h>
#if OPENSSL_VERSION_NUMBER > 0x20000000L
#include <openssl/params.h> 
#endif

#define UNUSED_ARG(arg)         ((void)(arg))


int SAMPLESIZE = 1000000;

#define PACKET_LENGTH 48
#define MAX_KEY_LENGTH 64

CMAC_CTX *cmac;
#if OPENSSL_VERSION_NUMBER > 0x20000000L
EVP_MAC_CTX *evp;
#endif

unsigned char answer[EVP_MAX_MD_SIZE];

static void ssl_init(void)
{
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	EVP_MAC *mac;
#endif
	ERR_load_crypto_strings();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_ciphers();
	cmac = CMAC_CTX_new();
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	mac = EVP_MAC_fetch(NULL, "cmac", NULL);
	if (NULL == mac)
		printf("## Oops, EVP_MAC_fetch() failed.\n");
	evp = EVP_MAC_CTX_new(mac);
	if (NULL == evp)
		printf("## Oops, EVP_MAC_CTX_new() failed.\n");
#endif
}

static const EVP_CIPHER *CheckCipher(const char *name) {
	const EVP_CIPHER *cipher;
	char cbc[100];
	snprintf(cbc, sizeof(cbc), "%s-CBC", name);
	cipher = EVP_get_cipherbyname(cbc);
	if (0 && NULL == cipher) {
		/* no error available */
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops: EVP_get_cipherbyname() failed: %s\n    %s\n", cbc, str);
		return NULL;
	}
	return cipher;
}

static void PrintHex(const unsigned char* bytes, int length) {
  printf("  ");
  for (int i=0; i<length; i++) {
    printf("%02x", bytes[i]);
  }
}

static size_t One_CMAC(
  const EVP_CIPHER *cipher, /* cipher algorithm */
  uint8_t *key,             /* key pointer */
  int     keylength,        /* key size */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	size_t len;
	if (1 != CMAC_Init(cmac, key, keylength, cipher, NULL)) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                printf("## Oops, CMAC_Init() failed:\n    %s.\n", str);
                return 0;
	}
	if (1 != CMAC_Update(cmac, pkt, pktlength)) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                printf("## Oops, CMAC_Update() failed:\n    %s.\n", str);
                return 0;
	}
	if (1 != CMAC_Final(cmac, answer, &len)) {
                unsigned long err = ERR_get_error();
                char * str = ERR_error_string(err, NULL);
                printf("## Oops, CMAC_Final() failed:\n    %s.\n", str);
                return 0;
	}
	return len;
}


static void DoCMAC(
  const char *name,       /* name of cipher */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	const EVP_CIPHER *cipher = CheckCipher(name);
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;

	if (NULL == cipher) {
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < SAMPLESIZE; i++) {
		digestlength = One_CMAC(cipher, key, keylength, pkt, pktlength);
		if (0 == digestlength)
			break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %2d %2lu %6.0f  %6.3f",
	       name, keylength, pktlength, digestlength, fast/SAMPLESIZE,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}

#if OPENSSL_VERSION_NUMBER > 0x10101000L
static size_t One_PKEY(
  EVP_PKEY *pkey,
  EVP_MD_CTX *ctx,          /* context  */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	size_t len = EVP_MAX_MD_SIZE;
	if (1 != EVP_DigestSignInit(ctx, NULL, NULL, NULL, pkey)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_DigestSignInit() failed:\n    %s.\n", str);
		return 0;
	}
	EVP_DigestSign(ctx, answer, &len, pkt, pktlength);
	return len;
}


static void DoPKEY(
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

	const EVP_CIPHER *cipher = CheckCipher(name);
	EVP_PKEY *pkey;
	EVP_MD_CTX *ctx;

	if (NULL == cipher) {
		return;
	}

	pkey = EVP_PKEY_new_CMAC_key(NULL, key, keylength, cipher);
	if (NULL == pkey) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_PKEY_new_CMAC_key() failed: %s\n    %s.\n", \
			name, str);
		return;
	}
	ctx = EVP_MD_CTX_new();
	if (NULL == ctx) {
		printf("## Oops, EVP_MD_CTX_new() failed.\n");
		return;
	}
#ifdef EVP_MD_CTX_FLAG_FINALISE
	EVP_MD_CTX_set_flags(ctx, EVP_MD_CTX_FLAG_FINALISE);
#endif

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < SAMPLESIZE; i++) {
		digestlength = One_PKEY(pkey, ctx, pkt, pktlength);
	}

	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %2d %2lu %6.0f  %6.3f",
	       name, keylength, pktlength, digestlength, fast/SAMPLESIZE,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
	EVP_MD_CTX_free(ctx);
	EVP_PKEY_free(pkey);
}
#endif

#if OPENSSL_VERSION_NUMBER > 0x20000000L
static size_t One_EVP_MAC(
  EVP_MAC_CTX *ctx,         /* context  */
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


static void Do_EVP_MAC(
  const char *name,       /* name of cipher */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
	const EVP_CIPHER *cipher = CheckCipher(name);
	struct timespec start, stop;
	double fast;
	unsigned long digestlength = 0;
	char cbc[100];

	if (NULL == cipher) {
		return;
	}
	snprintf(cbc, sizeof(cbc), "%s-CBC", name);


	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < SAMPLESIZE; i++) {
		digestlength = One_EVP_MAC(evp, key, keylength, pkt, pktlength);
if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %2d %2lu %6.0f  %6.3f",
	       name, keylength, pktlength, digestlength, fast/SAMPLESIZE,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}
static size_t One_EVP_MAC2(
  EVP_MAC_CTX *ctx,         /* context  */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
	EVP_MAC_CTX *dup;
	size_t len = EVP_MAX_MD_SIZE;

	// dup = ctx;
	dup = EVP_MAC_CTX_dup(ctx);

	if (0 == EVP_MAC_init(dup, NULL, 0, NULL)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_init() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_update(dup, pkt, pktlength)) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_update() failed: %s.\n", str);
		return 0;
	}
	if (0 == EVP_MAC_final(dup, answer, &len, sizeof(answer))) {
		unsigned long err = ERR_get_error();
		char * str = ERR_error_string(err, NULL);
		printf("## Oops, EVP_MAC_final() failed: %s.\n", str);
		return 0;
	}
	EVP_MAC_CTX_free(dup);
	return len;
}


static void Do_EVP_MAC2(
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
	char cbc[100];
	const EVP_CIPHER *cipher = CheckCipher(name);
	OSSL_PARAM params[3];

	if (NULL == cipher) {
		return;
	}
	snprintf(cbc, sizeof(cbc), "%s-CBC", name);

	params[0] =
          OSSL_PARAM_construct_utf8_string("cipher", cbc, 0);
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
	for (int i = 0; i < SAMPLESIZE; i++) {
		digestlength = One_EVP_MAC2(evp, pkt, pktlength);
if (0 == digestlength) break;
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %2d %2lu %6.0f  %6.3f",
	       name, keylength, pktlength, digestlength, fast/SAMPLESIZE,  fast/1E9);
	PrintHex(answer, digestlength);
	printf("\n");
}
#endif

int main(int argc, char *argv[])
{
	uint8_t key[MAX_KEY_LENGTH];
	uint8_t packet[PACKET_LENGTH];

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	setlinebuf(stdout);

	ssl_init();
	RAND_bytes((unsigned char *)&key, MAX_KEY_LENGTH);
	RAND_bytes((unsigned char *)&packet, PACKET_LENGTH);
	for (int i=0; i< MAX_KEY_LENGTH; i++) key[i]=i*i+0x23;
	for (int i=0; i< PACKET_LENGTH; i++) packet[i]=i*i+0x31;

	printf("# %s\n", OPENSSL_VERSION_TEXT);

	printf("\n");
	printf("# KL=key length, PL=packet length, CL=CMAC length\n");
	printf("# CMAC        KL PL CL  ns/op sec/run\n");

#if OPENSSL_VERSION_NUMBER < 0x20000000L
/* Hangs on 3.0.0  Checking OPENSSL_NO_DES doesn't work. */
	DoCMAC("DES",          key,  8, packet, PACKET_LENGTH);
#endif
	DoCMAC("DES-EDE",      key, 16, packet, PACKET_LENGTH);
	DoCMAC("DES-EDE3",     key, 24, packet, PACKET_LENGTH);
#ifndef OPENSSL_NO_SM4
	DoCMAC("SM4",          key, 16, packet, PACKET_LENGTH);
#endif
	DoCMAC("AES-128",      key, 16, packet, PACKET_LENGTH);
	DoCMAC("AES-192",      key, 24, packet, PACKET_LENGTH);
	DoCMAC("AES-256",      key, 32, packet, PACKET_LENGTH);
	DoCMAC("CAMELLIA-128", key, 16, packet, PACKET_LENGTH);
	DoCMAC("CAMELLIA-192", key, 24, packet, PACKET_LENGTH);
	DoCMAC("CAMELLIA-256", key, 32, packet, PACKET_LENGTH);
	DoCMAC("ARIA-128",     key, 16, packet, PACKET_LENGTH);
	DoCMAC("ARIA-192",     key, 24, packet, PACKET_LENGTH);
	DoCMAC("ARIA-256",     key, 32, packet, PACKET_LENGTH);

#if OPENSSL_VERSION_NUMBER > 0x10101000L
	printf("\n");
	printf("# KL=key length, PL=packet length, CL=CMAC length\n");
	printf("# PKEY        KL PL CL  ns/op sec/run\n");

#if OPENSSL_VERSION_NUMBER < 0x20000000L
	DoPKEY("DES",          key,  8, packet, PACKET_LENGTH);
#endif
	DoPKEY("DES-EDE",      key, 16, packet, PACKET_LENGTH);
	DoPKEY("DES-EDE3",     key, 24, packet, PACKET_LENGTH);
#ifndef OPENSSL_NO_SM4
	DoPKEY("SM4",          key, 16, packet, PACKET_LENGTH);
#endif
	DoPKEY("AES-128",      key, 16, packet, PACKET_LENGTH);
	DoPKEY("AES-192",      key, 24, packet, PACKET_LENGTH);
	DoPKEY("AES-256",      key, 32, packet, PACKET_LENGTH);
	DoPKEY("CAMELLIA-128", key, 16, packet, PACKET_LENGTH);
	DoPKEY("CAMELLIA-192", key, 24, packet, PACKET_LENGTH);
	DoPKEY("CAMELLIA-256", key, 32, packet, PACKET_LENGTH);
	DoPKEY("ARIA-128",     key, 16, packet, PACKET_LENGTH);
	DoPKEY("ARIA-192",     key, 24, packet, PACKET_LENGTH);
	DoPKEY("ARIA-256",     key, 32, packet, PACKET_LENGTH);
#endif

#if OPENSSL_VERSION_NUMBER > 0x20000000L
	printf("\n");
	printf("# KL=key length, PL=packet length, CL=CMAC length\n");
	printf("# EVP_MAC     KL PL CL  ns/op sec/run\n");

	Do_EVP_MAC("DES-EDE",      key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC("DES-EDE3",     key, 24, packet, PACKET_LENGTH);
#ifndef OPENSSL_NO_SM4
	Do_EVP_MAC("SM4",          key, 16, packet, PACKET_LENGTH);
#endif
	Do_EVP_MAC("AES-128",      key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC("AES-192",      key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC("AES-256",      key, 32, packet, PACKET_LENGTH);
	Do_EVP_MAC("CAMELLIA-128", key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC("CAMELLIA-192", key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC("CAMELLIA-256", key, 32, packet, PACKET_LENGTH);
	Do_EVP_MAC("ARIA-128",     key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC("ARIA-192",     key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC("ARIA-256",     key, 32, packet, PACKET_LENGTH);

	printf("\n");
	printf("Preload cipher and key.\n");
	Do_EVP_MAC2("DES-EDE",      key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC2("DES-EDE3",     key, 24, packet, PACKET_LENGTH);
#ifndef OPENSSL_NO_SM4
	Do_EVP_MAC2("SM4",          key, 16, packet, PACKET_LENGTH);
#endif
	Do_EVP_MAC2("AES-128",      key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC2("AES-192",      key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC2("AES-256",      key, 32, packet, PACKET_LENGTH);
	Do_EVP_MAC2("CAMELLIA-128", key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC2("CAMELLIA-192", key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC2("CAMELLIA-256", key, 32, packet, PACKET_LENGTH);
	Do_EVP_MAC2("ARIA-128",     key, 16, packet, PACKET_LENGTH);
	Do_EVP_MAC2("ARIA-192",     key, 24, packet, PACKET_LENGTH);
	Do_EVP_MAC2("ARIA-256",     key, 32, packet, PACKET_LENGTH);
#endif

	return 0;
}
