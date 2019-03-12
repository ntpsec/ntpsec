/*
 * nts2.h - NTS (Network Time Security) declarations
 * other half of nts.h which doesn't include openssl/ssh.h
 */
#ifndef GUARD_NTS2_H
#define GUARD_NTS2_H

#include <stdbool.h>
#include <stdint.h>
#include <openssl/ssl.h>


bool nts_load_certificate(SSL_CTX *ctx);
bool nts_load_ciphers(SSL_CTX *ctx);
bool nts_load_versions(SSL_CTX *ctx);

void nts_log_ssl_error(void);

int nts_get_key_length(uint16_t aead);
int nts_translate_version(const char *arg);
uint16_t nts_string_to_aead(const char* text);

bool nts_make_keys(SSL *ssl, uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen);

int nts_make_cookie(uint8_t *cookie,
  uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen);
bool nts_unpack_cookie(uint8_t *cookie, int cookielen,
  uint16_t *aead,
  uint8_t *c2s, uint8_t *s2c, int *keylen);



/* buffer packing/unpacking routines.
 * NB: The length field in NTP extensions includes the header
 * while the length field in NTS-KE data streams does not.
 *
 * These routines do not handle padding.  NTS-KE has no padding.
 * NTP extensions are padded to word (4 byte) boundaries.
 *
 * Note that data on the wire is big endian.
 * buffer is wire format, not host format.
 */


/* working finger into a buffer - updated by append/unpack routines */
struct BufCtl_t {
  uint8_t *next;  /* pointer to next data/space */
  int left;       /* data left or space available */
};
typedef struct BufCtl_t BufCtl;

/* xxx_append_record_foo makes whole record with one foo */
/* append_foo appends foo to existing partial record */
void ke_append_record_null(BufCtl* buf, uint16_t type);
void ke_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data);
void ke_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length);

void ex_append_record_null(BufCtl* buf, uint16_t type);
void ex_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data);
void ex_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length);

void ex_append_header(BufCtl* buf, uint16_t type, uint16_t length);
void append_header(BufCtl* buf, uint16_t type, uint16_t length);
void append_uint16(BufCtl* buf, uint16_t data);
void append_bytes(BufCtl* buf, uint8_t *data, int length);

uint16_t ke_next_record(BufCtl* buf, int *length);
uint16_t ex_next_record(BufCtl* buf, int *length);  /* body length */
uint16_t next_uint16(BufCtl* buf);
uint16_t next_bytes(BufCtl* buf, uint8_t *data, int length);


#endif /* GUARD_NTS2_H */
