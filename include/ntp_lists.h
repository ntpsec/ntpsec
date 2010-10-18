/*
 * ntp_lists.h - linked lists common code
 *
 * SLIST: singly-linked lists
 * ==========================
 *
 * These macros implement a simple singly-linked list template.  Both
 * the listhead and per-entry next fields are declared as pointers to
 * the list entry struct type.  Initialization to NULL is typically
 * implicit (for globals and statics) or handled by zeroing of the
 * containing structure.
 *
 * The name of the next link field is passed as an argument to allow
 * membership in several lists at once using multiple next link fields.
 *
 * When possible, placing the link field first in the entry structure
 * allows slightly smaller code to be generated on some platforms.
 *
 * LINK_SLIST(listhead, pentry, nextlink)
 *	add entry at head
 *
 * LINK_TAIL_SLIST(listhead, pentry, nextlink, entrytype)
 *	add entry at tail.  This is O(n), if this is a common
 *	operation the FIFO template may be more appropriate.
 *
 * LINK_SORT_SLIST(listhead, pentry, beforecur, nextlink, entrytype)
 *	add entry in sorted order.  beforecur is an expression comparing
 *	pentry with the current list entry.  The current entry can be
 *	referenced within beforecur as L_S_S_CUR(), which is short for
 *	LINK_SORT_SLIST_CUR().  beforecur is nonzero if pentry sorts
 *	before L_S_S_CUR().
 *
 * UNLINK_HEAD_SLIST(punlinked, listhead, nextlink)
 *	unlink first entry and point punlinked to it, or set punlinked
 *	to NULL if the list is empty.
 *
 * UNLINK_SLIST(punlinked, listhead, ptounlink, nextlink, entrytype)
 *	unlink entry pointed to by ptounlink.  punlinked is set to NULL
 *	if the entry is not found on the list, otherwise it is set to
 *	ptounlink.
 *
 * UNLINK_EXPR_SLIST(punlinked, listhead, expr, nextlink, entrytype)
 *	unlink entry where expression expr is nonzero.  expr can refer
 *	to the entry being tested using UNLINK_EXPR_SLIST_CURRENT(),
 *	alias U_E_S_CUR().  See the implementation of UNLINK_SLIST()
 *	below for an example.
 *	punlinked is pointed to the removed entry or NULL if none
 *	satisfy expr.
 *
 * FIFO: singly-linked lists plus tail pointer
 * ===========================================
 *
 * This is the same as FreeBSD's sys/queue.h STAILQ -- a singly-linked
 * list implementation with tail-pointer maintenance, so that adding
 * at the tail for first-in, first-out access is O(1).
 *
 * DECL_FIFO_ANCHOR(entrytype)
 *	provides the type specification portion of the declaration for
 *	a variable to refer to a FIFO queue (similar to listhead).  The
 *	anchor contains the head and indirect tail pointers.  Example:
 *
 *		#include "ntp_lists.h"
 *
 *		typedef struct myentry_tag myentry;
 *		struct myentry_tag {
 *			myentry *next_link;
 *			...
 *		};
 *
 *		DECL_FIFO_ANCHOR(myentry) my_fifo;
 *
 *		void somefunc(myentry *pentry)
 *		{
 *			LINK_FIFO(my_fifo, pentry, next_link);
 *		}
 *
 *	If DECL_FIFO_ANCHOR is used with stack or heap storage, it
 *	should be initialized to NULL pointers using a = { NULL };
 *	initializer or memset.
 *
 * HEAD_FIFO(anchor)
 * TAIL_FIFO(anchor)
 *	Pointer to first/last entry, NULL if FIFO is empty.
 *
 * LINK_FIFO(anchor, pentry, nextlink)
 *	add entry at tail.
 *
 * UNLINK_FIFO(punlinked, anchor, nextlink)
 *	unlink head entry and point punlinked to it, or set punlinked
 *	to NULL if the list is empty.
 *
 * CONCAT_FIFO(q1, q2, nextlink)
 *	empty fifoq q2 moving its nodes to q1 following q1's existing
 *	nodes.
 *
 * DLIST: doubly-linked lists
 * ==========================
 *
 * Elements on DLISTs always have non-NULL forward and back links,
 * because both link chains are circular.  The beginning/end is marked
 * by the listhead, which is the same type as elements for simplicity.
 * An empty list's listhead has both links set to its own address.
 *
 *
 */
#ifndef NTP_LISTS_H
#define NTP_LISTS_H

#include "ntp_assert.h"

/*
 * For now enable extra debugging code on all builds.  Before long, it
 * will probably make sense to only include the debugging #ifdef DEBUG
 */
#define NTP_DEBUG_LISTS_H

/*
 * If list debugging is not enabled, save a little time by not clearing
 * an entry's link pointer when it is unlinked, as the stale pointer
 * is harmless as long as it is ignored when the entry is not in a
 * list.
 */
#ifndef NTP_DEBUG_LISTS_H
#define MAYBE_Z_LISTS(p)	do { } while (0)
#else
#define MAYBE_Z_LISTS(p)	(p) = NULL
#endif

#ifndef TRUE
# define TRUE	1
# define NTP_LISTS_UNDEF_TRUE
#endif

#define LINK_SLIST(listhead, pentry, nextlink)			\
do {								\
	(pentry)->nextlink = (listhead);			\
	(listhead) = (pentry);					\
} while (0)

#define LINK_TAIL_SLIST(listhead, pentry, nextlink, entrytype)	\
do {								\
	entrytype **pptail;					\
								\
	pptail = &(listhead);					\
	while (*pptail != NULL)					\
		pptail = &((*pptail)->nextlink);		\
								\
	(pentry)->nextlink = NULL;				\
	*pptail = (pentry);					\
} while (0)

#define LINK_SORT_SLIST_CURRENT()	(*ppentry)
#define	L_S_S_CUR()			LINK_SORT_SLIST_CURRENT()

#define LINK_SORT_SLIST(listhead, pentry, beforecur, nextlink,	\
			entrytype)				\
do {								\
	entrytype **ppentry;					\
								\
	ppentry = &(listhead);					\
	while (*ppentry != NULL) {				\
		if (beforecur) {				\
			(pentry)->nextlink = *ppentry;		\
			*ppentry = (pentry);			\
			break;					\
		}						\
		ppentry = &((*ppentry)->nextlink);		\
		if (NULL == *ppentry) {				\
			(pentry)->nextlink = NULL;		\
			*ppentry = (pentry);			\
			break;					\
		}						\
	}							\
} while (0)

#define UNLINK_HEAD_SLIST(punlinked, listhead, nextlink)	\
do {								\
	(punlinked) = (listhead);				\
	if (NULL != (punlinked)) {				\
		(listhead) = (punlinked)->nextlink;		\
		(punlinked)->nextlink = NULL;			\
	}							\
} while (0)

#define UNLINK_EXPR_SLIST_CURRENT()	(*ppentry)
#define	U_E_S_CUR()			UNLINK_EXPR_SLIST_CURRENT()

#define UNLINK_EXPR_SLIST(punlinked, listhead, expr, nextlink,	\
			  entrytype)				\
do {								\
	entrytype **ppentry;					\
								\
	ppentry = &(listhead);					\
								\
	while (!(expr))						\
		if ((*ppentry)->nextlink != NULL)		\
			ppentry = &((*ppentry)->nextlink);	\
		else {						\
			ppentry = NULL;				\
			break;					\
		}						\
								\
	if (ppentry != NULL) {					\
		(punlinked) = *ppentry;				\
		*ppentry = (punlinked)->nextlink;		\
		(punlinked)->nextlink = NULL;			\
	} else							\
		(punlinked) = NULL;				\
} while (0)

#define UNLINK_SLIST(punlinked, listhead, ptounlink, nextlink,	\
		     entrytype)					\
	UNLINK_EXPR_SLIST(punlinked, listhead, (ptounlink) ==	\
	    U_E_S_CUR(), nextlink, entrytype)

#define CHECK_SLIST(listhead, nextlink, entrytype)		\
do {								\
	entrytype *pentry;					\
								\
	for (pentry = (listhead);				\
	     pentry != NULL;					\
	     pentry = pentry->nextlink){			\
		NTP_INSIST(pentry != pentry->nextlink);		\
		NTP_INSIST((listhead) != pentry->nextlink);	\
	}							\
} while (0)

/*
 * FIFO
 */

#define DECL_FIFO_ANCHOR(entrytype)				\
struct {							\
	entrytype *	phead;	/* NULL if list empty */	\
	entrytype **	pptail;	/* NULL if list empty */	\
}

#define HEAD_FIFO(anchor)	((anchor).phead)
#define TAIL_FIFO(anchor)	((NULL == (anchor).pptail)	\
					? NULL			\
					: *((anchor).pptail))

/*
 * For DEBUG builds only, verify both or neither of the anchor pointers
 * are NULL with each operation.
 */
#if !defined(NTP_DEBUG_LISTS_H) || !defined(DEBUG)
#define	CHECK_FIFO_CONSISTENCY(queue)	do { } while (0);
#else
#define	CHECK_FIFO_CONSISTENCY(anchor)				\
	NTP_REQUIRE((NULL == (anchor).phead			\
		     && NULL == (anchor).pptail)		\
		    || (NULL != (anchor).phead			\
		     && NULL != (anchor).pptail))
#endif

#define LINK_FIFO(anchor, pentry, nextlink)			\
do {								\
	CHECK_FIFO_CONSISTENCY(anchor);				\
								\
	(pentry)->nextlink = NULL;				\
	if (NULL != (anchor).pptail) {				\
		(*((anchor).pptail))->nextlink = (pentry);	\
		(anchor).pptail =				\
		    &((*((anchor).pptail))->nextlink);		\
	} else {						\
		(anchor).phead = (pentry);			\
		(anchor).pptail = &((anchor).phead);		\
	}							\
} while (0)

#define UNLINK_FIFO(punlinked, anchor, nextlink)		\
do {								\
	CHECK_FIFO_CONSISTENCY(anchor);				\
								\
	(punlinked) = (anchor).phead;				\
	if (NULL != (punlinked)) {				\
		(anchor).phead = (punlinked)->nextlink;		\
		if (NULL == (anchor).phead)			\
			(anchor).pptail = NULL;			\
		MAYBE_Z_LISTS((punlinked)->nextlink);		\
	}							\
} while (0)

#define CONCAT_FIFO(f1, f2, nextlink)				\
do {								\
	CHECK_FIFO_CONSISTENCY(f1);				\
	CHECK_FIFO_CONSISTENCY(f2);				\
								\
	if ((f2).pptail != NULL) {				\
		if ((f1).pptail != NULL) {			\
			(*(f1).pptail)->nextlink = (f2).phead;	\
			(f1).pptail = (f2).pptail;		\
		} else	{					\
			(f1) = (f2);				\
		}						\
		(f2).phead = NULL;				\
		(f2).pptail = NULL;				\
	}							\
} while (0)

/*
 * DLIST
 */
#define DECL_DLIST_LINK(entrytype, link)			\
struct {							\
	entrytype *	b;					\
	entrytype *	f;					\
} link

#define INIT_DLIST(listhead, link)				\
do {								\
	(listhead).link.f = &(listhead);			\
	(listhead).link.b = &(listhead);			\
} while (0)

#define HEAD_DLIST(listhead, link)				\
	(							\
		(&(listhead) != (listhead).link.f)		\
		    ? (listhead).link.f				\
		    : NULL					\
	)

#define TAIL_DLIST(listhead, link)				\
	(							\
		(&(listhead) != (listhead).link.b)		\
		    ? (listhead).link.b				\
		    : NULL					\
	)

#define NEXT_DLIST(listhead, entry, link)			\
	(							\
		(&(listhead) != (entry)->link.f)		\
		    ? (entry)->link.f				\
		    : NULL					\
	)

#define PREV_DLIST(listhead, entry, link)			\
	(							\
		(&(listhead) != (entry)->link.b)		\
		    ? (entry)->link.b				\
		    : NULL					\
	)

#define LINK_DLIST(listhead, pentry, link)			\
do {								\
	(pentry)->link.f = (listhead).link.f;			\
	(pentry)->link.b = &(listhead);				\
	(listhead).link.f->link.b = (pentry);			\
	(listhead).link.f = (pentry);				\
} while (0)

#define LINK_TAIL_DLIST(listhead, pentry, link)			\
do {								\
	(pentry)->link.b = (listhead).link.b;			\
	(pentry)->link.f = &(listhead);				\
	(listhead).link.b->link.f = (pentry);			\
	(listhead).link.b = (pentry);				\
} while (0)

#define UNLINK_DLIST(ptounlink, link)				\
do {								\
	(ptounlink)->link.b->link.f = (ptounlink)->link.f;	\
	(ptounlink)->link.f->link.b = (ptounlink)->link.b;	\
	(ptounlink)->link.b = NULL;				\
	(ptounlink)->link.f = NULL;				\
} while (0)

#define ITER_DLIST_BEGIN(listhead, iter, link, entrytype)	\
{								\
	entrytype *i_dl_nextiter;				\
								\
	for ((iter) = (listhead).link.f;			\
	     (iter) != &(listhead)				\
	     && ((i_dl_nextiter = (iter)->link.f), TRUE);	\
	     (iter) = i_dl_nextiter) {
#define ITER_DLIST_END()					\
	}							\
}

#define REV_ITER_DLIST_BEGIN(listhead, iter, link, entrytype)	\
{								\
	entrytype *i_dl_nextiter;				\
								\
	for ((iter) = (listhead).link.b;			\
	     (iter) != &(listhead)				\
	     && ((i_dl_nextiter = (iter)->link.b), TRUE);	\
	     (iter) = i_dl_nextiter) {
#define REV_ITER_DLIST_END()					\
	}							\
}


#ifdef NTP_LISTS_UNDEF_TRUE
# undef TRUE
#endif

#endif	/* NTP_LISTS_H */
