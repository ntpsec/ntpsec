/*
 * nts_cookie.c - Network Time Security (NTS) cookie processing
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 * This follows section 6, Suggested Format for NTS Cookies
 * It uses AEAD_AES_SIV_CMAC_256/384/512 from RFC 5297
 * It is currently a stand-alone library
 * but will probably migrate to OpenSSL/libcrypto.
 *
 * The selection is done by the key length.
 *
 */

#include "config.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <openssl/rand.h>
#include <aes_siv.h>

#include "ntp_stdlib.h"
#include "nts.h"

/* Cookie format:
 *  cookie is I,N,CMAC,C
 *    I Key index, see below
 *    N nonce
 *  C is encrypt(K, N, P)
 *  P is AEAD, C2S, S2C
 *  length of C2S and S2C depends upon AEAD
 *  CMAC is 16 bytes
 */

/* K and I should be preserved across boots, and rotated every day or so.
 * We need to support the old K/I for another day.
 * Encryption within cookies uses AEAD_AES_SIV_CMAC_nnn.  That's the
 * same family of algorithims as NTS uses on the wire.
 * The nnn is selected by the key length.
 *   32 => 256
 *   48 => 384
 *   64 => 512
 */

/* Max length:
 *   4 I
 *  16 N
 *  16 CMAC
 *   4 AEAD
 *  64 C2S    NTS_MAX_KEYLEN
 *  64 S2C    NTS_MAX_KEYLEN
 * ------
 * 168
 */

// FIXME - save K/I on disk
// FIXME - rotate K/I occasionally

/* cookies use same AEAD algorithms as wire */
uint8_t K[NTS_MAX_KEYLEN];
uint32_t I;

AES_SIV_CTX* cookie_ctx;  /* need one per thread */
pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;

/* This determines which algorithm we use.
 * Valid choices are 32, 48, and 64
 * making this a variable rather than #define
 * opens up the opportunity to pick one at run time. */
int K_length = AEAD_AES_SIV_CMAC_256_KEYLEN;

// FIXME duplicated in ntp_extens
#define NONCE_LENGTH 16

/* Associated data: aead (rounded up to 4) plus NONCE */
#define AD_LENGTH 20
#define AEAD_LENGTH 4

bool nts_cookie_init(void) {
  bool OK = true;
#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
  OK &= RAND_priv_bytes(K, sizeof(K));
  OK &= RAND_bytes((uint8_t *)&I, sizeof(I));
#else
  OK &= RAND_bytes(K, sizeof(K));
  OK &= RAND_bytes((uint8_t *)&I, sizeof(I));
#endif
  cookie_ctx = AES_SIV_CTX_new();
  if (NULL == cookie_ctx) {
    msyslog(LOG_ERR, "NTS: Can't init cookie_ctx");
    exit(1);
  }
// FIXME hack to allow testing
if (1) {
  I = 13;
  for (unsigned int i=0; i<sizeof(K); i++) K[i] = i;
}
  return OK;
}

/* returns actual length */
int nts_make_cookie(uint8_t *cookie,
  uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen) {
  uint8_t plaintext[NTS_MAX_COOKIELEN];
  uint8_t *nonce;
  int used, plainlength;
  bool ok;
  int err;

  // ASSERT(keylen<NTS_MAX_KEYLEN);
  
  uint8_t * finger;
  uint32_t temp;      /* keep 4 byte alignment */
  size_t left;

  err = pthread_mutex_lock(&cookie_lock);
  if (0 != err) {
    msyslog(LOG_ERR, "ERR: Can't lock cookie_lock: %d", err);
    exit(2);
  }

  /* collect plaintext
   * separate buffer avoids encrypt in place
   * but costs cache space
   */
  finger = plaintext;
  temp = aead;
  memcpy(finger, &temp, AEAD_LENGTH);
  finger += AEAD_LENGTH;
  memcpy(finger, c2s, keylen);
  finger += keylen;
  memcpy(finger, s2c, keylen);
  finger += keylen;
  plainlength = finger-plaintext;

  /* collect associated data */
  finger = cookie;

  memcpy(finger, &I, sizeof(I));
  finger += sizeof(I);

  nonce = finger;
  RAND_bytes(finger, NONCE_LENGTH);
  finger += NONCE_LENGTH;

  // require(AD_LENGTH==finger-cookie);

  used = finger-cookie;
  left = NTS_MAX_COOKIELEN-used;

  ok = AES_SIV_Encrypt(cookie_ctx,
           finger, &left,   /* left: in: max out length, out: length used */
           K, K_length,
           nonce, NONCE_LENGTH,
           plaintext, plainlength,
           cookie, AD_LENGTH);
  if (!ok) {
    msyslog(LOG_ERR, "NTS: nts_make_cookie - Error from AES_SIV_Encrypt");
    /* I don't think this should happen,
     * so crash rather than work incorrectly.
     * Hal, 2019-Feb-17
     * Similar code in ntp_extens
     */
    exit(1);
  }

  used += left;
  // ASSERT(length < NTS_MAX_COOKIELEN);
  // Need to encrypt

  err = pthread_mutex_unlock(&cookie_lock);
  if (0 != err) {
    msyslog(LOG_ERR, "ERR: Can't unlock cookie_lock: %d", err);
    exit(2);
  }
  return used;
}

/* can't decrypt in place - that would trash the unauthenticated packet */
bool nts_unpack_cookie(uint8_t *cookie, int cookielen,
  uint16_t *aead,
  uint8_t *c2s, uint8_t *s2c, int *keylen) {

  uint8_t *finger;
  uint8_t plaintext[NTS_MAX_COOKIELEN];
  uint8_t *nonce;
  uint32_t temp;
  size_t plainlength;
  int cipherlength;
  bool ok;

// FIXME - these are our cookies.  We should know the exact length
  if (cookielen > NTS_MAX_COOKIELEN)
    return false;

  finger = cookie;
  // FIXME should call routine to return key
  if (0 != memcmp(finger, &I, sizeof(I)))
    return false;
  finger += sizeof(I);
  nonce = finger;
  finger += NONCE_LENGTH;

  // require(AD_LENGTH==finger-cookie);
  
  cipherlength = cookielen - AD_LENGTH;
  plainlength = NTS_MAX_COOKIELEN;

  ok = AES_SIV_Decrypt(cookie_ctx,
           plaintext, &plainlength,
           K, K_length,
           nonce, NONCE_LENGTH,
           finger, cipherlength,
           cookie, AD_LENGTH);
  if (!ok)
    return false;

  *keylen = (plainlength-AEAD_LENGTH)/2;
  finger = plaintext;
  memcpy(&temp, finger, AEAD_LENGTH);
  *aead = temp;
  finger += AEAD_LENGTH;
  memcpy(c2s, finger, *keylen);
  finger += *keylen;
  memcpy(s2c, finger, *keylen);
  finger += *keylen;

  return true;
}



/* end */
