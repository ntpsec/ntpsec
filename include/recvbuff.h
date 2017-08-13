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
 * incoming packets and related information. Now, with faster processors
 * and lower latency in the synchronous I/O loop, that complexity
 * has been dropped.
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
	union {
		sockaddr_u	X_recv_srcadr;
		struct peer *	X_recv_peer;
	} X_from_where;
#define recv_srcadr		X_from_where.X_recv_srcadr
#define recv_peer		X_from_where.X_recv_peer
	sockaddr_u	srcadr;		/* where packet came from */
	endpt *		dstadr;		/* address pkt arrived on */
	SOCKET		fd;		/* fd on which it was received */
	int		cast_flags;	/* unicast/broadcast/manycast mode */
	l_fp		recv_time;	/* time of arrival */
	void		(*receiver)(struct recvbuf *); /* callback */
	size_t		recv_length;	/* number of octets received */
	union {
		struct pkt	X_recv_pkt;
		uint8_t		X_recv_buffer[RX_BUFF_SIZE];
	} recv_space;
#define	recv_pkt		recv_space.X_recv_pkt
#define	recv_buffer		recv_space.X_recv_buffer
	int used;		/* reference count */
#ifdef REFCLOCK
	bool network_packet;
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

/*   Add a buffer to the full list
 */
extern	void	add_full_recv_buffer(struct recvbuf *);

/* number of recvbufs on freelist */
extern unsigned long free_recvbuffs(void);    /* not really pure */
extern unsigned long full_recvbuffs(void);    /* not really pure */
extern unsigned long total_recvbuffs(void);   /* not really pure */
extern unsigned long lowater_additions(void); /* not really pure */
		
/*  Returns the next buffer in the full list.
 *
 */
extern	struct recvbuf *get_full_recv_buffer(void);

/*
 * purge_recv_buffers_for_fd() - purges any previously-received input
 *				 from a given file descriptor.
 */
extern	void purge_recv_buffers_for_fd(SOCKET);

/*
 * Checks to see if there are buffers to process
 */
extern bool has_full_recv_buffer(void); /* not really pure */

#endif	/* GUARD_RECVBUFF_H */
