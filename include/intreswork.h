/*
 * intreswork.h -- declarations private to ntp_intres.c, ntp_worker.c.
 */
#ifndef GUARD_INTRESWORK_H
#define GUARD_INTRESWORK_H

#include "ntp_worker.h"

#ifdef WORKER

extern int blocking_getaddrinfo(blocking_child *,
				blocking_pipe_header *);
extern int blocking_getnameinfo(blocking_child *,
				blocking_pipe_header *);

#endif	/* WORKER */

#endif	/* GUARD_INTRESWORK_H */
