/*
 * nts.h - NTS (Network Time Security) declarations
 */
#ifndef GUARD_NTS_H
#define GUARD_NTS_H

#include <stdbool.h>
#include <stdint.h>

/* default file names */
#define NTS_CERT_FILE "/etc/ntp/cert-chain.pem"
#define NTS_KEY_FILE "/etc/ntp/key.pem"
#define NTS_COOKIE_KEY_FILE "/var/lib/ntp/nts-keys"

#define NTS_KE_PORT		123

#define NTS_KE_TIMEOUT		3

bool nts_server_init(void);
bool nts_client_init(void);
bool nts_cookie_init(void);
bool nts_server_init2(void);    /* after sandbox */
bool nts_cookie_init2(void);






#define NTS_MAX_KEYLEN		64	/* used in cookies */
#define NTS_MAX_COOKIELEN	192	/* see nts_cookie.c */
#define NTS_MAX_COOKIES		8	/* RFC 4.1.6 */
#define NTS_UID_LENGTH		32	/* RFC 5.3 */
#define NTS_UID_MAX_LENGTH	64


/* Client side configuration data for an NTS association
 * All are optional.
 * part of peer struct */
struct ntscfg_t {
    char *server;	/* desired server; default is same as NTS-KE server */
    char *ca;		/* root/trusted certificates */
    char *cert;		/* client certificate  */
    char *aead;		/* AEAD algorithms on wire */
    uint32_t expire;
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
    bool ntsenable; 		/* enable NTS KE server on this ntpd */
    const char * mintls;	/* minimum TLS version allowed */
    const char * maxtls;	/* maximum TLS version allowed */
    const char *tlsciphers;	/* allowed TLS 1.2 ciphers */
    const char *tlsciphersuites;/* allowed TLS 1.3 ciphersuites */
    const char *cert;		/* file holding server certificate key */
    const char *key;		/* file holding server private key */
    const char *KI;		/* file holding K/I for making cookies */
    const char *ca;		/* root cert dir/file */
    const char *aead;		/* AEAD algorithms on wire */
};


/* Only valid for AEAD_AES_SIV_CMAC_nnn
 * but that's all we use. */
#define AEAD_CMAC_LNG 16

/* We are using AEAD_AES_SIV_CMAC_256, from RFC 5297 */
#define IANA_AEAD_AES_SIV_CMAC_256 15
#define IANA_AEAD_AES_SIV_CMAC_384 16
#define IANA_AEAD_AES_SIV_CMAC_512 17
#define AEAD_AES_SIV_CMAC_256_KEYLEN 32
#define AEAD_AES_SIV_CMAC_384_KEYLEN 48
#define AEAD_AES_SIV_CMAC_512_KEYLEN 64
#define NO_AEAD 0xffff

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

enum nts_errors_type {
  nts_unrecognized_critical_section = 0,
  nts_bad_request = 1
};

enum aead_ciphers {
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
extern uint64_t nts_client_recv;
extern uint64_t nts_client_recv_bad;
extern uint64_t nts_server_send;
extern uint64_t nts_server_recv;
extern uint64_t nts_server_recv_bad;
extern uint64_t nts_cookie_make;
extern uint64_t nts_cookie_decode;
extern uint64_t nts_cookie_decode_old;
extern uint64_t nts_cookie_decode_too_old;
extern uint64_t nts_cookie_decode_error;
extern uint64_t nts_ke_serves;
extern uint64_t nts_ke_serves_bad;
extern uint64_t nts_ke_probes;
extern uint64_t nts_ke_probes_bad;

#endif /* GUARD_NTS_H */
