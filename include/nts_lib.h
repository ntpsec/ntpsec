#ifndef GUARD_NTS_LIB_H
#define GUARD_NTS_LIB_H

#include <linux/types.h>
#include <openssl/evp.h>
#include <stdbool.h>

typedef struct {
  void *key_c2s;
  void *key_s2c;
  void *nonce;
  char *plaintext;
  void *ciphertext;
  char *cookie;
  void *iv;
  void *bit; // pointer for mempcpy, upf
  __u8 addr_peer[16];

  int tally;

  __u16 size_iv;
  __u16 size_key_csc;
  __u16 size_nonce;
  __u16 size_ciphertext;
  __u16 size_plaintext;
  __u16 nts_algo;
  __u16 cookie_key_id;
  __u16 recipe;
  __u16 countdown;
  __u16 now; // network order word
} cookie_bits;

typedef struct {
  __u8 *next;
  __u8 *record;
  __u8 *body;
  __u8 *bit; // pointer for mempcpy, upf

  __u16 body_length;
  __u16 record_type;
  __u16 record_length;
  __u16 now; // network order word
  bool critical;
} record_bits;

enum record_type {
  end_of_message = 0,
  next_protocol_negotiation = 1,
  error = 2,
  warning = 3,
  algorithm_negotiation = 4,
  new_cookie = 5,
  server_negotiation = 6,
  port_negotiation = 7
};

enum errors_type {
  unrecognized_critical_section = 0,
  bad_request = 1
};

enum aead_ciphers {
  AEAD_AES_128_GCM = 1,
  AEAD_AES_256_GCM = 2,
  AEAD_AES_128_CCM = 3,
  AEAD_AES_256_CCM = 4,

  AEAD_AES_128_GCM_8 = 5,
  AEAD_AES_256_GCM_8 = 6,
  AEAD_AES_128_GCM_12 = 7,
  AEAD_AES_256_GCM_12 = 8,

  AEAD_AES_128_CCM_SHORT = 9,
  AEAD_AES_256_CCM_SHORT = 10,
  AEAD_AES_128_CCM_SHORT_8 = 11,
  AEAD_AES_256_CCM_SHORT_8 = 12,
  AEAD_AES_128_CCM_SHORT_12 = 13,
  AEAD_AES_256_CCM_SHORT_12 = 14,

  AEAD_AES_SIV_CMAC_256 = 15,
  AEAD_AES_SIV_CMAC_384 = 16,
  AEAD_AES_SIV_CMAC_512 = 17,

  AEAD_AES_128_CCM_8 = 18,
  AEAD_AES_256_CCM_8 = 19,

  AEAD_AES_128_OCB_TAGLEN128 = 20,
  AEAD_AES_128_OCB_TAGLEN96 = 21,
  AEAD_AES_128_OCB_TAGLEN64 = 22,
  AEAD_AES_192_OCB_TAGLEN128 = 23,
  AEAD_AES_192_OCB_TAGLEN96 = 24,
  AEAD_AES_192_OCB_TAGLEN64 = 25,
  AEAD_AES_256_OCB_TAGLEN128 = 26,
  AEAD_AES_256_OCB_TAGLEN96 = 27,
  AEAD_AES_256_OCB_TAGLEN64 = 28,

  AEAD_CHACHA20_POLY1305 = 29
};

extern __u8 *upf(void *src, void *dest, size_t n);

extern int nts_record_form(record_bits *in);
extern int nts_record_parse(record_bits *in);

extern int nts_cookie_prep(cookie_bits *input);
extern int nts_cookie_clean(cookie_bits *a);
extern int nts_cookie_plaintext_parse(cookie_bits *out);
extern int nts_cookie_plaintext_form(cookie_bits *in);

#endif // GUARD_NTS_LIB_H