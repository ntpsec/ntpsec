/*
 * nts2.h - NTS (Network Time Security) declarations
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * other half of nts.h which doesn't include openssl/ssh.h
 * This split helps test routines.
 */

#ifndef GUARD_NTS2_H
#define GUARD_NTS2_H

#include <stdbool.h>
#include <stdint.h>
#include <openssl/ssl.h>

#include "nts.h"


bool nts_load_certificate(SSL_CTX *ctx);
void nts_reload_certificate(SSL_CTX *ctx);
bool nts_load_ciphers(SSL_CTX *ctx);
bool nts_load_versions(SSL_CTX *ctx);

int nts_ssl_read(SSL *ssl, uint8_t *buff, int buff_length);
int nts_ssl_write(SSL *ssl, uint8_t *buff, int buff_length);

void nts_log_ssl_error(void);

int nts_get_key_length(uint16_t aead);
int nts_translate_version(const char *arg);
uint16_t nts_string_to_aead(const char* text);

bool nts_make_keys(SSL *ssl, uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen);


#endif /* GUARD_NTS2_H */
