/*
 * ntp_extens.c - Network Time Protocol (NTP) extension processing
 *
 * NB: This module is working with the wire format packet.
 *     It must do byte swapping.
 *
 * We carefully arrange things so that no padding is necessary.
 *
 */

#include "config.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <openssl/rand.h>
#include <aes_siv.h>

#include "ntp_stdlib.h"
#include "ntp.h"
#include "ntpd.h"
#include "nts.h"

// FIXME Duplicated in nts_cookie
#define NONCE_LENGTH 16
#define CMAC_LENGTH 16

#define NTP_EX_HDR_LNG 4
#define NTP_EX_U16_LNG 2



enum NtpExtFieldType {
   Unique_Identifier = 10,
   NTS_Cookie = 11,
   NTS_Cookie_Placeholder = 12,
   NTS_AEEF = 13 /* Authenticated and Encrypted Extension Fields */
};

AES_SIV_CTX* wire_ctx = NULL;  /* need one per thread */


int extens_client_send(struct peer *peer, struct pkt *xpkt) {
  struct BufCtl_t buf;
  int used, adlength, idx;
  size_t left;
  uint8_t *nonce, *packet;
  bool ok;

  // FIXME - need init routine
  if (NULL == wire_ctx) {
    wire_ctx = AES_SIV_CTX_new();
    if (NULL == wire_ctx) {
      msyslog(LOG_ERR, "NTS: Can't init wire_ctx");
      exit(1);
    }
  }

  packet = (uint8_t*)xpkt;
  buf.next = xpkt->exten;
  buf.left = MAX_EXT_LEN;

  /* UID */
  RAND_bytes(peer->nts_state.UID, NTS_UID_LENGTH);
  nts_append_record_bytes(&buf, Unique_Identifier,
      peer->nts_state.UID, NTS_UID_LENGTH);

  /* cookie */
  idx = peer->nts_state.readIdx++;
  nts_append_record_bytes(&buf, NTS_Cookie,
      peer->nts_state.cookies[idx], peer->nts_state.cookielen);
  peer->nts_state.readIdx = peer->nts_state.readIdx % NTS_MAX_COOKIES;
  peer->nts_state.count--;
  // FIXME - what to do if out of cookies

  // FIXME - need more cookies?

  /* AEAD */
  adlength = buf.next-packet;
  nts_append_header(&buf, NTS_AEEF, NTP_EX_U16_LNG*2+NONCE_LENGTH+CMAC_LENGTH);
  nts_append_uint16(&buf, NONCE_LENGTH);
  nts_append_uint16(&buf, CMAC_LENGTH);
  nonce = buf.next;
  RAND_bytes(nonce, NONCE_LENGTH);
  buf.next += NONCE_LENGTH;
  buf.left -= NONCE_LENGTH;
  left = buf.left;
  ok = AES_SIV_Encrypt(wire_ctx,
           buf.next, &left,   /* left: in: max out length, out: length used */
           peer->nts_state.c2s, peer->nts_state.keylen,
           nonce, NONCE_LENGTH,
           NULL, 0,           /* no plain/cipher text */
           packet, adlength);
  if (!ok) {
    msyslog(LOG_ERR, "NTS: extens_client_send - Error from AES_SIV_Encrypt");
    /* I don't think this should happen,
     * so crash rather than work incorrectly.
     * Hal, 2019-Feb-17
     * Similar code in nts_cookie
     */
    exit(1);
  }
  buf.next += left;
  buf.left -= left;

  used = buf.next-xpkt->exten;
  return used;
}

bool extens_server_recv(struct ntspacket_t *ntspacket, uint8_t *pkt, int lng) {
  struct BufCtl_t buf;
  uint16_t aead;
  int noncelen, cmaclen;

  // FIXME - need init routine
  if (NULL == wire_ctx) {
    wire_ctx = AES_SIV_CTX_new();
    if (NULL == wire_ctx) {
      msyslog(LOG_ERR, "NTS: Can't init wire_ctx");
      exit(1);
    }
  }

  buf.next = pkt+LEN_PKT_NOMAC;
  buf.left = lng-LEN_PKT_NOMAC;

  while (buf.left > 0) {
    uint16_t type;
    bool critical = false;
    int length, adlength;
    size_t outlen;
    uint8_t *nonce, *cmac;
    bool ok;

    type = nts_next_record(&buf, &length);
    if (length > buf.left || length < 0)
      return false;
    if (NTS_CRITICAL & type) {
      critical = true;
      type &= ~NTS_CRITICAL;
    }
    switch (type) {
      case Unique_Identifier:
	if (length > NTS_UID_MAX_LENGTH)
	  return false;
        ntspacket->uidlen = length;
        nts_next_bytes(&buf, ntspacket->UID, length);
	break;
      case NTS_Cookie:
        ok = nts_unpack_cookie(buf.next, length,
            &aead,
	    ntspacket->c2s, ntspacket->s2c, &ntspacket->keylen);
	if (!ok)
	  return false;
        buf.next += length;
	buf.left -= length;
	ntspacket->valid = true;
	ntspacket->aead = aead;
	break;
      case NTS_Cookie_Placeholder:
        ntspacket->extra++;
        buf.next += length;
	buf.left -= length;
        break;
      case NTS_AEEF:
        if (!ntspacket->valid)
	  return false;			/* no cookie yet, no c2s */
        if (length != NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH)
	  return false;
        /* Additional data is up to this exten. */
	/* backup over header */
        adlength = buf.next-NTP_EX_HDR_LNG-pkt;
        noncelen = nts_next_uint16(&buf);
        cmaclen = nts_next_uint16(&buf);
	if (noncelen & 3)
          return false;			/* would require padding */
	if (CMAC_LENGTH != cmaclen)
          return false;
        nonce = buf.next;
	cmac = nonce+NONCE_LENGTH;
	outlen = 6;
        ok = AES_SIV_Decrypt(wire_ctx,
            NULL, &outlen,
            ntspacket->c2s, ntspacket->keylen,
            nonce, noncelen,
            cmac, CMAC_LENGTH,
            pkt, adlength);
	if (!ok)
	  return false;
	if (0 != outlen)
	  return false;
        buf.next += length;
	buf.left -= length;
        break;
      default:
        if (critical)
          return false;
        buf.next += length;
	buf.left -= length;
    }
  }

  if (!ntspacket->valid)
    return false;

  return true;
}

/* end */
