#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_io.h"
#include "recvbuff.h"
#include "iosignal.h"

#include <isc/list.h>
/*
 * Memory allocation
 */
static u_long volatile full_recvbufs;	/* number of recvbufs on fulllist */
static u_long volatile free_recvbufs;	/* number of recvbufs on freelist */
static u_long volatile total_recvbufs;	/* total recvbufs currently in use */
static u_long volatile lowater_adds;	/* number of times we have added memory */


static ISC_LIST(recvbuf_t)	full_list;	/* Currently used recv buffers */
static ISC_LIST(recvbuf_t)	free_list;	/* Currently unused buffers */
	
#if defined(HAVE_IO_COMPLETION_PORT)

/*
 * For Windows we need to set up a lock to manipulate the
 * recv buffers to prevent corruption. We keep it lock for as
 * short a time as possible
 */
static CRITICAL_SECTION RecvLock;
# define LOCK()		EnterCriticalSection(&RecvLock)
# define UNLOCK()	LeaveCriticalSection(&RecvLock)
#else
# define LOCK()	
# define UNLOCK()	
#endif

u_long
free_recvbuffs (void)
{
	return free_recvbufs;
}

u_long
full_recvbuffs (void)
{
	return full_recvbufs;
}

u_long
total_recvbuffs (void)
{
	return total_recvbufs;
}

u_long
lowater_additions(void)
{
	return lowater_adds;
}

static void 
initialise_buffer(recvbuf_t *buff)
{
	memset((char *) buff, 0, sizeof(recvbuf_t));

#if defined HAVE_IO_COMPLETION_PORT
	buff->wsabuff.len = RX_BUFF_SIZE;
	buff->wsabuff.buf = (char *) buff->recv_buffer;
#endif
}

static void
create_buffers(int nbufs)
{
	register recvbuf_t *buf;
	int i;
	buf = (recvbuf_t *)
	    emalloc(nbufs*sizeof(recvbuf_t));
	for (i = 0; i < nbufs; i++)
	{
		initialise_buffer(buf);
		ISC_LIST_APPEND(free_list, buf, link);
		buf++;
		free_recvbufs++;
		total_recvbufs++;
	}
	lowater_adds++;
}

void
init_recvbuff(int nbufs)
{

	/*
	 * Init buffer free list and stat counters
	 */
	ISC_LIST_INIT(full_list);
	ISC_LIST_INIT(free_list);
	free_recvbufs = total_recvbufs = 0;
	full_recvbufs = lowater_adds = 0;

	create_buffers(nbufs);

#if defined(HAVE_IO_COMPLETION_PORT)
	InitializeCriticalSection(&RecvLock);
#endif

}

/*
 * freerecvbuf - make a single recvbuf available for reuse
 */
void
freerecvbuf(recvbuf_t *rb)
{
	LOCK();
	BLOCKIO();
	ISC_LIST_APPEND(free_list, rb, link);
	free_recvbufs++;
	UNBLOCKIO();
	UNLOCK();
}

	
void
add_full_recv_buffer(recvbuf_t *rb)
{
	LOCK();
	ISC_LIST_APPEND(full_list, rb, link);
	full_recvbufs++;
	UNLOCK();
}

recvbuf_t *
get_free_recv_buffer(void)
{
	recvbuf_t * buffer = NULL;
	LOCK();
	if (free_recvbufs <= 0)
	{
		create_buffers(RECV_INC);
	}
	buffer = ISC_LIST_HEAD(free_list);
	if (buffer == NULL)
	{
		msyslog(LOG_ERR, "free recvbufs was incorrect (%ld)",
				    free_recvbufs);
	}
	else
	{
		ISC_LIST_DEQUEUE(free_list, buffer, link);
	}
	UNLOCK();
	return (buffer);
}

recvbuf_t *
get_full_recv_buffer(void)
{
	recvbuf_t *rbuf;
	LOCK();
	rbuf = ISC_LIST_HEAD(full_list);
	if (rbuf != NULL)
	{
		ISC_LIST_DEQUEUE(full_list, rbuf, link);
		--full_recvbufs;
	}
	UNLOCK();
	return (rbuf);
}
