/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

/* Hack to time the digest calculations for various algorithms.
 *
 * This is just the digest timing.
 * It doesn't include the copy or compare or finding the right key.
 *
 * Beware of overflows in the timing computations.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/objects.h>

#define UNUSED_ARG(arg)         ((void)(arg))


/* Get timing for old slower way too.  Pre Feb 2018 */
#define DoSLOW 1

int NUM = 1000000;

#define PACKET_LENGTH 48
/* Nothing magic about these key lengths.
 * ntpkeygen just happens to label things this way.
 */
#define MD5_KEY_LENGTH 16
#define SHA1_KEY_LENGTH 20
#define MAX_KEY_LENGTH 64

EVP_MD_CTX *ctx;

void ssl_init(void);
void ssl_init(void)
{
  ERR_load_crypto_strings();
  OpenSSL_add_all_digests();
#ifdef EVP_MD_CTX_new
  ctx = EVP_MD_CTX_new();
#else
  ctx = EVP_MD_CTX_create();
#endif
}

unsigned int SSL_Digest(
  const EVP_MD *digest,   /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,       /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
);
unsigned int SSL_Digest(
  const EVP_MD *digest,   /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,       /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
) {
  unsigned char answer[EVP_MAX_MD_SIZE];
  unsigned int len;
#ifdef EVP_MD_CTX_reset
  EVP_MD_CTX_reset(ctx);
#else
  EVP_MD_CTX_init(ctx);
#endif
  EVP_DigestInit(ctx, digest);
  EVP_DigestUpdate(ctx, key, keylength);
  EVP_DigestUpdate(ctx, pkt, pktlength);
  EVP_DigestFinal(ctx, answer, &len);
  return len;
}
unsigned int SSL_DigestSlow(
  int type,               /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,       /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
);
unsigned int SSL_DigestSlow(
  int type,               /* hash algorithm */
  uint8_t *key,           /* key pointer */
  int     keylength,       /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
) {
  EVP_MD_CTX *ctxx;
  unsigned char answer[EVP_MAX_MD_SIZE];
  unsigned int len;
#ifdef EVP_MD_CTX_new
  ctxx = EVP_MD_CTX_new();
#else
  ctxx = EVP_MD_CTX_create();
#endif
  EVP_DigestInit(ctxx, EVP_get_digestbynid(type));
  EVP_DigestUpdate(ctxx, key, keylength);
  EVP_DigestUpdate(ctxx, pkt, pktlength);
  EVP_DigestFinal(ctxx, answer, &len);
#ifdef EVP_MD_CTX_free
  EVP_MD_CTX_free(ctxx);
#else
  EVP_MD_CTX_destroy(ctxx);
#endif
  return len;
}

void DoOne(
  const char *name,       /* type of digest */
  uint8_t *key,           /* key pointer */
  int     keylength,      /* key size */
  uint8_t *pkt,           /* packet pointer */
  int     pktlength       /* packet length */
);
void DoOne(
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

  DoOne("MD5",    key, MD5_KEY_LENGTH, packet, PACKET_LENGTH);
  DoOne("MD5",    key, MD5_KEY_LENGTH-1, packet, PACKET_LENGTH);
  DoOne("MD5",    key, SHA1_KEY_LENGTH, packet, PACKET_LENGTH);
  DoOne("SHA1",   key, MD5_KEY_LENGTH, packet, PACKET_LENGTH);
  DoOne("SHA1",   key, SHA1_KEY_LENGTH, packet, PACKET_LENGTH);
  DoOne("SHA1",   key, SHA1_KEY_LENGTH-1, packet, PACKET_LENGTH);
  DoOne("SHA224", key, 16, packet, PACKET_LENGTH);
  DoOne("SHA224", key, 20, packet, PACKET_LENGTH);
  DoOne("SHA256", key, 16, packet, PACKET_LENGTH);
  DoOne("SHA256", key, 20, packet, PACKET_LENGTH);
  DoOne("SHA384", key, 16, packet, PACKET_LENGTH);
  DoOne("SHA384", key, 20, packet, PACKET_LENGTH);
  DoOne("SHA512", key, 16, packet, PACKET_LENGTH);
  DoOne("SHA512", key, 20, packet, PACKET_LENGTH);
  DoOne("SHA512", key, 24, packet, PACKET_LENGTH);
  DoOne("SHA512", key, 32, packet, PACKET_LENGTH);
  DoOne("RIPEMD160", key, 16, packet, PACKET_LENGTH);
  DoOne("RIPEMD160", key, 20, packet, PACKET_LENGTH);
  DoOne("RIPEMD160", key, 32, packet, PACKET_LENGTH);

  return 0;
  
}
