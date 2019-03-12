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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <openssl/rand.h>
#include <aes_siv.h>

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "nts.h"
#include "nts2.h"

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

bool nts_read_cookie_keys(void);
bool nts_make_cookie_key(void);
bool nts_write_cookie_keys(void);


/* cookies use same AEAD algorithms as wire */
/* This determines which algorithm we use.
 * Valid choices are 32, 48, and 64
 * making this a variable rather than #define
 * opens up the opportunity to pick one at run time.
 * The default (below) is 32/AEAD_AES_SIV_CMAC_256
 * You can change that by editing the keys file.
 */
int K_length = AEAD_AES_SIV_CMAC_256_KEYLEN;
uint8_t K[NTS_MAX_KEYLEN], K2[NTS_MAX_KEYLEN];
uint32_t I, I2;
time_t K_time = 0;	/* time K was created, 0 for none */

AES_SIV_CTX* cookie_ctx;  /* one per thread ?? */
pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;

/* Statistics for ntpq */
uint64_t nts_cookie_make = 0;
uint64_t nts_cookie_decode = 0;
uint64_t nts_cookie_decode_old = 0;
uint64_t nts_cookie_decode_too_old = 0;
uint64_t nts_cookie_decode_error = 0;


// FIXME duplicated in ntp_extens
#define NONCE_LENGTH 16

/* Associated data: aead (rounded up to 4) plus NONCE */
#define AD_LENGTH 20
#define AEAD_LENGTH 4

/* cookie_ctx needed for client side */
bool nts_cookie_init(void) {
  cookie_ctx = AES_SIV_CTX_new();
  if (NULL == cookie_ctx) {
    msyslog(LOG_ERR, "NTS: Can't init cookie_ctx");
    exit(1);
  }
  return true;
}

/* cookie key needed for server side */
bool nts_cookie_init2(void) {
  bool OK = true;
  if (!nts_read_cookie_keys()) {
    nts_make_cookie_key();  /* make new cookie key */
    nts_make_cookie_key();  /* push new to old, make new */
    K_time = time(NULL);
    nts_write_cookie_keys();
  }
  return OK;
}

/* Rotate key -- 24 hours after last rotate
 * That allows a cluster NTS-KE server to keep in sync
 * if we use ratchet rather than random.
 */
// #define SecondsPerDay 86400   FIXME
// Shorter for debugging
#define SecondsPerDay 3600
void nts_timer(void) {
  time_t now;
  long int steps;
  if (0 == K_time)
    return;
  now = time(NULL);
  if (SecondsPerDay > (now-K_time))
    return;
  nts_make_cookie_key();
  /* In case we were off for many days. */
  steps = (now-K_time) % SecondsPerDay;
  K_time += steps*SecondsPerDay;
  if (nts_write_cookie_keys() )
    msyslog(LOG_INFO, "NTS: Wrote new cookie key.");
  else
    msyslog(LOG_INFO, "NTS: Trouble writing new cookie key.");
  return;
}


bool nts_read_cookie_keys(void) {
  const char *cookie_filename = NTS_COOKIE_KEY_FILE;
  FILE *in;
  unsigned long templ;
  if (NULL != ntsconfig.KI)
    cookie_filename = ntsconfig.KI;
  in = fopen(cookie_filename, "r");
  if (NULL == in)
    // FIXME check errno - crash if exists but can't read
    return false;
  if (1 != fscanf(in, "T: %lu\n", &templ)) goto bail;
  K_time = templ;
  if (1 != fscanf(in, "L: %d\n", &K_length)) goto bail;
  if (1 != fscanf(in, "I: %u\n", &I)) goto bail;
  if (0 != fscanf(in, "K: ")) goto bail;
  for (int i=0; i< K_length; i++) {
    unsigned int temp;
    if (1 != fscanf(in, "%02x", &temp)) goto bail;
    K[i] = temp;
  }
  if (0 != fscanf(in, "\n")) goto bail;
  if (1 != fscanf(in, "I: %u\n", &I2)) goto bail;
  if (0 != fscanf(in, "K: ")) goto bail;
  for (int i=0; i< K_length; i++) {
    unsigned int temp;
    if (1 != fscanf(in, "%02x", &temp)) goto bail;
    K2[i] = temp;
  }
  if (0 != fscanf(in, "\n")) goto bail;
  fclose(in);
  return true;

bail:
  msyslog(LOG_ERR, "ERR: Error parsing cookie keys file");
  fclose(in);
  return false;
}

// FIXME need ratchet mode to make new keys
bool nts_make_cookie_key(void) {
  bool OK = true;
  memcpy(&K2, &K, sizeof(K2));	/* Push current cookie to old */
  I2 = I;
#if (OPENSSL_VERSION_NUMBER > 0x1010100fL)
  OK &= RAND_priv_bytes(K, sizeof(K));
#else
  OK &= RAND_bytes(K, sizeof(K));
#endif
  OK &= RAND_bytes((uint8_t *)&I, sizeof(I));
  return OK;
}

bool nts_write_cookie_keys(void) {
  const char *cookie_filename = NTS_COOKIE_KEY_FILE;
  int fd;
  FILE *out;
  if (NULL != ntsconfig.KI)
    cookie_filename = ntsconfig.KI;
  fd = open(cookie_filename, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
  if (-1 == fd) {
    msyslog(LOG_ERR, "ERR: can't open %s: %s", cookie_filename, strerror(errno));
    return false;
  }
  out = fdopen(fd, "w");
  if (NULL == out) {
    msyslog(LOG_ERR, "ERR: can't fdopen %s: %s", cookie_filename, strerror(errno));
    close(fd);
    return false;
  }
  // FIXME check return values
  fprintf(out, "T: %lu\n", (unsigned long)K_time);
  fprintf(out, "L: %d\n", K_length);
  fprintf(out, "I: %u\n", I);
  fprintf(out, "K: ");
  for (int i=0; i< K_length; i++) fprintf(out, "%02x", K[i]);
  fprintf(out, "\n");
  fprintf(out, "I: %u\n", I2);
  fprintf(out, "K: ");
  for (int i=0; i< K_length; i++) fprintf(out, "%02x", K2[i]);
  fprintf(out, "\n");
  fclose(out);
  return true;
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
  uint8_t * finger;
  uint32_t temp;	/* keep 4 byte alignment */
  size_t left;

  if (NULL == cookie_ctx)
    return 0;		/* We aren't initialized yet. */

  nts_cookie_make++;

  // ASSERT(keylen<NTS_MAX_KEYLEN);

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
  uint8_t *key;
  uint8_t *nonce;
  uint32_t temp;
  size_t plainlength;
  int cipherlength;
  bool ok;

  if (NULL == cookie_ctx)
    return false;	/* We aren't initialized yet. */

  /* We may get garbage from the net */
  if (cookielen > NTS_MAX_COOKIELEN)
    return false;

  finger = cookie;
  if (0 == memcmp(finger, &I, sizeof(I))) {
    key = K;
    nts_cookie_decode++;
  } else if (0 == memcmp(finger, &I2, sizeof(I2))) {
    key = K2;
    nts_cookie_decode_old++;
  } else {
    nts_cookie_decode_too_old++;
    return false;
  }
  finger += sizeof(I);
  nonce = finger;
  finger += NONCE_LENGTH;

  // require(AD_LENGTH==finger-cookie);

  cipherlength = cookielen - AD_LENGTH;
  plainlength = NTS_MAX_COOKIELEN;

  ok = AES_SIV_Decrypt(cookie_ctx,
           plaintext, &plainlength,
           key, K_length,
           nonce, NONCE_LENGTH,
           finger, cipherlength,
           cookie, AD_LENGTH);
  if (!ok) {
    nts_cookie_decode_error++;
    return false;
  }

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
