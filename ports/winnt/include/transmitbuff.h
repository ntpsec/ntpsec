#ifndef TRANSMITBUFF_H
#define TRANSMITBUFF_H

#include "ntp.h"
#include <isc/list.h>

/*
 * Format of a transmitbuf.  These are used by the asynchronous receive
 * routine to store outgoing packets and related information.
 */

typedef struct transmitbuf transmitbuf_t;

typedef struct transmitbuf {
	ISC_LINK(transmitbuf_t)	link;

	time_t	ts;		/* Time stamp for the request */

	/*
	 * union {
	 *	struct	pkt		pkt;
	 *	struct	ntp_control	ctlpkt;
	 *} pkt;
	 */
	char pkt[512];

} transmitbuf;


extern	void	init_transmitbuff	(void);


/* freetransmitbuf - make a single transmitbuf available for reuse
 */
extern	void	free_transmit_buffer	(transmitbuf_t *);

/*  Get a free buffer (typically used so an async
 *  read can directly place data into the buffer
 *
 *  The buffer is removed from the free list. Make sure
 *  you put it back with freetransmitbuf() or 
 */
extern transmitbuf_t *get_free_transmit_buffer (void);

#endif /* TRANSMITBUFF_H */

