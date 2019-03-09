/*
 * nts.c - Network Time Security (NTS) support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 * This module exposes mostly functions and structure pointers (not
 * structures) so that the NTS implementation can be sealed off from
 * the rest of the code. It supports both the client and server sides.
 *
 * The exception is client configuration, for which various bits have
 * to be set by the config parser.
 */

#include "config.h"

#include <arpa/inet.h>
#include <openssl/err.h>

#include "ntp_types.h"
#include "ntpd.h"
#include "nts.h"
#include "nts2.h"

struct ntsconfig_t ntsconfig = {
  .ntsenable = false,
  .mintls = NULL,
  .maxtls = NULL,
  .tlsciphers = NULL,
  .tlsciphersuites = NULL,
  .cert = NULL,
  .key = NULL,
  .KI = NULL,
  .ca = NULL
};



/*****************************************************/

void nts_log_ssl_error(void) {
  char buff[256];
  int err = ERR_get_error();
  SSL_load_error_strings();        /* Needed on NetBSD */
  while (0 != err) {
    ERR_error_string_n(err, buff, sizeof(buff));
    msyslog(LOG_INFO, "NTS: %s", buff);
    err = ERR_get_error();
  }
}

/*****************************************************/

/* NB: KE length is body length, Extension length includes header. */

/* 2 byte type, 2 byte length */
#define NTS_KE_HDR_LNG 4
#define NTS_KE_U16_LNG 2

/* Troubles with signed/unsigned compares when using sizeof() */

void ke_append_record_null(BufCtl* buf, uint16_t type) {
  append_header(buf, type, 0);
}

void ke_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data) {
  if (NTS_KE_HDR_LNG+NTS_KE_U16_LNG > buf->left)
    return;
  append_header(buf, type, NTS_KE_U16_LNG);
  append_uint16(buf, data);
}

void ke_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length) {
  if (NTS_KE_HDR_LNG+length > buf->left)
    return;
  append_header(buf, type, length);
  append_bytes(buf, data, length);
}

void ex_append_record_null(BufCtl* buf, uint16_t type) {
  append_header(buf, type, NTS_KE_HDR_LNG);
}

void ex_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data) {
  if (NTS_KE_HDR_LNG+NTS_KE_U16_LNG > buf->left)
    return;
  append_header(buf, type, NTS_KE_HDR_LNG+NTS_KE_U16_LNG);
  append_uint16(buf, data);
}

void ex_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length) {
  if (NTS_KE_HDR_LNG+length > buf->left)
    return;
  append_header(buf, type, NTS_KE_HDR_LNG+length);
  append_bytes(buf, data, length);
}

void ex_append_header(BufCtl* buf, uint16_t type, uint16_t length) {
  append_header(buf, type, NTS_KE_HDR_LNG+length);
}

void append_header(BufCtl* buf, uint16_t type, uint16_t length) {
  uint16_t * ptr = (uint16_t *)buf->next;
  if (NTS_KE_HDR_LNG > buf->left)
    return;
  *ptr++ = htons(type);
  *ptr++ = htons(length);
  buf->next += NTS_KE_HDR_LNG;
  buf->left -= NTS_KE_HDR_LNG;
  /* leaves buf pointing to where data will go */
}

void append_uint16(BufCtl* buf, uint16_t data) {
  uint16_t * ptr = (uint16_t *)buf->next;
  if (NTS_KE_U16_LNG > buf->left)
    return;
  *ptr++ = htons(data);
  buf->next += NTS_KE_U16_LNG;
  buf->left -= NTS_KE_U16_LNG;
}

void append_bytes(BufCtl* buf, uint8_t *data, int length) {
  if (length > buf->left)
    return;
  memcpy(buf->next, data, length);
  buf->next += length;
  buf->left -= length;
}

/* Reads type and length of the next record, and moves cursor to the data */
uint16_t ke_next_record(BufCtl* buf, int *length) {
  uint16_t *ptr = (uint16_t *)buf->next;
  uint16_t type = ntohs(*ptr++);
  *length = ntohs(*ptr++);
  buf->next += NTS_KE_HDR_LNG;
  buf->left -= NTS_KE_HDR_LNG;
  return type;
}

uint16_t ex_next_record(BufCtl* buf, int *length) {
  uint16_t *ptr = (uint16_t *)buf->next;
  uint16_t type = ntohs(*ptr++);
  *length = ntohs(*ptr++)-NTS_KE_HDR_LNG;
  buf->next += NTS_KE_HDR_LNG;
  buf->left -= NTS_KE_HDR_LNG;
  return type;
}

/* Reads a uint16 from the record and advances to the next data */
uint16_t next_uint16(BufCtl* buf) {
  uint16_t *ptr = (uint16_t *)buf->next;
  uint16_t data = ntohs(*ptr++);
  buf->next += NTS_KE_U16_LNG;
  buf->left -= NTS_KE_U16_LNG;
  return data;
}

/* Reads a string of bytes from the record and advances to the next data */
uint16_t next_bytes(BufCtl* buf, uint8_t *data, int length) {
  memcpy(data, buf->next, length);
  buf->next += length;
  buf->left -= length;
  return length;
}


/* end */
