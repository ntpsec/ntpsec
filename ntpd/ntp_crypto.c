/*
 * ntp_crypto.c - NTP version 4 public key routines
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef OPENSSL
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "ntp_string.h"
#include "ntp_crypto.h"

#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/rand.h"

#include <stdio.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include <openssl/asn1_mac.h>

#ifdef KERNEL_PLL
#include "ntp_syscall.h"
#endif /* KERNEL_PLL */

/*
 * Extension field message format (host byte order)
 *
 * +-------+-------+
 * |   op  |  len  |
 * +-------+-------+
 * |    assocID    |
 * +---------------+
 * |   timestamp   | <- value pointer
 * +---------------+
 * |   filestamp   |
 * +---------------+
 * |   value len   |
 * +---------------+
 * |               |
 * =     value     =
 * |               |
 * +---------------+
 * | signature len |
 * +---------------+
 * |               |
 * =   signature   =
 * |               |
 * +---------------+
 *
 * The CRYPTO_RESP bit is set to 0 for requests, 1 for responses.
 * Requests carry the association ID of the receiver; responses carry
 * the association ID of the sender. Some messages include only the
 * operation/length and association ID words and so have length 8
 * octets. Ohers include the value structure and associated value and
 * signature fields. These messages include the timestamp, filestamp,
 * value and signature words and so have length at least 24 octets. The
 * signature and/or value fields can be empty, in which case the
 * respective length words are zero. An empty value with nonempty
 * signature is syntactically valid, but semantically questionable.
 *
 * The filestamp represents the time when a cryptographic data file such
 * as a public/private key pair is created. It follows every reference
 * depending on that file and serves as a means to obsolete earlier data
 * of the same type. The timestamp represents the time when the
 * cryptographic data of the message were last signed. Creation of a
 * cryptographic data file or signing a message can occur only when the
 * creator or signor is synchronized to an authoritative source and
 * proventicated to a trusted authority.
 *
 * The cryptographic signing scheme is defined by the server sign
 * key, which defaults to the server rsa key, while the verifying scheme
 * is determined by the certificate received by the client.
 */
/*
 * Cryptodefines
 */
#define TAI_1972	10	/* initial TAI offset (s) */
#define MAX_LEAP	100	/* max UTC leapseconds (s) */
#define MIN_HOSTLEN	4	/* min host name length */
#define MAX_HOSTLEN	256	/* max host name length */
#define	DIGESTNAME	"MD5"	/* message digest algorithm name */
#define VALUE_LEN	(6 * 4) /* min response field length */

/*
 * Global cryptodata in host byte order
 */
u_int32	crypto_flags;		/* status word */
u_int	sys_tai;		/* current UTC offset from TAI */

/*
 * Global cryptodata in network byte order
 */
struct value host;		/* rsa key */
struct value cinfo;		/* certificate information */
struct value pubkey;		/* public key */
struct value tai_leap;		/* leapseconds table */

/*
 * Private cryptodata in host byte order
 */
static EVP_PKEY *rsa_pkey = NULL; /* rsa key */
static EVP_PKEY *sign_pkey = NULL; /* sign key */
static u_int sign_siglen;	/* sign key length */
static const EVP_MD *digest = NULL; /* message digest algorithm */
static char *keysdir = NTP_KEYSDIR; /* crypto keys directory */
static char *rand_file = NULL;	/* random seed file */
static char *rsa_file = NULL;	/* rsa key file */
static char *sign_file = NULL;	/* sign key file */
static char *cert_file = NULL;	/* certificate file */
static char *leap_file = NULL;	/* leapseconds file */

/*
 * Cryptotypes
 */
static	EVP_PKEY *crypto_key	P((char *, struct value *));
static	struct cert_info *crypto_cert P((char *));
static	void	crypto_tai	P((char *));
static	u_long	asn2ntp		P((ASN1_TIME *));
static	struct value * crypto_verify P((u_int32 *, struct value *,
				    struct peer *));
static	u_int	crypto_send	P((struct value *, struct value *));
static	int	crypto_encrypt	P((struct value *, struct value *,
				    keyid_t *));


/*
 * session_key - generate session key
 *
 * This routine generates a session key from the source address,
 * destination address, key ID and private value. The value of the
 * session key is the MD5 hash of these values, while the next key ID is
 * the first four octets of the hash.
 */
keyid_t				/* returns next key ID */
session_key(
	struct sockaddr_in *srcadr, /* source address */
	struct sockaddr_in *dstadr, /* destination address */
	keyid_t	keyno,		/* key ID */
	keyid_t	private,	/* private value */
	u_long	lifetime 	/* key lifetime */
	)
{
	EVP_MD_CTX ctx;		/* message digest context */
	u_char dgst[EVP_MAX_MD_SIZE]; /* message digest */
	keyid_t	keyid;		/* key identifer */
	u_int32	header[4];	/* data in network byte order */
	u_int	len;

	/*
	 * Generate the session key and key ID. If the lifetime is
	 * greater than zero, install the key and call it trusted.
	 */
	header[0] = srcadr->sin_addr.s_addr;
	header[1] = dstadr->sin_addr.s_addr;
	header[2] = htonl(keyno);
	header[3] = htonl(private);
	EVP_DigestInit(&ctx, EVP_md5());
	EVP_DigestUpdate(&ctx, (u_char *)header, sizeof(header));
	EVP_DigestFinal(&ctx, dgst, &len);
	memcpy(&keyid, dgst, 4);
	keyid = ntohl(keyid);
	if (lifetime != 0) {
		MD5auth_setkey(keyno, dgst, len);
		authtrust(keyno, lifetime);
	}
#ifdef DEBUG
	if (debug > 1)
		printf(
		    "session_key: %s > %s %08x %08x hash %08x life %lu\n",
		    numtoa(header[0]), numtoa(header[1]), keyno,
		    private, keyid, lifetime);
#endif
	return (keyid);
}


/*
 * make_keylist - generate key list
 *
 * This routine constructs a pseudo-random sequence by repeatedly
 * hashing the session key starting from a given source address,
 * destination address, private value and the next key ID of the
 * preceeding session key. The last entry on the list is saved along
 * with its sequence number and public signature.
 */
void
make_keylist(
	struct peer *peer,	/* peer structure pointer */
	struct interface *dstadr /* interface */
	)
{
	EVP_MD_CTX ctx;		/* signature context */
	l_fp	tstamp;		/* NTP timestamp */
	struct autokey *ap;	/* autokey pointer */
	struct value *vp;	/* value pointer */
	keyid_t	keyid;		/* next key ID */
	keyid_t	cookie;		/* private value */
	u_long	lifetime;
	u_int	len;
	int	i;

	/*
	 * Allocate the key list if necessary.
	 */
	L_CLR(&tstamp);
	if (sys_leap != LEAP_NOTINSYNC)
		get_systime(&tstamp);
	if (peer->keylist == NULL)
		peer->keylist = emalloc(sizeof(keyid_t) *
		    NTP_MAXSESSION);

	/*
	 * Generate an initial key ID which is unique and greater than
	 * NTP_MAXKEY.
	 */
	while (1) {
		keyid = (u_long)RANDOM & 0xffffffff;
		if (keyid <= NTP_MAXKEY)
			continue;
		if (authhavekey(keyid))
			continue;
		break;
	}

	/*
	 * Generate up to NTP_MAXSESSION session keys. Stop if the
	 * next one would not be unique or not a session key ID or if
	 * it would expire before the next poll. The private value
	 * included in the hash is zero if broadcast mode, the peer
	 * cookie if client mode or the host cookie if symmetric modes.
	 */
	lifetime = min(sys_automax, NTP_MAXSESSION * (1 <<
	    (peer->kpoll)));
	if (peer->hmode == MODE_BROADCAST)
		cookie = 0;
	else
		cookie = peer->pcookie;
	for (i = 0; i < NTP_MAXSESSION; i++) {
		peer->keylist[i] = keyid;
		peer->keynumber = i;
		keyid = session_key(&dstadr->sin, &peer->srcadr, keyid,
		    cookie, lifetime);
		lifetime -= 1 << peer->kpoll;
		if (auth_havekey(keyid) || keyid <= NTP_MAXKEY ||
		    lifetime <= (1 << (peer->kpoll)))
			break;
	}

	/*
	 * Save the last session key ID, sequence number and timestamp,
	 * then sign these values for later retrieval by the clients. Be
	 * careful not to use invalid key media.
	 */
	vp = &peer->sndval;
	if (vp->ptr == NULL)
		vp->ptr = emalloc(sizeof(struct autokey));
	ap = (struct autokey *)vp->ptr;
	ap->seq = htonl(peer->keynumber);
	ap->key = htonl(keyid);
	vp->tstamp = htonl(tstamp.l_ui);
	vp->fstamp = cinfo.fstamp;
	vp->vallen = htonl(sizeof(struct autokey));
	vp->siglen = 0;
	if (vp->tstamp != 0) {
		if (vp->sig == NULL)
			vp->sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, digest);
		EVP_SignUpdate(&ctx, (u_char *)vp, 12);
		EVP_SignUpdate(&ctx, vp->ptr, sizeof(struct autokey));
		if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey))
			vp->siglen = htonl(len);
		peer->flags |= FLAG_ASSOC;
	}
#if DEBUG
	if (debug)
		printf("make_keys: %d %08x %08x ts %u fs %u poll %d\n",
		    ntohl(ap->seq), ntohl(ap->key), cookie,
		    ntohl(vp->tstamp), ntohl(vp->fstamp), peer->kpoll);
#endif
}


/*
 * crypto_recv - parse extension fields
 *
 * This routine is called when the packet has been matched to an
 * association and passed sanity, format and MAC checks. We believe the
 * extension field values only if the field has proper format and
 * length, the timestamp and filestamp are valid and the signature has
 * valid length and is verified. There are a few cases where some values
 * are believed even if the signature fails, but only if the authentic
 * bit is not set.
 */
void
crypto_recv(
	struct peer *peer,	/* peer structure pointer */
	struct recvbuf *rbufp,	/* packet buffer pointer */
	int	is_org		/* timestamps test */
	)
{
	EVP_MD_CTX ctx;		/* signature context */
	const EVP_MD *dp = NULL; /* message digest pointer */
	EVP_PKEY *kp = NULL;	/* key pointer */
	u_int32	*pkt;		/* receive packet pointer */
	struct autokey *ap, *bp; /* autokey pointer */
	struct	cert_info *xp;	/* certificate information */
	struct value *vp;	/* value pointer */
	int	has_mac;	/* length of MAC field */
	int	authlen;	/* offset of MAC field */
	associd_t associd;	/* association ID */
	tstamp_t tstamp;	/* timestamp */
	tstamp_t fstamp;	/* filestamp */
	u_int	len;		/* extension field length */
	u_int	code;		/* extension field opcode */
	u_int	vallen;		/* value length */
	u_int	siglen;		/* signature length */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	keyid_t	cookie;		/* crumbles */
	int	j, rval;
	u_int32 temp32;
	u_int32	*ptr32;
#ifdef KERNEL_PLL
#if NTP_API > 3
	struct timex ntv;	/* kernel interface structure */
#endif /* NTP_API */
#endif /* KERNEL_PLL */

	/*
	 * Initialize. Note that the packet has already been checked for
	 * valid format and extension field lengths. First extract the
	 * field length, command code and association ID in host byte
	 * order. These are used with all commands and modes. Then check
	 * the version number, which must be 2, and length, which must
	 * be at least 8 for requests and VALUE_LEN (24) for responses.
	 * Packets that fail either test sink without a trace. The
	 * association ID is saved only if nonzero.
	 */
	authlen = LEN_PKT_NOMAC;
	while ((has_mac = rbufp->recv_length - authlen) > MAX_MAC_LEN) {
		pkt = (u_int32 *)&rbufp->recv_pkt + authlen / 4;
		code = ntohl(pkt[0]) & 0xffff0000;
		len = ntohl(pkt[0]) & 0x0000ffff;
		associd = ntohl(pkt[1]);
#ifdef DEBUG
		if (debug)
			printf(
			    "crypto_recv: flags 0x%x ext offset %d len %u code %x assocID %d\n",
			    peer->crypto, authlen, len, code >> 16,
			    associd);
#endif
		if (((code >> 24) & 0x3f) != CRYPTO_VN || len < 8 ||
		    (len < VALUE_LEN && (code & CRYPTO_RESP))) {
			sys_unknownversion++;
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: incorrect version or field length\n");
#endif
			peer->flash |= TEST12;
			return;
		}
		if (associd != 0)
			peer->assoc = associd;
		switch (code) {

		/*
		 * Install status word, host name, signature scheme and
		 * association ID. In OpenSSL the signature algorithm is
		 * bound to the digest algorithm, so the NID completely
		 * defines the signature scheme. This the only message
		 * not validated by signature, but its timestamps are
		 * not used by NTP.
		 *
		 * Discard the message if the status word has already
		 * been received or the response does not match the
		 * request..
		 */
		case CRYPTO_ASSOC | CRYPTO_RESP:
			if (peer->crypto || !is_org)
				break;
			vp = (struct value *)&pkt[2];
			fstamp = ntohl(vp->fstamp);
			vallen = ntohl(vp->vallen);
			rval = XEVNT_OK;

			/*
			 * We require only that the host name length be
			 * reasonable and the signature digest NID be
			 * valid. Timestamps and filestamps are
			 * irrelevant at this time.
			 */
			temp32 = fstamp >> 16;
			if (vallen < MIN_HOSTLEN || vallen >
			    MAX_HOSTLEN)
				rval = XEVNT_SBJ;
			else if ((dp = EVP_get_digestbynid(temp32)) ==
			    NULL)
				rval = XEVNT_MD;
			if (rval != XEVNT_OK) {
				report_event(rval, peer);
				peer->flash |= TEST12;
				break;
			}

			/*
			 * Save signature scheme and host name.
			 */
			(const EVP_MD *)peer->digest = dp;
			peer->crypto = fstamp;
			peer->keystr = emalloc(vallen + 1);
			memcpy(peer->keystr, vp->pkt, vallen);
			peer->keystr[vallen] = '\0';
			sprintf(statstr, "host %s %s (%u)",
			    peer->keystr, OBJ_nid2ln(temp32), temp32);
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Decode X509 certificate in ASN.1 format and extract
		 * the data containing, among other things, subject
		 * name and public key.
		 *
		 * Discard the message if the status word has not been
		 * received or the certificate has already been received
		 * or the response does not match the request.
		 */
		case CRYPTO_CERT | CRYPTO_RESP:
			if (!peer->crypto || peer->crypto &
			    CRYPTO_FLAG_PROV || !is_org)
				break;
			vp = (struct value *)&pkt[2];
			tstamp = ntohl(vp->tstamp);
			fstamp = ntohl(vp->fstamp);
			vallen = ntohl(vp->vallen);
			rval = XEVNT_OK;

			/*
			 * We require valid certificate, signature
			 * length and signature. The certificate subject
			 * name must match the previous host name and
			 * the public key used to verify the (self-
			 * signed) signature. The timestamp must be
			 * within the valid interval according to the
			 * certificate.
			 *
			 * In principle, we do not have to sign this
			 * message, since the certificate contains only
			 * public values. We do it anyway, since it
			 * serves as a good checksum if the certificate
			 * is not self-signed.
			 */
			j = 5 + (vallen + 3) / 4;
			siglen = ntohl(pkt[j++]);
			dp = (EVP_MD *)peer->digest;
			xp = NULL;
			if (tstamp == 0 || tstamp < fstamp) {
				rval = XEVNT_TSP;
			} else if ((xp = cert_parse((u_char *)vp->pkt,
			    vallen)) == NULL) {
				rval = XEVNT_PUB;
			} else if ((kp = xp->pkey) == NULL) {
				rval = XEVNT_PUB;
			} else if (strcmp(xp->subject, peer->keystr) !=
			    0) {
				rval = XEVNT_SBJ;
			} else if (tstamp < xp->first || tstamp >
			    xp->last) {
				rval = XEVNT_PER;
			} else if (siglen > EVP_PKEY_size(kp)) {
				rval = XEVNT_SGL;
			} else {
				EVP_VerifyInit(&ctx, dp);
				EVP_VerifyUpdate(&ctx, (u_char *)vp,
				    vallen + 12);
				if (!EVP_VerifyFinal(&ctx,
				    (u_char *)&pkt[j], siglen, kp))
					rval = XEVNT_SIG;
			}
			if (rval != XEVNT_OK) {
				if (xp != NULL)
					cert_free(xp);
				if (rval != XEVNT_TSP) {
					report_event(rval, peer);
					peer->flash |= TEST12;
				}
				break;
			}

			/*
			 * Install certificate variables and light the
			 * proventic bit. If this certificate updates a
			 * previous onw, a filestamp may have changed,
			 * so recompute the signatures.
			 */
			peer->cinfo.tstamp = vp->tstamp;
			peer->cinfo.fstamp = vp->fstamp;
			peer->cinfo.vallen = sizeof(struct cert_info);
			if (peer->cinfo.ptr != NULL)
				cert_free((struct cert_info *)
				    peer->cinfo.ptr);
			peer->cinfo.ptr = (u_char *)xp;
			crypto_sign();
			peer->crypto |= CRYPTO_FLAG_PROV;
			peer->flash &= ~TEST10;
			temp32 = ((struct cert_info *)
			    peer->cinfo.ptr)->nid;
			sprintf(statstr, "cert %s %s (%u) fs %u",
			    xp->subject, OBJ_nid2ln(temp32), temp32,
			    ntohl(vp->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Roll a random cookie and install in symmetric mode.
		 * Encrypt for the response, which is transmitted later.
		 *
		 * Discard the message if the certificate has not been
		 * received or the cookie has alread been received or
		 * the response does not match the request or another
		 * response has already been queued.
		 */
		case CRYPTO_COOK:
			if (!(peer->crypto & CRYPTO_FLAG_PROV) ||
			    peer->crypto & CRYPTO_FLAG_AGREE ||
			    !is_org || peer->cmmd != NULL)
				break;
			if ((vp = crypto_verify(pkt, &peer->cookval,
			    peer)) == NULL)
				break;

			ptr32 = emalloc(len);
			memcpy(ptr32, pkt, len);
			temp32 = CRYPTO_RESP;
			*ptr32 |= htonl(temp32);
			RAND_bytes((u_char *)&cookie, 4);
			value_free(&peer->encrypt);
			if (!crypto_encrypt(&peer->encrypt, vp,
			    &cookie)) {
				report_event(XEVNT_CRYPT, peer);
				free(ptr32);
				peer->flash |= TEST12;
				break;
			}
			peer->cmmd = ptr32;

			/*
			 * Install cookie values and light the cookie
			 * bit.
			 */
			key_expire(peer);
			peer->cookval.tstamp = vp->tstamp;
			peer->cookval.fstamp = vp->fstamp;
			peer->pcookie = cookie;
			peer->crypto &= ~CRYPTO_FLAG_AUTO;
			peer->crypto |= CRYPTO_FLAG_AGREE;
			peer->flash &= ~TEST10;
			sprintf(statstr, "cryp %x ts %u fs %u",
			    peer->pcookie, ntohl(vp->tstamp),
			    ntohl(vp->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Decrypt and install session cookie in client and
		 * symmetric modes. If the cookie bit is set, the
		 * working cookie is the EXOR of the current and new
		 * values.
		 *
		 * Discard the response if the certificate has not been
		 * received or the response does not match the request.
		 */
		case CRYPTO_COOK | CRYPTO_RESP:
			if (!(peer->crypto & CRYPTO_FLAG_PROV) ||
			    !is_org)
				break;
			if ((vp = crypto_verify(pkt, &peer->cookval,
			    peer)) == NULL)
				break;

			/*
			 * Decrypt the cookie.
			 */
			vallen = ntohl(vp->vallen);
			if (vallen == EVP_PKEY_size(rsa_pkey)) {
				RSA_private_decrypt(vallen,
				    (u_char *)vp->pkt,
				    (u_char *)&temp32,
				    rsa_pkey->pkey.rsa,
				    RSA_PKCS1_OAEP_PADDING);
				cookie = ntohl(temp32);
			} else {
				report_event(XEVNT_LEN, peer);
				peer->flash |= TEST12;
				break;
			}

			/*
			 * Install cookie values and light the cookie
			 * bit. If this is not broadcast client mode, we
			 * are done here.
			 */
			key_expire(peer);
			peer->cookval.tstamp = vp->tstamp;
			peer->cookval.fstamp = vp->fstamp;
			if (peer->crypto & CRYPTO_FLAG_AGREE)
				peer->pcookie ^= cookie;
			else
				peer->pcookie = cookie;
			if (peer->hmode == MODE_CLIENT &&
			    !(peer->cast_flags & MDF_BCLNT))
				peer->crypto |= CRYPTO_FLAG_AUTO;
			else
				peer->crypto &= ~CRYPTO_FLAG_AUTO;
			peer->crypto |= CRYPTO_FLAG_AGREE;
			peer->flash &= ~TEST10;
			sprintf(statstr, "cryp %x ts %u fs %u",
			    peer->pcookie, ntohl(vp->tstamp),
			    ntohl(vp->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Install autokey values in broadcast client and
		 * symmetric modes. We have to do this every time the
		 * sever/peer cookie changes or a new keylist is
		 * rolled. Ordinarily, this is automatic as this message
		 * is piggybacked on the first NTP packet sent upon
		 * either of these events.
		 *
		 * Discard the response if the certificate has not been
		 * received. A broadcast client or symmetric peer can
		 * receive this response without a matching request.
		 */
		case CRYPTO_AUTO | CRYPTO_RESP:
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;
			if ((vp = crypto_verify(pkt, &peer->recval,
			    peer)) == NULL)
				break;

			/*
			 * Install autokey values and light the
			 * autokey bit.
			 */
			if (peer->recval.ptr == NULL)
				peer->recval.ptr =
				    emalloc(sizeof(struct autokey));
			bp = (struct autokey *)peer->recval.ptr;
			peer->recval.tstamp = vp->tstamp;
			peer->recval.fstamp = vp->fstamp;
			ap = (struct autokey *)vp->pkt;
			bp->seq = ntohl(ap->seq);
			bp->key = ntohl(ap->key);
			peer->pkeyid = bp->key;
			peer->crypto |= CRYPTO_FLAG_AUTO;
			peer->flash &= ~TEST10;
			sprintf(statstr,
			    "auto seq %d key %x ts %u fs %u", bp->seq,
			    bp->key, ntohl(vp->tstamp),
			    ntohl(vp->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Install leapseconds table. This table is
		 * proventicated to the NIST primary servers, either by
		 * copying the file containing the table from a NIST
		 * server to a trusted server or directly using this
		 * protocol. While the entire table is installed at the
		 * server, presently only the current TAI offset is
		 * provided via the kernel to other applications.
		 *
		 * Discard the request if the certificate has not been
		 * received or the leapseconds table has already been
		 * received or the response does not match the request
		 * or another response has already been queued. Discard
		 * the response if the certificate has not been received
		 * or the response does not match the request.
		 */
		case CRYPTO_TAI:
			if (!(peer->crypto & CRYPTO_FLAG_PROV) ||
			    peer->crypto & CRYPTO_FLAG_LEAP ||
			    !is_org || peer->cmmd != NULL)
				break;
			ptr32 = emalloc(len);
			memcpy(ptr32, pkt, len);
			temp32 = CRYPTO_RESP;
			*ptr32 |= htonl(temp32);
			peer->cmmd = ptr32;
			if (len <= VALUE_LEN)
				break;
			/* fall through */

		case CRYPTO_TAI | CRYPTO_RESP:
			if (!(peer->crypto & CRYPTO_FLAG_PROV) ||
			    !is_org)
				break;
			if ((vp = crypto_verify(pkt, &peer->tai_leap,
			   peer)) == NULL)
				break;

			/*
			 * Initialize peer variables, leapseconds
			 * structure and extension field in network byte
			 * order. Since a filestamp may have changed,
			 * recompute the signatures.
			 */
			peer->tai_leap.tstamp = vp->tstamp;
			peer->tai_leap.fstamp = vp->fstamp;
			peer->tai_leap.vallen = vp->vallen;

			/*
			 * Install the new table if there is no stored
			 * table or the new table is more recent than
			 * the stored table. Since a filestamp may have
			 * changed, recompute the signatures.
			 */
			vallen = ntohl(vp->vallen);
			if (ntohl(peer->tai_leap.fstamp) >
			    ntohl(tai_leap.fstamp)) {
				tai_leap.fstamp = vp->fstamp;
				tai_leap.vallen = vp->vallen;
				if (tai_leap.ptr != NULL)
					free(tai_leap.ptr);
				tai_leap.ptr = emalloc(vallen);
				memcpy(tai_leap.ptr, vp->pkt, vallen);
				crypto_sign();
				sys_tai = vallen / 4 + TAI_1972 - 1;
			}
			crypto_flags |= CRYPTO_FLAG_TAI;
			peer->crypto |= CRYPTO_FLAG_LEAP;
			peer->flash &= ~TEST10;
#ifdef KERNEL_PLL
#if NTP_API > 3
			/*
			 * If the kernel cooperates, initialize the
			 * current TAI offset.
			 */
			ntv.modes = MOD_TAI;
			ntv.constant = sys_tai;
			if (ntp_adjtime(&ntv) == TIME_ERROR)
				msyslog(LOG_ERR,
				    "kernel TAI update failed");
#endif /* NTP_API */
#endif /* KERNEL_PLL */
			sprintf(statstr, "leap %u ts %u fs %u",
			    vallen, ntohl(vp->tstamp),
			    ntohl(vp->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * For other requests, save the request code for later;
		 * for unknown responses or errors, flash the error bit
		 * and just ignore for now.
		 */
		default:
			if (code & (CRYPTO_RESP | CRYPTO_ERROR))
				break;
			if (peer->cmmd != NULL)
				break;
			ptr32 = emalloc(len);
			memcpy(ptr32, pkt, len);
			temp32 = CRYPTO_RESP;
			*ptr32 |= htonl(temp32);
			peer->cmmd = ptr32;
			break;
		}
		authlen += len;
	}
	return;
}


/*
 * crypto_xmit - construct extension fields
 *
 * This routine is called both when an association is configured and
 * when one is not. The only case where this matters is to retrieve the
 * autokey information, in which case the caller has to provide the
 * association ID to match the association.
 */
int				/* return length of extension field */
crypto_xmit(
	struct pkt *xpkt,	/* transmit packet pointer */
	int	start,		/* offset to extension field */
	u_int32	*epkt,		/* extension field pointer*/
	keyid_t cookie,		/* session cookie */
	u_int	associd		/* association ID */
	)
{
	u_int32	*pkt;		/* packet pointer */
	struct peer *peer;	/* peer structure pointer */
	u_int	opcode;		/* extension field opcode */
	struct value *vp;	/* transmit packet value pointer */
	struct value *ep, *pp;	/* value structure pointer */
	u_int	len;
	struct value encrypt;
	keyid_t tcookie;

	/*
	 * Generate the requested extension field request code, length
	 * and association ID.
	 */
	pkt = (u_int32 *)xpkt + start / 4;
	opcode = ntohl(epkt[0]);
	pkt[1] = htonl(associd);
	len = 8;
	vp = (struct value *)&pkt[2];
	ep = (struct value *)&epkt[2];
	switch (opcode & 0xffff0000) {

	/*
	 * Send association, status and crypto info. Note, this message
	 * is not signed.
	 */
	case CRYPTO_ASSOC | CRYPTO_RESP:
		len += crypto_send(vp, &host);
		vp->fstamp = htonl(crypto_flags);
		break;

	/*
	 * Send certificate and signature.
	 */
	case CRYPTO_CERT | CRYPTO_RESP:
		len += crypto_send(vp, &cinfo);
		break;

	/*
	 * Send public key and signature.
	 */
	case CRYPTO_COOK:
		len += crypto_send(vp, &pubkey);
		break;

	/*
	 * Encrypt and send cookie and signature.
	 */
	case CRYPTO_COOK | CRYPTO_RESP:
		if ((opcode & 0xffff) < VALUE_LEN) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		if (PKT_MODE(xpkt->li_vn_mode) == MODE_SERVER) {
			pp = &encrypt;
			memset(pp, 0, sizeof(struct value));
			tcookie = cookie;
			if (crypto_encrypt(pp, ep, &tcookie)) {
				len += crypto_send(vp, pp);
			} else {
				opcode |= CRYPTO_ERROR;
				break;
			}
		} else {
			if ((peer = findpeerbyassoc(associd)) == NULL) {
				opcode |= CRYPTO_ERROR;
				break;
			}
			pp = &peer->encrypt;
		}
		len += crypto_send(vp, pp);
		value_free(pp);
		break;

	/*
	 * Find peer and send autokey data and signature in broadcast
	 * server and symmetric modes. If no association is found,
	 * either the server has restarted with new associations or some
	 * perp has replayed an old message.
	 */
	case CRYPTO_AUTO | CRYPTO_RESP:
		if ((peer = findpeerbyassoc(associd)) == NULL) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		peer->flags &= ~FLAG_ASSOC;
		len += crypto_send(vp, &peer->sndval);
		break;

	/*
	 * Send leapseconds table and signature.
	 */
	case CRYPTO_TAI:
	case CRYPTO_TAI | CRYPTO_RESP:
		if (crypto_flags & CRYPTO_FLAG_TAI) {
			len += crypto_send(vp, &tai_leap);
			break;
		}
		/* fall through */

	/*
	 * Default - Fall through for requests; for unknown responses,
	 * flag as error.
	 */
	default:
		if (opcode & CRYPTO_RESP)
			opcode |= CRYPTO_ERROR;
		break;
	}

	/*
	 * Round up the field length to a multiple of 8 octets and save
	 * the request code and length.
	 */
	len = ((len + 7) / 8) * 8;
	pkt[0] = htonl((opcode & 0xffff0000) | len);
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_xmit: ext offset %d len %u code %x assocID %d\n",
		    start, len, opcode>> 16, associd);
#endif
	return (len);
}


/*
 * crypto_encrypt - encrypt cookie and construct response message. 
 */
static int
crypto_encrypt(
	struct value *vp,	/* value structure pointer */
	struct value *ep,	/* value structure pointer */
	keyid_t	*cookie		/* server cookie */
	)
{
	EVP_PKEY *rkey;		/* public key */
	EVP_MD_CTX ctx;		/* signature context */
	l_fp	tstamp;		/* NTP timestamp */
	u_char	*ptr;
	u_int	len;
	u_int32	temp32;

	/*
	 * Extract the public key from the request.
	 */
	ptr = (u_char *)ep->pkt;
	if ((rkey = d2i_PublicKey(EVP_PKEY_RSA, NULL, &ptr,
	    ntohl(ep->vallen))) == NULL) {
		msyslog(LOG_ERR, "crypto_recv %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (0);
	}

	/*
	 * Encrypt the cookie, construct and sign the response.
	 */
	L_CLR(&tstamp);
	if (sys_leap != LEAP_NOTINSYNC)
		get_systime(&tstamp);
	vp->tstamp = htonl(tstamp.l_ui);
	vp->fstamp = host.fstamp;
	len = EVP_PKEY_size(rkey);
	vp->vallen = htonl(len);
	vp->ptr = emalloc(len);
	temp32 = htonl(*cookie);
	if (!RSA_public_encrypt(4, (u_char *)&temp32, vp->ptr,
	    rkey->pkey.rsa, RSA_PKCS1_OAEP_PADDING)) {
		msyslog(LOG_ERR, "crypto_recv %s",
		    ERR_error_string(ERR_get_error(), NULL));
		EVP_PKEY_free(rkey);
		return (0);
	}
	EVP_PKEY_free(rkey);
	vp->siglen = 0;
	vp->sig = emalloc(sign_siglen);
	EVP_SignInit(&ctx, digest);
	EVP_SignUpdate(&ctx, (u_char *)vp, 12);
	EVP_SignUpdate(&ctx, vp->ptr, len);
	if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey))
		vp->siglen = htonl(len);
	return (1);
}


/*
 * crypto_verify - parse and verify the packet and value structure
 */
static struct value *
crypto_verify(
	u_int32	*pkt,		/* packet pointer */
	struct value *up,	/* value structure pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	EVP_MD_CTX ctx;		/* signature context */
	EVP_PKEY *kp;		/* key pointer */
	struct	value *vp;	/* value pointer */
	struct cert_info *xp;	/* certificate information */
	tstamp_t tstamp;	/* timestamp */
	tstamp_t fstamp;	/* filestamp */
	u_int	vallen;		/* value length */
	u_int	siglen;		/* signature length */
	u_char	statstr[NTP_MAXSTRLEN];
	u_int	code, len;
	int	rval;
	int	i;

	/*
	 * We require valid field length, timestamp, public key,
	 * signature length and signature. A valid timestamp is nonzero
	 * and greater than the last timestamp received. Duplicates are
	 * valid if the autokey protocol has not completed. In addition,
	 * timestamps must be within the valid period of the
	 * certificate.
	 */
	len = ntohl(pkt[0]) & 0x0000ffff;
	code = ntohl(pkt[0]) & 0xffff0000;
	rval = XEVNT_OK;
	vp = (struct value *)&pkt[2];
	tstamp = ntohl(vp->tstamp);
	fstamp = ntohl(vp->fstamp);
	vallen = ntohl(vp->vallen);
	i = (vallen + 3) / 4;
	siglen = ntohl(vp->pkt[i++]);
	if (!peer->crypto || code & CRYPTO_ERROR) {
		rval = XEVNT_LEN;
	} else if (len < VALUE_LEN || len < VALUE_LEN + vallen +
	    siglen) {
		rval = XEVNT_LEN;
	} else if (tstamp == 0 || tstamp < ntohl(up->tstamp) ||
	    (tstamp == ntohl(up->tstamp) && (peer->crypto &
	    CRYPTO_FLAG_AUTO))) {
		rval = XEVNT_TSP;
	} else if (tstamp < ntohl(up->fstamp) || fstamp <
	    ntohl(up->fstamp)) {
		rval = XEVNT_FSP;
	} else if (!(peer->crypto & CRYPTO_FLAG_PROV)) {
		rval = XEVNT_PUB;
	} else if ((xp = (struct cert_info *)peer->cinfo.ptr) == NULL) {
		rval = XEVNT_PUB;
	} else if (tstamp < xp->first || tstamp > xp->last) {
		rval = XEVNT_TSP;
	} else if ((kp = xp->pkey) == NULL) {
		rval = XEVNT_PUB;
	} else if (siglen > EVP_PKEY_size(kp)) {
		rval = XEVNT_SGL;
	} else {
		EVP_VerifyInit(&ctx, (EVP_MD *)peer->digest);
		EVP_VerifyUpdate(&ctx, (u_char *)vp, vallen + 12);
		if (!EVP_VerifyFinal(&ctx, (u_char *)&vp->pkt[i],
		    siglen, kp))
			rval = XEVNT_SIG;
	}
#ifdef DEBUG
	if (debug > 1)
		printf(
		    "crypto_recv: verify %x vallen %u siglen %u ts %u fs %u\n",
		    rval, vallen, siglen, tstamp, fstamp);
#endif
	if (rval != XEVNT_OK) {
		sprintf(statstr, "error %x code %x ts %u fs %u", rval,
		    code, tstamp, fstamp);
		if (rval != XEVNT_TSP) {
			record_crypto_stats(&peer->srcadr, statstr);
			report_event(rval, peer);
		}
		peer->flash |= TEST12;
#ifdef DEBUG
		if (debug)
			printf("crypto_verify: %s\n", statstr);
#endif
		return (NULL);
	}
	return (vp);
}


/*
 * crypto_send - concatenate value and signature elements in extension
 * field. We are very conservative here; Jon would be proud.
 */
u_int
crypto_send(
	struct value *pp,	/* packet pointer */
	struct value *vp	/* value structure pointer */
	)
{
	u_int	len, temp32;
	int	i;

	/*
	 * Copy data. If the data field is empty or zero length, return
	 * a value field length of zero.
	 */
	pp->tstamp = vp->tstamp;
	pp->fstamp = vp->fstamp;
	pp->vallen = vp->vallen;
	len = 12;
	temp32 = ntohl(vp->vallen);
	if (temp32 == 0 || vp->ptr == NULL)
		return (len);
	memcpy(&pp->pkt[0], vp->ptr, temp32);

	/*
	 * Copy signature. If the signature field is empty or zero
	 * length, return an empty  signature with field length of zero.
	 */
	i = (temp32 + 3) / 4;
	len += i * 4 + 4;
	pp->pkt[i++] = vp->siglen;
	temp32 = ntohl(vp->siglen);
	if (temp32 == 0 || vp->sig == NULL)
		return (len);
	memcpy(&pp->pkt[i], vp->sig, temp32);
	len += temp32;
	return (len);
}


/*
 * crypto_sign - compute new public value and sign extension fields.
 * Note that the signature length will be zero if something breaks in
 * here.
 */
void
crypto_sign(void)
{
	EVP_MD_CTX ctx;		/* signature context */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	l_fp	tstamp;		/* NTP timestamp */
	u_int	len;

	/*
	 * This routine runs periodically, like once a day, and when a
	 * filestamp changes. It updates the autokey private value and
	 * the timestamps on four value structures:
	 *
	 * host		public/private key file
	 * pubkey	public key
	 * cinfo	certificate file
	 * tai_leap	ERTS leapseconds file
	 *
	 * Filestamps are proventicated data, so this routine is run
	 * only when the host has been synchronized to a proventicated
	 * source(s). Thus, the timestamp is proventicated, too, and can
	 * be used to deflect clogging attacks and cook breakfast.
	 */
	if (sys_leap == LEAP_NOTINSYNC)
		return;
	get_systime(&tstamp);
	host.tstamp = htonl(tstamp.l_ui);

	/*
	 * Sign certificate and timestamps. The filestamp is derived
	 * from the certificate file extension from wherever the file
	 * was generated.
	 */
	if (cinfo.vallen != 0) {
		cinfo.tstamp = host.tstamp;
		cinfo.siglen = 0;
		if (cinfo.sig == NULL)
			cinfo.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, digest);
		EVP_SignUpdate(&ctx, (u_char *)&cinfo, 12);
		EVP_SignUpdate(&ctx, cinfo.ptr, ntohl(cinfo.vallen));
		if (EVP_SignFinal(&ctx, cinfo.sig, &len, sign_pkey))
			cinfo.siglen = htonl(len);
	}

	/*
	 * Sign public key and timestamps. The filestamp is derived from
	 * the certicate filestamp.
	 */
	if (pubkey.vallen != 0) {
		pubkey.tstamp = host.tstamp;
		pubkey.siglen = 0;
		if (pubkey.sig == NULL)
			pubkey.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, digest);
		EVP_SignUpdate(&ctx, (u_char *)&pubkey, 12);
		EVP_SignUpdate(&ctx, pubkey.ptr, ntohl(pubkey.vallen));
		if (EVP_SignFinal(&ctx, pubkey.sig, &len, sign_pkey))
			pubkey.siglen = htonl(len);
	}

	/*
	 * Sign leapseconds table and timestamps. The filestamp is
	 * derived from the leapsecond file extension from wherever the
	 * file was generated.
	 */
	if (tai_leap.vallen != 0) {
		tai_leap.tstamp = host.tstamp;
		tai_leap.siglen = 0;
		if (tai_leap.sig == NULL)
			tai_leap.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, digest);
		EVP_SignUpdate(&ctx, (u_char *)&tai_leap, 12);
		EVP_SignUpdate(&ctx, tai_leap.ptr,
		    ntohl(tai_leap.vallen));
		if (EVP_SignFinal(&ctx, tai_leap.sig, &len, sign_pkey))
			tai_leap.siglen = htonl(len);
	}
	sprintf(statstr, "sign ts %u", ntohl(host.tstamp)); 
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("cypto_sign: %s\n", statstr);
#endif
}


/*
 * crypto_key - load key. Used for both host key and sign key.
 */
static EVP_PKEY *
crypto_key(
	char	*cp,		/* file name */
	struct value *vp	/* value structure pointer */
	)
{
	FILE	*str;		/* file handle */
	EVP_PKEY *pkey;		/* public/private key */
	char	filename[MAXFILENAME]; /* name of rsa key file */
	char	linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t fstamp;		/* filestamp */
	int	rval;
	u_char	*ptr;

	/*
	 * Open the key file. If the first character of the file
	 * name is not '/', prepend the keys directory string. If
	 * something goes wrong, abandon ship.
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	str = fopen(filename, "r");
	if (str == NULL)
		return (NULL);

	/*
	 * Read PEM-encoded key.
	 */
	pkey = PEM_read_PrivateKey(str, NULL, NULL, NULL);
	fclose(str);
	if (pkey == NULL) {
		msyslog(LOG_ERR, "crypto_key %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}

	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		ptr = strrchr(linkname, '.');
	} else {
		ptr = strrchr(filename, '.');
	}
	if (ptr != NULL)
		sscanf(++ptr, "%u", &fstamp);
	else
		fstamp = 0;
	vp->fstamp = htonl(fstamp);
	sprintf(statstr, "%s link %d fs %u mod %d", cp, rval, fstamp,
	    EVP_PKEY_size(pkey) * 8);
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("crypto_key: %s\n", statstr);
	if (debug > 1) {
		if (EVP_MD_type(pkey) == EVP_PKEY_DSA)
			DSA_print_fp(stdout, pkey->pkey.dsa, 0);
		else
			RSA_print_fp(stdout, pkey->pkey.rsa, 0);
	}
#endif
	return (pkey);
}


/*
 * crypto_cert - load X.509 RSA or DSA certificate.
 */
static struct cert_info *	/* certificate information */
crypto_cert(
	char	*cp		/* file name */
	)
{
	struct cert_info *ret; /* certificate information */
	FILE	*str;		/* file handle */
	char	filename[MAXFILENAME]; /* name of certificate file */
	char	linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t fstamp;	/* filestamp */
	int	rval;
	u_int	len;
	u_char	*ptr;
	char	*name, *header;
	u_char	*data;

	/*
	 * Open the certificate file. If the first character of the file
	 * name is not '/', prepend the keys directory string. If
	 * something goes wrong, abandon ship.
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	str = fopen(filename, "r");
	if (str == NULL)
		return (NULL);

	/*
	 * Read PEM-encoded certificate and install.
	 */
	if (!PEM_read(str, &name, &header, &data, (long *)&len)) {
		msyslog(LOG_ERR, "crypto_cert %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}
	free(header);
	if (strcmp(name, "CERTIFICATE") !=0) {
		msyslog(LOG_ERR, "crypto_cert wrong PEM type %s",
		    name);
		free(name);
		free(data);
		return (NULL);
	}
	free(name);

	/*
	 * Parse certificate for errors.
	 */
	ret = cert_parse(data, len);
	if (ret == NULL) {
		msyslog(LOG_ERR, "crypto_cert invalid certificate");
		free(data);
		return (NULL);
	}
	ret->cert = data;
	ret->cert_len = len;

	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		ptr = strrchr(linkname, '.');
	} else {
		ptr = strrchr(filename, '.');
	}
	if (ptr != NULL)
		sscanf(++ptr, "%u", &fstamp);
	else
		fstamp = 0;
	ret->fstamp = fstamp;
	sprintf(statstr, "%s link %d fs %u len %u", cp, rval, fstamp,
	    len);
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("crypto_cert: %s\n", statstr);
#endif
	return (ret);
}


/*
 * crypto_tai - load leapseconds table
 */
static void
crypto_tai(
	char	*cp		/* file name */
	)
{
	FILE	*str;		/* file handle */
	u_char	buf[NTP_MAXSTRLEN];	/* file line buffer */
	u_int	leapsec[MAX_LEAP]; /* NTP time at leaps */
	u_int	offset;		/* offset at leap (s) */
	char	filename[MAXFILENAME]; /* name of leapseconds file */
	char	linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t fstamp;	/* filestamp */
	u_int	len;
	u_char	*ptr;
	int	rval, i;
#ifdef KERNEL_PLL
#if NTP_API > 3
	struct timex ntv;	/* kernel interface structure */
#endif /* NTP_API */
#endif /* KERNEL_PLL */

	/*
	 * Open the file and discard comment lines. If the first
	 * character of the file name is not '/', prepend the keys
	 * directory string. If the file is not found, not to worry; it
	 * can be retrieved over the net. But, if it is found with
	 * errors, we crash and burn.
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	if ((str = fopen(filename, "r")) == NULL)
		return;

	/*
	 * We are rather paranoid here, since an intruder might cause a
	 * coredump by infiltrating naughty values. Empty lines and
	 * comments are ignored. Other lines must begin with two
	 * integers followed by junk or comments. The first integer is
	 * the NTP seconds of leap insertion, the second is the offset
	 * of TAI relative to UTC after that insertion. The second word
	 * must equal the initial insertion of ten seconds on 1 January
	 * 1972 plus one second for each succeeding insertion.
	 */
	i = 0;
	rval = XEVNT_OK;
	while (i < MAX_LEAP) {
		ptr = fgets(buf, NTP_MAXSTRLEN - 1, str);
		if (ptr == NULL)
			break;
		if (strlen(buf) < 1)
			continue;
		if (*buf == '#')
			continue;
		if (sscanf(buf, "%u %u", &leapsec[i], &offset) != 2)
			continue;
		if (i != offset - TAI_1972) { 
			rval = XEVNT_DAT;
			break;
		}
		i++;
	}
	fclose(str);
	if (rval != XEVNT_OK || i == 0) {
		msyslog(LOG_ERR,
		    "crypto_tai leapseconds file %s error %d", cp,
		    rval);
		exit (-1);
	}

	/*
	 * The extension field table entries consists of the NTP seconds
	 * of leap insertion in reverse order, so that the most recent
	 * insertion is the first entry in the table.
	 */
	len = i * 4;
	tai_leap.vallen = htonl(len);
	ptr = emalloc(len);
	tai_leap.ptr = ptr;
	for (; i >= 0; i--) {
		*ptr++ = htonl(leapsec[i]);
	}
	crypto_flags |= CRYPTO_FLAG_TAI;
	sys_tai = len / 4 + TAI_1972 - 1;
#ifdef KERNEL_PLL
#if NTP_API > 3
	ntv.modes = MOD_TAI;
	ntv.constant = sys_tai;
	if (ntp_adjtime(&ntv) == TIME_ERROR)
		msyslog(LOG_ERR, "crypto kernel TAI update failed");
#endif /* NTP_API */
#endif /* KERNEL_PLL */

	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		ptr = strrchr(linkname, '.');
	} else {
		ptr = strrchr(filename, '.');
	}
	if (ptr != NULL)
		sscanf(++ptr, "%u", &fstamp);
	else
		fstamp = 0;
	tai_leap.fstamp = htonl(fstamp);
	sprintf(statstr, "%s link %d fs %u offset %u", cp, rval, fstamp,
	    ntohl(tai_leap.vallen) / 4 + TAI_1972 - 1);
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("crypto_tai: %s\n", statstr);
#endif
}


/*
 * cert_parse - parse x509 certificate and create information structure.
 */
struct cert_info *		/* certificate information structure */
cert_parse(
	u_char	*asn1cert,	/* X509 certificate */
	u_int	len		/* certificate length */
	)
{
	struct cert_info *ret;
	u_char	pathbuf[MAXFILENAME];
	X509	*cert;

	/*
	 * Decode ASN.1 objects and construct certificate structure.
	 */
	if ((cert = d2i_X509(NULL, &asn1cert, len)) == NULL) {
		msyslog(LOG_ERR, "cert_parse %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}

	/*
	 * Extract public key, signature and message digest algorithms.
	 */
	ret = emalloc(sizeof(struct cert_info));
	memset(ret, 0, sizeof(struct cert_info));
	if ((ret->pkey = X509_get_pubkey(cert)) == NULL) {
		msyslog(LOG_ERR, "cert_parse %s",
		    ERR_error_string(ERR_get_error(), NULL));
		cert_free(ret);
		return (NULL);
	}

	/*
	 * Extract remaining objects. Note that the NTP serial number is
	 * the NTP seconds at the time of signing, but this might not be
	 * the case for other authority. We don't bother to check the
	 * objects at this time, since the real crunch can happen only
	 * when the time is valid but not yet certificated.
	 */
	ret->cert_version = X509_get_version(cert);
	ret->nid = OBJ_obj2nid(cert->cert_info->signature->algorithm);
	ret->serial =
	    (u_long)ASN1_INTEGER_get(X509_get_serialNumber(cert));
	X509_NAME_oneline(X509_get_subject_name(cert), pathbuf,
	    MAXFILENAME);
	ret->subject = emalloc(strlen(pathbuf) - 3);
	strcpy(ret->subject, &pathbuf[4]);
	X509_NAME_oneline(X509_get_issuer_name(cert), pathbuf,
	    MAXFILENAME);
	ret->issuer = emalloc(strlen(pathbuf) - 3);
	strcpy(ret->issuer, &pathbuf[4]);
	ret->first = asn2ntp(X509_get_notBefore(cert));
	ret->last = asn2ntp(X509_get_notAfter(cert));

	/*
	 * Verify self-signed certificate. This goes away someday.
	 */
	if (strcmp(ret->subject, ret->issuer) == 0 && !X509_verify(cert,
	    ret->pkey)) {
		msyslog(LOG_ERR, "signature not verified %s",
		    ret->subject);
		cert_free(ret);
		return (NULL);
	}
#if DEBUG
	if (debug > 1)
		X509_print_fp(stdout, cert);
#endif
	X509_free(cert);
	return (ret);
}


/*
 * cert_free - free certificate information structure.
 */
void
cert_free(
	struct cert_info *cinf	/* certificate information structure */ 
	)
{
	if (cinf->pkey != NULL)
		EVP_PKEY_free(cinf->pkey);
	if (cinf->subject != NULL)
		free(cinf->subject);
	if (cinf->issuer != NULL)
		free(cinf->issuer);
	if (cinf->cert != NULL)
		free(cinf->cert);
	free(cinf);
}

/*
 * value_free - free value structure components.
 */
void
value_free(
	struct value *vp	/* value structure */
	)
{
	if (vp->ptr != NULL)
		free(vp->ptr);
	if (vp->sig != NULL)
		free(vp->sig);
	memset(vp, 0, sizeof(struct value));
}


/*
 * asn2ntp - convert ASN1_TIME time structure to NTP time
 */
u_long
asn2ntp	(
	ASN1_TIME *asn1time	/* pointer to ASN1_TIME structure */
	)
{
	char	*v;		/* pointer to ASN1_TIME string */
	struct	tm tm;		/* used to convert to NTP time */

	/*
	 * Extract time string YYMMDDHHMMSSZ from ASN1 time structure.
	 * Note that the YY, MM, DD fields start with one, the HH, MM,
	 * SS fiels start with zero and the Z character should be 'Z'
	 * for UTC. Also note that years less than 50 map to years
	 * greater than 100. Dontcha love ASN.1? Better than MIL-188.
	 */
	if (asn1time->length > 13)
		return (-1);
	v = (char *)asn1time->data;
	tm.tm_year = (v[0] - '0') * 10 + v[1] - '0';
	if (tm.tm_year < 50)
		tm.tm_year += 100;
	tm.tm_mon = (v[2] - '0') * 10 + v[3] - '0' - 1;
	tm.tm_mday = (v[4] - '0') * 10 + v[5] - '0';
	tm.tm_hour = (v[6] - '0') * 10 + v[7] - '0';
	tm.tm_min = (v[8] - '0') * 10 + v[9] - '0';
	tm.tm_sec = (v[10] - '0') * 10 + v[11] - '0';
	tm.tm_wday = 0;
	tm.tm_yday = 0;
	tm.tm_isdst = 0;
	return (mktime(&tm) + JAN_1970);
}


/*
 * crypto_setup - load required rsa key, certificate and optional
 * leapseconds table. Initialize extension fields for later signatures.
 */
void
crypto_setup(void)
{
	struct cert_info *cinf;	/* certificate information */
	char	filename[MAXFILENAME]; /* name of rsa key file */
	l_fp	tstamp;		/* NTP timestamp */
	u_int	len, bytes;
	u_char	*ptr;

	/*
	 * Initialize structures.
	 */
	if (!crypto_flags)
		return;

	gethostname(filename, MAXFILENAME);
	bytes = strlen(filename) + 1;
	sys_hostname = emalloc(bytes);
	memcpy(sys_hostname, filename, bytes);
	memset(&host, 0, sizeof(host));
	memset(&pubkey, 0, sizeof(pubkey));
	memset(&cinfo, 0, sizeof(cinfo));
	memset(&tai_leap, 0, sizeof(tai_leap));

	/*
	 * Load required random seed file and seed the random number
	 * generator. Wiggle it a bit and write it back so the sequence
	 * does not repeat when we next restart.
	 */
	ERR_load_crypto_strings();
	if (rand_file == NULL) {
		if ((RAND_file_name(filename, MAXFILENAME)) != NULL) {
			rand_file = emalloc(strlen(filename) + 1);
			strcpy(rand_file, filename);
		}
	} else if (*rand_file != '/') {
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir,
		    rand_file);
		free(rand_file);
		rand_file = emalloc(strlen(filename) + 1);
		strcpy(rand_file, filename);
	}
	if (rand_file == NULL) {
		msyslog(LOG_ERR,
		    "crypto_setup random seed file not specified");
		exit (-1);
	}
	if ((bytes = RAND_load_file(rand_file, -1)) == 0) {
		msyslog(LOG_ERR, "crypto_setup %s",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}
	get_systime(&tstamp);
	RAND_seed(&tstamp, sizeof(tstamp));
	RAND_write_file(rand_file);
	OpenSSL_add_all_algorithms();
#ifdef DEBUG
	if (debug)
		printf("crypto_setup: %d bytes read from random seed file %s\n",
		    bytes, rand_file);
#endif

	/*
	 * Load required rsa key, default "ntpkey_key_<hostname>".
	 */
	if (rsa_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_key_%s",
		    sys_hostname);
		rsa_file = emalloc(strlen(filename) + 1);
		strcpy(rsa_file, filename);
	}
	if ((rsa_pkey = crypto_key(rsa_file, &host)) == NULL) {
		msyslog(LOG_ERR,
		    "rsa key file %s not found or corrupt",
		    rsa_file);
		exit (-1);
	}
	if (EVP_MD_type(rsa_pkey) != EVP_PKEY_RSA) {
		msyslog(LOG_ERR,
		    "rsa key file %s wrong type", rsa_file);
		exit (-1);
	}
	host.vallen = htonl(strlen(sys_hostname));
	host.ptr = sys_hostname;
	crypto_flags |= CRYPTO_FLAG_KEY;
	
	/*
	 * Construct public key extension field for agreement scheme.
	 * Note: The agreement scheme requires an RSA key.
	 */
	len = i2d_PublicKey(rsa_pkey, NULL);
	ptr = emalloc(len + 1);
	pubkey.ptr = ptr;
	i2d_PublicKey(rsa_pkey, &ptr);
	pubkey.vallen = htonl(len);
	pubkey.fstamp = htonl(host.fstamp);

	/*
	 * Load optional sign key, default "ntpkey_sign_<hostname>".
	 */
	if (sign_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_sign_%s",
		    sys_hostname);
		sign_file = emalloc(strlen(filename) + 1);
		strcpy(sign_file, filename);
	}
	if ((sign_pkey = crypto_key(sign_file, &cinfo)) != NULL) {
		crypto_flags |= CRYPTO_FLAG_SIGN;
	} else {
		sign_pkey = rsa_pkey;
		cinfo.fstamp = host.fstamp;
	}
	sign_siglen = EVP_PKEY_size(sign_pkey);

	/*
	 * Load required certificate, default "ntpkey_cert_<hostname>".
	 */
	if (cert_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_cert_%s",
		    sys_hostname);
		cert_file = emalloc(strlen(filename) + 1);
		strcpy(cert_file, filename);
	}
	if ((cinf = crypto_cert(cert_file)) == NULL) {
		msyslog(LOG_ERR,
		    "certificate file %s not found or corrupt",
		    cert_file);
		exit (-1);
	}
	cinfo.ptr = emalloc(cinf->cert_len);
	memcpy(cinfo.ptr, cinf->cert, cinf->cert_len);
	cinfo.vallen = htonl(cinf->cert_len);
	if (cinf->fstamp != htonl(cinfo.fstamp)) {
		msyslog(LOG_ERR,
		    "certificate file %s does not match sign key file",
		    cert_file);
		exit (-1);
	}
	cinfo.fstamp = htonl(cinf->fstamp);
	crypto_flags |= cinf->nid << 16;
	digest = EVP_get_digestbynid(cinf->nid);
	cert_free(cinf);

	/*
	 * Load optional leapseconds table, default "ntpkey_leap". If
	 * the file is missing or defective, the values can later be
	 * retrieved from a server.
	 */
	if (leap_file == NULL)
		leap_file = "ntpkey_leap";
	crypto_tai(leap_file);
#ifdef DEBUG
	if (debug)
		printf("crypto_setup: flags 0x%x signature scheme %s\n",
		    crypto_flags, OBJ_nid2ln(EVP_MD_pkey_type(digest)));
#endif
}


/*
 * crypto_config - configure crypto data from crypto configuration
 * command.
 */
void
crypto_config(
	int	item,		/* configuration item */
	char	*cp		/* file name */
	)
{
	switch (item) {

	/*
	 * Set random seed file name.
	 */
	case CRYPTO_CONF_RAND:
		rand_file = emalloc(strlen(cp) + 1);
		strcpy(rand_file, cp);
		break;

	/*
	 * Set rsa key file name.
	 */
	case CRYPTO_CONF_PRIV:
		rsa_file = emalloc(strlen(cp) + 1);
		strcpy(rsa_file, cp);
		break;

	/*
	 * Set sign key file name.
	 */
	case CRYPTO_CONF_SIGN:
		sign_file = emalloc(strlen(cp) + 1);
		strcpy(sign_file, cp);
		break;

	/*
	 * Set certificate file name.
	 */
	case CRYPTO_CONF_CERT:
		cert_file = emalloc(strlen(cp) + 1);
		strcpy(cert_file, cp);
		break;

	/*
	 * Set leapseconds file name.
	 */
	case CRYPTO_CONF_LEAP:
		leap_file = emalloc(strlen(cp) + 1);
		strcpy(leap_file, cp);
		break;

	/*
	 * Set crypto keys directory.
	 */
	case CRYPTO_CONF_KEYS:
		keysdir = emalloc(strlen(cp) + 1);
		strcpy(keysdir, cp);
		break;
	}
	crypto_flags = CRYPTO_FLAG_ENAB;
}
# else
int ntp_crypto_bs_pubkey;
# endif /* OPENSSL */
