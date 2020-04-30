/*
 * nts_cookie.c - Network Time Security (NTS) cookie processing
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 * This follows section 6, Suggested Format for NTS Cookies
 * It uses AEAD_AES_SIV_CMAC_256/384/512 from RFC 5297
 * The selection is done by the key length.
 *
 * We use the implementation in libaes_siv by Daniel Franke (Akamai)
 * There is a similar implementation in OpenSSL (or soon will be)
 * It has a slightly different API.  See libaes_siv/README.md
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
 *
 * If the file gets corrupted, blow it away and reboot.  It will get
 * recreated, we will start using new cookies, packets from clients
 * with old cookies will get dropped, and eventually clients will
 * run out of cookies and use NTS-KE to get new ones.
 *
 * It would be possible to run without a cookie file.  Nobody would
 * notice until the server was restarted.  Then there would be a flurry
 * of NTS-KE requests until all clients obtained new/working cookies.
 */

/* Encryption within cookies uses AEAD_AES_SIV_CMAC_nnn.  That's the
 * same family of algorithms as NTS uses on the wire.
 * The nnn is selected by the key length.
 *   32 => 256
 *   48 => 384
 *   64 => 512
 */

/* NTS_MAX_COOKIELEN:
 *   4 I
 *  16 N
 *  16 CMAC
 *   4 AEAD
 *  64 C2S    NTS_MAX_KEYLEN
 *  64 S2C    NTS_MAX_KEYLEN
 * ------
 * 168
 *
 * That's the max length for our cookies.
 * Round up a bit in case another implementation uses more.
 * #define is in include/nts.h
 */

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

/* The mutex protects cookie_ctx
 * The NTS-KE servers can make cookies
 *   while the main NTP server thread is unpacking and making cookies.
 * If this becomes a bottleneck, we could use a cookie_ctx per thread. */
pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;
AES_SIV_CTX* cookie_ctx;

/* Statistics for ntpq */
uint64_t nts_cookie_make = 0;
uint64_t nts_cookie_decode = 0;
uint64_t nts_cookie_decode_old = 0;
uint64_t nts_cookie_decode_too_old = 0;
uint64_t nts_cookie_decode_error = 0;

void nts_lock_cookielock(void);
void nts_unlock_cookielock(void);

// FIXME  AEAD_LENGTH
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
#define SecondsPerDay (24*60*60)
// Set this shorter for debugging
//  keys will timeout, packets will get dropped
//  after 8 lost packets, it should go through the NTS-KE dance again
// #define SecondsPerDay 3600
void nts_cookie_timer(void) {
	time_t now;
	if (0 == K_time) {
		return;
	}
	now = time(NULL);
	if (SecondsPerDay > (now-K_time)) {
		return;
	}
	nts_make_cookie_key();
	/* In case we were off for many days. */
	while (SecondsPerDay < (now-K_time)) {
		K_time += SecondsPerDay;
	}
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
	if (NULL == in) {
		char errbuf[100];
		if (ENOENT == errno)
			return false;		/* File doesn't exist */
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't read old cookie file: %s=>%s",
			cookie_filename, errbuf);
		exit(1);
	}
	if (1 != fscanf(in, "T: %lu\n", &templ)) {
		goto bail;
	}
	K_time = templ;
	if (1 != fscanf(in, "L: %d\n", &K_length)) {
		goto bail;
	}
	if ( !((32 == K_length) || (48 == K_length) || (64 == K_length))) {
		goto bail;
	}
	if (1 != fscanf(in, "I: %u\n", &I)) {
		goto bail;
	}
	if (0 != fscanf(in, "K: ")) {
		goto bail;
	}
	for (int i=0; i< K_length; i++) {
		unsigned int temp;
		if (1 != fscanf(in, "%02x", &temp)) {
			goto bail;
		}
		K[i] = temp;
	}
	if (0 != fscanf(in, "\n")) {
		goto bail;
	}
	if (1 != fscanf(in, "I: %u\n", &I2)) {
		goto bail;
	}
	if (0 != fscanf(in, "K: ")) {
		goto bail;
	}
	for (int i=0; i< K_length; i++) {
		unsigned int temp;
		if (1 != fscanf(in, "%02x", &temp)) {
			goto bail;
		}
		K2[i] = temp;
	}
	if (0 != fscanf(in, "\n")) {
		goto bail;
	}
	fclose(in);
	return true;

  bail:
	msyslog(LOG_ERR, "ERR: Error parsing cookie keys file");
	fclose(in);
	return false;
}

/* The draft describes a ratchet mode to make new keys
 * That's one way to implement a KE server for a cluster of NTP servers.
 * The KE server and the NTP servers stay in sync without communication
 * after a one-time copy of the cookie file from NTP server to KE server.
 */
void nts_make_cookie_key(void) {
	memcpy(&K2, &K, sizeof(K2));	/* Push current cookie to old */
	I2 = I;
	ntp_RAND_priv_bytes(K, sizeof(K));
	ntp_RAND_bytes((uint8_t *)&I, sizeof(I));
	return;
}

bool nts_write_cookie_keys(void) {
	const char *cookie_filename = NTS_COOKIE_KEY_FILE;
	int fd;
	FILE *out;
	char errbuf[100];
	if (NULL != ntsconfig.KI)
		cookie_filename = ntsconfig.KI;
	fd = open(cookie_filename, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
	if (-1 == fd) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "ERR: can't open %s: %s", cookie_filename, errbuf);
		return false;
	}
	out = fdopen(fd, "w");
	if (NULL == out) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "ERR: can't fdopen %s: %s", cookie_filename, errbuf);
		close(fd);
		return false;
	}
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
	uint8_t * finger;
	uint32_t temp;	/* keep 4 byte alignment */
	size_t left;

	if (NULL == cookie_ctx)
		return 0;		/* We aren't initialized yet. */

	nts_cookie_make++;

	INSIST(keylen <= NTS_MAX_KEYLEN);

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
	ntp_RAND_bytes(finger, NONCE_LENGTH);
	finger += NONCE_LENGTH;

	used = finger-cookie;
	left = NTS_MAX_COOKIELEN-used;

	nts_lock_cookielock();

	ok = AES_SIV_Encrypt(cookie_ctx,
			     finger, &left,   /* left: in: max out length, out: length used */
			     K, K_length,
			     nonce, NONCE_LENGTH,
			     plaintext, plainlength,
			     cookie, AD_LENGTH);

	nts_unlock_cookielock();

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
	INSIST(used <= NTS_MAX_COOKIELEN);

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

	nts_lock_cookielock();

	ok = AES_SIV_Decrypt(cookie_ctx,
			     plaintext, &plainlength,
			     key, K_length,
			     nonce, NONCE_LENGTH,
			     finger, cipherlength,
			     cookie, AD_LENGTH);

	nts_unlock_cookielock();

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

void nts_lock_cookielock(void) {
	int err = pthread_mutex_lock(&cookie_lock);
	if (0 != err) {
		msyslog(LOG_ERR, "ERR: Can't lock cookie_lock: %d", err);
		exit(2);
	}
}

void nts_unlock_cookielock(void) {
	int err = pthread_mutex_unlock(&cookie_lock);
	if (0 != err) {
		msyslog(LOG_ERR, "ERR: Can't unlock cookie_lock: %d", err);
		exit(2);
	}
}

/* end */
