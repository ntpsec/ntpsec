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
    char *server;	/* if NUL, use the peer itself (normal case) */
    char *ca;		/* if NUL, use the system default (normal case) */
    char *cert;		/* if NUL, use the system default (normal case) */
    uint32_t flags;
    uint32_t expire;
};

/* Client-side state per connection to server */
struct ntsstate_t {
    char cookies[NTS_MAX_COOKIES][NTS_COOKIELEN];
    int current_cookie;
    int cookie_count;
};

#endif /* GUARD_NTS_H */
