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
time_t K_time = 0;	/* time K was created, 0 for none */
struct NTS_Key nts_keys[NTS_nKEYS];
int nts_nKeys = 0;

/* The mutex protects cookie_ctx
 * The NTS-KE servers can make cookies
 *   while the main NTP server thread is unpacking and making cookies.
 * If this becomes a bottleneck, we could use a cookie_ctx per thread. */
pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;
AES_SIV_CTX* cookie_ctx;

/* Statistics for ntpq */
uint64_t nts_cookie_make = 0;
uint64_t nts_cookie_decode = 0;
uint64_t nts_cookie_decode_old = 0;	/* one day old */
uint64_t nts_cookie_decode_old2 = 0;	/* two days old */
uint64_t nts_cookie_decode_older = 0;	/* more than 2 days old */
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
		/* Can't read cookie file.  Make one */
		nts_make_cookie_key();
		K_time = time(NULL);
		nts_write_cookie_keys();
	}
	return OK;
}

/* Rotate key -- 24 hours after last rotation
 * That allows a cluster NTS-KE server to keep in sync
 * if we use ratchet rather than random.
 */
#define SecondsPerDay (24*60*60)
// Set this shorter for debugging
//  keys will timeout, packets will get dropped
//  after 8 lost packets, it should go through the NTS-KE dance again
// Just uncommenting the next line will generate a warning reminder.
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
		msyslog(LOG_INFO, "NTS: Wrote new cookie file, %d keys.", nts_nKeys);
	else
		msyslog(LOG_INFO, "NTS: Trouble writing new cookie file.");
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
	nts_nKeys = 0;
	for (int i=0; i<NTS_nKEYS; i++) {
	  struct NTS_Key *key = &nts_keys[i];
	  if (1 != fscanf(in, "I: %u\n", &key->I)) {
		if (0 < nts_nKeys) break;
		goto bail;
	  }
	  if (0 != fscanf(in, "K: ")) {
		goto bail;
	  }
	  for (int j=0; j< K_length; j++) {
		unsigned int temp;
		if (1 != fscanf(in, "%02x", &temp)) {
			goto bail;
		}
		key->K[j] = temp;
	  }
	  if (0 != fscanf(in, "\n")) {
		goto bail;
	  }
	  nts_nKeys = i+1;
	}
	fclose(in);
	msyslog(LOG_INFO, "NTS: Read cookie file, %d keys.", nts_nKeys);
	return true;

  bail:
	msyslog(LOG_ERR, "ERR: Error parsing cookie keys file");
	fclose(in);
	return false;
}

/* RFC 8915 describes a ratchet mode to make new keys
 * That's one way to implement a KE server for a cluster of NTP servers.
 * The KE server and the NTP servers stay in sync without communication
 * after a one-time copy of the cookie file from NTP server to KE server.
 * An alternative is to elect one system to generate new keys and
 * they copy the key file to other systems and have them load it.
 */
void nts_make_cookie_key(void) {
	if (nts_nKeys < NTS_nKEYS) nts_nKeys++;
	for (int i=nts_nKeys-1; i>0; i--) {
	  nts_keys[i] = nts_keys[i-1];
	}
	ntp_RAND_priv_bytes(nts_keys[0].K, K_length);
	ntp_RAND_bytes((uint8_t *)&nts_keys[0].I, sizeof(nts_keys[0].I));
	return;
}

bool nts_write_cookie_keys(void) {
	const char *cookiefile = NTS_COOKIE_KEY_FILE;
	char tempfile[PATH_MAX];
	int fd;
	FILE *out;
	char errbuf[100];
	if (NULL != ntsconfig.KI)
		cookiefile = ntsconfig.KI;
	strlcpy(tempfile, cookiefile, sizeof(tempfile));
	strlcat(tempfile, "-tmp", sizeof(tempfile));
	fd = open(tempfile, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
	if (-1 == fd) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "ERR: can't open %s: %s", tempfile, errbuf);
		return false;
	}
	out = fdopen(fd, "w");
	if (NULL == out) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "ERR: can't fdopen %s: %s", tempfile, errbuf);
		close(fd);
		return false;
	}

	fprintf(out, "T: %lu\n", (unsigned long)K_time);
	fprintf(out, "L: %d\n", K_length);
	for (int i=0; i<nts_nKeys; i++) {
	  struct NTS_Key *key = &nts_keys[i];
	  fprintf(out, "I: %u\n", key->I);
	  fprintf(out, "K: ");
	    for (int j=0; j< K_length; j++) fprintf(out, "%02x", key->K[j]);
	  fprintf(out, "\n");
	  key++;
	}
	fclose(out);
        if (rename(tempfile, cookiefile)) {
	    ntp_strerror_r(errno, errbuf, sizeof(errbuf));
            msyslog(LOG_WARNING,
                    "LOG: Unable to rename temp cookie file %s to %s, %s",
                    tempfile, cookiefile, errbuf);
	    return false;
	}
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

	memcpy(finger, &nts_keys[0].I, sizeof(nts_keys[0].I));
	finger += sizeof(nts_keys[0].I);

	nonce = finger;
	ntp_RAND_bytes(finger, NONCE_LENGTH);
	finger += NONCE_LENGTH;

	used = finger-cookie;
	left = NTS_MAX_COOKIELEN-used;

	nts_lock_cookielock();

	ok = AES_SIV_Encrypt(cookie_ctx,
			     finger, &left,   /* left: in: max out length, out: length used */
			     nts_keys[0].K, K_length,
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
	uint8_t *nonce;
	uint32_t temp;
	size_t plainlength;
	int cipherlength;
	bool ok;
	struct NTS_Key *key;
	int i;

	if (NULL == cookie_ctx)
		return false;	/* We aren't initialized yet. */

	/* We may get garbage from the net */
	if (cookielen > NTS_MAX_COOKIELEN)
		return false;

	finger = cookie;
	key = NULL;		/* squash uninitialized warning */
	for (i=0; i<nts_nKeys; i++) {
	  key = &nts_keys[i];
	  if (0 == memcmp(finger, &key->I, sizeof(key->I))) {
		break;
	  }
	}
	if (0 == i) {
		nts_cookie_decode++;
	} else if (nts_nKeys == i) {
		nts_cookie_decode_too_old++;
		return false;
	} else if (1 == i) {
		nts_cookie_decode_old++;
	} else if (2 == i) {
		nts_cookie_decode_old2++;
	} else {
		nts_cookie_decode_older++;
	}
#if 0
	if (1<i) {
	  /* Hack for debugging */
	  /* Beware: DoS possibility on a public server */
	  msyslog(LOG_INFO, "NTS: Old cookie: %d days.", i);
	}
#endif

	finger += sizeof(key->I);
	nonce = finger;
	finger += NONCE_LENGTH;

	// require(AD_LENGTH==finger-cookie);

	cipherlength = cookielen - AD_LENGTH;
	plainlength = NTS_MAX_COOKIELEN;

	nts_lock_cookielock();

	ok = AES_SIV_Decrypt(cookie_ctx,
			     plaintext, &plainlength,
			     key->K, K_length,
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
