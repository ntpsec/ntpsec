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

#include "openssl/asn1_mac.h"
#include "openssl/bn.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/rand.h"

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
 * signatursiglene and/or value fields can be empty, in which case the
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
#define VALUE_LEN	(6 * 4) /* min response field length */
#define YEAR		(60 * 60 * 24 * 365) /* seconds in year */
#define NTP_RANDFILE	"/.rnd"	/* OpenSSL random seed file */

/*
 * Global cryptodata in host byte order
 */
u_int32	crypto_flags = 0x0;	/* status word */
u_int	sys_tai;		/* current UTC offset from TAI */

/*
 * Global cryptodata in network byte order
 */
struct cert_info *cinfo = NULL;	/* certificate info/value */
struct value hostval;		/* host value */
struct value pubkey;		/* public key */
struct value tai_leap;		/* leapseconds table */

/*
 * Private cryptodata in host byte order
 */
static EVP_PKEY *rsa_pkey = NULL; /* host key */
static EVP_PKEY *sign_pkey = NULL; /* sign key */
static const EVP_MD *sign_digest = NULL; /* sign digest */
static u_int sign_siglen;	/* sign key length */
static char *keysdir = NTP_KEYSDIR; /* crypto keys directory */
static char *rand_file = NTP_RANDFILE; /* random seed file */
static char *rsa_file = NULL;	/* rsa key file */
static char *sign_file = NULL;	/* sign key file */
static char *cert_file = NULL;	/* certificate file */
static char *leap_file = NULL;	/* leapseconds file */

/*
 * Cryptotypes
 */
static	int	crypto_verify	P((struct exten *, struct value *,
				    struct peer *));
static	int	crypto_encrypt	P((struct exten *, struct value *,
				    keyid_t *));
static	u_int	crypto_send	P((struct exten *, struct value *));
static	tstamp_t crypto_time	P((void));
static	u_long	asn2ntp		P((ASN1_TIME *));
static	struct cert_info *cert_parse P((u_char *, u_int, tstamp_t));
static	int	cert_sign	P((struct exten *, struct value *));
static	int	cert_valid	P((struct cert_info *, EVP_PKEY *));
static	int	cert_install	P((struct exten *, struct peer *));
static	void	cert_free	P((struct cert_info *));
static	EVP_PKEY *crypto_key	P((char *, tstamp_t *));
static	struct cert_info *crypto_cert P((char *));
static	void	crypto_tai	P((char *));

/*
 * session_key - generate session key
 *
 * This routine generates a session key from the source address,
 * destination address, key ID and private value. The value of the
 * session key is the MD5 hash of these values, while the next key ID is
 * the first four octets of the hash.
 *
 * Returns the next key ID
 */
keyid_t
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
	tstamp_t tstamp;	/* NTP timestamp */
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
	tstamp = crypto_time();
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
	 * careful not to use invalid key media. Use the public values
	 * timestamp as filestamp. 
	 */
	vp = &peer->sndval;
	if (vp->ptr == NULL)
		vp->ptr = emalloc(sizeof(struct autokey));
	ap = (struct autokey *)vp->ptr;
	ap->seq = htonl(peer->keynumber);
	ap->key = htonl(keyid);
	vp->tstamp = htonl(tstamp);
	vp->fstamp = hostval.tstamp;
	vp->vallen = htonl(sizeof(struct autokey));
	vp->siglen = 0;
	if (vp->tstamp != 0) {
		if (vp->sig == NULL)
			vp->sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)vp, 12);
		EVP_SignUpdate(&ctx, vp->ptr, sizeof(struct autokey));
		if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey))
			vp->siglen = htonl(len);
		peer->flags |= FLAG_ASSOC;
	}
#ifdef DEBUG
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
	struct recvbuf *rbufp	/* packet buffer pointer */
	)
{
	const EVP_MD *dp;	/* message digest algorithm */
	u_int32	*pkt;		/* receive packet pointer */
	struct autokey *ap, *bp; /* autokey pointer */
	struct exten *ep, *fp;	/* extension pointers */
	int	has_mac;	/* length of MAC field */
	int	authlen;	/* offset of MAC field */
	associd_t associd;	/* association ID */
	tstamp_t tstamp = 0;	/* timestamp */
	tstamp_t fstamp = 0;	/* filestamp */
	u_int	len;		/* extension field length */
	u_int	code;		/* extension field opcode */
	u_int	vallen = 0;		/* value length */
	struct pkt *rpkt;	/* NTP header pointer */
	X509	*cert;		/* X509 certificate */
	u_char	*ptr;
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	keyid_t	cookie;		/* crumbles */
	int	rval;
	u_int32 temp32;
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
	rpkt = &rbufp->recv_pkt;
	authlen = LEN_PKT_NOMAC;
	while ((has_mac = rbufp->recv_length - authlen) > MAX_MAC_LEN) {
		pkt = (u_int32 *)&rbufp->recv_pkt + authlen / 4;
		ep = (struct exten *)pkt;
		code = ntohl(ep->opcode) & 0xffff0000;
		len = ntohl(ep->opcode) & 0x0000ffff;
		associd = ntohl(pkt[1]);
#ifdef DEBUG
		if (debug)
			printf(
			    "crypto_recv: flags 0x%x ext offset %d len %u code %x assocID %d\n",
			    peer->crypto, authlen, len, code >> 16,
			    associd);
#endif

		/*
		 * Check version number and field length.
		 */
		if (((code >> 24) & 0x3f) != CRYPTO_VN || len < 8 ||
		    (len < VALUE_LEN && (code & CRYPTO_RESP))) {
			sys_unknownversion++;
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: incorrect version or field length\n");
#endif
			peer->flash |= TEST12;
			code |= CRYPTO_ERROR;;
		}
		if (associd != 0)
			peer->assoc = associd;
		if (len >= VALUE_LEN) {
			tstamp = ntohl(ep->tstamp);
			fstamp = ntohl(ep->fstamp);
			vallen = ntohl(ep->vallen);
		} else {
			tstamp = fstamp = 0;
			vallen = 0;
		}
		rval = XEVNT_OK;
		switch (code) {

		/*
		 * Install status word, host name, signature scheme and
		 * association ID. In OpenSSL the signature algorithm is
		 * bound to the digest algorithm, so the NID completely
		 * defines the signature scheme. This the only message
		 * not validated by signature.
		 */
		case CRYPTO_ASSOC | CRYPTO_RESP:

			/*
			 * Discard the response if the status word has
			 * already been stored. Discard the response if
			 * it has invalid length or does not match the
			 * request.
			 */
			if (peer->crypto || len < VALUE_LEN + vallen)
				break;

			/*
			 * Discard the response and report an error if
			 * the host name length is unreasonable or the
			 * signature digest NID is not supported.
			 */
			temp32 = fstamp >> 16;
			dp =
			    (const EVP_MD *)EVP_get_digestbynid(temp32);
			if (vallen < MIN_HOSTLEN || vallen >
			    MAX_HOSTLEN)
				rval = XEVNT_LEN;
			else if (dp == NULL)
				rval = XEVNT_MD;
			if (rval != XEVNT_OK) {
				report_event(rval, peer);
				peer->flash |= TEST12;
				break;
			}

			/*
			 * Save status word, host name and message
			 * digest.
			 */
			peer->digest = dp;
			peer->crypto = fstamp;
			peer->subject = emalloc(vallen + 1);
			memcpy(peer->subject, ep->pkt, vallen);
			peer->subject[vallen] = '\0';
			sprintf(statstr, "host %s %s (%u)",
			    peer->subject, OBJ_nid2ln(temp32), temp32);
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
		 */
		case CRYPTO_CERT | CRYPTO_RESP:

			/*
			 * Discard the message if not verified. Scan the
			 * certificate list to delete old versions and
			 * link the newest version first on the list.
			 */
			if (crypto_verify(ep, NULL, peer) != XEVNT_OK)
				break;

			rval = cert_install(ep, peer);
			if (rval != XEVNT_OK) {
				if (rval != XEVNT_TSP) {
					report_event(rval, peer);
					peer->flash |= TEST12;
				}
				break;
			}

			/*
			 * If the subject name matches the issuer name,
			 * this is self-signed certificate. If from a
			 * primary (stratum 1) server, it is ipso facto
			 * valid. If valid, recharge the burst count.
			 */
			if (strcmp(cinfo->subject, cinfo->issuer) ==
			    0 && PKT_TO_STRATUM(rpkt->stratum) <= 1)
				cinfo->flags |= CERT_VALID;
			if (cinfo->flags & CERT_VALID) {
				peer->crypto |= CRYPTO_FLAG_PROV;
				if (peer->burst > 0)
					peer->burst = NTP_BURST;
			}

			/*
			 * We plug in the first public key we find, as
			 * this is the first certificate sent. However,
			 * note that this certificate might not be
			 * signed by the server, so we can't check the
			 * signature/digest NID.
			 */
			if (peer->pkey == NULL) {
				ptr = (u_char *)cinfo->cert.ptr;
				cert = d2i_X509(NULL, &ptr,
				    ntohl(cinfo->cert.vallen));
				peer->pkey = X509_get_pubkey(cert);
				X509_free(cert);
				peer->issuer =
				    emalloc(strlen(cinfo->issuer) + 1);
				strcpy(peer->issuer, cinfo->issuer);
			}
			peer->flash &= ~TEST10;
			temp32 = cinfo->nid;
			sprintf(statstr, "cert %s %s (%u) fs %u",
			    cinfo->subject, OBJ_nid2ln(temp32), temp32,
			    ntohl(ep->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * X509 certificate sign request in symmetric modes.
		 * Validate the certificate and pass to transmit for
		 * signature. Note that the request can only be sent by
		 * a proventicated machine and thus always has a value
		 * component in the extension field.
		 */
		case CRYPTO_SIGN:

			/*
			 * discard the message if not certified or not
			 * verifed.
			 */
			if (crypto_verify(ep, NULL, peer) != XEVNT_OK)
				break;

			if (crypto_verify(ep, NULL, peer) != XEVNT_OK)
				break;

			/*
			 * Pass the extension field to the transmit
			 * side, but only if has a value field.
			 */
			if (len < VALUE_LEN)
				break;
			fp = emalloc(len);
			memcpy(fp, ep, len);
			temp32 = CRYPTO_RESP;
			fp->opcode |= htonl(temp32);
			peer->cmmd = fp;
			break;

		/*
		 * X509 certificate sign response. Validate the
		 * certificate signed by the server and install. Later
		 * this can be provided to clients of this server in
		 * lieu of the self-signed certificate in order to
		 * validate the public key.
		 */
		case CRYPTO_SIGN | CRYPTO_RESP:

			/*
			 * Discard the message if not certified or not
			 * verified with respect to the public key
			 * values. Scan the certificate list to delete
			 * old versions and link the newest version
			 * first on the list.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			if (crypto_verify(ep, &pubkey, peer) !=
			    XEVNT_OK)
				break;

			rval = cert_install(ep, peer);
			if (rval != XEVNT_OK) {
				if (rval != XEVNT_TSP) {
					report_event(rval, peer);
					peer->flash |= TEST12;
				}
				break;
			}
			peer->flash &= ~TEST10;
			temp32 = cinfo->nid;
			sprintf(statstr, "sign %s %s (%u) fs %u",
			    cinfo->issuer, OBJ_nid2ln(temp32), temp32,
			    ntohl(ep->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Cookie request in symmetric modes. Roll a random
		 * cookie and install in symmetric mode. Encrypt for the
		 * response, which is transmitted later.
		 */
		case CRYPTO_COOK:

			/*
			 * Discard the message if not certified or not
			 * verified.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			if (crypto_verify(ep, NULL, peer) != XEVNT_OK)
				break;

			/*
			 * Pass the extension field to the transmit
			 * side, but only if it has a value field.
			 */
			if (len < VALUE_LEN + vallen)
				break;
			peer->flash &= ~TEST10;
			fp = emalloc(len);
			memcpy(fp, ep, len);
			temp32 = CRYPTO_RESP;
			fp->opcode |= htonl(temp32);
			peer->cmmd = fp;
			if (peer->crypto & CRYPTO_FLAG_AGREE)
				break;

			/*
			 * Install cookie values and light the cookie
			 * bit. The transmit side will pick up and
			 * encrypt it for the response.
			 */
			key_expire(peer);
			peer->cookval.tstamp = ep->tstamp;
			peer->cookval.fstamp = ep->fstamp;
			RAND_bytes((u_char *)&peer->pcookie, 4);
			peer->crypto &= ~CRYPTO_FLAG_AUTO;
			peer->crypto |= CRYPTO_FLAG_AGREE;
			peer->flash &= ~TEST10;
			sprintf(statstr, "cryp %x ts %u fs %u",
			    peer->pcookie, ntohl(ep->tstamp),
			    ntohl(ep->fstamp));
			record_crypto_stats(&peer->srcadr, statstr);
#ifdef DEBUG
			if (debug)
				printf("crypto_recv: %s\n", statstr);
#endif
			break;

		/*
		 * Cookie response in client and symmetric modes. If the
		 * cookie bit is set, the working cookie is the EXOR of
		 * the current and new values.
		 */
		case CRYPTO_COOK | CRYPTO_RESP:

			/*
			 * Discard the message if not certified or not
			 * verified with respect to the cookie values.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			else if (crypto_verify(ep, &peer->cookval,
			    peer) != XEVNT_OK)
				break;

			/*
			 * Decrypt the cookie.
			 */
			if (vallen == EVP_PKEY_size(rsa_pkey)) {
				RSA_private_decrypt(vallen,
				    (u_char *)ep->pkt,
				    (u_char *)&temp32,
				    rsa_pkey->pkey.rsa,
				    RSA_PKCS1_OAEP_PADDING);
				cookie = ntohl(temp32);
			} else {
				report_event(XEVNT_CKY, peer);
				peer->flash |= TEST12;
				break;
			}

			/*
			 * Install cookie values and light the cookie
			 * bit. If this is not broadcast client mode, we
			 * are done here.
			 */
			key_expire(peer);
			peer->cookval.tstamp = ep->tstamp;
			peer->cookval.fstamp = ep->fstamp;
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
			    peer->pcookie, ntohl(ep->tstamp),
			    ntohl(ep->fstamp));
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
		 */
		case CRYPTO_AUTO | CRYPTO_RESP:

			/*
			 * Discard the message if not certified or not
			 * verified with respect to the autokey values.
			 * Note that a broadcast client or symmetric
			 * peer can receive this response without a
			 * matching request.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			if (crypto_verify(ep, &peer->recval, peer) !=
			    XEVNT_OK)
				break;

			/*
			 * Install autokey values and light the
			 * autokey bit.
			 */
			if (peer->recval.ptr == NULL)
				peer->recval.ptr =
				    emalloc(sizeof(struct autokey));
			bp = (struct autokey *)peer->recval.ptr;
			peer->recval.tstamp = ep->tstamp;
			peer->recval.fstamp = ep->fstamp;
			ap = (struct autokey *)ep->pkt;
			bp->seq = ntohl(ap->seq);
			bp->key = ntohl(ap->key);
			peer->pkeyid = bp->key;
			peer->crypto |= CRYPTO_FLAG_AUTO;
			peer->flash &= ~TEST10;
			sprintf(statstr,
			    "auto seq %d key %x ts %u fs %u", bp->seq,
			    bp->key, ntohl(ep->tstamp),
			    ntohl(ep->fstamp));
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
		 */
		case CRYPTO_TAI:

			/*
			 * Discard the message if not verified. This
			 * message can be received without a value
			 * component.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			if (len > VALUE_LEN) {
				if (crypto_verify(ep, NULL, peer) !=
				    XEVNT_OK)
					break;

				peer->flash &= ~TEST10;
			}

			/*
			 * Pass the extension field to the transmit
			 * side. Continue below if a leapseconds table
			 * accompanies the message.
			 */
			peer->flash &= ~TEST10;
			fp = emalloc(len);
			memcpy(fp, ep, len);
			temp32 = CRYPTO_RESP;
			fp->opcode |= htonl(temp32);
			peer->cmmd = fp;
			if (len <= VALUE_LEN)
				break;

			/* fall through */

		case CRYPTO_TAI | CRYPTO_RESP:

			/*
			 * Discard the message if not certified or not
			 * verified with respect to the leapseconds
			 * values.
			 */
			if (!(peer->crypto & CRYPTO_FLAG_PROV))
				break;

			if (crypto_verify(ep, &peer->tai_leap, peer) !=
			    XEVNT_OK)
				break;

			/*
			 * Initialize peer variables, leapseconds
			 * structure and extension field in network byte
			 * order. Since a filestamp may have changed,
			 * recompute the signatures.
			 */
			peer->tai_leap.tstamp = ep->tstamp;
			peer->tai_leap.fstamp = ep->fstamp;
			peer->tai_leap.vallen = ep->vallen;

			/*
			 * Install the new table if there is no stored
			 * table or the new table is more recent than
			 * the stored table. Since a filestamp may have
			 * changed, recompute the signatures.
			 */
			if (ntohl(peer->tai_leap.fstamp) >
			    ntohl(tai_leap.fstamp)) {
				tai_leap.fstamp = ep->fstamp;
				tai_leap.vallen = ep->vallen;
				if (tai_leap.ptr != NULL)
					free(tai_leap.ptr);
				tai_leap.ptr = emalloc(vallen);
				memcpy(tai_leap.ptr, ep->pkt, vallen);
				crypto_update();
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
			(void)ntp_adjtime(&ntv);
#endif /* NTP_API */
#endif /* KERNEL_PLL */
			sprintf(statstr, "leap %u ts %u fs %u",
			    vallen, ntohl(ep->tstamp),
			    ntohl(ep->fstamp));
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

			fp = emalloc(len);
			memcpy(fp, ep, len);
			temp32 = CRYPTO_RESP;
			fp->opcode |= htonl(temp32);
			peer->cmmd = fp;
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
 *
 * Returns length of extension field.
 */
int
crypto_xmit(
	struct pkt *xpkt,	/* transmit packet pointer */
	int	start,		/* offset to extension field */
	struct exten *ep,	/* extension pointer */
	keyid_t cookie		/* session cookie */
	)
{
	u_int32	*pkt;		/* packet pointer */
	struct peer *peer;	/* peer structure pointer */
	u_int	opcode;		/* extension field opcode */
	struct exten *fp;	/* extension pointers */
	struct cert_info *cp;	/* certificate info/value pointer */
	char	certname[MAX_HOSTLEN + 1]; /* subject name buffer */
	u_int	vallen;	
	struct value vtemp;
	associd_t associd;
	u_int	len;
	keyid_t tcookie;

	/*
	 * Generate the requested extension field request code, length
	 * and association ID. If this is a response and the host is not
	 * synchronized, light the error bit and go home.
	 */
	pkt = (u_int32 *)xpkt + start / 4;
	fp = (struct exten *)pkt;
	opcode = ntohl(ep->opcode);
	associd = ntohl(ep->associd);
	fp->associd = htonl(associd);
	len = 8;
	if (opcode & CRYPTO_RESP && sys_leap == LEAP_NOTINSYNC)
		opcode |= CRYPTO_ERROR;
	switch (opcode & 0xffff0000) {

	/*
	 * Send association response with status word and crypto info.
	 * Note, this message is not signed and the filestamp contains
	 * only the status word.
	 */
	case CRYPTO_ASSOC | CRYPTO_RESP:
		len += crypto_send(fp, &hostval);
		fp->fstamp = htonl(crypto_flags);
		break;

	/*
	 * Send certificate request. Use the values from the extension
	 * field.
	 */
	case CRYPTO_CERT:
		memset(&vtemp, 0, sizeof(vtemp));
		vtemp.tstamp = ep->tstamp;
		vtemp.fstamp = ep->fstamp;
		vtemp.vallen = ep->vallen;
		vtemp.ptr = (char *)ep->pkt;
		len += crypto_send(fp, &vtemp);
		break;

	/*
	 * Send certificate response or sign request. Use the values
	 * from the certificate. If the request contains no subject
	 * name, assume the name of this host. This is for backwards
	 * compatibility.  Light the error bit if no certificate with
	 * the given subject name is found.
	 */
	case CRYPTO_SIGN:
	case CRYPTO_CERT | CRYPTO_RESP:
		vallen = ntohl(ep->vallen);
		if (vallen == 8) {
			strcpy(certname, sys_hostname);
		} else if (vallen < MIN_HOSTLEN || vallen >
		    MAX_HOSTLEN) {
			opcode |= CRYPTO_ERROR;
			break;

		} else {
			memcpy(certname, ep->pkt, vallen);
			certname[vallen] = '\0';
		}
		for (cp = cinfo; cp != NULL; cp = cp->link) {
			if (strcmp(certname, cp->subject) == 0) {
				len += crypto_send(fp, &cp->cert);
				break;
			}
		}
		if (cp == NULL)
			opcode |= CRYPTO_ERROR;
		break;

	/*
	 * Send certificate sign response. The integrity of the request
	 * certificate has already been verified on the receive side.
	 * Sign the response using the local server key. Use the
	 * filestamp from the request and use the timestamp as the
	 * current time. Light the error bit if the certificate is
	 * invalid or contains an unverified signature.
	 */
	case CRYPTO_SIGN | CRYPTO_RESP:
		if (cert_sign(ep, &vtemp) == XEVNT_OK)
			len += crypto_send(fp, &vtemp);
		else
			opcode |= CRYPTO_ERROR;
		value_free(&vtemp);
		break;

	/*
	 * Send public key and signature. Use the values from the public
	 * key.
	 */
	case CRYPTO_COOK:
		len += crypto_send(fp, &pubkey);
		break;

	/*
	 * Encrypt and send cookie and signature. Light the error bit if
	 * anything goes wrong.
	 */
	case CRYPTO_COOK | CRYPTO_RESP:
		if ((opcode & 0xffff) < VALUE_LEN) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		if (PKT_MODE(xpkt->li_vn_mode) == MODE_SERVER) {
			tcookie = cookie;
		} else {
			if ((peer = findpeerbyassoc(associd)) == NULL) {
				opcode |= CRYPTO_ERROR;
				break;
			}
			tcookie = peer->pcookie;
		}
		if (crypto_encrypt(ep, &vtemp, &tcookie) == XEVNT_OK)
			len += crypto_send(fp, &vtemp);
		else
			opcode |= CRYPTO_ERROR;
		value_free(&vtemp);
		break;

	/*
	 * Find peer and send autokey data and signature in broadcast
	 * server and symmetric modes. Use the values in the autokey
	 * structure. If no association is found, either the server has
	 * restarted with new associations or some perp has replayed an
	 * old message, in which case light the error bit.
	 */
	case CRYPTO_AUTO | CRYPTO_RESP:
		if ((peer = findpeerbyassoc(associd)) == NULL) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		peer->flags &= ~FLAG_ASSOC;
		len += crypto_send(fp, &peer->sndval);
		break;

	/*
	 * Send leapseconds table and signature. Use the values from the
	 * tai structure. If no table has been loaded, just send a
	 * request.
	 */
	case CRYPTO_TAI:
	case CRYPTO_TAI | CRYPTO_RESP:
		if (crypto_flags & CRYPTO_FLAG_TAI)
			len += crypto_send(fp, &tai_leap);
		break;

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
	 * Round up the field length to a multiple of 8 bytes and save
	 * the request code and length.
	 */
	len = ((len + 7) / 8) * 8;
	fp->opcode = htonl((opcode & 0xffff0000) | len);
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_xmit: ext offset %d len %u code %x assocID %d\n",
		    start, len, opcode>> 16, associd);
#endif
	return (len);
}


/*
 * crypto_verify - parse and verify the extension field and value
 *
 * Returns
 * XEVNT_OK	success
 * XEVNT_LEN	bad field format or length
 * XEVNT_TSP	bad timestamp
 * XEVNT_FSP	bad filestamp
 * XEVNT_PUB	bad or missing public key
 * XEVNT_SGL	bad signature length
 * XEVNT_SIG	signature not verified
 */
static int
crypto_verify(
	struct exten *ep,	/* extension pointer */
	struct value *vp,	/* value pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp;	/* timestamp */
	tstamp_t fstamp;	/* filestamp */
	u_int	vallen;		/* value length */
	u_int	siglen;		/* signature length */
	u_char	statstr[NTP_MAXSTRLEN];
	u_int	opcode, len;
	int	rval;
	int	i;

	/*
	 * We require valid opcode and field length, timestamp,
	 * filestamp, public key, signature length and signature, where
	 * relevant. Note that preliminary length checks are done in the
	 * main loop.
	 */
	len = ntohl(ep->opcode) & 0x0000ffff;
	opcode = ntohl(ep->opcode) & 0xffff0000;

	/*
	 * Check for valid operation code and protocol. The opcode must
	 * not have the error bit set. If a request, the response queue
	 * must be empty; if a response, it must match a request
	 * previously sent.
	 */
	if (!peer->crypto || opcode & CRYPTO_ERROR)
		return (XEVNT_LEN);
	else if (!(opcode & CRYPTO_RESP) && peer->cmmd != NULL)
		return (XEVNT_LEN);
	else if (len < VALUE_LEN)
		return (XEVNT_OK);

	/*
	 * Check for valid field length and timestamp. The field length
	 * must be be at long enough to contain the value header, value
	 * and signature. The timestamp must not precede the filestamp.
	 */
	tstamp = ntohl(ep->tstamp);
	fstamp = ntohl(ep->fstamp);
	vallen = ntohl(ep->vallen);
	i = (vallen + 3) / 4;
	siglen = ntohl(ep->pkt[i++]);
	rval = XEVNT_OK;
	if (len < VALUE_LEN + vallen + siglen) {
		return (XEVNT_LEN);
	} else if (tstamp == 0 || tstamp < fstamp) {
		rval = XEVNT_TSP;

	/*
	 * Check for valid timestamp and filestamp in the value
	 * structure given. The timestamp and filestamp must not precede
	 * the corresponding values in the value structure. Once the
	 * autokey values have been installed, the timestamp must always
	 * be later than the corresponding value in the value structure.
	 * Duplicate timestamps are illegal once the cookie has been
	 * received.
	 */
	} else if (vp != NULL && (tstamp < ntohl(vp->tstamp) ||
	    (tstamp == ntohl(vp->tstamp) && (peer->crypto &
	    CRYPTO_FLAG_AUTO)))) {
		rval = XEVNT_TSP;
	} else if (vp != NULL && (tstamp < ntohl(vp->fstamp) || fstamp <
	    ntohl(vp->fstamp))) {
		rval = XEVNT_FSP;

	/*
	 * If a public key is present, check the key length and then
	 * check for valid signature.
	 */ 
	} else if (peer->pkey == NULL) {
		/* fall through */
	} else if (siglen > EVP_PKEY_size(peer->pkey)) {
		rval = XEVNT_SGL;
	} else {
		EVP_VerifyInit(&ctx, peer->digest);
		EVP_VerifyUpdate(&ctx, (u_char *)&ep->tstamp, vallen +
		    12);
		if (!EVP_VerifyFinal(&ctx, (u_char *)&ep->pkt[i],
		    siglen, peer->pkey))
			rval = XEVNT_SIG;
	}
#ifdef DEBUG
	if (debug > 1)
		printf(
		    "crypto_recv: verify %x vallen %u siglen %u ts %u fs %u\n",
		    rval, vallen, siglen, tstamp, fstamp);
#endif

	/*
	 * We ignore opcode, length and timestamp errors, mostly to
	 * crisp the logs and reduce clogging hazards.
	 */
	if (rval > XEVNT_TSP) {
		sprintf(statstr, "error %x code %x ts %u fs %u", rval,
		    opcode, tstamp, fstamp);
		record_crypto_stats(&peer->srcadr, statstr);
		report_event(rval, peer);
		peer->flash |= TEST12;
#ifdef DEBUG
		if (debug)
			printf("crypto_verify: %s\n", statstr);
#endif
	}
	return (rval);
}

/*
 * crypto_encrypt - construct encrypted cookie and signature from
 * extension field and cookie
 *
 * Returns
 * XEVNT_OK	success
 * XEVNT_PUB	bad or missing public key
 * XEVNT_CKY	bad or missing cookie
 */
static int
crypto_encrypt(
	struct exten *ep,	/* extension pointer */
	struct value *vp,	/* value pointer */
	keyid_t	*cookie		/* server cookie */
	)
{
	EVP_PKEY *rkey;		/* public key */
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp;	/* NTP timestamp */
	u_int32	temp32;
	u_int	len;
	u_char	*ptr;

	/*
	 * Extract the public key from the request extensino field.
	 */
	tstamp = crypto_time();
	ptr = (u_char *)ep->pkt;
	len = ntohl(ep->vallen);
	if ((rkey = d2i_PublicKey(EVP_PKEY_RSA, NULL, &ptr, len)) ==
	    NULL) {
		msyslog(LOG_ERR, "crypto_recv %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_PUB);
	}

	/*
	 * Encrypt the cookie, construct and sign the response value.
	 */
	memset(vp, 0, sizeof(struct value));
	vp->tstamp = htonl(tstamp);
	vp->fstamp = hostval.tstamp;
	len = EVP_PKEY_size(rkey);
	vp->vallen = htonl(len);
	vp->ptr = emalloc(len);
	temp32 = htonl(*cookie);
	if (!RSA_public_encrypt(4, (u_char *)&temp32, vp->ptr,
	    rkey->pkey.rsa, RSA_PKCS1_OAEP_PADDING)) {
		msyslog(LOG_ERR, "crypto_recv %s",
		    ERR_error_string(ERR_get_error(), NULL));
		EVP_PKEY_free(rkey);
		return (XEVNT_CKY);
	}
	EVP_PKEY_free(rkey);
	vp->siglen = 0;
	vp->sig = emalloc(sign_siglen);
	EVP_SignInit(&ctx, sign_digest);
	EVP_SignUpdate(&ctx, (u_char *)&vp->tstamp, 12);
	EVP_SignUpdate(&ctx, vp->ptr, len);
	if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey))
		vp->siglen = htonl(len);
	return (XEVNT_OK);
}


/*
 * crypto_args - construct extension field from arguments
 *
 * This routine creates an extension field with current timestamps and
 * specified opcode, association ID and optional string. Note that the
 * extension field is created here, but freed after the crypto_xmit()
 * call in the protocol module.
 *
 * Returns extension field pointer (no errors).
 */
struct exten *
crypto_args(
	u_int	opcode,		/* operation code */
	associd_t associd,	/* association ID */
	u_char	*str		/* argument string */
	)
{
	tstamp_t tstamp;	/* NTP timestamp */
	struct exten *ep;	/* extension field pointer */
	u_int	len;		/* extension field length */

	tstamp = crypto_time();
	len = sizeof(struct exten);
	if (str != NULL)
		len += strlen(str);
	ep = emalloc(len);
	memset(ep, 0, len);
	ep->opcode = htonl(opcode + len);
	ep->associd = htonl(associd);
	ep->tstamp = htonl(tstamp);
	ep->fstamp = hostval.tstamp;
	ep->vallen = 0;
	if (str != NULL) {
		ep->vallen = htonl(strlen(str));
		memcpy(ep->pkt, str, strlen(str));
	}
	return (ep);
}


/*
 * crypto_send - construct extension field from value components
 *
 * Returns extension field length.
 */
u_int
crypto_send(
	struct exten *ep,	/* extension field pointer */
	struct value *vp	/* value pointer */
	)
{
	u_int	len, temp32;
	int	i;

	/*
	 * Copy data. If the data field is empty or zero length, return
	 * a value field length of zero.
	 */
	ep->tstamp = vp->tstamp;
	ep->fstamp = vp->fstamp;
	ep->vallen = vp->vallen;
	len = 12;
	temp32 = ntohl(vp->vallen);
	if (temp32 == 0 || vp->ptr == NULL)
		return (len);
	memcpy(ep->pkt, vp->ptr, temp32);

	/*
	 * Copy signature. If the signature field is empty or zero
	 * length, return an empty signature with field length of zero.
	 */
	i = (temp32 + 3) / 4;
	len += i * 4 + 4;
	ep->pkt[i++] = vp->siglen;
	temp32 = ntohl(vp->siglen);
	if (temp32 == 0 || vp->sig == NULL)
		return (len);
	memcpy(&ep->pkt[i], vp->sig, temp32);
	len += temp32;
	return (len);
}


/*
 * crypto_update - compute new public value and sign extension fields
 *
 * This routine runs periodically, like once a day, and when something
 * changes. It updates the timestamps on three value structures and one
 * value structure list, then signs all the structures:
 *
 * hostval	host name (not signed)
 * pubkey	public key
 * cinfo	certificate info/value list
 * tai_leap	leapseconds file
 *
 * Filestamps are proventicated data, so this routine is run only when
 * the host has been synchronized to a proventicated source. Thus, the
 * timestamp is proventicated, too, and can be used to deflect
 * clogging attacks and even cook breakfast.
 *
 * Returns void (no errors)
 */
void
crypto_update(void)
{
	EVP_MD_CTX ctx;		/* message digest context */
	struct cert_info *cp, **zp; /* certificate info/value */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t tstamp;	/* NTP timestamp */
	u_int	len;

	if ((tstamp = crypto_time()) == 0)
		return;
	hostval.tstamp = htonl(tstamp);

	/*
	 * Sign public key and timestamps. The filestamp is derived from
	 * the host key file extension from wherever the file was
	 * generated. 
	 */
	if (pubkey.vallen != 0) {
		pubkey.tstamp = hostval.tstamp;
		pubkey.siglen = 0;
		if (pubkey.sig == NULL)
			pubkey.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)&pubkey, 12);
		EVP_SignUpdate(&ctx, pubkey.ptr, ntohl(pubkey.vallen));
		if (EVP_SignFinal(&ctx, pubkey.sig, &len, sign_pkey))
			pubkey.siglen = htonl(len);
	}

	/*
	 * Sign certificates and timestamps. The filestamp is derived
	 * from the certificate file extension from wherever the file
	 * was generated. At the same time expired certificates are
	 * expunged.
	 */
	zp = &cinfo;
	for (cp = cinfo; cp != NULL; cp = cp->link) {
		if (tstamp > cp->last) {
			*zp = cp->link;
			cert_free(cp);
		} else {
			cp->cert.tstamp = hostval.tstamp;
			cp->cert.siglen = 0;
			if (cp->cert.sig == NULL)
				cp->cert.sig = emalloc(sign_siglen);
			EVP_SignInit(&ctx, sign_digest);
			EVP_SignUpdate(&ctx, (u_char *)&cp->cert, 12);
			EVP_SignUpdate(&ctx, cp->cert.ptr,
			    ntohl(cp->cert.vallen));
			if (EVP_SignFinal(&ctx, cp->cert.sig, &len,
			    sign_pkey))
				cp->cert.siglen = htonl(len);
		}
		zp = &cp->link;
	}

	/*
	 * Sign leapseconds table and timestamps. The filestamp is
	 * derived from the leapsecond file extension from wherever the
	 * file was generated.
	 */
	if (tai_leap.vallen != 0) {
		tai_leap.tstamp = hostval.tstamp;
		tai_leap.siglen = 0;
		if (tai_leap.sig == NULL)
			tai_leap.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)&tai_leap, 12);
		EVP_SignUpdate(&ctx, tai_leap.ptr,
		    ntohl(tai_leap.vallen));
		if (EVP_SignFinal(&ctx, tai_leap.sig, &len, sign_pkey))
			tai_leap.siglen = htonl(len);
	}
	sprintf(statstr, "sign ts %u", ntohl(hostval.tstamp)); 
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("cypto_sign: %s\n", statstr);
#endif
}


/*
 * value_free - free value structure components.
 *
 * Returns void (no errors)
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
 * crypto_time - returns current NTP time in seconds.
 */
tstamp_t
crypto_time()
{
	l_fp	tstamp;		/* NTP time */	L_CLR(&tstamp);

	L_CLR(&tstamp);
	if (sys_leap != LEAP_NOTINSYNC)
		get_systime(&tstamp);
	return (tstamp.l_ui);
}


/*
 * asn2ntp - convert ASN1_TIME time structure to NTP time in seconds.
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
	return (timegm(&tm) + JAN_1970);
}


/*
 ***********************************************************************
 *								       *
 * The following routines are used to manipulate certificates          *
 *								       *
 ***********************************************************************
 */
/*
 * cert_parse - parse x509 certificate and create info/value structures.
 *
 * The server certificate includes the version number, issuer name,
 * subject name, public key and valid date interval. If the issuer name
 * is the same as the subject name, the certificate is self-signed and
 * valid only if the server is configured as trustable. If the names are
 * different, another issuer has signed the server certificate and
 * vouched for it. In this case the server certificate is valid if
 * verified by the issuer public key.
 *
 * Returns certificate info/value pointer if valid, NULL if not.
 */
struct cert_info *		/* certificate information structure */
cert_parse(
	u_char	*asn1cert,	/* X509 certificate */
	u_int	len,		/* certificate length */
	tstamp_t fstamp		/* filestamp */
	)
{
	X509	*cert;		/* X509 certificate */
	struct cert_info *ret;	/* certificate info/value */
	u_char	pathbuf[MAXFILENAME];
	u_char	*ptr;

	/*
	 * Decode ASN.1 objects and construct certificate structure.
	 */
	ptr = asn1cert;
	if ((cert = d2i_X509(NULL, &ptr, len)) == NULL) {
		msyslog(LOG_ERR, "cert_parse %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}

	/*
	 * Extract version, subject name and public key.
	 */
	ret = emalloc(sizeof(struct cert_info));
	memset(ret, 0, sizeof(struct cert_info));
	if ((ret->pkey = X509_get_pubkey(cert)) == NULL) {
		msyslog(LOG_ERR, "cert_parse %s",
		    ERR_error_string(ERR_get_error(), NULL));
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->version = X509_get_version(cert);

	/*
	 * The default behavior is to assume the certificate valid if
	 * the version is 1 and to require the server to sign it if
	 * later versions. If the trust bit is set, the server is
	 * required to sign it for all versions. This is required for
	 * backward compatibility.
	 */ 
	if (ret->version > 0 && !(crypto_flags & CRYPTO_FLAG_TRUST))
		ret->flags |= CERT_VALID;
	X509_NAME_oneline(X509_get_subject_name(cert), pathbuf,
	    MAXFILENAME - 1);
	if ((ptr = strchr(pathbuf, '=')) == NULL) {
		msyslog(LOG_ERR, "cert_parse: invalid subject %s",
		    pathbuf);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->subject = emalloc(strlen(++ptr) + 1);
	strcpy(ret->subject, ptr);

	/*
	 * Extract remaining objects. Note that the NTP serial number is
	 * the NTP seconds at the time of signing, but this might not be
	 * the case for other authority. We don't bother to check the
	 * objects at this time, since the real crunch can happen only
	 * when the time is valid but not yet certificated.
	 */
	ret->nid = OBJ_obj2nid(cert->cert_info->signature->algorithm);
	ret->digest = (const EVP_MD *)EVP_get_digestbynid(ret->nid);
	ret->serial =
	    (u_long)ASN1_INTEGER_get(X509_get_serialNumber(cert));
	X509_NAME_oneline(X509_get_issuer_name(cert), pathbuf,
	    MAXFILENAME);
	if ((ptr = strchr(pathbuf, '=')) == NULL) {
		msyslog(LOG_ERR, "cert_parse: invalid issuer %s",
		    pathbuf);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->issuer = emalloc(strlen(++ptr) + 1);
	strcpy(ret->issuer, ptr);
	ret->first = asn2ntp(X509_get_notBefore(cert));
	ret->last = asn2ntp(X509_get_notAfter(cert));

	/*
	 * If certificate is self-signed, verify signature.
	 */
	if (strcmp(ret->subject, ret->issuer) == 0) {
		if (!X509_verify(cert, ret->pkey)) {
			msyslog(LOG_ERR,
			    "cert_parse: signature not verified %s",
			    pathbuf);
			cert_free(ret);
			X509_free(cert);
			return (NULL);
		}
	}

	/*
	 * Verify certificate valid times. Note that certificates cannot
	 * be retroactive.
	 */
	if (ret->first > ret->last || ret->first < fstamp) {
		msyslog(LOG_ERR,
		    "certificate signature not valid %s",
		    ret->subject);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}

	/*
	 * Build the value structure to sign and send later.
	 */
	ret->cert.fstamp = htonl(fstamp);
	ret->cert.vallen = htonl(len);
	ret->cert.ptr = emalloc(len);
	memcpy(ret->cert.ptr, asn1cert, len);
#ifdef DEBUG
	if (debug > 1)
		X509_print_fp(stdout, cert);
#endif
	X509_free(cert);
	return (ret);
}


/*
 * cert_sign - sign x509 certificate and update value structure.
 *
 * The certificate request is a copy of the client certificate, which
 * includes the version number, subject name and public key of the
 * client. The resulting certificate includes these values plus the
 * serial number, issuer name and validity interval of the server. The
 * validity interval extends from the current time to the same time one
 * year hence. For NTP purposes, it is convenient to use the NTP seconds
 * of the current time as the serial number.
 *
 * Returns
 * XEVNT_OK	success
 * XEVNT_CRT	bad or missing certificate
 * XEVNT_SIG	signature not verified
 */
static int
cert_sign(
	struct exten *ep,	/* extension field pointer */
	struct value *vp	/* value pointer */
	)
{
	X509	*req;		/* X509 certificate request */
	X509	*cert;		/* X509 certificate */
	ASN1_INTEGER *serial;	/* serial number */
	X509_NAME *subj;	/* distinguished (common) name */
	EVP_PKEY *pkey;		/* public key */
	EVP_MD_CTX ctx;		/* message digest context */
	tstamp_t tstamp;	/* NTP timestamp */
	u_int	len;
	u_char	*ptr;

	/*
	 * Decode ASN.1 objects and construct certificate structure.
	 */
	tstamp = crypto_time();
	ptr = (u_char *)ep->pkt;
	if ((req = d2i_X509(NULL, &ptr, ntohl(ep->vallen))) == NULL) {
		msyslog(LOG_ERR, "cert_sign %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_CRT);
	}
	/*
	 * Extract public key and check for errors.
	 */
	if ((pkey = X509_get_pubkey(req)) == NULL) {
		msyslog(LOG_ERR, "cert_sign %s",
		    ERR_error_string(ERR_get_error(), NULL));
		X509_free(req);
		return (XEVNT_PUB);
	}

	/*
	 * Generate X509 certificate signed by this server. For this
	 * prupose the issuer name is the server name.
	 */
	cert = X509_new();
	X509_set_version(cert, X509_get_version(req));
	serial = ASN1_INTEGER_new();
	ASN1_INTEGER_set(serial, tstamp);
	X509_set_serialNumber(cert, serial);
	X509_gmtime_adj(X509_get_notBefore(cert), 0L);
	X509_gmtime_adj(X509_get_notAfter(cert), YEAR);
	subj = X509_get_issuer_name(cert);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    sys_hostname, strlen(sys_hostname), -1, 0);
	subj = X509_get_subject_name(req);
	X509_set_subject_name(cert, subj);
	X509_set_pubkey(cert, pkey);
	X509_free(req);

	/*
	 * Sign and verify the certificate.
	 */
	X509_sign(cert, sign_pkey, sign_digest);
	if (!X509_verify(cert, sign_pkey)) {
		printf("Verify certificate fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		X509_free(cert);
		return (XEVNT_SIG);
	}
	len = i2d_X509(cert, NULL);

	/*
	 * Build and sign the value structure. We have to sign it here,
	 * since the response has to be returned right away. This is a
	 * clogging hazard.
	 */
	memset(vp, 0, sizeof(struct value));
	vp->tstamp = htonl(tstamp);
	vp->fstamp = ep->fstamp;
	vp->vallen = htonl(len);
	vp->ptr = emalloc(len);
	ptr = vp->ptr;
	i2d_X509(cert, &ptr);
	vp->siglen = 0;
	vp->sig = emalloc(sign_siglen);
	EVP_SignInit(&ctx, sign_digest);
	EVP_SignUpdate(&ctx, (u_char *)vp, 12);
	EVP_SignUpdate(&ctx, vp->ptr, len);
	if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey))
		vp->siglen = htonl(len);
#ifdef DEBUG
	if (debug > 1)
		X509_print_fp(stdout, cert);
#endif
	X509_free(cert);
	return (XEVNT_OK);
}


/*
 * cert_valid - verify certificate with given public key
 *
 * This is pretty ugly, as the certificate has to be verified in the
 * OpenSSL X509 structure, not in the DER format in the info/value
 * structure.
 *
 * Returns
 * XEVNT_OK	success
 * XEVNT_SIG	signature not verified
 */
int
cert_valid(
	struct cert_info *cinf,	/* certificate information structure */
	EVP_PKEY *pkey		/* public key */
	)
{
	X509	*cert;		/* X509 certificate */
	u_char	*ptr;

	if (cinf->flags & CERT_VALID)
		return (XEVNT_OK);
	ptr = (u_char *)cinf->cert.ptr;
	cert = d2i_X509(NULL, &ptr, ntohl(cinf->cert.vallen));
	if (!X509_verify(cert, pkey))
		return (XEVNT_SIG);
	cinf->flags |= CERT_VALID;
	X509_free(cert);
	return (XEVNT_OK);
}


/*
 * cert - install certificate in certificate list
 *
 * This routine encodes an extension field into a certificate info/value
 * structure. It searches the certificate list for duplicates and
 * expunges whichever is older. It then searches the list for other
 * certificates that might be verified by latest one. Finally, it
 * appends the latest certificate to the beginning of the list.
 *
 * Returns
 * XEVNT_OK	success
 * XEVNT_SIG	signature not verified
 * XEVNT_PER	certificate expired
 * XEVNT_CRT	bad or missing certificate 
 */
int
cert_install (
	struct exten *ep,	/* cert info/value */
	struct peer *peer	/* peer structure */
	)
{
	struct cert_info *cp, *xp, *yp, **zp;
	int	rval;
	tstamp_t tstamp;

	/*
	 * Parse and validate the signed certificate. If valid,
	 * construct the info/value structure; otherwise, scamper home.
	 * Note this allows a certificate not-before time to be in the
	 * future, but not a not-after time to be in the past.
	 */
	if ((cp = cert_parse((u_char *)ep->pkt, ntohl(ep->vallen),
	    ntohl(ep->fstamp))) == NULL)
		return (XEVNT_CRT);

	tstamp = crypto_time();
	if (tstamp > cp->last) {
		cert_free(cp);
		return (XEVNT_PER);
	}

	/*
	 * Scan certificate list looking for another certificate with
	 * the same subject and issuer. If another is found with the
	 * same or older filestamp, unlink it and return the goodies to
	 * the heap. If another is found with a later filetsamp, discard
	 * the given one. 
	 */
	yp = cp;
	zp = &cinfo;
	for (xp = cinfo; xp != NULL; xp = xp->link) {
		if (strcmp(cp->subject, xp->subject) == 0 &&
		    strcmp(cp->issuer, xp->issuer) == 0) {
			*zp = xp->link;;
			if (ntohl(cp->cert.fstamp) <=
			    ntohl(xp->cert.fstamp)) {
				cert_free(xp);
			} else {
				cert_free(cp);
				yp = xp;
			}
			break;
		}
		zp = &xp->link;
	}

	/*
	 * Scan the certificate list looking for certificates to
	 * validate. If the subject name of this certificate S matches
	 * the issuer name of of another certificate X and the signature
	 * of X is valid when using the S public key, then X is valid.
	 *
	 * If the subject name of X matches the host name and the
	 * certificate is valid, set the proventic bit and recharge the
	 * burst count.

	 */
	rval = XEVNT_OK;
	for (xp = cinfo; xp != NULL; xp = xp->link) {
		if (strcmp(xp->issuer, yp->subject) != 0)
			continue;

		rval = cert_valid(xp, yp->pkey);
		if (rval != XEVNT_OK)
			break;

		xp->flags |= CERT_VALID;
		if (strcmp(xp->subject, peer->subject) == 0) {
			peer->crypto |= CRYPTO_FLAG_PROV;
				if (peer->burst > 0)
					peer->burst = NTP_BURST;
		} else if (strcmp(xp->subject, sys_hostname) == 0) {
			peer->crypto |= CRYPTO_FLAG_VRFY;
		}
	}

	/*
	 * That was heavy. If we survived this far, the new certificate
	 * goes first on the list.
	 */
	yp->link = cinfo;
	cinfo = yp;
	crypto_update();
	return (rval);
}


/*
 * cert_free - free certificate information structure
 */
void
cert_free(
	struct cert_info *cinf	/* certificate info/value structure */ 
	)
{
	if (cinf->pkey != NULL)
		EVP_PKEY_free(cinf->pkey);
	if (cinf->subject != NULL)
		free(cinf->subject);
	if (cinf->issuer != NULL)
		free(cinf->issuer);
	value_free(&cinf->cert);
	free(cinf);
}


/*
 ***********************************************************************
 *								       *
 * The following routines are used only at initialization time         *
 *								       *
 ***********************************************************************
 */
/*
 * crypto_key - load host and sign keys from file
 *
 * This routine loads a PEM-encoded public/private key pair and extracts
 * the filestamp from the file name.
 *
 * Returns public key pointer if valid, NULL if not. Side effect updates
 * the filestamp if valid.
 */
static EVP_PKEY *
crypto_key(
	char	*cp,		/* file name */
	tstamp_t *fstamp	/* filestamp */
	)
{
	FILE	*str;		/* file handle */
	EVP_PKEY *pkey;		/* public/private key */
	char	filename[MAXFILENAME]; /* name of rsa key file */
	char	linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
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
		sscanf(++ptr, "%u", fstamp);
	else
		*fstamp = 0;
	sprintf(statstr, "%s link %d fs %u mod %d", cp, rval, *fstamp,
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
 * crypto_cert - load certificate from file
 *
 * This routine loads a X.509 RSA or DSA certificate from a file and
 * constructs a info/cert value structure for this machine. The
 * structure includes a filestamp extracted from the file name. Later
 * the certificate can be sent to another machine by request.
 *
 * Returns certificate info/value pointer if valid, NULL if not.
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

	/*
	 * Parse certificate and generate info/value structure.
	 */
	ret = cert_parse(data, len, fstamp);
	free(data);
	if (ret == NULL) {
		msyslog(LOG_ERR, "crypto_cert invalid certificate");
		return (NULL);
	}
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
 * crypto_tai - load leapseconds table from file
 *
 * This routine loads the ERTS leapsecond file in NIST text format,
 * converts to a value structure and extracts a filestamp from the file
 * name. The data are used to establish the TAI offset from UTC, which
 * is provided to the kernel if supported. Later the data can be sent to
 * another machine on request.
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
			break;
		}
		i++;
	}
	fclose(str);
	if (ptr != NULL) {
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
	sprintf(statstr, "%s link %d fs %u offset %u", cp, rval, fstamp,
	    ntohl(tai_leap.vallen) / 4 + TAI_1972 - 1);
	record_crypto_stats(NULL, statstr);
#ifdef DEBUG
	if (debug)
		printf("crypto_tai: %s\n", statstr);
#endif
}

/*
 * crypto_setup - load keys, certificate and leapseconds table
 *
 * This routine loads the public/private host key and certificate. If
 * available, it loads the public/private sign key, which defaults to
 * the host key, and leapseconds table. The host key must be RSA, but
 * the sign key can be either RSA or DSA. In either case, the public key
 * on the certificate must agree with the sign key.
 */
void
crypto_setup(void)
{
	char	filename[MAXFILENAME]; /* name of rsa key file */
	l_fp	seed;		/* crypto PRNG seed as NTP timestamp */
	tstamp_t fstamp;	/* filestamp */
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
	memset(&hostval, 0, sizeof(hostval));
	memset(&pubkey, 0, sizeof(pubkey));
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
	get_systime(&seed);
	RAND_seed(&seed, sizeof(l_fp));
	RAND_write_file(rand_file);
	OpenSSL_add_all_algorithms();
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_setup: %d bytes read from seed file %s\n",
		    bytes, rand_file);
#endif

	/*
	 * Load required host key, default "ntpkey_key_<hostname>".
	 */
	if (rsa_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_key_%s",
		    sys_hostname);
		rsa_file = emalloc(strlen(filename) + 1);
		strcpy(rsa_file, filename);
	}
	if ((rsa_pkey = crypto_key(rsa_file, &fstamp)) == NULL) {
		msyslog(LOG_ERR,
		    "host key file %s not found or corrupt",
		    rsa_file);
		exit (-1);
	}
	hostval.fstamp = htonl(fstamp);
	if (EVP_MD_type(rsa_pkey) != EVP_PKEY_RSA) {
		msyslog(LOG_ERR,
		    "rsa key file %s wrong type", rsa_file);
		exit (-1);
	}
	hostval.vallen = htonl(strlen(sys_hostname));
	hostval.ptr = sys_hostname;
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
	pubkey.fstamp = hostval.fstamp;

	/*
	 * Load optional sign key, default "ntpkey_sign_<hostname>". If
	 * not found, use the host key..
	 */
	if (sign_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_sign_%s",
		    sys_hostname);
		sign_file = emalloc(strlen(filename) + 1);
		strcpy(sign_file, filename);
	}
	if ((sign_pkey = crypto_key(sign_file, &fstamp)) != NULL)
		crypto_flags |= CRYPTO_FLAG_SIGN;
	else
		sign_pkey = rsa_pkey;
	sign_siglen = EVP_PKEY_size(sign_pkey);

	/*
	 * Load required self-signed certificate, default
	 * "ntpkey_cert_<hostname>".
	 */
	if (cert_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_cert_%s",
		    sys_hostname);
		cert_file = emalloc(strlen(filename) + 1);
		strcpy(cert_file, filename);
	}
	if ((cinfo = crypto_cert(cert_file)) == NULL) {
		msyslog(LOG_ERR,
		    "certificate file %s not found or corrupt",
		    cert_file);
		exit (-1);
	}
	if (ntohl(cinfo->cert.fstamp) < ntohl(hostval.fstamp)) {
		msyslog(LOG_ERR,
		    "certificate file %s does not match sign key",
		    cert_file);
		exit (-1);
	}
	sign_digest = cinfo->digest;
	crypto_flags |= cinfo->nid << 16;

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
		    crypto_flags, OBJ_nid2ln(cinfo->nid));
#endif
}


/*
 * crypto_config - configure data from crypto configuration command.
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

	/*
	 * Set certificate trust level. Right now all we do is set the
	 * trust bit; in future things might get much more
	 * sophisticated.
	 */
	case CRYPTO_CONF_TRST:
		crypto_flags |= CRYPTO_FLAG_TRUST;
		break;
	}
	crypto_flags |= CRYPTO_FLAG_ENAB;
}
# else
int ntp_crypto_bs_pubkey;
# endif /* OPENSSL */
