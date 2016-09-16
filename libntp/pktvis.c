/*
 * pktvis.c - display format for NTP packets, and inverting it.
 */
#include <config.h>

#include <stdio.h>
#include <inttypes.h>

#include "ntpd.h"
#include "ntp_assert.h"
#include "ntp_fp.h"
#include "lib_strbuf.h"

#define BIGEND_BYTESHIFT(i, m)	(8 * ((m - 1) - (i % (m))))
#define BIGEND_GETBYTE(u32, i)	(((u32) >> BIGEND_BYTESHIFT(i, 4)) & 0xff)
#define BIGEND_PUTBYTE(b, i)	(((b) & 0xff) << BIGEND_BYTESHIFT(i, 4))

static const int LFP_MAX_PRECISION = 10;

void packet_dump(char *buf, size_t buflen, struct pkt *pkt, size_t len)
{
    size_t i;
    snprintf(buf, buflen, "%d:%d:%d:%d:%u:%u:%08x:%s:%s:%s:%s:",
	     pkt->li_vn_mode, pkt->stratum, pkt->ppoll, pkt->precision,
	     pkt->rootdelay, pkt->rootdisp,
	     pkt->refid,
	     lfptoa(&pkt->reftime, LFP_MAX_PRECISION),
	     lfptoa(&pkt->org, LFP_MAX_PRECISION),
	     lfptoa(&pkt->rec, LFP_MAX_PRECISION),
	     lfptoa(&pkt->xmt, LFP_MAX_PRECISION));

    if (len == LEN_PKT_NOMAC)
	strlcat(buf, "nomac", buflen);
    else
	/* dump MAC as len - LEN_PKT_NOMAC chars in hex */
	for (i = 0; i + LEN_PKT_NOMAC < len; i++) {
	    snprintf(buf + strlen(buf), buflen - strlen(buf),
		     "%02x", BIGEND_GETBYTE(pkt->exten[i / sizeof(uint32_t)], i));
	}
}

static int packet_parse(char *pktbuf, struct pkt *pkt)
{
    char refbuf[32], orgbuf[32], recbuf[32], xmtbuf[32], macbuf[BUFSIZ];
    int li_vn_mode = 0, stratum = 0, ppoll = 0, precision = 0;
    size_t pktlen;

    if (sscanf(pktbuf, "%d:%d:%d:%d:%u:%u:%08x:%[^:]:%[^:]:%[^:]:%[^:]:%s",
		     &li_vn_mode, &stratum,
		     &ppoll, &precision,
		     &pkt->rootdelay, &pkt->rootdisp,
		     &pkt->refid,
	       refbuf, orgbuf, recbuf, xmtbuf, macbuf) != 12)
	return -1;

    /* extra transfers required because the struct members are int8_t */
    pkt->li_vn_mode = (uint8_t)li_vn_mode;
    pkt->stratum = (uint8_t)stratum;
    pkt->ppoll = (uint8_t)ppoll;
    pkt->precision = (int8_t)precision;
    atolfp(refbuf, &pkt->reftime);
    atolfp(orgbuf, &pkt->org);
    atolfp(recbuf, &pkt->rec);
    atolfp(xmtbuf, &pkt->xmt);

    pktlen = LEN_PKT_NOMAC;
    memset(pkt->exten, '\0', sizeof(pkt->exten));
    if (strcmp(macbuf, "nomac") != 0) {
	size_t i;
	for (i = 0; i < strlen(macbuf)/2; i++) {
	    int hexval;
	    if (sscanf(macbuf + 2*i, "%02x", &hexval) != 1) {
		return -1;
	    }
	    pkt->exten[i / sizeof(uint32_t)] |= BIGEND_PUTBYTE(hexval, i);
	}

	pktlen += i;
    }
    return pktlen;
}

size_t packet_undump(char *bin, int len, char *pktbuf)
{
    struct pkt pkt;
    int pktlen = packet_parse(pktbuf, &pkt);
    INSIST((pktlen != -1) && len >= pktlen);
    /* works because pkt fields and extension are in network byte order */
    memcpy(bin, (char *)&pkt, pktlen);
    return pktlen;
}

/* end */
