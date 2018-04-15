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
    rpkt->rootdelay = (u_fp)rbufp->recv_buffer[4] << 24;
    rpkt->rootdelay |= (u_fp)rbufp->recv_buffer[5] << 16;
    rpkt->rootdelay |= (u_fp)rbufp->recv_buffer[6] << 8;
    rpkt->rootdelay |= (u_fp)rbufp->recv_buffer[7];
    rpkt->rootdelay = ntohl(rpkt->rootdelay);
	// rootdisp
    rpkt->rootdisp = (u_fp)rbufp->recv_buffer[8] << 24;
    rpkt->rootdisp |= (u_fp)rbufp->recv_buffer[9] << 16;
    rpkt->rootdisp |= (u_fp)rbufp->recv_buffer[10] << 8;
    rpkt->rootdisp |= (u_fp)rbufp->recv_buffer[11];
    rpkt->rootdisp = ntohl(rpkt->rootdisp);
	// refid
    rpkt->refid = (uint32_t)rbufp->recv_buffer[12] << 24;
    rpkt->refid |= (uint32_t)rbufp->recv_buffer[13] << 16;
    rpkt->refid |= (uint32_t)rbufp->recv_buffer[14] << 8;
    rpkt->refid |= (uint32_t)rbufp->recv_buffer[15];
    rpkt->refid = ntohl(rpkt->refid);
	// reftime
	rpkt->reftime.l_ui = (uint32_t)rbufp->recv_buffer[16] << 24;
	rpkt->reftime.l_ui |= (uint32_t)rbufp->recv_buffer[17] << 16;
	rpkt->reftime.l_ui |= (uint32_t)rbufp->recv_buffer[18] << 8;
	rpkt->reftime.l_ui |= (uint32_t)rbufp->recv_buffer[19];
	rpkt->reftime.l_ui = ntohl(rpkt->reftime.l_ui);
	rpkt->reftime.l_uf = (uint32_t)rbufp->recv_buffer[20] << 24;
	rpkt->reftime.l_uf |= (uint32_t)rbufp->recv_buffer[21] << 16;
	rpkt->reftime.l_uf |= (uint32_t)rbufp->recv_buffer[22] << 8;
	rpkt->reftime.l_uf |= (uint32_t)rbufp->recv_buffer[23];
	rpkt->reftime.l_uf = ntohl(rpkt->reftime.l_uf);
	// org
	rpkt->org.l_ui = (uint32_t)rbufp->recv_buffer[24] << 24;
	rpkt->org.l_ui |= (uint32_t)rbufp->recv_buffer[25] << 16;
	rpkt->org.l_ui |= (uint32_t)rbufp->recv_buffer[26] << 8;
	rpkt->org.l_ui |= (uint32_t)rbufp->recv_buffer[27];
	rpkt->org.l_ui = ntohl(rpkt->org.l_ui);
	rpkt->org.l_uf = (uint32_t)rbufp->recv_buffer[28] << 24;
	rpkt->org.l_uf |= (uint32_t)rbufp->recv_buffer[29] << 16;
	rpkt->org.l_uf |= (uint32_t)rbufp->recv_buffer[30] << 8;
	rpkt->org.l_uf |= (uint32_t)rbufp->recv_buffer[31];
	rpkt->org.l_uf = ntohl(rpkt->org.l_uf);
	// rec
	rpkt->rec.l_ui = (uint32_t)rbufp->recv_buffer[32] << 24;
	rpkt->rec.l_ui |= (uint32_t)rbufp->recv_buffer[33] << 16;
	rpkt->rec.l_ui |= (uint32_t)rbufp->recv_buffer[34] << 8;
	rpkt->rec.l_ui |= (uint32_t)rbufp->recv_buffer[35];
	rpkt->rec.l_ui = ntohl(rpkt->rec.l_ui);
	rpkt->rec.l_uf = (uint32_t)rbufp->recv_buffer[36] << 24;
	rpkt->rec.l_uf |= (uint32_t)rbufp->recv_buffer[37] << 16;
	rpkt->rec.l_uf |= (uint32_t)rbufp->recv_buffer[38] << 8;
	rpkt->rec.l_uf |= (uint32_t)rbufp->recv_buffer[39];
	rpkt->rec.l_uf = ntohl(rpkt->rec.l_uf);
	// xmt
	rpkt->xmt.l_ui = (uint32_t)rbufp->recv_buffer[40] << 24;
	rpkt->xmt.l_ui |= (uint32_t)rbufp->recv_buffer[41] << 16;
	rpkt->xmt.l_ui |= (uint32_t)rbufp->recv_buffer[42] << 8;
	rpkt->xmt.l_ui |= (uint32_t)rbufp->recv_buffer[43];
	rpkt->xmt.l_ui = ntohl(rpkt->xmt.l_ui);
	rpkt->xmt.l_uf = (uint32_t)rbufp->recv_buffer[44] << 24;
	rpkt->xmt.l_uf |= (uint32_t)rbufp->recv_buffer[45] << 16;
	rpkt->xmt.l_uf |= (uint32_t)rbufp->recv_buffer[46] << 8;
	rpkt->xmt.l_uf |= (uint32_t)rbufp->recv_buffer[47];
	rpkt->xmt.l_uf = ntohl(rpkt->xmt.l_uf);
}
