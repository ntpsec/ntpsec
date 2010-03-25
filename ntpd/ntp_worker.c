/*
 * ntp_worker.c
 */
#include <config.h>
#include "ntp_workimpl.h"

#ifdef WORKER

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "iosignal.h"
#include "ntp_stdlib.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "ntp_unixtime.h"
#include "ntp_intres.h"


#define CHILD_MAX_IDLE	(3 * 60)	/* seconds, idle worker limit */

int	intres_req_pending;


int
queue_blocking_request(
	blocking_work_req	rtype,
	void *			req,
	size_t			reqsize,
	blocking_work_callback	done_func,
	void *			context
	)
{
	blocking_pipe_header req_hdr;

	req_hdr.octets = sizeof(req_hdr) + reqsize;
	req_hdr.magic_sig = BLOCKING_REQ_MAGIC;
	req_hdr.rtype = rtype;
	req_hdr.done_func = done_func;
	req_hdr.context = context;

	if (0 == intres_req_pending)
		worker_idle_timer = 0;
	intres_req_pending++;

	return send_blocking_req_internal(&req_hdr, req);
}

int queue_blocking_response(
	blocking_pipe_header *		resp,
	size_t				respsize,
	const blocking_pipe_header *	req
	)
{
	resp->octets = respsize;
	resp->magic_sig = BLOCKING_RESP_MAGIC;
	resp->rtype = req->rtype;
	resp->context = req->context;
	resp->done_func = req->done_func;

	return send_blocking_resp_internal(resp);
}


void
process_blocking_response(
	void
	)
{
	blocking_pipe_header *	resp;
	void *			data;

	/*
	 * On Windows send_blocking_resp_internal() may signal the
	 * blocking_response_ready event multiple times while we're
	 * processing a response, so always consume all available
	 * responses before returning to test the event again.
	 */
#ifdef SYS_WINNT
	do {
#endif
		resp = receive_blocking_resp_internal();
		if (NULL != resp) {
			NTP_REQUIRE(BLOCKING_RESP_MAGIC == resp->magic_sig);
			data = (char *)resp + sizeof(*resp);
			intres_req_pending--;
			(*resp->done_func)(resp->rtype, resp->context, resp->octets - sizeof(*resp), data);
			free(resp);
			if (0 == intres_req_pending)
				worker_idle_timer = current_time 
						    + CHILD_MAX_IDLE;
		}
#ifdef SYS_WINNT
	} while (NULL != resp);
#endif
}


/*
 * blocking_child_common runs as a forked child or thread
 */
int
blocking_child_common (
	void
	)
{
	int say_bye;
	blocking_pipe_header *req;

	NLOG(NLOG_SYSEVENT)
		msyslog(LOG_NOTICE, "DNS worker started");

	say_bye = 0;
	while (!say_bye) {
		req = receive_blocking_req_internal();
		if (NULL == req) {
			say_bye = 1;
			break;
		}

		NTP_REQUIRE(BLOCKING_REQ_MAGIC == req->magic_sig);

		switch (req->rtype) {
		case BLOCKING_GETADDRINFO:
			if (blocking_getaddrinfo(req))
				say_bye = 1;
			break;

		case BLOCKING_GETNAMEINFO:
			if (blocking_getnameinfo(req))
				say_bye = 1;
			break;

		default:
			msyslog(LOG_ERR, "unknown req %d to blocking worker", req->rtype);
			say_bye = 1;
		}
		
		free(req);
	}

	NLOG(NLOG_SYSEVENT)
		msyslog(LOG_NOTICE, "DNS worker finished");

	return 0;
}


#else	/* !WORKER follows */
int ntp_worker_nonempty_compilation_unit;
#endif
