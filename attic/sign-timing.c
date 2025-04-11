/*
 * Copyright the NTPsec project contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/* Hack to time signing calculations.
  Build with:
    cc -g -Wall -lcrypto -lssl -o sign-timing sign-timing.c

  This uses strncpy because that is more portable.
    (And our usage is trivial.)
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/objects.h>
#include <openssl/ssl.h>

#define UNUSED_ARG(arg)         ((void)(arg))

#if OPENSSL_VERSION_NUMBER > 0x20000000L


int NUM = 10000;

#define SIGLEN 4096
unsigned char sig[SIGLEN];
#define TBSLEN 10000
unsigned char tbs[TBSLEN];

EVP_MD_CTX *ctx;

BIO *bio_out;


static void ssl_init(void) {

	ctx = EVP_MD_CTX_new();
        if (NULL == ctx) {
                printf("EVP_MD_CTX_new() failed.\n");
                exit(1);
        }

	bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
}

static void ShowError(void) {
        char buff[256];
        int err = ERR_get_error();
        SSL_load_error_strings();        /* Needed on NetBSD */
        while (0 != err) {
                ERR_error_string_n(err, buff, sizeof(buff));
                printf("  %s\n", buff);
                err = ERR_get_error();
        }

}

/* from man EVP_PKEY-DSA, openssl-genpkey: 2048 256 */
static EVP_PKEY* key_gen(unsigned int pbits, unsigned int qbits) {
  int gindex = 1;
  OSSL_PARAM params[6];
  EVP_PKEY *param_key = NULL;
  EVP_PKEY *key = NULL;
  EVP_PKEY_CTX *pctx = NULL;
  EVP_PKEY_CTX *gctx = NULL;
  int err;
  char stupid_api[20];

  pctx = EVP_PKEY_CTX_new_from_name(NULL, "ED25519", NULL);
  if (NULL == ctx) {
    printf("EVP_PKEY_CTX_new_from_name() failed.\n");
    ShowError();
    exit(1);
  }
  err = EVP_PKEY_paramgen_init(pctx);
  if (1 != err) {
    printf("EVP_PKEY_paramgen_init() failed.\n");
    ShowError();
    exit(1);
  }
  params[0] = OSSL_PARAM_construct_uint("pbits", &pbits);
  params[1] = OSSL_PARAM_construct_uint("qbits", &qbits);
  params[2] = OSSL_PARAM_construct_int("gindex", &gindex);
  strncpy(stupid_api, "SHA256", sizeof(stupid_api));
  params[3] = OSSL_PARAM_construct_utf8_string("digest", stupid_api, 0);
  params[4] = OSSL_PARAM_construct_end();
  err = EVP_PKEY_CTX_set_params(pctx, params);
  if (1 != err ) {
    printf("EVP_PKEY_CTX_set_params() failed.\n");
    ShowError();
    exit(1);
  }
  err = EVP_PKEY_generate(pctx, &param_key);
  if (1 != err ) {
    printf("EVP_PKEY_generate()#1 failed.\n");
    ShowError();
    exit(1);
  }
  EVP_PKEY_CTX_free(pctx);

  gctx = EVP_PKEY_CTX_new_from_pkey(NULL, param_key, NULL);
  EVP_PKEY_keygen_init(gctx);
  err = EVP_PKEY_generate(gctx, &key);
  if (1 != err ) {
    printf("EVP_PKEY_generate()#2 failed.\n");
    ShowError();
    exit(1);
  }
  EVP_PKEY_CTX_free(gctx);

  if (0)
    EVP_PKEY_print_params(bio_out, key, 0, NULL);

  return key;
}


/* man EVP_KEYMGMT-ED25519 EVP_SIGNATURE-ED25519 */
static void DoSign(
  unsigned int pbits,
  unsigned int qbits,
  size_t tbslen        /* data length */
) {
  EVP_PKEY *key = key_gen(pbits, qbits);
  struct timespec start, stop;
  struct timespec t1, t2, t3;
  uint64_t init = 0, data = 0;
  double fast;
  size_t siglen = SIGLEN;
  int err;
  char stupid_api1[20];
  char stupid_api2[50];

  const char context[] = "RoughTime v1 delegation signature-";
  OSSL_PARAM params[3];

  strncpy(stupid_api1, "Ed25519ctx", sizeof(stupid_api1));
  params[0] = OSSL_PARAM_construct_utf8_string("instance", stupid_api1, 0);
  strncpy(stupid_api2, context, sizeof(stupid_api2));
  params[1] = OSSL_PARAM_construct_octet_string("context-string", stupid_api2, sizeof(context));
  params[2] = OSSL_PARAM_construct_end();
  err = EVP_DigestSignInit_ex(ctx, NULL, NULL, NULL, NULL, key, params);
  if (1 != err ) {
    printf("EVP_DigestSignInit_ex()1 failed.\n");
    ShowError();
    exit(1);
  }


  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < NUM; i++) {
    clock_gettime(CLOCK_MONOTONIC, &t1);
    err = EVP_DigestSignInit_ex(ctx, NULL, NULL, NULL, NULL, NULL, NULL);
    if (1 != err ) {
      printf("EVP_DigestSignInit_ex()2 failed.\n");
      ShowError();
      exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    err = EVP_DigestSign(ctx, sig, &siglen, tbs, tbslen);
    if (1 != err ) {
      printf("EVP_DigestSign() failed, %d.\n", i);
      ShowError();
      exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC, &t3);
    init += (t2.tv_sec-t1.tv_sec)*1E9 + (t2.tv_nsec-t1.tv_nsec);
    data += (t3.tv_sec-t2.tv_sec)*1E9 + (t3.tv_nsec-t2.tv_nsec);
  }
  clock_gettime(CLOCK_MONOTONIC, &stop);

  fast = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
  printf("%7u %5u %6llu %5llu %8.0f  %6.3f %6llu %5llu\n",
    pbits, qbits, (unsigned long long)siglen, (unsigned long long)tbslen,
    fast/NUM, fast/1E9, (unsigned long long)init/NUM, (unsigned long long)data/NUM);

  EVP_PKEY_free(key);
}

int main(int argc, char *argv[])
{
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	setlinebuf(stdout);

	ssl_init();
	RAND_bytes((unsigned char *)&tbs, TBSLEN);

	printf("# %s\n", OPENSSL_VERSION_TEXT);
	printf("# pbits qbits siglen  data  ns/sign \n");

	DoSign(2048, 256, 32);
	DoSign(2048, 256, 32);
	DoSign(2048, 256, 128);
	DoSign(2048, 256, 1024);
	DoSign(1024, 256, 32);
	DoSign(1024, 256, 128);
	DoSign(1024, 256, 1024);
	DoSign(4096, 256, 32);
	DoSign(4096, 256, 128);
	DoSign(4096, 256, 1024);

	DoSign(2048, 512, 32);
	DoSign(2048, 512, 128);
	DoSign(2048, 512, 1024);

	return 0;
}

#else /* #if OPENSSL_VERSION_NUMBER */
int main(int argc, char *argv[])
{
	UNUSED_ARG(argc);
	UNUSED_ARG(argv);
	return(0);
}
#endif

