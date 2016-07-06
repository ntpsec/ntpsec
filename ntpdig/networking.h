#ifndef GUARD_NETWORKING_H
#define GUARD_NETWORKING_H

#include <arpa/inet.h>
#include <netinet/in.h>

#include <strings.h>	
#include <errno.h>
#include <config.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <ntp_rfc2553.h>
#include <ntp_stdlib.h>
#include <ntp_machine.h>
#include <ntp_unixtime.h>
#include <ntp_fp.h>
#include <ntp.h>

#include "crypto.h"
#include "log.h"
#include "utilities.h"

/* FIXME To be replaced by the constants in ntp.h */
#define SERVER_UNUSEABLE -1 /* Skip server */
#define PACKET_UNUSEABLE -2 /* Discard packet and try to get a useable packet again if not tried too often */
#define SERVER_AUTH_FAIL -3 /* Authentication failed, act upon settings */
#define KOD_DEMOBILIZE -4   /* KOD packet with code DENY or RSTR, stop all communication and save KOD information */
#define KOD_RATE -5	    /* KOD packet with code RATE, reduce poll intervall */
#define BROADCAST_FAILED -6

/* prototypes */
int sendpkt(SOCKET rsock, sockaddr_u *dest, struct pkt *pkt, int len);
int recvdata(SOCKET rsock, sockaddr_u *sender, void *rdata,
	     int rdata_len);
int recvpkt(SOCKET rsock, struct pkt *rpkt, unsigned int rsize,
	    struct pkt *spkt);
int process_pkt(struct pkt *rpkt, sockaddr_u *sas, int pkt_len,
		int mode, struct pkt *spkt, const char *func_name,
		bool authenticate);

/* Shortened peer structure. Not absolutely necessary yet */
struct speer {
	struct speer *next;
	sockaddr_u srcadr;
	uint8_t version;
	uint8_t hmode;
	uint8_t hpoll;
	uint8_t minpoll;
	uint8_t maxpoll;
	u_int flags;
	uint8_t num_events;
	uint8_t ttl;
	uint8_t leap;
	uint8_t pmode;
	uint8_t stratum;
	uint8_t ppoll;
	uint8_t precision;	/* should be int8_t */
	uint32_t refid;
	l_fp reftime;
	keyid_t keyid;

#define clear_to_zero status
	
	l_fp	rec;		/* receive time stamp */
	l_fp	xmt;		/* transmit time stamp */
	l_fp	dst;		/* destination timestamp */
	l_fp	org;		/* origin timestamp */
	double	offset;		/* peer clock offset */
	double	delay;		/* peer roundtrip delay */
};





#endif
