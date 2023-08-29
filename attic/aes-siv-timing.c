/* Last modified on Sat Aug 28 14:30:11 PDT 1999 by murray */

/* 
 * Hack to time aead routines from libaes_siv
 *
 * RFC5297, October 2008
 * Synthetic Initialization Vector (SIV) Authenticated Encryption
 *           Using the Advanced Encryption Standard (AES)
 *
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/rand.h>
#include "aes_siv.h"
#include "ntp_fp.h"
#include "nts.h"

#define UNUSED_ARG(arg)         ((void)(arg))
#define INSIST(x)       assert(x)

int SAMPLESIZE = 1000000;

AES_SIV_CTX* cookie_ctx;
uint32_t key_I = 123;  /* timestamp or whatever used to \
                        * indicate key used for this cookie */
uint8_t key_K[NTS_MAX_KEYLEN];
int     key_K_length;

pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;


/* sizeof(key_I) + sizeof(NONCE) */
#define AD_LENGTH 20

static void ssl_init(void)
{
  cookie_ctx = AES_SIV_CTX_new();
  if (NULL == cookie_ctx) {
    printf("NTS: Can't init cookie_ctx\n");
    exit(1);
  }
}

static void nts_lock_cookielock(void) {
        int err = pthread_mutex_lock(&cookie_lock);
        if (0 != err) {      
                printf("ERR: Can't lock cookie_lock: %d\n", err);
                exit(2);
        }                    
}
        
static void nts_unlock_cookielock(void) {
        int err = pthread_mutex_unlock(&cookie_lock);
        if (0 != err) {
                printf("ERR: Can't unlock cookie_lock: %d\n", err);
                exit(2);
        }
}

static void ntp_RAND_bytes(unsigned char *buf, int num) {
        int err;
        err = RAND_bytes(buf, num);
        if (1 != err) {
                printf("ERR: RAND_bytes failed\n");
                exit(1);
        }
}



static void DoLock(void)
{
	struct timespec start, stop;
	double fast;
	int samplesize = SAMPLESIZE;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
          nts_lock_cookielock();
          nts_unlock_cookielock();
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("LocK          %6.0f %7.3f",
	       fast/samplesize,  fast/1E9);
	printf("\n");
}


static void DoNonce(void)
{
        uint8_t nonce[NONCE_LENGTH];
	struct timespec start, stop;
	double fast;
	int samplesize = SAMPLESIZE;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
          ntp_RAND_bytes(nonce, NONCE_LENGTH);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("Nonce     %3d %6.0f %7.3f",
	       NONCE_LENGTH, fast/samplesize,  fast/1E9);
	printf("\n");
}


/* Clone of nts_make_cookie() from ntpd/nts_cookie.c */
/* returns actual length */
int nts_make_cookie(uint8_t *cookie,
  uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen) {
        uint8_t plaintext[NTS_MAX_COOKIELEN];
        uint8_t *nonce;
        int used, plainlength;
        bool ok;
        uint8_t * finger;
        uint32_t temp;  /* keep 4 byte alignment */
        size_t left;

        if (NULL == cookie_ctx)
                return 0;               /* We aren't initialized yet. */

//        nts_cookie_make++;

        INSIST(keylen <= NTS_MAX_KEYLEN);

        finger = plaintext;
        temp = aead;
        memcpy(finger, &temp, sizeof(temp));
        finger += sizeof(temp);
        memcpy(finger, c2s, keylen);
        finger += keylen;
        memcpy(finger, s2c, keylen);
        finger += keylen;
        plainlength = finger-plaintext;

        /* collect associated data */
        finger = cookie;

        memcpy(finger, &key_I, sizeof(key_I));
        finger += sizeof(key_I);

        nonce = finger;
        ntp_RAND_bytes(nonce, NONCE_LENGTH);
        finger += NONCE_LENGTH;

        used = finger-cookie;
        left = NTS_MAX_COOKIELEN-used;

        nts_lock_cookielock();
        ok = AES_SIV_Encrypt(cookie_ctx,
                             finger, &left,   /* left: in: max out length, out: length used */
                             key_K, key_K_length,
                             nonce, NONCE_LENGTH,
                             plaintext, plainlength,
                             cookie, AD_LENGTH);
        nts_unlock_cookielock();

        if (!ok) {
                printf("NTS: nts_make_cookie - Error from AES_SIV_Encrypt\n");
                exit(1);
        }

        used += left;
        INSIST(used <= NTS_MAX_COOKIELEN);

        return used;
}


static void DoMakeCookie(
  const char *name,       /* name of aead */
  int     aead,		  /* algorithm used to make cookie */
  int     keylength       /* length of c2s and s2c */
)
{
	uint8_t cookie[NTS_MAX_COOKIELEN];
	uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
	struct timespec start, stop;
	double fast;
	int cookielength = 0;
	int samplesize = SAMPLESIZE;

	switch (aead) {
	  case AEAD_AES_SIV_CMAC_256:
		key_K_length = AEAD_AES_SIV_CMAC_256_KEYLEN;
		break;
	  case AEAD_AES_SIV_CMAC_384:
		key_K_length = AEAD_AES_SIV_CMAC_384_KEYLEN;
		break;
	  case AEAD_AES_SIV_CMAC_512:
		key_K_length = AEAD_AES_SIV_CMAC_512_KEYLEN;
		break;
	  default:
		printf("Bogus aead\n");
		exit(1);
	} 

        ntp_RAND_bytes(c2s, NTS_MAX_KEYLEN);
        ntp_RAND_bytes(s2c, NTS_MAX_KEYLEN);

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
		cookielength = nts_make_cookie(
			cookie, aead, c2s, s2c, keylength);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %6.0f %7.3f",
	       name, keylength, cookielength, fast/samplesize,  fast/1E9);
	printf("\n");
}


static void DoMakeCrypto(
  const char *name,       /* name of aead */
  int     aead,		  /* algorithm used to make cookie */
  int     keylen          /* length of c2s and s2c */
)
{
	uint8_t cookie[NTS_MAX_COOKIELEN];
	uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
	struct timespec start, stop;
	double fast;
	int cookielength = 0;
	int samplesize = SAMPLESIZE;

        uint8_t plaintext[NTS_MAX_COOKIELEN];
        uint8_t *nonce;
        int used, plainlength;
        int ok = 0;
        uint8_t * finger;
        uint32_t temp;  /* keep 4 byte alignment */
        size_t left;

	switch (aead) {
	  case AEAD_AES_SIV_CMAC_256:
		key_K_length = AEAD_AES_SIV_CMAC_256_KEYLEN;
		break;
	  case AEAD_AES_SIV_CMAC_384:
		key_K_length = AEAD_AES_SIV_CMAC_384_KEYLEN;
		break;
	  case AEAD_AES_SIV_CMAC_512:
		key_K_length = AEAD_AES_SIV_CMAC_512_KEYLEN;
		break;
	  default:
		printf("Bogus aead\n");
		exit(1);
	} 

        ntp_RAND_bytes(c2s, NTS_MAX_KEYLEN);
        ntp_RAND_bytes(s2c, NTS_MAX_KEYLEN);

        finger = plaintext;
        temp = aead;
        memcpy(finger, &temp, sizeof(temp));
        finger += sizeof(temp);
        memcpy(finger, c2s, keylen);
        finger += keylen;
        memcpy(finger, s2c, keylen);
        finger += keylen;
        plainlength = finger-plaintext;

        /* collect associated data */
        finger = cookie;

        memcpy(finger, &key_I, sizeof(key_I));
        finger += sizeof(key_I);

        nonce = finger;
        ntp_RAND_bytes(finger, NONCE_LENGTH);
        finger += NONCE_LENGTH;

        used = finger-cookie;
        left = NTS_MAX_COOKIELEN-used;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < samplesize; i++) {
          ok += AES_SIV_Encrypt(cookie_ctx,
             finger, &left,   /* left: in: max out length, out: length used */
             key_K, key_K_length,
             nonce, NONCE_LENGTH,
             plaintext, plainlength,
             cookie, AD_LENGTH);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

        if (samplesize != ok) {
                printf("NTS: nts_make_cookie - Error from AES_SIV_Encrypt\n");
                exit(1);
        }

        used += left;
        INSIST(used <= NTS_MAX_COOKIELEN);

	cookielength = used;

	fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
	printf("%12s  %2d %4d %6.0f %7.3f",
	       name, keylen, cookielength, fast/samplesize,  fast/1E9);
	printf("\n");
}

int main(int argc, char *argv[])
{
	char *ctimetxt;
	time_t now;
	char buff[256];

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	setlinebuf(stdout);

	ssl_init();

        ntp_RAND_bytes(key_K, NTS_MAX_KEYLEN);

	now = time(NULL);
	ctimetxt = ctime(&now);
	ctimetxt[24] = 0;	/* Hack: smash return */
	gethostname(buff, sizeof(buff));
	printf("# %s on %s\n", ctimetxt, buff);
	printf("# %s\n", OPENSSL_VERSION_TEXT);

	printf("\n");
	printf("#       bytes  ns/op sec/run\n");
	DoLock();
	DoNonce();

	printf("\n");
	printf("# wKL=sizeof(c2s), CL=sizeof(cookie)\n");
	printf("# Make cookie    wKL   CL  ns/op sec/run\n");

// Sample numbers using old cmac.h
// run on a Dell 9020/MT with Intel i7-4790 @ 3.60GHz
	/* 3rd arg is length of c2s and s2c */
	DoMakeCookie("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 32);
	DoMakeCookie("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 48);
	DoMakeCookie("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 64);
// AES_SIV_CMAC_256  32  104   3127   3.127
// AES_SIV_CMAC_256  48  136   3186   3.186
// AES_SIV_CMAC_256  64  168   3220   3.220
	DoMakeCookie("AES_SIV_CMAC_512", AEAD_AES_SIV_CMAC_512, 32);
	DoMakeCookie("AES_SIV_CMAC_512", AEAD_AES_SIV_CMAC_512, 48);
	DoMakeCookie("AES_SIV_CMAC_512", AEAD_AES_SIV_CMAC_512, 64);
// AES_SIV_CMAC_512  32  104   3183   3.183
// AES_SIV_CMAC_512  48  136   3247   3.247
// AES_SIV_CMAC_512  64  168   3303   3.303
	printf("\n");

	printf("# Cookie Crypto  wKL   CL  ns/op sec/run\n");
	DoMakeCrypto("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 32);
	DoMakeCrypto("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 48);
	DoMakeCrypto("AES_SIV_CMAC_256", AEAD_AES_SIV_CMAC_256, 64);
// AES_SIV_CMAC_256  32  104   2066   2.066
// AES_SIV_CMAC_256  48  136   2119   2.119
// AES_SIV_CMAC_256  64  168   2157   2.157

	return 0;
}
