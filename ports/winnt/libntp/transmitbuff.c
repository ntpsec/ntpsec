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
#define TRANSMIT_INIT		10	/* 10 buffers initially */
#define TRANSMIT_LOWAT		 3	/* when we're down to three buffers get more */
#define TRANSMIT_INC		 5	/* get 5 more at a time */
#define TRANSMIT_TOOMANY	40	/* this is way too many buffers */

/*
 * Memory allocation
 */
static volatile u_long full_transmitbufs = 0;	/* number of transmitbufs on fulllist */
static volatile u_long free_transmitbufs = 0;	/* number of transmitbufs on freelist */

ISC_LIST(transmitbuf_t)	free_list;		/* Currently used transmit buffers */
ISC_LIST(transmitbuf_t)	full_list;		/* Currently used transmit buffers */

static u_long total_transmitbufs = 0;		/* total transmitbufs currently in use */
static u_long lowater_additions = 0;		/* number of times we have added memory */

static CRITICAL_SECTION TransmitLock;
# define LOCK(lock)	EnterCriticalSection(lock)
# define UNLOCK(lock)	LeaveCriticalSection(lock)

static void 
initialise_buffer(transmitbuf *buff)
{
	memset((char *) buff, 0, sizeof(transmitbuf));

	buff->wsabuf.len = 0;
	buff->wsabuf.buf = (char *) &buff->pkt;
}

static void
add_buffer_to_freelist(transmitbuf *tb)
{
	ISC_LIST_APPEND(free_list, tb, link);
	free_transmitbufs++;
}

static void
create_buffers(int nbufs)
{
	transmitbuf_t *buf;
	int i;

	buf = (transmitbuf_t *) emalloc(nbufs*sizeof(transmitbuf_t));
	for (i = 0; i < nbufs; i++)
	{
		initialise_buffer(buf);
		add_buffer_to_freelist(buf);
		total_transmitbufs++;
		buf++;
	}

	lowater_additions++;
}

extern void
init_transmitbuff(void)
{
	/*
	 * Init buffer free list and stat counters
	 */
	ISC_LIST_INIT(full_list);
	ISC_LIST_INIT(free_list);
	free_transmitbufs = total_transmitbufs = 0;
	full_transmitbufs = lowater_additions = 0;
	create_buffers(TRANSMIT_INIT);

	InitializeCriticalSection(&TransmitLock);
}

static void
delete_buffer_from_full_list(transmitbuf_t *tb) {

	transmitbuf_t *next = NULL;
	transmitbuf_t *lbuf = ISC_LIST_HEAD(full_list);

	while (lbuf != NULL) {
		next = ISC_LIST_NEXT(lbuf, link);
		if (lbuf == tb) {
			ISC_LIST_DEQUEUE(full_list, lbuf, link);
			break;
		}
		else
			lbuf = next;
	}
	full_transmitbufs--;
}

extern void
free_transmit_buffer(transmitbuf_t *rb)
{
	LOCK(&TransmitLock);
	delete_buffer_from_full_list(rb);
	add_buffer_to_freelist(rb);
	UNLOCK(&TransmitLock);
}


extern transmitbuf *
get_free_transmit_buffer(void)
{

	transmitbuf_t * buffer = NULL;
	LOCK(&TransmitLock);
	if (free_transmitbufs <= 0) {
		create_buffers(TRANSMIT_INC);
	}
	buffer = ISC_LIST_HEAD(free_list);
	if (buffer != NULL)
	{
		ISC_LIST_DEQUEUE(free_list, buffer, link);
		free_transmitbufs--;
		ISC_LIST_APPEND(full_list, buffer, link);
		full_transmitbufs++;
	}
	UNLOCK(&TransmitLock);
	return (buffer);
}

