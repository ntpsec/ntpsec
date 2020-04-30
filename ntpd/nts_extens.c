/*
 * ntp_extens.c - Network Time Protocol (NTP) extension processing
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * NB: This module is working with the wire format packet.
 *     It must do byte swapping.
 *
 * We carefully arrange things so that no padding is necessary.
 *
 * This is only called by the main ntpd thread, so we don't need
 * a lock to protect wire_ctx.
 */

#include "config.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <aes_siv.h>

#include "ntp_stdlib.h"
#include "ntp.h"
#include "ntpd.h"
#include "nts.h"
#include "nts2.h"

#define NONCE_LENGTH 16
#define CMAC_LENGTH 16

#define NTP_EX_HDR_LNG 4
#define NTP_EX_U16_LNG 2

/* Statistics for ntpq */
uint64_t nts_client_send = 0;
uint64_t nts_client_recv_good = 0;
uint64_t nts_client_recv_bad = 0;
uint64_t nts_server_send = 0;
uint64_t nts_server_recv_good = 0;
uint64_t nts_server_recv_bad = 0;

enum NtpExtFieldType {
	Unique_Identifier = 0x104,
	NTS_Cookie = 0x204,
	NTS_Cookie_Placeholder = 0x304,
	NTS_AEEF = 0x404 /* Authenticated and Encrypted Extension Fields */
};

/* This is only called by the main thread so we don't need a lock. */
AES_SIV_CTX* wire_ctx = NULL;


bool extens_init(void) {
	wire_ctx = AES_SIV_CTX_new();
	if (NULL == wire_ctx) {
		msyslog(LOG_ERR, "NTS: Can't init wire_ctx");
		exit(1);
	}
	return true;
}

int extens_client_send(struct peer *peer, struct pkt *xpkt) {
	struct BufCtl_t buf;
	int used, adlength, idx;
	size_t left;
	uint8_t *nonce, *packet;
	bool ok;

	packet = (uint8_t*)xpkt;
	buf.next = xpkt->exten;
	buf.left = MAX_EXT_LEN;

	/* UID */
	ntp_RAND_bytes(peer->nts_state.UID, NTS_UID_LENGTH);
	ex_append_record_bytes(&buf, Unique_Identifier,
			       peer->nts_state.UID, NTS_UID_LENGTH);

	/* cookie */
	idx = peer->nts_state.readIdx++;
	ex_append_record_bytes(&buf, NTS_Cookie,
			       peer->nts_state.cookies[idx], peer->nts_state.cookielen);
	peer->nts_state.readIdx = peer->nts_state.readIdx % NTS_MAX_COOKIES;
	peer->nts_state.count--;

	/* Need more cookies? */
	for (int i=peer->nts_state.count+1; i<NTS_MAX_COOKIES; i++) {
		/* WARN: This may get too big for the MTU. */
		ex_append_header(&buf, NTS_Cookie_Placeholder, peer->nts_state.cookielen);
		memset(buf.next, 0, peer->nts_state.cookielen);
		buf.next += peer->nts_state.cookielen;
		buf.left -= peer->nts_state.cookielen;
	}

	/* AEAD */
	adlength = buf.next-packet;
	ex_append_header(&buf, NTS_AEEF, NTP_EX_U16_LNG*2+NONCE_LENGTH+CMAC_LENGTH);
	append_uint16(&buf, NONCE_LENGTH);
	append_uint16(&buf, CMAC_LENGTH);
	nonce = buf.next;
	ntp_RAND_bytes(nonce, NONCE_LENGTH);
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
	nts_client_send++;
	return used;
}

bool extens_server_recv(struct ntspacket_t *ntspacket, uint8_t *pkt, int lng) {
	struct BufCtl_t buf;
	uint16_t aead;
	int noncelen, cmaclen;
	bool sawcookie, sawAEEF;
	int cookielen;			/* cookie and placeholder(s) */

	nts_server_recv_bad++;		/* assume bad, undo if OK */

	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = lng-LEN_PKT_NOMAC;

	sawcookie = sawAEEF = false;
	cookielen = 0;
	ntspacket->uidlen = 0;
	ntspacket->needed = 0;

	while (buf.left > 0) {
		uint16_t type;
		bool critical = false;
		int length, adlength;
		size_t outlen;
		uint8_t *nonce, *cmac;
		bool ok;

		type = ex_next_record(&buf, &length); /* length excludes header */
		if (length&3 || length > buf.left || length < 0) {
			return false;
		}
		if (NTS_CRITICAL & type) {
			critical = true;
			type &= ~NTS_CRITICAL;
		}
		switch (type) {
		    case Unique_Identifier:
			if (length > NTS_UID_MAX_LENGTH) {
				return false;
			}
			ntspacket->uidlen = length;
			next_bytes(&buf, ntspacket->UID, length);
			break;
		    case NTS_Cookie:
			/* cookies and placeholders must be the same length
			 * in order to avoid amplification attacks.
			 */
			if (sawcookie) {
				return false; /* second cookie */
			}
			if (0 == cookielen) {
				cookielen = length;
			}
			else if (length != cookielen) {
				return false;
			}
			ok = nts_unpack_cookie(buf.next, length, &aead, ntspacket->c2s,
					       ntspacket->s2c, &ntspacket->keylen);
			if (!ok) {
				return false;
			}
			buf.next += length;
			buf.left -= length;
			sawcookie = true;
			ntspacket->needed++;
			ntspacket->aead = aead;
			break;
		    case NTS_Cookie_Placeholder:
			if (0 == cookielen) {
				cookielen = length;
			}
			else if (length != cookielen) {
				return false;
			}
			ntspacket->needed++;
			buf.next += length;
			buf.left -= length;
			break;
		    case NTS_AEEF:
			if (!sawcookie) {
				return false; /* no cookie yet, no c2s */
			}
			if (length != NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH) {
				return false;
			}
			/* Additional data is up to this exten. */
			/* backup over header */
			adlength = buf.next-NTP_EX_HDR_LNG-pkt;
			noncelen = next_uint16(&buf);
			cmaclen = next_uint16(&buf);
			if (noncelen & 3) {
				return false; /* would require padding */
			}
			if (CMAC_LENGTH != cmaclen) {
				return false;
			}
			nonce = buf.next;
			cmac = nonce+NONCE_LENGTH;
			outlen = 6;
			ok = AES_SIV_Decrypt(wire_ctx,
					     NULL, &outlen,
					     ntspacket->c2s, ntspacket->keylen,
					     nonce, noncelen,
					     cmac, CMAC_LENGTH,
					     pkt, adlength);
			if (!ok) {
				return false;
			}
			if (0 != outlen) {
				return false;
			}
			/* we already used 2 length slots way above*/
			length -= (NTP_EX_U16_LNG+NTP_EX_U16_LNG);
			buf.next += length;
			buf.left -= length;
			if (0 != buf.left) {
				return false; /* Reject extens after AEEF block */
			}
			sawAEEF = true;
			break;
		    default:
			/* Non NTS extensions on requests at server.
			 * Call out when we get some that we want.
			 * Until then, it's probably a bug. */
			if (critical) {
				return false;
			}
			buf.next += length;
			buf.left -= length;
			return false;
		}
	}

	if (!sawAEEF) {
		return false;
	}
	//  printf("ESRx: %d, %d, %d\n",
	//      lng-LEN_PKT_NOMAC, ntspacket->needed, ntspacket->keylen);
	ntspacket->valid = true;
	nts_server_recv_good++;
	nts_server_recv_bad--;
	return true;
}

int extens_server_send(struct ntspacket_t *ntspacket, struct pkt *xpkt) {
	struct BufCtl_t buf;
	int used, adlength;
	size_t left;
	uint8_t *nonce, *packet;
	uint8_t *plaintext, *ciphertext;;
	uint8_t cookie[NTS_MAX_COOKIELEN];
	int cookielen, plainleng, aeadlen;
	bool ok;

	/* get first cookie now so we have length */
	cookielen = nts_make_cookie(cookie, ntspacket->aead,
				    ntspacket->c2s, ntspacket->s2c, ntspacket->keylen);

	packet = (uint8_t*)xpkt;
	buf.next = xpkt->exten;
	buf.left = MAX_EXT_LEN;

	/* UID */
	if (0 < ntspacket->uidlen)
		ex_append_record_bytes(&buf, Unique_Identifier,
				       ntspacket->UID, ntspacket->uidlen);

	adlength = buf.next-packet;		/* up to here is Additional Data */

	/* length of whole AEEF */
	plainleng = ntspacket->needed*(NTP_EX_HDR_LNG+cookielen);
	/* length of whole AEEF header */
	aeadlen = NTP_EX_U16_LNG*2+NONCE_LENGTH+CMAC_LENGTH + plainleng;
	ex_append_header(&buf, NTS_AEEF, aeadlen);
	append_uint16(&buf, NONCE_LENGTH);
	append_uint16(&buf, plainleng+CMAC_LENGTH);

	nonce = buf.next;
	ntp_RAND_bytes(nonce, NONCE_LENGTH);
	buf.next += NONCE_LENGTH;
	buf.left -= NONCE_LENGTH;

	ciphertext = buf.next;	/* cipher text starts here */
	left = buf.left;
	buf.next += CMAC_LENGTH;	/* skip space for CMAC */
	buf.left -= CMAC_LENGTH;
	plaintext = buf.next;		/* encrypt in place */

	ex_append_record_bytes(&buf, NTS_Cookie,
			       cookie, cookielen);
	for (int i=1; i<ntspacket->needed; i++) {
		/* WARN: This may get too big for the MTU. See length calculation above.
		 * Responses are the same length as requests to avoid DDoS amplification.
		 * So if it got to us, there is a good chance it will get back.  */
		nts_make_cookie(cookie, ntspacket->aead,
				ntspacket->c2s, ntspacket->s2c, ntspacket->keylen);
		ex_append_record_bytes(&buf, NTS_Cookie,
				       cookie, cookielen);
	}

	//printf("ESSa: %d, %d, %d, %d\n",
	//  adlength, plainleng, cookielen, ntspacket->needed);

	ok = AES_SIV_Encrypt(wire_ctx,
			     ciphertext, &left,   /* left: in: max out length, out: length used */
			     ntspacket->s2c, ntspacket->keylen,
			     nonce, NONCE_LENGTH,
			     plaintext, plainleng,
			     packet, adlength);
	if (!ok) {
		msyslog(LOG_ERR, "NTS: extens_server_send - Error from AES_SIV_Encrypt");
		nts_log_ssl_error();
		/* I don't think this should happen,
		 * so crash rather than work incorrectly.
		 * Hal, 2019-Feb-17
		 * Similar code in nts_cookie
		 */
		exit(1);
	}

	used = buf.next-xpkt->exten;

	// printf("ESSx: %lu, %d\n", (long unsigned)left, used);

	nts_server_send++;
	return used;
}

bool extens_client_recv(struct peer *peer, uint8_t *pkt, int lng) {
	struct BufCtl_t buf;
	int idx;
	bool sawAEEF = false;

	nts_client_recv_bad++;		/* assume bad, undo if OK */

	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = lng-LEN_PKT_NOMAC;

	while (buf.left > 0) {
		uint16_t type;
		bool critical = false;
		int length, adlength, noncelen;
		uint8_t *nonce, *ciphertext, *plaintext;
		size_t outlen;
		bool ok;

		type = ex_next_record(&buf, &length); /* length excludes header */
		if (length&3 || length > buf.left || length < 0)
			return false;
		if (NTS_CRITICAL & type) {
			critical = true;
			type &= ~NTS_CRITICAL;
		}
		//     printf("ECR: %d, %d, %d\n", type, length, buf.left);
		switch (type) {
		    case Unique_Identifier:
			if (NTS_UID_LENGTH != length)
				return false;
			if (0 != memcmp(buf.next, peer->nts_state.UID, NTS_UID_LENGTH))
				return false;
			buf.next += length;
			buf.left -= length;
			break;
		    case NTS_Cookie:
			if (!sawAEEF)
				return false;			/* reject unencrypted cookies */
			if (NTS_MAX_COOKIES <= peer->nts_state.count)
				return false;			/* reject extra cookies */
			if (length != peer->nts_state.cookielen)
				return false;			/* reject length change */
			idx = peer->nts_state.writeIdx++;
			memcpy((uint8_t*)&peer->nts_state.cookies[idx], buf.next, length);
			peer->nts_state.writeIdx = peer->nts_state.writeIdx % NTS_MAX_COOKIES;
			peer->nts_state.count++;
			buf.next += length;
			buf.left -= length;
			break;
		    case NTS_AEEF:
			adlength = buf.next-NTP_EX_HDR_LNG-pkt;  /* backup over header */
			noncelen = next_uint16(&buf);
			outlen = next_uint16(&buf);
			if (noncelen&3 || outlen&3)
				return false;                 /* else round up */
			nonce = buf.next;
			ciphertext = nonce+noncelen;
			plaintext = ciphertext+CMAC_LENGTH;
			outlen = buf.left-NONCE_LENGTH-CMAC_LENGTH;
			//      printf("ECRa: %lu, %d\n", (long unsigned)outlen, noncelen);
			ok = AES_SIV_Decrypt(wire_ctx,
					     plaintext, &outlen,
					     peer->nts_state.s2c, peer->nts_state.keylen,
					     nonce, noncelen,
					     ciphertext, outlen+CMAC_LENGTH,
					     pkt, adlength);
			//      printf("ECRb: %d, %lu\n", ok, (long unsigned)outlen);
			if (!ok)
				return false;
			/* setup to process encrypted headers */
			buf.next += NONCE_LENGTH+CMAC_LENGTH;
			buf.left -= NONCE_LENGTH+CMAC_LENGTH;
			sawAEEF = true;
			break;
		    default:
			/* Non NTS extensions on reply from server.
			 * Call out when we get some that we want.
			 * For now, it's probably a bug. */
			if (critical)
				return false;
			buf.next += length;
			buf.left -= length;
			return false;
		}
	}

	//  printf("ECRx: %d, %d  %d, %d\n", sawAEEF, peer->nts_state.count,
	//      peer->nts_state.writeIdx, peer->nts_state.readIdx);
	if (!sawAEEF) {
		return false;
	}
	nts_client_recv_good++;
	nts_client_recv_bad--;
	return true;
}
/* end */
