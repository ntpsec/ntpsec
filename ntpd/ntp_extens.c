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
  nts_append_header(&buf, NTS_AEEF, 4+NONCE_LENGTH+CMAC_LENGTH);
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

/* end */
