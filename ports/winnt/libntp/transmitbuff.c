#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"

#include <isc/list.h>
#include "transmitbuff.h"

/*
 * transmitbuf memory management
 */
#define TRANSMIT_INIT	10	/* 10 buffers initially */
#define TRANSMIT_LOWAT	3	/* when we're down to three buffers get more */
#define TRANSMIT_INC	5	/* get 5 more at a time */
#define TRANSMIT_TOOMANY	40	/* this is way too many buffers */

/*
 * Maximum time in seconds to allow transmit request to complete
 * After that we are free to delete it if we need the buffer
 */

#define	MAX_TRANSMIT_SEND_TIME	60.0	

/*
 * Memory allocation
 */
static volatile u_long full_transmitbufs = 0;		/* number of transmitbufs on fulllist */
static volatile u_long free_transmitbufs = 0;		/* number of transmitbufs on freelist */

typedef struct transmitb transmitb_t;

struct transmitb {
	transmitbuf			*tb;
	ISC_LINK(transmitb_t)		link;
};

ISC_LIST(transmitb_t)	fulllist;		/* Currently used transmit buffers */

static	transmitbuf *volatile freelist = NULL;  /* free buffers */
static	transmitbuf *volatile beginlist = NULL; /* fifo buffers with data */

static u_long total_transmitbufs = 0;		/* total transmitbufs currently in use */
static u_long lowater_additions = 0;	/* number of times we have added memory */

static	transmitbuf initial_bufs[TRANSMIT_INIT]; /* initial allocation */


static CRITICAL_SECTION TransmitLock;
# define LOCK(lock)	EnterCriticalSection(lock)
# define UNLOCK(lock)	LeaveCriticalSection(lock)

static	struct transmitbuf initial_bufs[TRANSMIT_INIT]; /* initial allocation */
static	int eventid = 0;

static void 
initialise_buffer(transmitbuf *buff)
{
	memset((char *) buff, 0, sizeof(transmitbuf));

	buff->wsabuf.len = 0;
	buff->wsabuf.buf = (char *) &buff->pkt;
}


extern void
init_transmitbuff(void)
{
	int i;
	/*
	 * Init buffer free list and stat counters
	 */
	freelist = NULL;
	for (i = 0; i < TRANSMIT_INIT; i++)
	{
		initialise_buffer(&initial_bufs[i]);
		initial_bufs[i].next = (transmitbuf *) freelist;
		freelist = &initial_bufs[i];
	}

	ISC_LIST_INIT(fulllist);
	free_transmitbufs = total_transmitbufs = TRANSMIT_INIT;
	full_transmitbufs = lowater_additions = 0;

	InitializeCriticalSection(&TransmitLock);
}

static void
add_buffer_to_freelist(transmitbuf *tb)
{
	tb->next = freelist;
	freelist = tb;
	free_transmitbufs++;
}

static void
delete_buffer_from_full_list(transmitbuf *tb) {

	transmitb_t *next;
	transmitb_t *lbuf = ISC_LIST_HEAD(fulllist);

	while(lbuf != NULL) {
		next = ISC_LIST_NEXT(lbuf, link);
		if(lbuf->tb == tb) {
			ISC_LIST_DEQUEUE_TYPE(fulllist, lbuf, link, transmitb_t);
			free(lbuf);
			break;
		}
		else
			lbuf = next;
	}
}

/*
 * routine to free up any buffer that has not been freed up
 * after MAX_TRANSMIT_SEND_TIME seconds. Note that we are not being
 * too careful here about the correct value of time since we just need
 * and approximate measure of how much time has elapsed since the
 * packet was sent and this routine is only called if we run out
 * of tranmit buffers.
 */
static int
free_unsent_buffers()
{
	int tot_freed = 0;
	double elapsed_time;
	time_t ct;
	transmitbuf *buf;
	transmitb_t *next;
	transmitb_t *lbuf = ISC_LIST_HEAD(fulllist);

	time(&ct);	/* Current Time */

	LOCK(&TransmitLock);
	while(lbuf != NULL) {
		next = ISC_LIST_NEXT(lbuf, link);
		elapsed_time = difftime(ct, lbuf->tb->ts);
		if (elapsed_time > MAX_TRANSMIT_SEND_TIME) {
			ISC_LIST_DEQUEUE_TYPE(fulllist, lbuf, link, transmitb_t);
			free(lbuf);
			add_buffer_to_freelist(lbuf->tb);
			tot_freed++;
		}
		else
			lbuf = next;
	}
	UNLOCK(&TransmitLock);
	return (tot_freed);
}

static void
create_buffers(void)
{
	transmitbuf *buf;
	int i;
	if (free_unsent_buffers() > 0)
		return;

	buf = (transmitbuf *) emalloc(TRANSMIT_INC*sizeof(transmitbuf));
	for (i = 0; i < TRANSMIT_INC; i++)
	{
		initialise_buffer(buf);
		buf->next = (transmitbuf *) freelist;
		freelist = buf;
		buf++;
	}

	free_transmitbufs += TRANSMIT_INC;
	total_transmitbufs += TRANSMIT_INC;
	lowater_additions++;
}


extern void
free_transmit_buffer(transmitbuf *rb)
{
	LOCK(&TransmitLock);
	add_buffer_to_freelist(rb);
	delete_buffer_from_full_list(rb);
	UNLOCK(&TransmitLock);
}


extern transmitbuf *
get_free_transmit_buffer(void)
{
	transmitb_t *lbuf = (transmitb_t *)malloc(sizeof(transmitb_t));

	transmitbuf * buffer = NULL;
	LOCK(&TransmitLock);
	if (free_transmitbufs <= 0) {
		create_buffers();
	}
	buffer = freelist;
	freelist = buffer->next;
	buffer->next = NULL;
	time(&buffer->ts);	/* Time we gave out the transmit buffer */
	lbuf->tb = buffer;
	ISC_LIST_APPEND(fulllist, lbuf, link);
	--free_transmitbufs;
	UNLOCK(&TransmitLock);
	return buffer;
}

