/* Copyright Akamai Technologies, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 * Demo program for libaes-siv
 */

#define _POSIX_C_SOURCE 200112L
#define _ISOC99_SOURCE 1

#include "aes_siv.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Load a file into memory.
 * @param[in] filename pathname of the file to load.
 * @param[out] the buf pointer will be set to a new malloc buffer which contains the file data.
 * @param[out] the len pointer will be set to the size of buf in bytes, which is the same as the file size in bytes.
 * @return 0 upon success, the file could be read and the buf,len output parameters have been set.
 * @return -1 on error; errno will be set.
 */
static int load_file(const char *filename, unsigned char **buf, size_t *len)
{
  FILE *f = NULL;
  unsigned char *buf_ = NULL;
  size_t len_;
  long ftell_len;
  int saved_errno;

  /* Get file size */
  f = fopen(filename, "rb");
  if (f == NULL) {
    goto fail;
  }

  if (fseek(f, 0, SEEK_END) < 0) {
    goto fail;
  }

  ftell_len = ftell(f);
  if(ftell_len < 0) {
    goto fail;
  }
  len_ = (size_t)ftell_len;

  if(fseek(f, 0, SEEK_SET) < 0) {
    goto fail;
  }

  /* allocate memory and read */
  buf_ = (unsigned char*) malloc(len_);
  if (buf_ == NULL) {
    goto fail;
  }
  if (fread(buf_, 1, len_, f) != len_) {
    assert(errno);
    goto fail;
  }

  fclose(f);
  f = NULL;
  *buf = buf_;
  *len = len_;
  return 0;

  fail:
  saved_errno = errno;
  if(f != NULL) {
    fclose(f);
  }
  free(buf_);
  errno = saved_errno;
  return -1;
}

void help(void)
{
  fprintf(stderr, "Usage: aes_siv_test [-d] <key file> <ad file> [nonce file]\n");
  fprintf(stderr, "This program encrypts or decrypts STDIN to STDOUT using the AES-SIV algorithm.\n");
  fprintf(stderr, "-d           Decrypt STDIN, by default STDIN is encrypted\n");
  fprintf(stderr, "<key file>   Filename which is read for key data, must have a size of 32, 48, 64 bytes.\n");
  fprintf(stderr, "<ad file>    Filename which is used for associate data. Can have any size.\n");
  fprintf(stderr, "[nonce file] Optional filename which is used for nonce data. Can have any size.\n");
}

int main(int argc, char const* argv[])
{
  int arg = 1;
  int decrypt_mode = 0;
  const char *key_file = NULL;
  const char *nonce_file = NULL;
  const char *ad_file = NULL;
  unsigned char *key = NULL;
  size_t key_len = 0;
  unsigned char *nonce = NULL;
  size_t nonce_len = 0;
  unsigned char *ad = NULL;
  size_t ad_len = 0;
  unsigned char *out = NULL;
  size_t out_len = 0;
  size_t plaintext_allocated = 1024;
  unsigned char *plaintext = malloc(plaintext_allocated);
  size_t plaintext_len = 0;
  AES_SIV_CTX *ctx = NULL;

  if(plaintext == NULL) {
    perror("malloc");
    goto fail;
  }

  /* Parse command line */
  arg = 1;
  if (arg < argc && strcmp(argv[arg], "-d") == 0)
  {
    decrypt_mode = 1;
    ++arg;
  }

  if(arg >= argc)
  {
    fprintf(stderr, "Missing key filename\n\n");
    help();
    goto fail;
  }
  key_file = argv[arg++];

  if(arg >= argc)
  {
    fprintf(stderr, "Missing associate data filename\n\n");
    help();
    goto fail;
  }
  ad_file = argv[arg++];

  if(arg < argc)
  {
    nonce_file = argv[arg++];
  }

  if(arg < argc)
  {
    fprintf(stderr, "Unknown command line argument: %s\n\n", argv[arg]);
    help();
    goto fail;
  }

  /* Load files */
  if(load_file(key_file, &key, &key_len) < 0)
  {
    fprintf(stderr, "Could not load key file %s : %s\n", key_file, strerror(errno));
    goto fail;
  }

  assert(key != NULL);
  if(! (key_len == 32 ||
        key_len == 48 ||
        key_len == 64))
  {
    fprintf(stderr, "Invalid key length %zu bytes, must be one of 32, 48, or 64\n", key_len);
    goto fail;
  }

  if(load_file(ad_file, &ad, &ad_len) < 0)
  {
    fprintf(stderr, "Could not load associated data file %s : %s\n", ad_file, strerror(errno));
    goto fail;
  }
  assert(ad);
  assert(ad_len > 0);
  if(load_file(nonce_file, &nonce, &nonce_len) < 0)
  {
    fprintf(stderr, "could not load nonce file %s : %s\n", nonce_file, strerror(errno));
    goto fail;
  }

  /* Read all of STDIN */
  while(!feof(stdin))
  {
    unsigned char buf[1024];
    unsigned char *plaintext_new;
    size_t r = fread(buf, 1, sizeof(buf), stdin);
    if(r > 0)
    {
      if(plaintext_len + r > plaintext_allocated)
      {
        plaintext_allocated *= 2;
        plaintext_new = realloc(plaintext, plaintext_allocated);
        if(plaintext_new == NULL)
        {
          perror("realloc");
          goto fail;
        }
        plaintext = plaintext_new;
      }
      assert(plaintext_len + r <= plaintext_allocated);
      memcpy(plaintext + plaintext_len, buf, r);
      plaintext_len += r;
    } else if(ferror(stdin)) {
      perror("fread");
      goto fail;
    }
  }

  /* allocate output buffer */
  out_len = plaintext_len + 16;
  out = (unsigned char*) malloc(out_len);
  if (out == NULL)
  {
    perror("malloc");
    goto fail;
  }

  /* do AES-SIV */
  ctx = AES_SIV_CTX_new();
  if (ctx == NULL)
  {
    perror("AES_SIV_CTX_new");
    goto fail;
  }
  if (decrypt_mode)
  {
    if (AES_SIV_Decrypt(ctx, out, &out_len, key, key_len, nonce, nonce_len, plaintext, plaintext_len, ad, ad_len) != 1)
    {
      fprintf(stderr, "Could not decrypt AES-SIV\n");
      goto fail;
    }
  }
  else
  {
    if (AES_SIV_Encrypt(ctx, out, &out_len, key, key_len, nonce, nonce_len, plaintext, plaintext_len, ad, ad_len) != 1)
    {
      fprintf(stderr, "Could not encrypt AES-SIV\n");
      goto fail;
    }
  }
  AES_SIV_CTX_free(ctx);

  /* write to stdout */
  if(fwrite(out, 1, out_len, stdout) < out_len) {
    perror("fwrite");
    goto fail;
  }

  free(plaintext);
  free(key);
  free(ad);
  free(nonce);
  free(out);
  return EXIT_SUCCESS;

 fail:
  free(plaintext);
  free(key);
  free(ad);
  free(nonce);
  free(out);
  return EXIT_FAILURE;
}
