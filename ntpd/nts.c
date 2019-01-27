/*
 * nts.c - Network Time Security (NTS) support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 * This module exposes mostly functions and structure pointers (not
 * structures) so that the NTS implementation can be sealed off from
 * the rest of the code. It supports both the client and server sides.
 *
 * The exception is client configuration, for which various bits have
 * to be set by the config parser.
 */
#include "config.h"
#include "ntp_types.h"
#include "ntpd.h"

/* By design, there is no per-client-side state on the server */

/*
 * Key exchange support.
 *
 * These function headers will probably need to change
 */

/*
 * RFC section 4:
 *- Generate KE request message.
 * - Include Record Types:
 *  o NTS Next Protocol Negotiation
 *  o AEAD Algorithm Negotiation
 *  o <NTP Server Negotiation>
 *  o End of Message
 */
int nts_client_ke_request(void)
{
    return 0;
}

/*
 * RFC section 4:
 * - Verify client request message.
 * - Extract TLS key material.
 * - Generate KE response message.
 *   - Include Record Types:
 *       o NTS Next Protocol Negotiation
 *       o AEAD Algorithm Negotiation
 *       o NTP Server Negotiation
 *       o New Cookie for NTPv4
 *       o <New Cookie for NTPv4>
 *       o End of Message
 */
int nts_server_ke_verify(void)
{
	return 0;
}

/*
 * RFC section 4:
 * - Verify server response message
 * - Extract cookie(s). 
 */
int nts_client_ke_verify(struct ntspeer_t *ntspeer)
{
	UNUSED_ARG(ntspeer);
	return 0;
}

/*
 * Daily rotation of server-side master keys.
 */
int nts_daily(void) 
{
	return 0;
}

/* */

/*
 * Extract and validate NTS validation information from packet
 * extension fields in an incoming request or response.  On the server
 * side, the ntspeer pointer is expected to be NULL as there is no
 * per-client server state.  A nonzero return causes the packet to be
 * discarded.
 */
int nts_validate(struct parsed_pkt *pkt, struct ntspeer_t *ntspeer)
{
	UNUSED_ARG(pkt);
	UNUSED_ARG(ntspeer);
	return 0;
}

/*
 * Decorate an outgoing client request or server response with packet
 * extension fields carrying NTS information.  For a server response,
 * the ntspeer pointer is expected to be NULL as there is no
 * per-client server state.  Return the count of words appended.
 */
int nts_decorate(uint32_t *extdata, size_t extlen, struct ntspeer_t *ntspeer)
{
	UNUSED_ARG(extdata);
	UNUSED_ARG(extlen);
	UNUSED_ARG(ntspeer);
	return 0;
}

/* end */
