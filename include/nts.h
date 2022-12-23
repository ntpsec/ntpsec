/*
 * nts.h - NTS (Network Time Security) declarations
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef GUARD_NTS_H
#define GUARD_NTS_H

#include <stdbool.h>
#include <stdint.h>

/* default file names */
#define NTS_CERT_FILE "/etc/ntp/cert-chain.pem"
#define NTS_KEY_FILE "/etc/ntp/key.pem"
#define NTS_COOKIE_KEY_FILE "/var/lib/ntp/nts-keys"

#define NTS_KE_PORT		4460
#define NTS_KE_PORTA		"4460"

#define NTS_KE_TIMEOUT		3

bool nts_server_init(void);
bool nts_client_init(void);
bool nts_cookie_init(void);
bool nts_server_init2(void);    /* after sandbox */
bool nts_cookie_init2(void);

void nts_cert_timer(void);
void nts_cookie_timer(void);

bool nts_read_cookie_keys(void);
void nts_make_cookie_key(void);
bool nts_write_cookie_keys(void);

int nts_make_cookie(uint8_t *cookie,
  uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen);
bool nts_unpack_cookie(uint8_t *cookie, int cookielen,
  uint16_t *aead,
  uint8_t *c2s, uint8_t *s2c, int *keylen);

/* working finger into a buffer - updated by append/unpack routines */
struct BufCtl_t {
    uint8_t *next;  /* pointer to next data/space */
    int left;       /* data left or space available */
};
typedef struct BufCtl_t BufCtl;

bool nts_ke_process_receive(struct BufCtl_t *buf, int *aead);
bool nts_ke_setup_send(struct BufCtl_t *buf, int aead,
       uint8_t *c2s, uint8_t *s2c, int keylen);

/***********************************************************/

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


/* 2 byte type, 2 byte length */
#define NTS_KE_HDR_LNG 4
#define NTS_KE_U16_LNG 2

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

/***********************************************************/

#define NTS_MAX_KEYLEN		64	/* used in cookies */
#define NTS_MAX_COOKIELEN	192	/* see nts_cookie.c */
#define NTS_MAX_COOKIES		8	/* RFC 4.1.6 */
#define NTS_UID_LENGTH		32	/* RFC 5.3 */
#define NTS_UID_MAX_LENGTH	64

/* Here for tester */
struct NTS_Key {
  uint8_t K[NTS_MAX_KEYLEN];
  uint32_t I;
  };
#ifndef NTS_KEYS
  #define NTS_nKEYS 10
#endif
extern struct NTS_Key nts_keys[NTS_nKEYS];
extern int nts_nKeys;


/* Client side configuration data for an NTS association
 * All are optional.
 * part of peer struct */
struct ntscfg_t {
	char *ca;		/* root/trusted certificates */
	char *aead;		/* AEAD algorithms on wire */
};

/* Client-side state per connection to server */
struct ntsclient_t {
	/* wire connection */
	uint16_t aead;	/* AEAD algorithm used on wire */
	int keylen;
	uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
	/* UID of last request sent - RFC 5.3 */
	uint8_t UID[NTS_UID_LENGTH];
	/* cookies */
	int readIdx, writeIdx;
	int count;			/* -1 if not in NTS mode */
	int cookielen;
	uint8_t cookies[NTS_MAX_COOKIES][NTS_MAX_COOKIELEN];
};

/* Server-side state per packet */
struct ntspacket_t {
	bool valid;
	int uidlen;
	uint8_t UID[NTS_UID_MAX_LENGTH];
	int needed;
	uint16_t aead;
	int keylen;
	uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
};


/* Configuration data for an NTS server or client instance */
struct ntsconfig_t {
	bool ntsenable; 	/* enable NTS KE server on this ntpd */
	const char * mintls;	/* minimum TLS version allowed */
	const char * maxtls;	/* maximum TLS version allowed */
	const char *tlsciphersuites;/* allowed TLS 1.3 ciphersuites */
	const char *cert;	/* file holding server certificate key */
	const char *key;	/* file holding server private key */
	const char *KI;		/* file holding K/I for making cookies */
	const char *ca;		/* root cert dir/file */
	const char *aead;	/* AEAD algorithms on wire */
};


/* CMAC length is wired into AEAD_AES_SIV_CMAC_nnn. */
#define CMAC_LENGTH 16
/* The NONCE length comes from RFC 5116 and/or 5297. */
#define NONCE_LENGTH 16

/* NTS protocol constants */

#define NTS_CRITICAL 0x8000
enum nts_record_type {
	nts_end_of_message = 0,		/* CRITICAL */
	nts_next_protocol_negotiation = 1,	/* CRITICAL */
	nts_error = 2,			/* CRITICAL */
	nts_warning = 3,
	nts_algorithm_negotiation = 4,
	nts_new_cookie = 5,
	nts_server_negotiation = 6,
	nts_port_negotiation = 7
};

enum nts_protocol_type {
	nts_protocol_NTP = 0,
};


enum nts_errors_type {
	nts_unrecognized_critical_section = 0,
	nts_bad_request = 1
};

enum aead_ciphers {
#define NO_AEAD 0xffff
	AEAD_AES_128_GCM = 1,
	AEAD_AES_256_GCM = 2,
	AEAD_AES_128_CCM = 3,
	AEAD_AES_256_CCM = 4,

	AEAD_AES_128_GCM_8 = 5,
	AEAD_AES_256_GCM_8 = 6,
	AEAD_AES_128_GCM_12 = 7,
	AEAD_AES_256_GCM_12 = 8,

	AEAD_AES_128_CCM_SHORT = 9,
	AEAD_AES_256_CCM_SHORT = 10,
	AEAD_AES_128_CCM_SHORT_8 = 11,
	AEAD_AES_256_CCM_SHORT_8 = 12,
	AEAD_AES_128_CCM_SHORT_12 = 13,
	AEAD_AES_256_CCM_SHORT_12 = 14,

	AEAD_AES_SIV_CMAC_256 = 15,     /* RFC 5297 */
	AEAD_AES_SIV_CMAC_384 = 16,     /* These 3 are the ones we use */
	AEAD_AES_SIV_CMAC_512 = 17,
#define AEAD_AES_SIV_CMAC_256_KEYLEN 32
#define AEAD_AES_SIV_CMAC_384_KEYLEN 48
#define AEAD_AES_SIV_CMAC_512_KEYLEN 64

	AEAD_AES_128_CCM_8 = 18,
	AEAD_AES_256_CCM_8 = 19,

	AEAD_AES_128_OCB_TAGLEN128 = 20,
	AEAD_AES_128_OCB_TAGLEN96 = 21,
	AEAD_AES_128_OCB_TAGLEN64 = 22,
	AEAD_AES_192_OCB_TAGLEN128 = 23,
	AEAD_AES_192_OCB_TAGLEN96 = 24,
	AEAD_AES_192_OCB_TAGLEN64 = 25,
	AEAD_AES_256_OCB_TAGLEN128 = 26,
	AEAD_AES_256_OCB_TAGLEN96 = 27,
	AEAD_AES_256_OCB_TAGLEN64 = 28,

	AEAD_CHACHA20_POLY1305 = 29
};



extern struct ntsconfig_t ntsconfig;



/* NTS-related statistics visible via ntpq -c nts */
extern uint64_t nts_client_send;
extern uint64_t nts_client_recv_good;
extern uint64_t nts_client_recv_bad;
extern uint64_t nts_server_send;
extern uint64_t nts_server_recv_good;
extern uint64_t nts_server_recv_bad;
extern uint64_t nts_cookie_make;
extern uint64_t nts_cookie_decode;
extern uint64_t nts_cookie_decode_old;
extern uint64_t nts_cookie_decode_old2;
extern uint64_t nts_cookie_decode_older;
extern uint64_t nts_cookie_decode_too_old;
extern uint64_t nts_cookie_decode_error;
extern uint64_t nts_ke_serves_good;
extern uint64_t nts_ke_serves_bad;
extern uint64_t nts_ke_probes_good;
extern uint64_t nts_ke_probes_bad;

#endif /* GUARD_NTS_H */
