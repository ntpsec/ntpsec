/*
 * ntp_crypto.c - NTP version 4 public key routines
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef AUTOKEY
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_string.h"
#include "ntp_crypto.h"

#ifdef KERNEL_PLL
#include "ntp_syscall.h"
#endif /* KERNEL_PLL */

/*
 * Extension field message formats
 *
 *   +-------+-------+   +-------+-------+   +-------+-------+
 * 0 |   3   |  len  |   |   4   |  len  |   |   5   |  len  |
 *   +-------+-------+   +-------+-------+   +-------+-------+
 * 1 |    assoc ID   |   |    assoc ID   |   |    assoc ID   |
 *   +---------------+   +---------------+   +---------------+
 * 2 |   timestamp   |   |   timestamp   |   |   timestamp   |
 *   +---------------+   +---------------+   +---------------+
 * 3 |   final seq   |   |     cookie    |   |   filestamp   |
 *   +---------------+   +---------------+   +---------------+
 * 4 |   final key   |   | signature len |   |   value len   |
 *   +---------------+   +---------------+   +---------------+
 * 5 | signature len |   |               |   |               |
 *   +---------------+   =   signature   =   =     value     =
 * 6 |               |   |               |   |               |
 *   =   signature   =   +---------------+   +---------------+
 * 7 |               |   CRYPTO_PRIV rsp     | signature len |
 *   +---------------+                       +---------------+
 *   CRYPTO_AUTO rsp                         |               |
 *                                           =   signature   =
 *                                           |               |
 *                                           +---------------+
 *                                           CRYPTO_DHPAR rsp
 *                                           CRYPTO_DH rsp
 *                                           CRYPTO_NAME rsp
 *                                           CRYPTO_TAI rsp
 *                                           
 *   CRYPTO_STAT  1  -    offer/select
 *   CRYPTO_ASSOC 2  8    association ID
 *   CRYPTO_AUTO  3  88   autokey values
 *   CRYPTO_PRIV  4  84   cookie value
 *   CRYPTO_DHPAR 5  220  agreement parameters
 *   CRYPTO_DH    6  152  public value
 *   CRYPTO_NAME  7  460  host name/public key
 *   CRYPTO_TAI   8  144  leapseconds table
 *
 *   Note: requests carry the association ID of the receiver; responses
 *   carry the association ID of the sender.
 */

/*
 * Global cryptodata in host byte order.
 */
u_int	crypto_flags;		/* status word */
u_int	sys_tai;		/* current UTC offset from TAI */

#ifdef PUBKEY
/*
 * Cryptodefines
 */
#define TAI_1972	10	/* initial TAI offset */
#define MAX_LEAP	100	/* max UTC leapseconds */
#define MAX_LINLEN	1024	/* max line */
#define MAX_KEYLEN	1024	/* max key */
#define MAX_ENCLEN	(ENCODED_CONTENT_LEN(1024)) /* max enc key */

/*
 * Private cryptodata in networ byte order.
 */
static R_RSA_PRIVATE_KEY private_key; /* private key */
static R_RSA_PUBLIC_KEY public_key; /* public key */
static R_DH_PARAMS dh_params;	/* agreement parameters */
static u_char *dh_private;	/* private value */
static u_int dh_keyLen;		/* private value length */
static char *keysdir = NTP_KEYSDIR; /* crypto keys directory */
static char *private_key_file = NULL; /* private key file */
static char *public_key_file = NULL; /* public key file */
static char *dh_params_file = NULL; /* agreement parameters file */
static char *tai_leap_file = NULL; /* leapseconds file */

/*
 * Global cryptodata in network byte order
 */
struct value host;		/* host name/public key */
struct value dhparam;		/* agreement parameters */
struct value dhpub;		/* public value */
struct value tai_leap;		/* leapseconds table */

/*
 * Cryptotypes
 */
static	u_int	crypto_rsa	P((char *, u_char *, u_int));
static	void	crypto_dh	P((char *));
static	void	crypto_tai	P((char *));
#endif /* PUBKEY */

/*
 * Autokey protocol status codes
 */
#define RV_OK		0x0	/* success */
#define RV_TSP		0x1	/* invalid timestamp */
#define RV_FSP		0x2	/* invalid filestamp */
#define RV_PUB		0x3	/* missing public key */
#define RV_KEY		0x4	/* invalid RSA modulus */
#define RV_SIG		0x5	/* invalid signature length */
#define RV_DH		0x6	/* invalid agreement parameters */
#define RV_FIL		0x7	/* missing or corrupted key file */
#define RV_DAT		0x8	/* missing or corrupted data */
#define RV_DEC		0x9	/* PEM decoding error */


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
	keyid_t keyno,		/* key ID */
	keyid_t private,	/* private value */
	u_long lifetime 	/* key lifetime */
	)
{
	MD5_CTX ctx;		/* MD5 context */
	keyid_t keyid;		/* key identifer */
	u_int32 header[4];	/* data in network byte order */
	u_char digest[16];	/* message digest */

	/*
	 * Generate the session key and key ID. If the lifetime is
	 * greater than zero, install the key and call it trusted.
	 */
	header[0] = srcadr->sin_addr.s_addr;
	header[1] = dstadr->sin_addr.s_addr;
	header[2] = htonl(keyno);
	header[3] = htonl(private);
	MD5Init(&ctx);
	MD5Update(&ctx, (u_char *)header, sizeof(header));
	MD5Final(digest, &ctx);
	memcpy(&keyid, digest, 4);
	keyid = ntohl(keyid);
	if (lifetime != 0) {
		MD5auth_setkey(keyno, digest, 16);
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
	struct autokey *ap;	/* autokey pointer */
	keyid_t keyid;		/* next key ID */
	keyid_t cookie;		/* private value */
	l_fp tstamp;		/* NTP timestamp */
	u_long ltemp;
	int i;
#ifdef PUBKEY
	R_SIGNATURE_CTX ctx;	/* signature context */
	int rval;		/* return value */
	u_int len;
#endif /* PUBKEY */

	/*
	 * Allocate the key list if necessary.
	 */
	L_CLR(&tstamp);
	if (sys_leap != LEAP_NOTINSYNC)
		get_systime(&tstamp);
	if (peer->keylist == NULL)
		peer->keylist = (keyid_t *)emalloc(sizeof(keyid_t) *
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
	ltemp = sys_automax;
	peer->hcookie = session_key(&dstadr->sin, &peer->srcadr, 0,
	    sys_private, 0);
	if (peer->hmode == MODE_BROADCAST)
		cookie = 0;
	else
		cookie = peer->pcookie.key;
	for (i = 0; i < NTP_MAXSESSION; i++) {
		peer->keylist[i] = keyid;
		peer->keynumber = i;
		keyid = session_key(&dstadr->sin, &peer->srcadr, keyid,
		    cookie, ltemp);
		ltemp -= 1 << peer->kpoll;
		if (auth_havekey(keyid) || keyid <= NTP_MAXKEY ||
		    ltemp <= (1 << (peer->kpoll + 1)))
			break;
	}

	/*
	 * Save the last session key ID, sequence number and timestamp,
	 * then sign these values for later retrieval by the clients. Be
	 * careful not to use invalid key media.
	 */
	ap = &peer->sndauto;
	ap->tstamp = htonl(tstamp.l_ui);
	ap->seq = htonl(peer->keynumber);
	ap->key = htonl(keyid);
	ap->siglen = 0;
#if DEBUG
	if (debug)
		printf("make_keys: %d %08x %08x ts %u\n",
		    ntohl(ap->seq), ntohl(ap->key), cookie,
		    ntohl(ap->tstamp));
#endif
#ifdef PUBKEY
	if(!crypto_flags)
		return;
	if (ap->sig == NULL)
		ap->sig = emalloc(private_key.bits / 8);
	R_SignInit(&ctx, DA_MD5);
	R_SignUpdate(&ctx, (u_char *)ap, 12);
	rval = R_SignFinal(&ctx, ap->sig, &len, &private_key);
	if (rval != RV_OK)
		msyslog(LOG_ERR, "crypto: keylist signature fails %x",
		    rval);
	else
		ap->siglen = htonl(len);
#endif /* PUBKEY */
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
	u_int32 *pkt;		/* packet pointer */
	struct autokey *ap;	/* autokey pointer */
	struct cookie *cp;	/* cookie pointer */
	int has_mac;		/* length of MAC field */
	int authlen;		/* offset of MAC field */
	int len;		/* extension field length */
	u_int code;		/* extension field opcode */
	tstamp_t tstamp;	/* extension field timestamp */
	int i, rval;
	u_int temp;
#ifdef PUBKEY
	R_SIGNATURE_CTX ctx;	/* signature context */
	struct value *vp;	/* value pointer */
	u_char dh_key[MAX_KEYLEN]; /* agreed key */
	R_RSA_PUBLIC_KEY *kp;	/* temporary public key pointer */
	u_int32 *pp;		/* packet pointer */
	u_int rsalen = sizeof(R_RSA_PUBLIC_KEY) - sizeof(u_int) + 4;
	u_int bits;
	int j;
#ifdef KERNEL_PLL
#if NTP_API > 3
	struct timex ntv;	/* kernel interface structure */
#endif /* NTP_API */
#endif /* KERNEL_PLL */
#endif /* PUBKEY */

	/*
	 * Initialize. Note that the packet has already been checked for
	 * valid format and extension field lengths. We first extract
	 * the field length, command code and timestamp in host byte
	 * order. These are used with all commands and modes. We discard
	 * old timestamps and filestamps; but, for duplicate timestamps
	 * we discard only if the authentic bit is set. Cute.
	 */
	pkt = (u_int32 *)&rbufp->recv_pkt;
	authlen = LEN_PKT_NOMAC;
	while ((has_mac = rbufp->recv_length - authlen) > MAX_MAC_LEN) {
		i = authlen / 4;
		len = ntohl(pkt[i]) & 0xffff;
		code = (ntohl(pkt[i]) >> 16) & 0xffff;
		tstamp = ntohl(pkt[i + 2]);
#ifdef DEBUG
		if (debug)
			printf(
			    "crypto_recv: ext offset %d len %d code %x assoc ID %d\n",
			    authlen, len, code, (u_int32)ntohl(pkt[i +
			    1]));
#endif
		switch (code) {

		/*
		 * Install association ID and status word.
		 */
		case CRYPTO_ASSOC | CRYPTO_RESP:
			if (peer->flags & FLAG_AUTOKEY)
				break;
			if (ntohl(pkt[i + 1]) != 0)
				peer->assoc = ntohl(pkt[i + 1]);
			peer->crypto = ntohl(pkt[i + 2]);
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: flags %x\n",
				    peer->crypto);
#endif
			break;

		/*
		 * Install autokey values in broadcast client and
		 * symmetric modes. 
		 */
		case CRYPTO_AUTO | CRYPTO_RESP:
			if (!(peer->flags & FLAG_AUTOKEY) &&
			    ntohl(pkt[i + 1]) != 0)
				peer->assoc = ntohl(pkt[i + 1]);
			ap = (struct autokey *)&pkt[i + 2];
#ifdef PUBKEY
			temp = ntohl(ap->siglen);
			kp = (R_RSA_PUBLIC_KEY *)peer->pubkey.ptr;
			if (tstamp < peer->recauto.tstamp || (tstamp ==
			    peer->recauto.tstamp && (peer->flags &
			    FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else if (!crypto_flags) {
				rval = RV_OK;
			} else if (kp == NULL) {
				rval = RV_PUB;
			} else if (temp != kp->bits / 8) {
				rval = RV_SIG;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)ap, 12);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)ap->pkt, temp, kp);
			}
#else /* PUBKEY */
			if (tstamp < peer->recauto.tstamp || (tstamp ==
			    peer->recauto.tstamp && (peer->flags &
			    FLAG_AUTOKEY)))
				rval = RV_TSP;
			else
				rval = RV_OK;
#endif /* PUBKEY */
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: verify %x autokey %d %08x ts %u (%u)\n",
				    rval, ntohl(ap->seq),
				    ntohl(ap->key), tstamp,
				    peer->recauto.tstamp);
#endif
			if (rval != RV_OK) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x autokey %d %08x ts %u (%u)\n",
					    rval, ntohl(ap->seq),
					    ntohl(ap->key), tstamp,
					    peer->recauto.tstamp);
				break;
			}
			peer->flags |= FLAG_AUTOKEY;
			peer->flash &= ~TEST10;
			peer->assoc = ntohl(pkt[i + 1]);
			peer->recauto.tstamp = tstamp;
			peer->recauto.seq = ntohl(ap->seq);
			peer->recauto.key = ntohl(ap->key);
			peer->pkeyid = peer->recauto.key;
			break;

		/*
		 * Install session cookie in client mode. Use this also
		 * in symmetric modes for test when rsaref20 has not
		 * been installed.
		 */
		case CRYPTO_PRIV:
			peer->cmmd = ntohl(pkt[i]);
			/* fall through */

		case CRYPTO_PRIV | CRYPTO_RESP:
			cp = (struct cookie *)&pkt[i + 2];
#ifdef PUBKEY
			temp = ntohl(cp->siglen);
			kp = (R_RSA_PUBLIC_KEY *)peer->pubkey.ptr;
			if (tstamp < peer->pcookie.tstamp || (tstamp ==
			    peer->pcookie.tstamp && (peer->flags &
			    FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else if (!crypto_flags) {
				rval = RV_OK;
			} else if (kp == NULL) {
				rval = RV_PUB;
			} else if (temp != kp->bits / 8) {
				rval = RV_SIG;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)cp, 8);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)cp->pkt, temp, kp);
			}
#else /* PUBKEY */
			if (tstamp <= peer->pcookie.tstamp || (tstamp ==
			    peer->pcookie.tstamp && (peer->flags &
			    FLAG_AUTOKEY)))
				rval = RV_TSP;
			else
				rval = RV_OK;
#endif /* PUBKEY */

			/*
			 * Tricky here. If in client mode, use the
			 * server cookie; otherwise, use EXOR of both
			 * peer cookies. We call this Daffy-Hooligan
			 * agreement.
			 */
			if (peer->hmode == MODE_CLIENT)
				temp = ntohl(cp->key);
			else
				temp = ntohl(cp->key) ^ peer->hcookie;
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: verify %x cookie %08x ts %u (%u)\n",
				    rval, temp, tstamp,
				    peer->pcookie.tstamp);
#endif
			if (rval != RV_OK) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x cookie %08x ts %u (%u)\n",
					    rval, temp, tstamp,
					    peer->pcookie.tstamp);
					peer->cmmd |= CRYPTO_ERROR;
				break;
			}
			if (!(peer->flags & FLAG_MCAST2))
				peer->flags |= FLAG_AUTOKEY;
			peer->flash &= ~TEST10;
			peer->assoc = ntohl(pkt[i + 1]);
			peer->pcookie.tstamp = tstamp;
			if (temp != peer->pcookie.key) {
				peer->pcookie.key = temp;
				key_expire(peer);
			}
			break;

		/*
		 * The following commands and responses work only when
		 * public-key cryptography has been configured. If
		 * configured, but disabled due to no crypto command in
		 * the configuration file, they are ignored.
		 */
#ifdef PUBKEY
		/*
		 * Install agreement parameters in symmetric modes.
		 */
		case CRYPTO_DHPAR | CRYPTO_RESP:
			if (!crypto_flags)
				break;
			vp = (struct value *)&pkt[i + 2];
			temp = ntohl(vp->vallen);
			kp = (R_RSA_PUBLIC_KEY *)peer->pubkey.ptr;
			j = i + 5 + temp / 4;
			if (kp == NULL) {
				rval = RV_PUB;
			} else if (ntohl(pkt[j]) != kp->bits / 8) {
				rval = RV_SIG;
			} else if (tstamp < ntohl(dhparam.tstamp) ||
			    (tstamp == ntohl(dhparam.tstamp) &&
			    (peer->flags & FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else if (tstamp <= ntohl(dhparam.fstamp) ||
			    ntohl(vp->fstamp) < ntohl(dhparam.fstamp))
			    {
				rval = RV_FSP;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)vp,
				    temp + 12);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)&pkt[j + 1],
				    ntohl(pkt[j]), kp);
			}
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: verify %x parameters %u ts %u fs %u\n",
				    rval, temp, tstamp,
				    ntohl(vp->fstamp));
#endif
			if (rval != RV_OK || temp == 0) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x parameters %u ts %u fs %u\n",
					    rval, temp, tstamp,
					    ntohl(vp->fstamp));
				break;
			}
			peer->flash &= ~TEST10;
			crypto_flags |= CRYPTO_FLAG_DH;

			/*
			 * Initialize agreement parameters and extension
			 * field in network byte order. Note the private
			 * key length is set arbitrarily at half the
			 * prime length.
			 */
			dhparam.tstamp = vp->tstamp;
			dhparam.fstamp = vp->fstamp;
			dhparam.vallen = vp->vallen;
			if (dhparam.ptr != NULL)
				free(dhparam.ptr);
			pp = emalloc(temp);
			dhparam.ptr = (u_char *)pp;
			memcpy(pp, vp->pkt, temp);
			dh_params.primeLen = ntohl(*pp++);
			dh_params.prime = (u_char *)pp;
			pp += dh_params.primeLen / 4;
			dh_params.generatorLen = ntohl(*pp++);
			dh_params.generator = (u_char *)pp;
			dh_keyLen = dh_params.primeLen / 2;
			if (dh_private != NULL)
				free(dh_private);
			dh_private = emalloc(dh_keyLen);
			if (dhparam.sig == NULL)
				dhparam.sig = emalloc(private_key.bits /
				    8);

			/*
			 * Initialize public value extension field.
			 */
			dhpub.tstamp = vp->tstamp;
			dhpub.fstamp = vp->fstamp;
			dhpub.vallen = htonl(dh_params.primeLen);
			if (dhpub.ptr != NULL)
				free(dhpub.ptr);
			dhpub.ptr = emalloc(dh_params.primeLen);
			if (dhpub.sig == NULL)
				dhpub.sig = emalloc(private_key.bits /
				    8);
			crypto_agree();
			break;

		/*
		 * Verify public value and compute agreed key in
		 * symmetric modes. If the filestamp is later than the
		 * current value, we abandon and refill the agreement
		 * parameters.
		 */
		case CRYPTO_DH:
			peer->cmmd = ntohl(pkt[i]);
			if (!crypto_flags)
				peer->cmmd |= CRYPTO_ERROR;
			/* fall through */

		case CRYPTO_DH | CRYPTO_RESP:
			if (!crypto_flags)
				break;
			vp = (struct value *)&pkt[i + 2];
			temp = ntohl(vp->vallen);
			kp = (R_RSA_PUBLIC_KEY *)peer->pubkey.ptr;
			j = i + 5 + temp / 4;
			if (temp != dh_params.primeLen) {
				rval = RV_DH;
			} else if (kp == NULL) {
				rval = RV_PUB;
			} else if (ntohl(pkt[j]) != kp->bits / 8) {
				rval = RV_SIG;
			} else if (tstamp < peer->pcookie.tstamp ||
			    (tstamp == peer->pcookie.tstamp &&
			    (peer->flags & FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)vp,
				    temp + 12);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)&pkt[j + 1],
				    ntohl(pkt[j]), kp);
			}

			/*
			 * Run the agreement algorithm and stash the key
			 * value. We use only the first u_int32 for the
			 * host cookie. Wasteful. If the association ID
			 * is zero, the other guy hasn't seen us as
			 * synchronized, in which case both of us should
			 * be using a zero cookie.
			 */
			if (rval != RV_OK) {
				temp = 0;
			} else if (ntohl(vp->fstamp) > dhparam.fstamp) {
				rval = RV_FSP;
				crypto_flags &= ~CRYPTO_FLAG_DH;
			} else {
				rval = R_ComputeDHAgreedKey(dh_key,
				    (u_char *)&pkt[i + 5], dh_private,
				    dh_keyLen, &dh_params);
				temp = ntohl(*(u_int32 *)dh_key);
			}
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: verify %x agreement %08x ts %u (%u) fs %u\n",
				    rval, temp, tstamp,
				    peer->pcookie.tstamp,
				    ntohl(vp->fstamp));
#endif
			if (rval != RV_OK) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x agreement %08x ts %u (%u) fs %u\n",
					    rval, temp, tstamp,
					    peer->pcookie.tstamp,
					    ntohl(vp->fstamp));
					peer->cmmd |= CRYPTO_ERROR;
				break;
			}
			peer->flash &= ~TEST10;
			peer->assoc = ntohl(pkt[i + 1]);
			peer->pcookie.tstamp = tstamp;
			if (temp != peer->pcookie.key) {
				peer->pcookie.key = temp;
				key_expire(peer);
			}
			break;

		/*
		 * Install public key and host name.
		 */
		case CRYPTO_NAME | CRYPTO_RESP:
			if (!crypto_flags)
				break;
			vp = (struct value *)&pkt[i + 2];
			temp = ntohl(vp->vallen);
			j = i + 5 + ntohl(vp->vallen) / 4;
			bits = ntohl(pkt[i + 5]);
			if (temp < rsalen || bits <
			    MIN_RSA_MODULUS_BITS || bits >
			    MAX_RSA_MODULUS_BITS) {
				rval = RV_KEY;
			} else if (ntohl(pkt[j]) != bits / 8) {
				rval = RV_SIG;
			} else if (tstamp < peer->pubkey.tstamp ||
			    (tstamp == peer->pubkey.tstamp &&
			    (peer->flags & FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else if (tstamp <= peer->pubkey.fstamp ||
			    ntohl(vp->fstamp) < peer->pubkey.fstamp) {
				rval = RV_FSP;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)vp,
				    temp + 12);
				kp = emalloc(sizeof(R_RSA_PUBLIC_KEY));
				kp->bits = bits;
				memcpy(kp->modulus, &pkt[i + 6],
				    rsalen - 4);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)&pkt[j + 1],
				    ntohl(pkt[j]), kp);
				if (rval != 0) {
					free(kp);
				} else {
					j = i + 5 + rsalen / 4;
					peer->pubkey.ptr = (u_char *)kp;
					temp = strlen((char *)&pkt[j]);
					peer->keystr = emalloc(temp);
					strcpy(peer->keystr,
					    (char *)&pkt[j]);
					peer->pubkey.tstamp = tstamp;
					peer->pubkey.fstamp =
					    ntohl(vp->fstamp);
					peer->flash &= ~TEST10;
				}
			}
#ifdef DEBUG
			if (debug)

				printf(
				    "crypto_recv: verify %x host %s ts %u fs %u\n",
				    rval, (char *)&pkt[i + 5 + rsalen /
				    4], tstamp, ntohl(vp->fstamp));
#endif
			if (rval != RV_OK) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x host %s ts %u fs %u\n",
					    rval, (char *)&pkt[i + 5 +
					    rsalen / 4], tstamp,
					    ntohl(vp->fstamp));
			}
			break;
		/*
		 * Install leapseconds table.
		 */
		case CRYPTO_TAI | CRYPTO_RESP:
			if (!crypto_flags)
				break;
			vp = (struct value *)&pkt[i + 2];
			temp = ntohl(vp->vallen);
			kp = (R_RSA_PUBLIC_KEY *)peer->pubkey.ptr;
			j = i + 5 + temp / 4;
			if (kp == NULL) {
				rval = RV_PUB;
			} else if (ntohl(pkt[j]) != kp->bits / 8) {
				rval = RV_SIG;
			} else if (tstamp < ntohl(tai_leap.tstamp) ||
			    (tstamp == ntohl(tai_leap.tstamp) &&
			    (peer->flags & FLAG_AUTOKEY))) {
				rval = RV_TSP;
			} else if (tstamp <= ntohl(tai_leap.fstamp) ||
			    ntohl(vp->fstamp) < ntohl(tai_leap.fstamp))
			    {
				rval = RV_FSP;
			} else {
				R_VerifyInit(&ctx, DA_MD5);
				R_VerifyUpdate(&ctx, (u_char *)vp,
				    temp + 12);
				rval = R_VerifyFinal(&ctx,
				    (u_char *)&pkt[j + 1],
				    ntohl(pkt[j]), kp);
			}
#ifdef DEBUG
			if (debug)
				printf(
				    "crypto_recv: verify %x leapseconds %u ts %u fs %u\n",
				    rval, temp, tstamp,
				    ntohl(vp->fstamp));
#endif
			if (rval != RV_OK || temp == 0) {
				if (rval != RV_TSP)
					msyslog(LOG_ERR,
					    "crypto: %x leapseconds %u ts %u fs %u\n",
					    rval, temp, tstamp,
					    ntohl(vp->fstamp));
				break;
			}
			peer->flash &= ~TEST10;
			crypto_flags |= CRYPTO_FLAG_TAI;
			sys_tai = temp / 4 + TAI_1972 - 1;
#ifdef KERNEL_PLL
#if NTP_API > 3
			ntv.modes = MOD_TAI;
			ntv.constant = sys_tai;
			if (ntp_adjtime(&ntv) == TIME_ERROR)
				msyslog(LOG_ERR,
				    "kernel TAI update failed");
#endif /* NTP_API */
#endif /* KERNEL_PLL */

			/*
			 * Initialize leapseconds table and extension
			 * field in network byte order.
			 */
			tai_leap.tstamp = vp->tstamp;
			tai_leap.fstamp = vp->fstamp;
			tai_leap.vallen = vp->vallen;
			if (tai_leap.ptr == NULL)
				free(tai_leap.ptr);
			tai_leap.ptr = emalloc(temp);
			if (tai_leap.sig == NULL)
				tai_leap.sig =
				    emalloc(private_key.bits / 8);
			memcpy(tai_leap.ptr, vp->pkt, temp);
			crypto_agree();
			break;
#endif /* PUBKEY */

		/*
		 * For other requests, save the request code for later;
		 * for unknown responses or errors, just ignore for now.
		 */
		default:
			if (code & (CRYPTO_RESP | CRYPTO_ERROR))
				break;
			peer->cmmd = ntohl(pkt[i]);
			break;

		}
		authlen += len;
	}
}


/*
 * crypto_xmit - construct extension fields
 *
 * This routine is called both when an association is configured and
 * when one is not. The only case where this matters now is to retrieve
 * the autokey information, in which case the caller has to provide the
 * association ID to match the association.
 */
int				/* return length of extension field */
crypto_xmit(
	u_int32 *xpkt,		/* packet pointer */
	int start,		/* offset to extension field */
	u_int code,		/* extension field code */
	keyid_t cookie,		/* session cookie */
	u_int associd		/* association ID */
	)
{
	struct peer *peer;	/* peer structure pointer */
	struct autokey *ap;	/* autokey pointer */
	struct cookie *cp;	/* cookie pointer */
	int len;		/* extension field length */
	u_int opcode;		/* extension field opcode */
	int i;
#ifdef PUBKEY
	R_SIGNATURE_CTX ctx;	/* signature context */
	struct value *vp;	/* value pointer */
	int rval;		/* return value */
	u_int temp;
	int j;
#endif /* PUBKEY */

	/*
	 * Generate the requested extension field request code, length
	 * and association ID. Note that several extension fields are
	 * used with and without public-key cryptography. If public-key
	 * cryptography has not been configured, we do the same thing,
	 * but leave off the signature.
	 */
	i = start / 4;
	opcode = code;
	xpkt[i + 1] = htonl(associd);
	len = 8;
	switch (opcode) {

	/*
	 * Send association ID and status word.
	 */
	case CRYPTO_ASSOC | CRYPTO_RESP:
		xpkt[i + 2] = htonl(crypto_flags);
		len += 4;
		break;

	/*
	 * Find peer and send autokey data and signature in broadcast
	 * server and symmetric modes. If no association is found,
	 * either the server has restarted with new associations or some
	 * perp has replayed an old message.
	 */
	case CRYPTO_AUTO | CRYPTO_RESP:
		peer = findpeerbyassoc(associd);
		if (peer == NULL) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		ap = (struct autokey *)&xpkt[i + 2];
		ap->tstamp = peer->sndauto.tstamp;
		ap->seq = peer->sndauto.seq;
		ap->key = peer->sndauto.key;
		ap->siglen = peer->sndauto.siglen;
		len += 16;
#ifdef PUBKEY
		if (!crypto_flags)
			break;
		temp = ntohl(ap->siglen);
		if (temp != 0)
			memcpy(ap->pkt, peer->sndauto.sig, temp);
		len += temp;
#endif /* PUBKEY */
		break;

	/*
	 * Send peer cookie and signature in server mode.
	 */
	case CRYPTO_PRIV:
	case CRYPTO_PRIV | CRYPTO_RESP:
		cp = (struct cookie *)&xpkt[i + 2];
		cp->key = htonl(cookie);
		cp->siglen = 0;
		len += 12;
#ifdef PUBKEY
		cp->tstamp = host.tstamp;
		if (!crypto_flags)
			break;
		R_SignInit(&ctx, DA_MD5);
		R_SignUpdate(&ctx, (u_char *)cp, 8);
		rval = R_SignFinal(&ctx, (u_char *)cp->pkt, &temp,
		    &private_key);
		if (rval != RV_OK) {
			msyslog(LOG_ERR,
			    "crypto: cookie signature fails %x", rval);
			break;
		}
		cp->siglen = htonl(temp);
		len += temp;
#endif /* PUBKEY */
		break;


#ifdef PUBKEY
	/*
	 * The following commands and responses work only when public-
	 * key cryptography has been configured. If configured, but
	 * disabled due to no crypto command in the configuration file,
	 * they are ignored and an error response is returned.
	 */
	/*
	 * Send agreement parameters, timestamp and signature.
	 */
	case CRYPTO_DHPAR | CRYPTO_RESP:
		if (!crypto_flags) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		vp = (struct value *)&xpkt[i + 2];
		vp->tstamp = dhparam.tstamp;
		vp->fstamp = dhparam.fstamp;
		vp->vallen = 0;
		len += 12;
		temp = ntohl(dhparam.vallen);
		if (temp == 0)
			break;
		vp->vallen = htonl(temp);
		memcpy(vp->pkt, dhparam.ptr, temp);
		len += temp;
		j = i + 5 + temp / 4;
		temp = public_key.bits / 8;
		xpkt[j++] = htonl(temp);
		memcpy(&xpkt[j], dhparam.sig, temp);
		len += temp + 4;
		break;

	/*
	 * Send public value, timestamp and signature.
	 */
	case CRYPTO_DH:
	case CRYPTO_DH | CRYPTO_RESP:
		if (!crypto_flags) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		vp = (struct value *)&xpkt[i + 2];
		vp->tstamp = dhpub.tstamp;
		vp->fstamp = dhpub.fstamp;
		vp->vallen = 0;
		len += 12;
		temp = ntohl(dhpub.vallen);
		if (temp == 0)
			break;
		vp->vallen = htonl(temp);
		memcpy(vp->pkt, dhpub.ptr, temp);
		len += temp;
		j = i + 5 + temp / 4;
		temp = public_key.bits / 8;
		xpkt[j++] = htonl(temp);
		memcpy(&xpkt[j], dhpub.sig, temp);
		len += temp + 4;
		break;

	/*
	 * Send public key, host name, timestamp and signature.
	 */
	case CRYPTO_NAME | CRYPTO_RESP:
		if (!crypto_flags) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		vp = (struct value *)&xpkt[i + 2];
		vp->tstamp = host.tstamp;
		vp->fstamp = host.fstamp;
		vp->vallen = 0;
		len += 12;
		temp = ntohl(host.vallen);
		if (temp == 0)
			break;
		vp->vallen = htonl(temp);
		memcpy(vp->pkt, host.ptr, temp);
		len += temp;
		j = i + 5 + temp / 4;
		temp = public_key.bits / 8;
		xpkt[j++] = htonl(temp);
		memcpy(&xpkt[j], host.sig, temp);
		len += temp + 4;
		break;
	/*
	 * Send leapseconds table, timestamp and signature.
	 */
	case CRYPTO_TAI | CRYPTO_RESP:
		if (!crypto_flags) {
			opcode |= CRYPTO_ERROR;
			break;
		}
		vp = (struct value *)&xpkt[i + 2];
		vp->tstamp = tai_leap.tstamp;
		vp->fstamp = tai_leap.fstamp;
		vp->vallen = 0;
		len += 12;
		temp = ntohl(tai_leap.vallen);
		if (temp == 0)
			break;
		vp->vallen = htonl(temp);
		memcpy(vp->pkt, tai_leap.ptr, temp);
		len += temp;
		j = i + 5 + temp / 4;
		temp = public_key.bits / 8;
		xpkt[j++] = htonl(temp);
		memcpy(&xpkt[j], tai_leap.sig, temp);
		len += temp + 4;
		break;
#endif /* PUBKEY */

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
	if (len >= 4) {
		xpkt[i] = htonl((u_int32)((opcode << 16) | len));
#ifdef DEBUG
		if (debug)
			printf(
			    "crypto_xmit: ext offset %d len %d code %x assoc ID %d\n",
			    start, len, code, associd);
#endif
	}
	return (len);
}

#ifdef PUBKEY
/*
 * crypto_setup - load private key, public key, optional agreement
 * parameters and optional leapseconds table, then initialize extension
 * fields for later signatures.
 */
void
crypto_setup(void)
{
	char filename[MAXFILENAME];
	u_int fstamp;			/* filestamp */
	u_int len, temp;
	u_int32 *pp;

	/*
	 * Initialize structures.
	 */
	memset(&private_key, 0, sizeof(private_key));
	memset(&public_key, 0, sizeof(public_key));
	memset(&dh_params, 0, sizeof(dh_params));
	memset(&host, 0, sizeof(host));
	memset(&dhparam, 0, sizeof(dhparam));
	memset(&dhpub, 0, sizeof(dhpub));
	memset(&tai_leap, 0, sizeof(tai_leap));
	if (!crypto_flags)
		return;

	/*
	 * Load required private key from file, default "ntpkey".
	 */
	if (private_key_file == NULL)
		private_key_file = "ntpkey";
	host.fstamp = htonl(crypto_rsa(private_key_file,
	    (u_char *)&private_key, sizeof(R_RSA_PRIVATE_KEY)));

	/*
	 * Load required public key from file, default
	 * "ntpkey_host", where "host" is the canonical name of this
	 * machine.
	 */
	if (public_key_file == NULL) {
		snprintf(filename, MAXFILENAME, "ntpkey_%s",
		    sys_hostname);
		public_key_file = emalloc(strlen(filename) + 1);
		strcpy(public_key_file, filename);
	}
	fstamp = htonl(crypto_rsa(public_key_file,
	    (u_char *)&public_key, sizeof(R_RSA_PUBLIC_KEY)));
	if (fstamp != host.fstamp || strstr(public_key_file,
	    sys_hostname) == NULL) {
		msyslog(LOG_ERR,
		    "crypto: public/private key files mismatch");
		exit (-1);
	}
	crypto_flags |= CRYPTO_FLAG_RSA;

	/*
	 * Assemble public key and host name in network byte order.
	 * These data will later be signed and sent in response to
	 * a client request. Note that the modulus must be a u_int32 in
	 * network byte order independent of the host order or u_int
	 * size.
	 */
	strcpy(filename, sys_hostname);
	for (len = strlen(filename) + 1; len % 4 != 0; len++)
		filename[len - 1] = 0;
	temp = sizeof(R_RSA_PUBLIC_KEY) - sizeof(u_int) + 4;
	host.vallen = htonl(temp + len);
	pp = emalloc(temp + len);
	host.ptr = (u_char *)pp;
	*pp++ = htonl(public_key.bits);
	memcpy(pp--, public_key.modulus, temp - 4);
	pp += temp / 4;
	memcpy(pp, filename, len);
	host.sig = emalloc(private_key.bits / 8);

	/*
	 * Load optional agreement parameters from file, default
	 * "ntpkey_dh". If the file is missing or defective, the values
	 * can later be retrieved from a server.
	 */
	if (dh_params_file == NULL)
		dh_params_file = "ntpkey_dh";
	crypto_dh(dh_params_file);

	/*
	 * Load optional leapseconds from file, default "ntpkey_leap".
	 * If the file is missing or defective, the values can later be
	 * retrieved from a server.
	 */
	if (tai_leap_file == NULL)
		tai_leap_file = "ntpkey_leap";
	crypto_tai(tai_leap_file);
}


/*
 * crypto_agree - compute new public value and sign extension fields.
 */
void
crypto_agree(void)
{
	R_RANDOM_STRUCT randomstr; /* wiggle bits */
	R_SIGNATURE_CTX ctx;	/* signature context */
	l_fp lstamp;		/* NTP time */
	tstamp_t tstamp;	/* seconds timestamp */
	u_int len, temp;
	int rval, i;

	/*
	 * Sign host name and timestamps.
	 */
	if (sys_leap == LEAP_NOTINSYNC) {
		tstamp = 0;
	} else {
		get_systime(&lstamp);
		tstamp = lstamp.l_ui;
	}
	host.tstamp = htonl(tstamp);
	if (!crypto_flags)
		return;
	R_SignInit(&ctx, DA_MD5);
	R_SignUpdate(&ctx, (u_char *)&host, 12);
	R_SignUpdate(&ctx, host.ptr, ntohl(host.vallen));
	rval = R_SignFinal(&ctx, host.sig, &len, &private_key);
	if (rval != RV_OK || len != private_key.bits / 8) {
		msyslog(LOG_ERR, "crypto: host signature fails %x",
		    rval);
		exit (-1);
	}
	host.siglen = ntohl(len);

	/*
	 * Sign agreement parameters and timestamps.
	 */
	if (dhparam.vallen != 0) {
		dhparam.tstamp = htonl(tstamp);
		R_SignInit(&ctx, DA_MD5);
		R_SignUpdate(&ctx, (u_char *)&dhparam, 12);
		R_SignUpdate(&ctx, dhparam.ptr, ntohl(dhparam.vallen));
		rval = R_SignFinal(&ctx, dhparam.sig, &len,
		    &private_key);
		if (rval != RV_OK || len != private_key.bits / 8) {
			msyslog(LOG_ERR,
			    "crypto: parameters signature fails %x",
			    rval);
			exit (-11);
		}
		dhparam.siglen = ntohl(len);

		/*
		 * Compute public value.
		 */
		R_RandomInit(&randomstr);
		R_GetRandomBytesNeeded(&len, &randomstr);
		for (i = 0; i < len; i++) {
			temp = RANDOM;
			R_RandomUpdate(&randomstr, (u_char *)&temp, 1);
		}
		rval = R_SetupDHAgreement(dhpub.ptr, dh_private,
		    dh_keyLen, &dh_params, &randomstr);
		if (rval != RV_OK) {
			msyslog(LOG_ERR,
			    "crypto: invalid public value");
			exit (-1);
		}

		/*
		 * Sign public value and timestamps.
		 */
		dhpub.tstamp = htonl(tstamp);
		R_SignInit(&ctx, DA_MD5);
		R_SignUpdate(&ctx, (u_char *)&dhpub, 12);
		R_SignUpdate(&ctx, dhpub.ptr, ntohl(dhpub.vallen));
		rval = R_SignFinal(&ctx, dhpub.sig, &len,
		    &private_key);
		if (rval != RV_OK || len != private_key.bits / 8) {
			msyslog(LOG_ERR,
			    "crypto: public value signature fails %x",
			    rval);
			exit (-1);
		}
		dhpub.siglen = ntohl(len);
	}

	/*
	 * Sign leapseconds table and timestamps.
	 */
	if (tai_leap.vallen != 0) {
		tai_leap.tstamp = htonl(tstamp);
		R_SignInit(&ctx, DA_MD5);
		R_SignUpdate(&ctx, (u_char *)&tai_leap, 12);
		R_SignUpdate(&ctx, tai_leap.ptr,
		    ntohl(tai_leap.vallen));
		rval = R_SignFinal(&ctx, tai_leap.sig, &len,
		    &private_key);
		if (rval != RV_OK || len != private_key.bits / 8) {
			msyslog(LOG_ERR,
			    "crypto: leapseconds signature fails %x",
			    rval);
			exit (-1);
		}
		tai_leap.siglen = ntohl(len);
	}
#ifdef DEBUG
	if (debug)
		printf(
		    "cypto_agree: ts %u host %u par %u pub %u leap %u\n",
		    tstamp, ntohl(host.fstamp), ntohl(dhparam.fstamp),
		    ntohl(dhpub.fstamp), ntohl(tai_leap.fstamp));
#endif
}


/*
 * crypto_rsa - read RSA key, decode and check for errors.
 */
static u_int
crypto_rsa(
	char *cp,		/* file name */
	u_char *key,		/* key pointer */
	u_int keylen		/* key length */
	)
{
	FILE *str;		/* file handle */
	u_char buf[MAX_LINLEN];	/* file line buffer */
	u_char encoded_key[MAX_ENCLEN]; /* encoded key buffer */
	char filename[MAXFILENAME]; /* name of parameter file */
	char linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_int fstamp;		/* filestamp */
	u_int bits, len;
	char *rptr;
	int rval;

	/*
	 * Open the key file and discard comment lines. If the first
	 * character of the file name is not '/', prepend the keys
	 * directory string. 
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	str = fopen(filename, "r");
	if (str == NULL) {
		msyslog(LOG_ERR, "crypto: RSA file %s not found",
		    filename);
		exit (-1);
	}

	/*
	 * Ignore initial comments and empty lines.
	 */
	while ((rptr = fgets(buf, MAX_LINLEN - 1, str)) != NULL) {
		len = strlen(buf);
		if (len < 1)
			continue;
		if (*buf == '#' || *buf == '\r' || *buf == '\0')
			continue;
		break;
	}

	/*
	 * We are rather paranoid here, since an intruder might cause a
	 * coredump by infiltrating a naughty key. The line must contain
	 * a single integer followed by a PEM encoded, null-terminated
	 * string.
	 */
	if (rptr == NULL)
		rval = RV_DAT;
	else if (sscanf(buf, "%d %s", &bits, encoded_key) != 2)
		rval = RV_DAT;
	else if (R_DecodePEMBlock(&buf[sizeof(u_int)], &len,
		    encoded_key, strlen(encoded_key)))
		rval = RV_DEC;
	else if ((len += sizeof(u_int)) != keylen)
		rval = RV_KEY;
	else if (bits < MIN_RSA_MODULUS_BITS || bits >
	    MAX_RSA_MODULUS_BITS)
		rval = RV_KEY;
	else
		rval = RV_OK;
	if (rval != RV_OK) {
		fclose(str);
		msyslog(LOG_ERR, "crypto: key file %s error %x", cp,
		    rval);
		exit (-1);
	}
	fclose(str);
	*(u_int *)buf = bits;
	memcpy(key, buf, keylen);

	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		rptr = strrchr(linkname, '.');
	} else {
		rptr = strrchr(filename, '.');
	}
	if (rptr != NULL)
		sscanf(++rptr, "%u", &fstamp);
	else
		fstamp = 0;
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_rsa: key file %s link %d fs %u modulus %d\n",
		    cp, rval, fstamp, bits);
#endif
	return (fstamp);
}


/*
 * crypto_dh - read agreement parameters, decode and check for errors.
 */
static void
crypto_dh(
	char *cp		/* file name */
	)
{
	FILE *str;		/* file handle */
	u_char buf[MAX_LINLEN];	/* file line buffer */
	u_char encoded_key[MAX_ENCLEN]; /* encoded key buffer */
	u_char prime[MAX_KEYLEN]; /* decoded prime */
	u_char generator[MAX_KEYLEN]; /* decode generator */
	u_int primelen;		/* prime length (octets) */
	u_int generatorlen;	/* generator length (octets) */
	char filename[MAXFILENAME]; /* name of parameter file */
	char linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_int fstamp;		/* filestamp */
	u_int32 *pp;
	u_int len;
	char *rptr;
	int rval;

	/*
	 * Open the key file and discard comment lines. If the first
	 * character of the file name is not '/', prepend the keys
	 * directory string. If the file is not found, not to worry; it
	 * can be retrieved over the net. But, if it is found with
	 * errors, we crash and burn.
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	str = fopen(filename, "r");
	if (str == NULL) {
		msyslog(LOG_INFO,
		    "crypto: parameters file %s not found", filename);
		return;
	}

	/*
	 * Ignore initial comments and empty lines.
	 */
	while ((rptr = fgets(buf, MAX_LINLEN - 1, str)) != NULL) {
		if (strlen(buf) < 1)
			continue;
		if (*buf == '#' || *buf == '\r' || *buf == '\0')
			continue;
		break;
	}

	/*
	 * We are rather paranoid here, since an intruder might cause a
	 * coredump by infiltrating a naughty key. There must be two
	 * lines; the first contains the prime, the second the
	 * generator. Each line must contain a single integer followed
	 * by a PEM encoded, null-terminated string.
	 */
	if (rptr == NULL)
		rval = RV_DAT;
	else if (sscanf(buf, "%u %s", &primelen, encoded_key) != 2)
		rval = RV_DAT;
	else if (primelen > MAX_KEYLEN)
		rval = RV_KEY;
	else if (R_DecodePEMBlock(prime, &len, encoded_key,
	    strlen(encoded_key)))
		rval = RV_DEC;
	else if (primelen != len || primelen >
	    DECODED_CONTENT_LEN(strlen(encoded_key)))
		rval = RV_DAT;
	else if (fscanf(str, "%u %s", &generatorlen, encoded_key) != 2)
		rval = RV_DAT;
	else if (generatorlen > MAX_KEYLEN)
		rval = RV_KEY;
	else if (R_DecodePEMBlock(generator, &len, encoded_key,
	    strlen(encoded_key)))
		rval = RV_DEC;
	else if (generatorlen != len || generatorlen >
	    DECODED_CONTENT_LEN(strlen(encoded_key)))
		rval = RV_DAT;
	else
		rval = RV_OK;
	if (rval != RV_OK) {
		msyslog(LOG_ERR,
		    "crypto: parameter file %s error %x", cp,
		    rval);
		exit (-1);
	}
	fclose(str);

	/*
	 * Initialize agreement parameters and extension field in
	 * network byte order. Note the private key length is set
	 * arbitrarily at half the prime length.
	 */
	len = 4 + primelen + 4 + generatorlen;
	dhparam.vallen = htonl(len);
	pp = emalloc(len);
	dhparam.ptr = (u_char *)pp;
	*pp++ = htonl(primelen);
	memcpy(pp, prime, primelen);
	dh_params.prime = (u_char *)pp;
	pp += primelen / 4;
	*pp++ = htonl(generatorlen);
	memcpy(pp, &generator, generatorlen);
	dh_params.generator = (u_char *)pp;

	dh_params.primeLen = primelen;
	dh_params.generatorLen = generatorlen;
	dh_keyLen = primelen / 2;
	dh_private = emalloc(dh_keyLen);
	dhparam.sig = emalloc(private_key.bits / 8);
	crypto_flags |= CRYPTO_FLAG_DH;

	/*
	 * Initialize public value extension field.
	 */
	dhpub.vallen = htonl(dh_params.primeLen);
	dhpub.ptr = emalloc(dh_params.primeLen);
	dhpub.sig = emalloc(private_key.bits / 8);

	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		rptr = strrchr(linkname, '.');
	} else {
		rptr = strrchr(filename, '.');
	}
	if (rptr != NULL)
		sscanf(++rptr, "%u", &fstamp);
	else
		fstamp = 0;
	dhparam.fstamp = htonl(fstamp);
	dhpub.fstamp = htonl(fstamp);
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_dh: pars file %s link %d fs %u prime %u gen %u\n",
		    dh_params_file, rval, fstamp, dh_params.primeLen,
		    dh_params.generatorLen);
#endif
}


/*
 * crypto_tai - read leapseconds table and check for errors.
 */
static void
crypto_tai(
	char *cp		/* file name */
	)
{
	FILE *str;		/* file handle */
	u_char buf[MAX_LINLEN];	/* file line buffer */
	u_int leapsec[MAX_LEAP]; /* NTP time at leaps */
	u_int offset;		/* offset at leap (s) */
	char filename[MAXFILENAME]; /* name of parameter file */
	char linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_int fstamp;		/* filestamp */
	u_int32 *pp;
	u_int len;
	char *rptr;
	int rval, i;
#ifdef KERNEL_PLL
#if NTP_API > 3
	struct timex ntv;	/* kernel interface structure */
#endif /* NTP_API */
#endif /* KERNEL_PLL */

	/*
	 * Open the key file and discard comment lines. If the first
	 * character of the file name is not '/', prepend the keys
	 * directory string. If the file is not found, not to worry; it
	 * can be retrieved over the net. But, if it is found with
	 * errors, we crash and burn.
	 */
	if (*cp == '/')
		strcpy(filename, cp);
	else
		snprintf(filename, MAXFILENAME, "%s/%s", keysdir, cp);
	str = fopen(filename, "r");
	if (str == NULL) {
		msyslog(LOG_INFO,
		    "crypto: leapseconds file %s not found",
		    filename);
		return;
	}

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
	rval = RV_OK;
	while (i < MAX_LEAP) {
		rptr = fgets(buf, MAX_LINLEN - 1, str);
		if (rptr == NULL)
			break;
		if (strlen(buf) < 1)
			continue;
		if (*buf == '#')
			continue;
		if (sscanf(buf, "%u %u", &leapsec[i], &offset) != 2)
			continue;
		if (i != offset - TAI_1972) { 
			rval = RV_DAT;
			break;
		}
		i++;
	}
	fclose(str);
	if (rval != RV_OK || i == 0) {
		msyslog(LOG_ERR,
		    "crypto: leapseconds file %s error %d", cp,
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
	pp = emalloc(len);
	tai_leap.ptr = (u_char *)pp;
	for (; i >= 0; i--) {
		*pp++ = htonl(leapsec[i]);
	}
	tai_leap.sig = emalloc(private_key.bits / 8);
	crypto_flags |= CRYPTO_FLAG_TAI;
	sys_tai = len / 4 + TAI_1972 - 1;
#ifdef KERNEL_PLL
#if NTP_API > 3
	ntv.modes = MOD_TAI;
	ntv.constant = sys_tai;
	if (ntp_adjtime(&ntv) == TIME_ERROR)
		msyslog(LOG_ERR, "kernel TAI update failed");
#endif /* NTP_API */
#endif /* KERNEL_PLL */


	/*
	 * Extract filestamp if present.
	 */
	rval = readlink(filename, linkname, MAXFILENAME - 1);
	if (rval > 0) {
		linkname[rval] = '\0';
		rptr = strrchr(linkname, '.');
	} else {
		rptr = strrchr(filename, '.');
	}
	if (rptr != NULL)
		sscanf(++rptr, "%u", &fstamp);
	else
		fstamp = 0;
	tai_leap.fstamp = htonl(fstamp);
#ifdef DEBUG
	if (debug)
		printf(
		    "crypto_tai: leap file %s link %d fs %u offset %u\n",
		    tai_leap_file, rval, fstamp,
		    ntohl(tai_leap.vallen) / 4 + TAI_1972);
#endif
}

#if 0
/*
 * crypto_public - read and install the public key from the public key
 * file. The name of the file is in the form "ntpkey_host", where host
 * is the DNS canonical name of the host. If the file is not found or
 * has errors, we just keep going and expect the host to fetch the
 * public key from the peer via the extension field.
 */
int
crypto_public(
	struct peer *peer,	/* peer structure pointer */
	u_char *cp,		/* canonical host name */
	u_int fstamp		/* filestamp */
	)
{
	R_RSA_PUBLIC_KEY keybuf;
	u_int keylen = sizeof(R_RSA_PUBLIC_KEY);
	char filename[MAXFILENAME];
	u_int temp;

	/*
	 * If the filestamp is nonzero, append it as the file name
	 * extension. If not and a link name has a filestamp, append
	 * that for looks.
	 */
	if (fstamp == 0)
		snprintf(filename, MAXFILENAME, "ntpkey_%s", cp);
	else
		snprintf(filename, MAXFILENAME, "ntpkey_%s.%u", cp,
		    fstamp);
	crypto_rsa(filename, &temp, (u_char *)&keybuf, keylen);
	if (keybuf.bits == 0)
		return (0);
	if (fstamp == 0 && temp != 0)
		sprintf(filename, "%s.%u", filename, temp);
	if (peer->keystr != NULL)
		free(peer->keystr);
	peer->keystr = emalloc(strlen(filename) + 1);
	strcpy(peer->keystr, filename);
	if (peer->pubkey.ptr == NULL)
		peer->pubkey.ptr = emalloc(keylen);
	memcpy(peer->pubkey.ptr, (char *)&keybuf, keylen);
	return (1);
}
#endif


/*
 * crypto_config - configure crypto data from crypto configuration
 * command.
 */
void
crypto_config(
	int item,		/* configuration item */
	char *cp		/* file name */
	)
{
	switch (item) {

	/*
	 * Initialize flags
	 */
	case CRYPTO_CONF_FLAGS:
		sscanf(cp, "%x", &crypto_flags);
		break;

	/*
	 * Set private key file name.
	 */
	case CRYPTO_CONF_PRIV:
		private_key_file = emalloc(strlen(cp) + 1);
		strcpy(private_key_file, cp);
		break;

	/*
	 * Set public key file name.
	 */
	case CRYPTO_CONF_PUBL:
		public_key_file = emalloc(strlen(cp) + 1);
		strcpy(public_key_file, cp);
		break;

	/*
	 * Set agreement parameter file name.
	 */
	case CRYPTO_CONF_DH:
		dh_params_file = emalloc(strlen(cp) + 1);
		strcpy(dh_params_file, cp);
		break;

	/*
	 * Set leapseconds table file name.
	 */
	case CRYPTO_CONF_LEAP:
		tai_leap_file = emalloc(strlen(cp) + 1);
		strcpy(tai_leap_file, cp);
		break;

	/*
	 * Set crypto keys directory.
	 */
	case CRYPTO_CONF_KEYS:
		keysdir = emalloc(strlen(cp) + 1);
		strcpy(keysdir, cp);
		break;
	}
	crypto_flags |= CRYPTO_FLAG_ENAB;
}
# else
int ntp_crypto_bs_pubkey;
# endif /* PUBKEY */
#else
int ntp_crypto_bs_autokey;
#endif /* AUTOKEY */
