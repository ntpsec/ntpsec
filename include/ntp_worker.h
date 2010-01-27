/*
 * ntp_worker.h
 */

#ifndef NTP_WORKER_H
#define NTP_WORKER_H

#include "ntp_workimpl.h"

#ifdef WORKER

/* #define TEST_BLOCKING_WORKER */	/* ntp_config.c ntp_intres.c */

typedef enum blocking_work_req_tag {
	BLOCKING_GETNAMEINFO,
	BLOCKING_GETADDRINFO,
} blocking_work_req;

typedef void (*blocking_work_callback)(blocking_work_req, void *, size_t, void *);

typedef enum blocking_magic_sig_e {
	BLOCKING_REQ_MAGIC  = 0xb10c7ecf,
	BLOCKING_RESP_MAGIC = 0xb10c7e54,
} blocking_magic_sig;

/*
 * The same header is used for both requests to and responses from
 * the child.  In the child, done_func and context are opaque.
 */
typedef struct blocking_pipe_header_tag {
	size_t			octets;
	blocking_magic_sig	magic_sig;
	blocking_work_req	rtype;
	blocking_work_callback	done_func;
	void *			context;
} blocking_pipe_header;

extern int	intres_req_pending;

extern int queue_blocking_request(blocking_work_req, void *, size_t, blocking_work_callback, void *);
extern int queue_blocking_response(blocking_pipe_header *, size_t, const blocking_pipe_header *);
extern int send_blocking_req_internal(blocking_pipe_header *, void *);
extern int send_blocking_resp_internal(blocking_pipe_header *);
extern blocking_pipe_header *receive_blocking_req_internal(void);
extern blocking_pipe_header *receive_blocking_resp_internal(void);
extern int blocking_child_common (void);
extern void exit_worker(int);
extern int worker_sleep(time_t);

#endif	/* WORKER */

#endif	/* !NTP_WORKER_H */
