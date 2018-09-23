#ifndef GUARD_RECVBUFF_H
#define GUARD_RECVBUFF_H

#include "ntp.h"
#include "ntp_net.h"
#include "ntp_lists.h"

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
 *  the maximum length NTP packet contains the NTP header, one Autokey
 *  request, one Autokey response and the MAC (Autokey has been removed
 *  from NTPsec, but we need to deal with the largest packets from legacy
 *  versions). Assuming certificates don't get too big, the maximum packet
 *  length is set arbitrarily at 1000.
 */
#define	RX_BUFF_SIZE	1000		/* hail Mary */


typedef struct recvbuf recvbuf_t;

struct recvbuf {
	recvbuf_t *	link;	/* next in list */
	sockaddr_u	recv_srcadr;
	sockaddr_u	srcadr;		/* where packet came from */
	struct netendpt *	dstadr;		/* address pkt arrived on */
	SOCKET		fd;		/* fd on which it was received */
	l_fp		recv_time;	/* time of arrival */
	size_t		recv_length;	/* number of octets received */
	union {
		struct pkt	X_recv_pkt;
		uint8_t		X_recv_buffer[RX_BUFF_SIZE];
	} recv_space;
#define	recv_pkt		recv_space.X_recv_pkt
#define	recv_buffer		recv_space.X_recv_buffer
	struct parsed_pkt pkt;  /* host-order copy of data from wire */
	int used;		/* reference count */
	bool keyid_present;
	keyid_t keyid;
	int mac_len;
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
