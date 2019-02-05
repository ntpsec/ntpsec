#include "config.h"
#include "nts_lib.h"
#include "ntp_types.h"
#include "ntpd.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

// a binary unpack function it should sort of reverse mempcpy
__u8 *upf(void *src, void *dest, size_t n) {
  (void)memcpy(dest, src, n);
  return (__u8 *)src + n;
}

/* parse an NTS record to retrieve the body, body_length and record_type
 *
 * Assumes the record field is set to an appropriate value.
 *
 * sets the record_length, record_type and body fields
 *
 * returns 0 on success
 */
int nts_record_parse(record_bits *in) {
  UNUSED_ARG(in);
  return 0;
}

/* form an NTS record
 *
 * Assumes the record_length, record_type and body fields are set to appropriate
 * values
 *
 * sets the record_length and record fields.
 *
 * returns 0 on success
 * returns 1 on memory allcation failure;
 */
int nts_record_form(record_bits *in) {
  UNUSED_ARG(in);
  return 0;
}

// Allocate & initialize structure & fields for NTS cookie generation/parsing
int nts_cookie_prep(cookie_bits *input) {
  UNUSED_ARG(input);
  return 0;
}

// Free (most) storage used by NTS cookie generation/parsing routines
int nts_cookie_clean(cookie_bits *a) {
  UNUSED_ARG(a);
  return 0;
}

/* Parse the plaintext to retrieve the AEAD algorithm to use when processing,
 * the c2s key used to decipher the AEAF extension from the client and the
 * s2c key used to encipher the AEAF extension returned to the client.
 *
 * Assumes the plaintext and recipe fields are set.
 *
 * Sets the key_c2s, key_s2c and size_key_csc fields
 * optionally sets fields addr_peer and countdown
 *
 * returns number of bytes remaining in the plaintext (should be 0)
 */
int nts_cookie_plaintext_parse(cookie_bits *out) {
  UNUSED_ARG(out);
  return 0;
}

/* Form the cookie plaintext from the AEAD algorithm number, the c2s key and
 * the sc key extracted during the NTS-KE session.
 *
 * Assumes the key_c2s, key_s2c, recipe and size_key_csc fields are set
 * optionally uses fields addr_peer and countdown
 *
 * sets the plaintext and size_plaintext fields.
 *
 * returns 0 on success
 */
int nts_cookie_plaintext_form(cookie_bits *in) {
  UNUSED_ARG(in);
  return 0;
}
