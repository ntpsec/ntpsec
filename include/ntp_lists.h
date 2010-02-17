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
 *	add entry at tail
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

#define LINK_DLIST(listhead, pentry, link)			\
do {								\
	(pentry)->link.f = (listhead).link.f;			\
	(pentry)->link.b = &(listhead);				\
	(listhead).link.f->link.b = (pentry);			\
	(listhead).link.f = (pentry);				\
} while (0)

#define UNLINK_DLIST(punlink, link)				\
do {								\
	(punlink)->link.b->link.f = (punlink)->link.f;		\
	(punlink)->link.f->link.b = (punlink)->link.b;		\
	(punlink)->link.b = NULL;				\
	(punlink)->link.f = NULL;				\
} while (0)

#define ITER_DLIST_BEGIN(listhead, iter, link, entrytype)	\
{								\
	entrytype *i_dl_next;					\
								\
	for ((iter) = (listhead).link.f;			\
	     (iter) != &(listhead)				\
	     && ((i_dl_next = (iter)->link.f), TRUE);	\
	     (iter) = i_dl_next) {

#define ITER_DLIST_END()					\
	}							\
}


#ifdef NTP_LISTS_UNDEF_TRUE
# undef TRUE
#endif

#endif	/* NTP_LISTS_H */
