#include "config.h"

#include <stdio.h>

#include "ntp_assert.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_lists.h"
#include "recvbuff.h"


/*
 * Memory allocation.
 */
static unsigned long free_recvbufs;	/* recvbufs on free_recv_list */
static unsigned long total_recvbufs;	/* total recvbufs currently in use */
static unsigned long lowater_adds;	/* # of times we have added memory */
static unsigned long buffer_shortfall;	/* # of missed free receive buffers
					   between replenishments */
static recvbuf_t *		   free_recv_list;

#ifdef DEBUG
static void uninit_recvbuff(void);
#endif


unsigned long
free_recvbuffs (void)
{
	return free_recvbufs;
}

unsigned long
total_recvbuffs (void)
{
	return total_recvbufs;
}

unsigned long
lowater_additions(void)
{
	return lowater_adds;
}

static inline void
initialise_buffer(recvbuf_t *buff)
{
	ZERO(*buff);
}

static void
create_buffers(unsigned int nbufs)
{
	recvbuf_t *bufp;
	unsigned int i, abuf;

	abuf = nbufs + buffer_shortfall;
	buffer_shortfall = 0;

#ifndef DEBUG
	bufp = emalloc_zero(abuf * sizeof(*bufp));
#endif

	for (i = 0; i < abuf; i++) {
#ifdef DEBUG
		/*
		 * Allocate each buffer individually so they can be
		 * free()d during ntpd shutdown on DEBUG builds to
		 * keep them out of heap leak reports.
		 */
		bufp = emalloc_zero(sizeof(*bufp));
#endif
		LINK_SLIST(free_recv_list, bufp, link);
		bufp++;
		free_recvbufs++;
		total_recvbufs++;
	}
	lowater_adds++;
	/* coverity[leaked_storage] */
}

void
init_recvbuff(unsigned int nbufs)
{

	/*
	 * Init buffer free list and stat counters
	 */
	free_recvbufs = total_recvbufs = lowater_adds = 0;

	create_buffers(nbufs);

#ifdef DEBUG
	atexit(&uninit_recvbuff);
#endif
}


#ifdef DEBUG
static void
uninit_recvbuff(void)
{
	recvbuf_t *rbunlinked;

	for (;;) {
		UNLINK_HEAD_SLIST(rbunlinked, free_recv_list, link);
		if (rbunlinked == NULL)
			break;
		free(rbunlinked);
	}
}
#endif	/* DEBUG */


recvbuf_t *
get_free_recv_buffer(void)
{
        recvbuf_t *buffer;

        UNLINK_HEAD_SLIST(buffer, free_recv_list, link);
        if (buffer != NULL) {
                free_recvbufs--;
                initialise_buffer(buffer);
                buffer->used++;
        } else {
                buffer_shortfall++;
        }

        return buffer;
}

/*
 * freerecvbuf - make a single recvbuf available for reuse
 */
void
freerecvbuf(recvbuf_t *rb)
{
	if (rb == NULL) {
		msyslog(LOG_ERR, "ERR: freerecvbuff received NULL buffer");
		return;
	}

	rb->used--;
	if (rb->used != 0)
		msyslog(LOG_ERR, "ERR: ******** freerecvbuff non-zero usage: %d *******", rb->used);
	LINK_SLIST(free_recv_list, rb, link);
	free_recvbufs++;
}



#ifdef NTP_DEBUG_LISTS
void
check_gen_fifo_consistency(void *fifo)
{
	gen_fifo *pf;
	gen_node *pthis;
	gen_node **pptail;

	pf = fifo;
	REQUIRE((NULL == pf->phead && NULL == pf->pptail) ||
		(NULL != pf->phead && NULL != pf->pptail));

	pptail = &pf->phead;
	for (pthis = pf->phead;
	     pthis != NULL;
	     pthis = pthis->link)
		if (NULL != pthis->link)
			pptail = &pthis->link;

	REQUIRE(NULL == pf->pptail || pptail == pf->pptail);
}
#endif	/* NTP_DEBUG_LISTS */

