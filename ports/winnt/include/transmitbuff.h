#if !defined __transmitbuff_h
#define __transmitbuff_h

#include "ntp.h"
#if defined HAVE_IO_COMPLETION_PORT
# include "ntp_iocompletionport.h"
#endif

/*
 * Format of a transmitbuf.  These are used by the asynchronous receive
 * routine to store incoming packets and related information.
 */

typedef struct transmitbuf {
	struct transmitbuf *next;		/* next buffer in chain */

	WSABUF	wsabuf;
	time_t	ts;		/* Time stamp for the request */

	/*
	 * union {
	 *	struct	pkt		pkt;
	 *	struct	ntp_control	ctlpkt;
	 *} pkt;
	 */
	char pkt[512];

} transmitbuf;


extern	void	init_transmitbuff	P((void));


/* freetransmitbuf - make a single transmitbuf available for reuse
 */
extern	void	free_transmit_buffer	P((transmitbuf *));

/*  Get a free buffer (typically used so an async
 *  read can directly place data into the buffer
 *
 *  The buffer is removed from the free list. Make sure
 *  you put it back with freetransmitbuf() or 
 */
extern transmitbuf *get_free_transmit_buffer P((void));

#endif /* defined __transmitbuff_h */

