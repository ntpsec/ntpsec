/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

/* Hack to time the digest calculations for various algorithms.
 *
 * This is just the digest timing.
 * It doesn't include the copy or compare or finding the right key.
 *
 * Beware of overflows in the timing computations.
 *
 * Disable AES-NI (Intel hardware: NI == New Instruction) with:
 *    OPENSSL_ia32cap="~0x200000200000000"
 * Check /proc/cpuinfo flags for "aes" to see if you have it.
 */

/* This may not be high enough.
 * 0x10000003  1.0.0b fails
 * 0x1000105fL 1.0.1e works.
 */
#define CMAC_VERSION_CUTOFF 0x10000003

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <openssl/opensslv.h>
#include <openssl/err.h>
#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
#include <openssl/cmac.h>
#endif
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/objects.h>

#define UNUSED_ARG(arg)         ((void)(arg))

#ifndef EVP_MD_CTX_reset
/* Slightly older version of OpenSSL */
/* Similar hack in ssl_init.c */
#define EVP_MD_CTX_new() EVP_MD_CTX_create()
#define EVP_MD_CTX_free(ctx) EVP_MD_CTX_destroy(ctx)
#define EVP_MD_CTX_reset(ctx) EVP_MD_CTX_init(ctx)
#endif


/* Get timing for old slower way too.  Pre Feb 2018 */
#define DoSLOW 1

int NUM = 1000000;

#define PACKET_LENGTH 48
/* Nothing magic about these key lengths.
 * ntpkeygen just happens to label things this way.
 */
#define AES_KEY_LENGTH 16
#define MD5_KEY_LENGTH 16
#define SHA1_KEY_LENGTH 20
#define MAX_KEY_LENGTH 64

EVP_MD_CTX *ctx;
#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
CMAC_CTX *cmac;
#endif

static void ssl_init(void)
{
  ERR_load_crypto_strings();
  OpenSSL_add_all_digests();
  OpenSSL_add_all_ciphers();
  ctx = EVP_MD_CTX_new();
#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
  cmac = CMAC_CTX_new();
#endif
}

static unsigned int SSL_Digest(
  const EVP_MD *digest,   /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,       /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
) {
  unsigned char answer[EVP_MAX_MD_SIZE];
  unsigned int len;
  EVP_MD_CTX_reset(ctx);
  EVP_DigestInit(ctx, digest);
  EVP_DigestUpdate(ctx, key, keylength);
  EVP_DigestUpdate(ctx, pkt, pktlength);
  EVP_DigestFinal(ctx, answer, &len);
  return len;
}

static unsigned int SSL_DigestSlow(
  int type,               /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
) {
  EVP_MD_CTX *ctxx;
  unsigned char answer[EVP_MAX_MD_SIZE];
  unsigned int len;
  ctxx = EVP_MD_CTX_new();
  EVP_DigestInit(ctxx, EVP_get_digestbynid(type));
  EVP_DigestUpdate(ctxx, key, keylength);
  EVP_DigestUpdate(ctxx, pkt, pktlength);
  EVP_DigestFinal(ctxx, answer, &len);
  EVP_MD_CTX_free(ctxx);
  return len;
}

#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
static size_t SSL_CMAC(
  const EVP_CIPHER *cipher, /* cipher algorithm */
  uint8_t *key,             /* key pointer */
  int     keylength,        /* key size */
  uint8_t *pkt,             /* packet pointer */
  int     pktlength         /* packet length */
) {
  unsigned char answer[EVP_MAX_MD_SIZE];
  size_t len;
  CMAC_resume(cmac);
  CMAC_Init(cmac, key, keylength, cipher, NULL);
  CMAC_Update(cmac, pkt, pktlength);
  CMAC_Final(cmac, answer, &len);
  return len;
}
#endif

static void DoDigest(
  const char *name,       /* type of digest */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
  int type = OBJ_sn2nid(name);
  const EVP_MD *digest = EVP_get_digestbynid(type);
  struct timespec start, stop;
  int i;
  double fast, slow;
  unsigned int digestlength = 0;

  if (NULL == digest) return;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (i = 0; i < NUM; i++) {
    digestlength = SSL_Digest(digest, key, keylength, pkt, pktlength);
  }
  clock_gettime(CLOCK_MONOTONIC, &stop);
  fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
  printf("%10s  %2d %2d %2u %6.0f  %6.3f",
    name, keylength, pktlength, digestlength, fast/NUM,  fast/1E9);

#ifdef DoSLOW
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (i = 0; i < NUM; i++) {
    digestlength = SSL_DigestSlow(type, key, keylength, pkt, pktlength);
  }
  clock_gettime(CLOCK_MONOTONIC, &stop);
  slow = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
  printf("   %6.0f  %2.0f %4.0f",
    slow/NUM, (slow-fast)*100.0/slow, (slow-fast)/NUM);
#endif
  printf("\n");
}

#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
static void DoCMAC(
  const char *name,       /* name of cipher */
  const EVP_CIPHER *cipher,
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key length */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
)
{
  struct timespec start, stop;
  int i;
  double fast;
  unsigned long digestlength = 0;

  if (NULL == cipher) return;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (i = 0; i < NUM; i++) {
    digestlength = SSL_CMAC(cipher, key, keylength, pkt, pktlength);
  }
  clock_gettime(CLOCK_MONOTONIC, &stop);
  fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
  printf("%10s  %2d %2d %2lu %6.0f  %6.3f",
    name, keylength, pktlength, digestlength, fast/NUM,  fast/1E9);

  printf("\n");
}
#endif


int main(int argc, char *argv[])
{
  uint8_t key[MAX_KEY_LENGTH];
  uint8_t packet[PACKET_LENGTH];

  UNUSED_ARG(argc);
  UNUSED_ARG(argv);

  ssl_init();
  RAND_bytes((unsigned char *)&key, MAX_KEY_LENGTH);
  RAND_bytes((unsigned char *)&packet, PACKET_LENGTH);

  printf("# %s\n", OPENSSL_VERSION_TEXT);
  printf("# KL=key length, PL=packet length, DL=digest length\n");
  printf("# Digest    KL PL DL  ns/op sec/run     slow   %% diff\n");

  DoDigest("MD5",    key, MD5_KEY_LENGTH, packet, PACKET_LENGTH);
  DoDigest("MD5",    key, MD5_KEY_LENGTH-1, packet, PACKET_LENGTH);
  DoDigest("MD5",    key, SHA1_KEY_LENGTH, packet, PACKET_LENGTH);
  DoDigest("SHA1",   key, MD5_KEY_LENGTH, packet, PACKET_LENGTH);
  DoDigest("SHA1",   key, SHA1_KEY_LENGTH, packet, PACKET_LENGTH);
  DoDigest("SHA1",   key, SHA1_KEY_LENGTH-1, packet, PACKET_LENGTH);
  DoDigest("SHA224", key, 16, packet, PACKET_LENGTH);
  DoDigest("SHA224", key, 20, packet, PACKET_LENGTH);
  DoDigest("SHA256", key, 16, packet, PACKET_LENGTH);
  DoDigest("SHA256", key, 20, packet, PACKET_LENGTH);
  DoDigest("SHA384", key, 16, packet, PACKET_LENGTH);
  DoDigest("SHA384", key, 20, packet, PACKET_LENGTH);
  DoDigest("SHA512", key, 16, packet, PACKET_LENGTH);
  DoDigest("SHA512", key, 20, packet, PACKET_LENGTH);
  DoDigest("SHA512", key, 24, packet, PACKET_LENGTH);
  DoDigest("SHA512", key, 32, packet, PACKET_LENGTH);
  DoDigest("RIPEMD160", key, 16, packet, PACKET_LENGTH);
  DoDigest("RIPEMD160", key, 20, packet, PACKET_LENGTH);
  DoDigest("RIPEMD160", key, 32, packet, PACKET_LENGTH);

#if OPENSSL_VERSION_NUMBER > CMAC_VERSION_CUTOFF
  printf("\n");
  printf("# KL=key length, PL=packet length, CL=CMAC length\n");
  printf("# CMAC      KL PL CL  ns/op sec/run\n");

  DoCMAC("DES",     EVP_des_cbc(),          key,  8, packet, PACKET_LENGTH);
  DoCMAC("AES-128", EVP_aes_128_cbc(),      key, 16, packet, PACKET_LENGTH);
  DoCMAC("AES-192", EVP_aes_192_cbc(),      key, 24, packet, PACKET_LENGTH);
  DoCMAC("AES-256", EVP_aes_256_cbc(),      key, 32, packet, PACKET_LENGTH);
#ifndef OPENSSL_NO_CAMELLIA
  DoCMAC("CAM-128", EVP_camellia_128_cbc(), key, 16, packet, PACKET_LENGTH);
  DoCMAC("CAM-192", EVP_camellia_192_cbc(), key, 24, packet, PACKET_LENGTH);
  DoCMAC("CAM-256", EVP_camellia_256_cbc(), key, 32, packet, PACKET_LENGTH);
#endif
#endif

  return 0;

}
