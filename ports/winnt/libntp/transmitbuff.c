#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"

#include "transmitbuff.h"

extern int debug;
/*
 * transmitbuf memory management
 */
#define TRANSMIT_INIT	10	/* 10 buffers initially */
#define TRANSMIT_LOWAT	3	/* when we're down to three buffers get more */
#define TRANSMIT_INC	5	/* get 5 more at a time */
#define TRANSMIT_TOOMANY	40	/* this is way too many buffers */

/*
 * Memory allocation
 */
static volatile u_long full_transmitbufs = 0;		/* number of transmitbufs on fulllist */
static volatile u_long free_transmitbufs = 0;		/* number of transmitbufs on freelist */

static	struct transmitbuf *volatile freelist = NULL;  /* free buffers */
static	struct transmitbuf *volatile fulllist = NULL;  /* lifo buffers with data */
static	struct transmitbuf *volatile beginlist = NULL; /* fifo buffers with data */

static u_long total_transmitbufs = 0;		/* total transmitbufs currently in use */
static u_long lowater_additions = 0;	/* number of times we have added memory */

static	struct transmitbuf initial_bufs[TRANSMIT_INIT]; /* initial allocation */


#if defined(HAVE_SIGNALED_IO)
# define TRANSMIT_BLOCK_IO()	BLOCKIO()
# define TRANSMIT_UNBLOCK_IO()	UNBLOCKIO()
#elif defined(HAVE_IO_COMPLETION_PORT)
static CRITICAL_SECTION TransmitCritSection;
# define TRANSMIT_BLOCK_IO()	EnterCriticalSection(&TransmitCritSection)
# define TRANSMIT_UNBLOCK_IO()	LeaveCriticalSection(&TransmitCritSection)
#else
# define TRANSMIT_BLOCK_IO()
# define TRANSMIT_UNBLOCK_IO()
#endif


static void 
initialise_buffer(struct transmitbuf *buff)
{
	memset((char *) buff, 0, sizeof(struct transmitbuf));

#if defined HAVE_IO_COMPLETION_PORT
	buff->iocompletioninfo.overlapped.hEvent = CreateEvent(NULL, FALSE,FALSE, NULL);
	buff->wsabuf.len = 0;
	buff->wsabuf.buf = (char *) &buff->pkt;
#endif
}


extern void
init_transmitbuff(void)
{
	int i;
	/*
	 * Init buffer free list and stat counters
	 */
	freelist = 0;
	for (i = 0; i < TRANSMIT_INIT; i++)
	{
		initialise_buffer(&initial_bufs[i]);
		initial_bufs[i].next = (struct transmitbuf *) freelist;
		freelist = &initial_bufs[i];
	}

	fulllist = 0;
	free_transmitbufs = total_transmitbufs = TRANSMIT_INIT;
	full_transmitbufs = lowater_additions = 0;

#if defined(HAVE_IO_COMPLETION_PORT)
	InitializeCriticalSection(&TransmitCritSection);
#endif
}


static void
create_buffers(void)
{
	register struct transmitbuf *buf;
	int i;

	buf = (struct transmitbuf *)
	    emalloc(TRANSMIT_INC*sizeof(struct transmitbuf));
	for (i = 0; i < TRANSMIT_INC; i++)
	{
		initialise_buffer(buf);
		buf->next = (struct transmitbuf *) freelist;
		freelist = buf;
		buf++;
	}

	free_transmitbufs += TRANSMIT_INC;
	total_transmitbufs += TRANSMIT_INC;
	lowater_additions++;
}


extern void
free_transmit_buffer(
	struct transmitbuf *rb
	)
{
	TRANSMIT_BLOCK_IO();
	rb->next = freelist;
	freelist = rb;
	free_transmitbufs++;
	TRANSMIT_UNBLOCK_IO();
}


extern struct transmitbuf *
get_free_transmit_buffer(void)
{
	struct transmitbuf * buffer = NULL;
	TRANSMIT_BLOCK_IO();
	if (free_transmitbufs <= 0) {
		create_buffers();
	}
	buffer = freelist;
	freelist = buffer->next;
	buffer->next = NULL;
	--free_transmitbufs;
	TRANSMIT_UNBLOCK_IO();
	return buffer;
}

