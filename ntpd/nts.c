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

#include "ntp_types.h"
#include "ntpd.h"

struct ntsconfig_t ntsconfig = {
  .ntsenable = false,
  .mintls = 0,
  .maxtls = 0,
  .tlsciphers = NULL,
  .tlsciphersuites = NULL,
  .ca = NULL,
  .cert = NULL
};

/* By design, there is no per-client-side state on the server */

/*
 * Key exchange support.
 */

/*
 * RFC section 4:
 *- Generate KE request message.
 * - Include Record Types:
 *  o NTS Next Protocol Negotiation
 *  o AEAD Algorithm Negotiation
 *  o <NTP Server Negotiation>
 *  o End of Message
 */
int nts_client_ke_request(struct ntscfg_t *cfg)
{
	UNUSED_ARG(cfg);
	return 0;
}

/*
 * RFC section 4:
 * - Verify client request message.
 * - Extract TLS key material.
 * - Generate KE response message.
 *   - Include Record Types:
 *       o NTS Next Protocol Negotiation
 *       o AEAD Algorithm Negotiation
 *       o NTP Server Negotiation
 *       o New Cookie for NTPv4
 *       o <New Cookie for NTPv4>
 *       o End of Message
 */
int nts_server_ke_verify(struct ntscfg_t *cfg)
{
	UNUSED_ARG(cfg);
	return 0;
}

/*
 * RFC section 4:
 * - Verify server response message
 * - Extract cookie(s).
 */
int nts_client_ke_verify(struct ntscfg_t *cfg, struct ntsstate_t *state)
{
	UNUSED_ARG(cfg);
	UNUSED_ARG(state);
	return 0;
}

/*
 * Daily rotation of server-side master keys.
 */
int nts_daily(struct ntscfg_t *cfg)
{
	UNUSED_ARG(cfg);
	return 0;
}

/* */

/*
 * Extract and validate NTS validation information from packet
 * extension fields in an incoming request or response.  On the server
 * side, the ntscfg and state pointers are expected to be NULL as
 * there is no per-client server state.  A nonzero return causes the
 * packet to be discarded.
 */
int nts_validate(struct ntscfg_t *cfg, struct ntsstate_t *state,
		 struct parsed_pkt *pkt)
{
	UNUSED_ARG(cfg);
	UNUSED_ARG(pkt);
	UNUSED_ARG(state);
	return 0;
}

/*
 * Decorate an outgoing client request or server response with packet
 * extension fields carrying NTS information.  For a server response,
 * the ntscfg and state pointers are expected to be NULL as there
 * is no per-client server state.  Return the count of words appended.
 */
int nts_decorate(struct ntscfg_t *cfg, struct ntsstate_t *state,
		 uint32_t *extdata, size_t extlen)
{
	UNUSED_ARG(cfg);
	UNUSED_ARG(extdata);
	UNUSED_ARG(extlen);
	UNUSED_ARG(state);
	return 0;
}

/*****************************************************/

// 2 byte type, 2 byte length
#define NTS_KE_HDR_LNG 4
#define NTS_KE_DATA2_LNG 2

/* Troubles with signed/unsigned compares when using sizeof() */

void nts_append_record_null(BufCtl* buf, uint16_t type) {
  nts_append_header(buf, type, NTS_KE_DATA2_LNG);
}

void nts_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data) {
  if (NTS_KE_HDR_LNG+NTS_KE_DATA2_LNG > buf->left)
    return;
  nts_append_header(buf, type, NTS_KE_DATA2_LNG);
  nts_append_uint16(buf, data);
}

void nts_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length) {
  if (NTS_KE_HDR_LNG+length > buf->left)
    return;
  nts_append_header(buf, type, length);
  nts_append_bytes(buf, data, length);
}

void nts_append_header(BufCtl* buf, uint16_t type, uint16_t length) {
  uint16_t * ptr = (uint16_t *)buf->next;
  if (NTS_KE_HDR_LNG > buf->left)
    return;
  *ptr++ = htons(type);
  *ptr++ = htons(length);
  buf->next += NTS_KE_HDR_LNG;
  buf->left -= NTS_KE_HDR_LNG;
  /* leaves buf pointing to where data will go */
}

void nts_append_uint16(BufCtl* buf, uint16_t data) {
  uint16_t * ptr = (uint16_t *)buf->next;
  if (NTS_KE_DATA2_LNG > buf->left)
    return;
  *ptr++ = htons(data);
  buf->next += NTS_KE_DATA2_LNG;
  buf->left -= NTS_KE_DATA2_LNG;
}

void nts_append_bytes(BufCtl* buf, uint8_t *data, int length) {
  if (length > buf->left)
    return;
  memcpy(buf->next, data, length);
  buf->next += length;
  buf->left -= length;
}


uint16_t nts_next_record(BufCtl* buf, int *length) {
  uint16_t *ptr = (uint16_t *)buf->next;
  uint16_t type = ntohs(*ptr++);
  *length = ntohs(*ptr++);
  buf->next += sizeof(type)+sizeof(*length);
  buf->left -= sizeof(type)+sizeof(*length);
  return type;
}

uint16_t nts_next_uint16(BufCtl* buf) {
  uint16_t *ptr = (uint16_t *)buf->next;
  uint16_t data = ntohs(*ptr++);
  buf->next += sizeof(data);
  buf->left -= sizeof(data);
  return data;
}


/* end */
