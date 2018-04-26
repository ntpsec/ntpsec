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
static unsigned long full_recvbufs;	/* recvbufs on full_recv_fifo */
static unsigned long free_recvbufs;	/* recvbufs on free_recv_list */
static unsigned long total_recvbufs;	/* total recvbufs currently in use */
static unsigned long lowater_adds;	/* # of times we have added memory */
static unsigned long buffer_shortfall;	/* # of missed free receive buffers
					   between replenishments */
/*
 * Unmarshalling helper
 */
extern uint32_t extract_32bits_from_stream(uint8_t *);

static DECL_FIFO_ANCHOR(recvbuf_t) full_recv_fifo;
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
full_recvbuffs (void)
{
	return full_recvbufs;
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
	free_recvbufs = total_recvbufs = 0;
	full_recvbufs = lowater_adds = 0;

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
		UNLINK_FIFO(rbunlinked, full_recv_fifo, link);
		if (rbunlinked == NULL)
			break;
		free(rbunlinked);
	}

	for (;;) {
		UNLINK_HEAD_SLIST(rbunlinked, free_recv_list, link);
		if (rbunlinked == NULL)
			break;
		free(rbunlinked);
	}
}
#endif	/* DEBUG */


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

	
void
add_full_recv_buffer(recvbuf_t *rb)
{
	if (rb == NULL) {
		msyslog(LOG_ERR, "ERR: add_full_recv_buffer received NULL buffer");
		return;
	}
	LINK_FIFO(full_recv_fifo, rb, link);
	full_recvbufs++;
}


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


recvbuf_t *
get_full_recv_buffer(void)
{
	recvbuf_t *	rbuf;

	/*
	 * try to grab a full buffer
	 */
	UNLINK_FIFO(rbuf, full_recv_fifo, link);
	if (rbuf != NULL)
		full_recvbufs--;

	return rbuf;
}


/*
 * purge_recv_buffers_for_fd() - purges any previously-received input
 *				 from a given file descriptor.
 */
void
purge_recv_buffers_for_fd(
	SOCKET	fd
	)
{
	recvbuf_t *rbufp;
	recvbuf_t *next;
	recvbuf_t *punlinked;

	for (rbufp = HEAD_FIFO(full_recv_fifo);
	     rbufp != NULL;
	     rbufp = next) {
		next = rbufp->link;
		if (rbufp->fd == fd) {
			UNLINK_MID_FIFO(punlinked, full_recv_fifo,
					rbufp, link, recvbuf_t);
			INSIST(punlinked == rbufp);
			full_recvbufs--;
			freerecvbuf(rbufp);
		}
	}
}


/*
 * Checks to see if there are buffers to process
 */
bool has_full_recv_buffer(void)
{
	if (HEAD_FIFO(full_recv_fifo) != NULL)
		return (true);
	else
		return (false);
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

void
unmarshall_pkt(struct pkt *rpkt, struct recvbuf *rbufp)
{
    rpkt->li_vn_mode = (uint8_t)rbufp->recv_buffer[0];
    rpkt->stratum = (uint8_t)rbufp->recv_buffer[1];
    rpkt->ppoll = (uint8_t)rbufp->recv_buffer[2];
    rpkt->precision = (int8_t)rbufp->recv_buffer[3];
	// rootdelay
	rpkt->rootdelay = extract_32bits_from_stream(&rbufp->recv_buffer[4]);
	// rootdisp
	rpkt->rootdisp = extract_32bits_from_stream(&rbufp->recv_buffer[8]);
	// refid
	rpkt->refid = extract_32bits_from_stream(&rbufp->recv_buffer[12]);
	// reftime
	rpkt->reftime.l_ui = extract_32bits_from_stream(&rbufp->recv_buffer[16]);
	rpkt->reftime.l_uf = extract_32bits_from_stream(&rbufp->recv_buffer[20]);
	// org
	rpkt->org.l_ui = extract_32bits_from_stream(&rbufp->recv_buffer[24]);
	rpkt->org.l_uf = extract_32bits_from_stream(&rbufp->recv_buffer[28]);
	// rec
	rpkt->rec.l_ui = extract_32bits_from_stream(&rbufp->recv_buffer[32]);
	rpkt->rec.l_uf = extract_32bits_from_stream(&rbufp->recv_buffer[36]);
	// xmt
	rpkt->xmt.l_ui = extract_32bits_from_stream(&rbufp->recv_buffer[40]);
	rpkt->xmt.l_uf = extract_32bits_from_stream(&rbufp->recv_buffer[44]);
}

uint32_t
extract_32bits_from_stream(uint8_t *addr)
{
    uint32_t var = 0;
	var = (uint32_t)*addr << 24;
	var |= (uint32_t)*(addr + 1) << 16;
	var |= (uint32_t)*(addr + 2) << 8;
	var |= (uint32_t)*(addr + 3);
	var = ntohl(var);
	return var;
}
