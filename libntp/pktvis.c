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

#define BIGEND_BYTESHIFT(i, m)	(8 * ((m) - (i % (m))))
#define BIGEND_GETBYTE(u32, i)	(((u32) >> BIGEND_BYTESHIFT(i, 4)) & 0xff)
#define BIGEND_PUTBYTE(b, i)	(((b) & 0xff) << BIGEND_BYTESHIFT(i, 4))

static char *lfpdump(l_fp *fp)
{
    char *buf;
    uint64_t	np;

    LIB_GETBUF(buf);

    np = fp->l_ui;
    np <<= FRACTION_PREC;
    np |= fp->l_uf;

    snprintf(buf, LIB_BUFLENGTH, "%" PRIu64, np);

    return buf;
}

void packet_dump(char *buf, size_t buflen,
			sockaddr_u *dest, struct pkt *pkt, size_t len)
{
    size_t i;
    /*
     * Format is three tokens: source address, packet, MAC token. 
     *
     * FIXME: struct pkt fields are in network byte order. Need to
     * add htonl()/ntohl() calls here.
     */
    snprintf(buf, buflen, "%s %d:%d:%d:%d:%u:%u:%u:%s:%s:%s:%s ",
	   socktoa(dest),
	   pkt->li_vn_mode, pkt->stratum, pkt->ppoll, pkt->precision,
	   pkt->rootdelay, pkt->rootdisp,
	   pkt->refid,
	   lfpdump(&pkt->reftime), lfpdump(&pkt->org),
	   lfpdump(&pkt->rec), lfpdump(&pkt->xmt));

    if (len == LEN_PKT_NOMAC)
	strlcat(buf, "nomac", buflen);
    else
	/* dump MAC as len - LEN_PKT_NOMAC chars in hex */
	for (i = 0; i + LEN_PKT_NOMAC < len; i++) {
	    snprintf(buf + strlen(buf), buflen - strlen(buf),
		     "%02x", BIGEND_GETBYTE(pkt->exten[i / sizeof(uint32_t)], i));
	}
}

static void lfpload(char *str, l_fp *fp)
{
    uint64_t	np;

    INSIST(sscanf(str, "%" PRIu64, &np) == 1);
    
    (fp)->l_uf = (np) & 0xFFFFFFFF;
    (fp)->l_ui = (((np) >> FRACTION_PREC) & 0xFFFFFFFF);
}

static int packet_parse(char *pktbuf, struct pkt *pkt)
{
    char refbuf[32], orgbuf[32], recbuf[32], xmtbuf[32], macbuf[BUFSIZ];
    int li_vn_mode = 0, stratum = 0, ppoll = 0, precision = 0;
    size_t pktlen;

    if (sscanf(pktbuf, "%d:%d:%d:%d:%u:%u:%u:%[^:]:%[^:]:%[^:]:%[^:] %s",
		     &li_vn_mode, &stratum,
		     &ppoll, &precision,
		     &pkt->rootdelay, &pkt->rootdisp,
		     &pkt->refid,
	       refbuf, orgbuf, recbuf, xmtbuf, macbuf) != 11)
	return -1;

    /* extra transfers required because the struct members are int8_t */
    pkt->li_vn_mode = (uint8_t)li_vn_mode;
    pkt->stratum = (uint8_t)stratum;
    pkt->ppoll = (uint8_t)ppoll;
    pkt->precision = (int8_t)precision;
    lfpload(refbuf, &pkt->reftime);
    lfpload(orgbuf, &pkt->org);
    lfpload(recbuf, &pkt->rec);
    lfpload(xmtbuf, &pkt->xmt);

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
	    ++pktlen;
	}
    }
    return pktlen;
}

size_t packet_undump(char *bin, int len, char *pktbuf)
{
    struct pkt pkt;
    int pktlen = packet_parse(pktbuf, &pkt);
    INSIST((pktlen != -1) && len >= pktlen);
    /* works because pkt fields and extension are in network byte order */
    memcpy(bin, (char *)&pkt, sizeof(pkt));
    memcpy(bin + sizeof(pkt), (char *)&pkt.exten, pktlen -  sizeof(pkt));
    return pktlen;
}

/* end */
