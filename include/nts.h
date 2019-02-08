/*
 * nts.h - NTS (Network Time Security) declarations
 */
#ifndef GUARD_NTS_H
#define GUARD_NTS_H

#define NTS_MAX_COOKIES	8	/* RFC 4.1.6 */
#define NTS_COOKIELEN	128	/* placeholder - see RFC 6 */

#define FLAG_NTS	0x01u	/* use NTS (network time security) */
#define FLAG_NTS_ASK	0x02u	/* NTS, ask for specified server */
#define FLAG_NTS_REQ	0x04u	/* NTS, ask for specified server */
#define FLAG_NTS_NOVAL	0x08u	/* do not validate the server certificate */

/* Configuration data for an NTS association */
struct ntscfg_t {
    char *server;	/* if NULL, use the peer itself (normal case) */
    char *ca;		/* if NULL, use the system default (normal case) */
    char *cert;		/* if NULL, use the system default (normal case) */
    uint32_t flags;
    uint32_t expire;
};

/* Client-side state per connection to server */
struct ntsstate_t {
    char cookies[NTS_MAX_COOKIES][NTS_COOKIELEN];
    int current_cookie;
    int cookie_count;
};

/* Configuration data for an NTS server of client instance */
struct ntsconfig_t {
    bool ntsenable; 		/* enable NTS on this ntpd instance */
    float mintls;		/* minimum TLS version allowed */
    float maxtls;		/* maximum TLS version allowed */
    char *tlsciphers;	/* allowed TLS 1.2 ciphers */
    char *tlsciphersuites;	/* allowed TLS 1.3 ciphersuites */
};

extern struct ntsconfig_t ntsconfig;

#endif /* GUARD_NTS_H */
