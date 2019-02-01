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

/* Configuration data for an NTS association */
struct ntscfg_t {
    char *server;
    uint32_t flags;
};

/* Client-side state per connection to server */
struct ntsstate_t {
    char cookies[NTS_MAX_COOKIES][NTS_COOKIELEN];
    int current_cookie;
    int cookie_count;
};

#endif /* GUARD_NTS_H */
