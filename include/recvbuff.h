#ifndef GUARD_RECVBUFF_H
#define GUARD_RECVBUFF_H

#include "ntp.h"
#include "ntp_net.h"
#include "ntp_lists.h"
#include "nts.h"

/*
 * recvbuf memory management
 */
#define RECV_INIT	10	/* 10 buffers initially */
#define RECV_LOWAT	3	/* when we're down to three buffers get more */
#define RECV_INC	5	/* get 5 more at a time */
#define RECV_TOOMANY	40	/* this is way too many buffers */

/*
 * Format of a recvbuf.  Back when ntpd did true asynchronous
 * I/O, these were used by the asynchronous receive routine to store
 * incoming packets and related information. Now, with kernel
 * time stamps on received packets, that complexity has been dropped.
 */

/*
 *  The maximum length NTP packet contains the NTP header, one Autokey
 *  request, one Autokey response, the MAC, and extension fields
 *  (Autokey has been removed from NTPsec, but we need to deal with
 *  the largest packets from legacy versions).  The only free parameter
 *  here is the maximum length of extension data.
 */
#define	RX_BUFF_SIZE	(LEN_PKT_NOMAC + MAX_MAC_LEN + MAX_EXT_LEN)


typedef struct recvbuf recvbuf_t;

struct recvbuf {
	recvbuf_t *	link;		/* next in list */
	sockaddr_u	recv_srcadr;	/* where packet came from */
	struct netendpt *	dstadr;	/* address pkt arrived on */
	SOCKET		fd;		/* fd on which it was received */
	l_fp		recv_time;	/* time of arrival */
	size_t		recv_length;	/* number of octets received */
	uint8_t		recv_buffer[RX_BUFF_SIZE];
	struct parsed_pkt pkt;  /* host-order copy of data from wire */
	int used;		/* reference count */
	bool keyid_present;
	keyid_t keyid;
	int mac_len;
	bool extens_present;
	struct ntspacket_t ntspacket;
#ifdef REFCLOCK
	struct peer *	recv_peer;
#endif /* REFCLOCK */
};

extern	void	init_recvbuff(unsigned int); /* not really pure */

/* freerecvbuf - make a single recvbuf available for reuse
 */
extern	void	freerecvbuf(struct recvbuf *);

/*  Get a free buffer (typically used so an async
 *  read can directly place data into the buffer
 *
 *  The buffer is removed from the free list. Make sure
 *  you put it back with freerecvbuf() or
 */

/* signal safe - no malloc */
extern	struct recvbuf *get_free_recv_buffer(void);


/* number of recvbufs on freelist */
extern unsigned long free_recvbuffs(void);    /* not really pure */
extern unsigned long total_recvbuffs(void);   /* not really pure */
extern unsigned long lowater_additions(void); /* not really pure */

#endif	/* GUARD_RECVBUFF_H */
