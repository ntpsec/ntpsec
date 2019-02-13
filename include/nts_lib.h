#ifndef GUARD_NTS_LIB_H
#define GUARD_NTS_LIB_H

#include <stdint.h>
#include <openssl/evp.h>
#include <stdbool.h>

#include "nts.h"

typedef struct {
  void *key_c2s;
  void *key_s2c;
  void *nonce;
  char *plaintext;
  void *ciphertext;
  char *cookie;
  void *iv;
  void *bit; // pointer for mempcpy, upf
  uint8_t addr_peer[16];

  int tally;

  uint16_t size_iv;
  uint16_t size_key_csc;
  uint16_t size_nonce;
  uint16_t size_ciphertext;
  uint16_t size_plaintext;
  uint16_t nts_algo;
  uint16_t cookie_key_id;
  uint16_t recipe;
  uint16_t countdown;
  uint16_t now; // network order word
} cookie_bits;

typedef struct {
  uint8_t *next;
  uint8_t *record;
  uint8_t *body;
  uint8_t *bit; // pointer for mempcpy, upf

  uint16_t body_length;
  uint16_t record_type;
  uint16_t record_length;
  uint16_t now; // network order word
  bool critical;
} record_bits;

extern uint8_t *upf(void *src, void *dest, size_t n);

extern int nts_record_form(record_bits *in);
extern int nts_record_parse(record_bits *in);

extern int nts_cookie_prep(cookie_bits *input);
extern int nts_cookie_clean(cookie_bits *a);
extern int nts_cookie_plaintext_parse(cookie_bits *out);
extern int nts_cookie_plaintext_form(cookie_bits *in);

#endif // GUARD_NTS_LIB_H
