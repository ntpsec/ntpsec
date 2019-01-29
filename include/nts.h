/*
 * nts.h - NTS (Network Time Security) declarations
 */
#ifndef GUARD_NTS_H
#define GUARD_NTS_H

#define NTS_MAX_COOKIES	8	/* RFC 4.1.6 */
#define NTS_COOKIELEN	128	/* placeholder - see RFC 6 */

/* Client-side state per connection to server */
struct ntspeer_t {
    /* we treat an empty cookie string as a sentinel */
    char cookies[NTS_MAX_COOKIES][NTS_COOKIELEN];
    int current_cookie;
};

#endif /* GUARD_NTS_H */
