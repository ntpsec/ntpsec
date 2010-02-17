/*
 * ntp_intres.c - Implements a generic blocking worker child or thread,
 *		  initially to provide a nonblocking solution for DNS
 *		  name to address lookups available with getaddrinfo().
 *
 * This is a new implementation as of 2009 sharing the filename and
 * very little else with the prior implementation, which used a
 * temporary file to receive a single set of requests from the parent,
 * and a NTP mode 7 authenticated request to push back responses.
 *
 * A primary goal in rewriting this code was the need to support the
 * pool configuration directive's requirement to retrieve multiple
 * addresses resolving a single name, which has previously been
 * satisfied with blocking resolver calls from the ntpd mainline code.
 *
 * A secondary goal is to provide a generic mechanism for other
 * blocking operations to be delegated to a worker using a common
 * model for both Unix and Windows ntpd.  ntp_worker.c, work_fork.c,
 * and work_thread.c implement the generic mechanism.  This file
 * implements the two current consumers, getaddrinfo_sometime() and the
 * presently unused getnameinfo_sometime().
 *
 * Both routines deliver results to a callback and manage memory
 * allocation, meaning there is no freeaddrinfo_sometime().
 *
 * The initial implementation for Unix uses a pair of unidirectional
 * pipes, one each for requests and responses, connecting the forked
 * blocking child worker with the ntpd mainline.  The threaded code
 * uses arrays of pointers to queue requests and responses.
 *
 * Memory is managed differently for a child process, which mallocs
 * request buffers to read from the pipe into, whereas the threaded
 * code mallocs a copy of the request to hand off to the worker via
 * the queueing array.  The resulting request buffer is free()d by
 * platform-independent code.  A wrinkle is the request needs to be
 * available to the requestor during response processing.
 *
 * Response memory allocation is also platform-dependent.  With a
 * separate process and pipes, the response is free()d after being
 * written to the pipe.  With threads, the same memory is handed
 * over and the requestor frees it after processing is completed.
 *
 * The code should be generalized to support threads on Unix using
 * much of the same code used for Windows initially.
 *
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_workimpl.h"

#ifdef WORKER

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

/**/
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <arpa/inet.h>
/**/
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>		/* MAXHOSTNAMELEN (often) */
#endif

#if !defined(HAVE_RES_INIT) && defined(HAVE___RES_INIT)
# define HAVE_RES_INIT
#endif

#if defined(HAVE_RESOLV_H) && defined(HAVE_RES_INIT)
# ifdef HAVE_ARPA_NAMESER_H
#  include <arpa/nameser.h> /* DNS HEADER struct */
# endif
# ifdef HAVE_NETDB_H
#  include <netdb.h>
# endif
# include <resolv.h>
# ifdef RES_TIMEOUT
#  undef RES_TIMEOUT	/* resolv.h has one, we want ours */
# endif
# ifdef HAVE_INT32_ONLY_WITH_DNS
#  define HAVE_INT32
# endif
# ifdef HAVE_U_INT32_ONLY_WITH_DNS
#  define HAVE_U_INT32
# endif
#endif

#include "ntp_stdlib.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "ntp_unixtime.h"
#include "ntp_intres.h"


/*
 * Following are implementations of getaddrinfo_sometime() and
 * getnameinfo_sometime().  Each is implemented in three routines:
 *
 * getaddrinfo_sometime()		getnameinfo_sometime()
 * blocking_getaddrinfo()		blocking_getnameinfo()
 * getaddrinfo_sometime_complete()	getnameinfo_sometime_complete()
 *
 * The first runs in the parent and marshalls (or serializes) request
 * parameters into a request blob which is processed in the child by
 * the second routine, blocking_*(), which serializes the results into
 * a response blob unpacked by the third routine, *_complete(), which
 * calls the callback routine provided with the request and frees
 * _request_ memory allocated by the first routine.  Response memory
 * is managed by the code which calls the *_complete routines.
 */

#define	INITIAL_DNS_RETRY	2	/* seconds between queries */

typedef struct blocking_gai_req_tag {
	size_t			octets;
	time_t			scheduled;
	time_t			earliest;
	struct addrinfo		hints;
	int			retry;
	gai_sometime_callback	callback;
	void *			context;
	size_t			nodesize;
	size_t			servsize;
} blocking_gai_req;

typedef struct blocking_gai_resp_tag {
	size_t			octets;
	int			retcode;
	int			gai_errno; /* for EAI_SYSTEM case */
	int			ai_count;
	/*
	 * Followed by ai_count struct addrinfo and then ai_count
	 * sockaddr_u and finally the canonical name strings.
	 */
} blocking_gai_resp;

typedef struct blocking_gni_req_tag {
	size_t			octets;
	time_t			scheduled;
	time_t			earliest;
	int			retry;
	size_t			hostoctets;
	size_t			servoctets;
	int			flags;
	gni_sometime_callback	callback;
	void *			context;
	sockaddr_u		socku;
} blocking_gni_req;

typedef struct blocking_gni_resp_tag {
	size_t			octets;
	int			retcode;
	int			gni_errno; /* for EAI_SYSTEM case */
	size_t			hostoctets;
	size_t			servoctets;
	/*
	 * Followed by hostoctets bytes of null-terminated host,
	 * then servoctets bytes of null-terminated service.
	 */
} blocking_gni_resp;

static	time_t	next_dns_timeslot;
static	time_t	ignore_scheduled_before;
#ifdef HAVE_RES_INIT
static	time_t	next_res_init;
#endif

static	void	scheduled_sleep(time_t, time_t);
static	void	manage_dns_retry_interval(time_t *, time_t *, int *);
static	int	should_retry_dns(int, int);
static	void	getaddrinfo_sometime_complete(blocking_work_req, void *,
					      size_t, void *);
static	void	getnameinfo_sometime_complete(blocking_work_req, void *,
					      size_t, void *);



/*
 * getaddrinfo_sometime - uses blocking child to call getaddrinfo then
 *			  invokes provided callback completion function.
 */
int
getaddrinfo_sometime(
	const char *		node,
	const char *		service,
	const struct addrinfo *	hints,
	gai_sometime_callback	callback,
	void *			context
	)
{
	blocking_gai_req *	gai_req;
	size_t			req_size;
	size_t			nodesize;
	size_t			servsize;
	time_t			now;
	
	NTP_REQUIRE(NULL != node);
	if (NULL != hints) {
		NTP_REQUIRE(0 == hints->ai_addrlen);
		NTP_REQUIRE(NULL == hints->ai_addr);
		NTP_REQUIRE(NULL == hints->ai_canonname);
		NTP_REQUIRE(NULL == hints->ai_next);
	}

	nodesize = strlen(node) + 1;
	servsize = strlen(service) + 1;
	req_size = sizeof(*gai_req) + nodesize + servsize;

	gai_req = emalloc(req_size);

	gai_req->octets = req_size;
	now = time(NULL);
	next_dns_timeslot = max(now, next_dns_timeslot);
	gai_req->scheduled = now;
	gai_req->earliest = next_dns_timeslot;
	if (NULL == hints)
		memset(&gai_req->hints, 0, sizeof(gai_req->hints));
	else
		gai_req->hints = *hints;
	gai_req->retry = INITIAL_DNS_RETRY;
	gai_req->callback = callback;
	gai_req->context = context;
	gai_req->nodesize = nodesize;
	gai_req->servsize = servsize;

	memcpy((char *)gai_req + sizeof(*gai_req), node, nodesize);
	memcpy((char *)gai_req + sizeof(*gai_req) + nodesize, service,
	       servsize);

	if (queue_blocking_request(
		BLOCKING_GETADDRINFO,
		gai_req,
		req_size, 
		&getaddrinfo_sometime_complete, 
		gai_req)) {

		msyslog(LOG_ERR, "unable to queue getaddrinfo request");
		errno = EFAULT;
		return -1;
	}

	return 0;
}

int
blocking_getaddrinfo(
	blocking_pipe_header *	req
	)
{
	blocking_gai_req *	gai_req;
	blocking_pipe_header *	resp;
	blocking_gai_resp *	gai_resp;
	char *			node;
	char *			service;
	struct addrinfo *	ai_res;
	struct addrinfo *	ai;
	struct addrinfo *	serialized_ai;
	size_t			canons_octets;
	size_t			this_octets;
	size_t			resp_octets;
	char *			cp;
	time_t			time_now;

	gai_req = (void *)((char *)req + sizeof(*req));
	node = (char *)gai_req + sizeof(*gai_req);
	service = node + gai_req->nodesize;

	scheduled_sleep(gai_req->scheduled, gai_req->earliest);

#ifdef HAVE_RES_INIT
	/*
	 * This is ad-hoc.  Reload /etc/resolv.conf once per minute
	 * to pick up on changes from the DHCP client.  [Bug 1226]
	 */
	time_now = time(NULL);
	if (next_res_init <= time_now) {
		if (next_res_init)
			res_init();
		next_res_init = time_now + 60;
	}
#endif

	/*
	 * Take a shot at the final size, better to overestimate
	 * at first and then realloc to a smaller size.
	 */

	resp = emalloc(sizeof(*resp) + sizeof(*gai_resp) 
			   + 16 * (sizeof(struct addrinfo)
				   + sizeof(sockaddr_u))
			   + 256);
	gai_resp = (void *)(resp + 1);

	DPRINTF(2, ("blocking_getaddrinfo given node %s serv %s fam %d flags %x\n", 
		    node, service, gai_req->hints.ai_family,
		    gai_req->hints.ai_flags));
	
	ai_res = NULL;
	gai_resp->retcode = getaddrinfo(node, service, &gai_req->hints, &ai_res);

	switch (gai_resp->retcode) {
#ifdef EAI_SYSTEM
	case EAI_SYSTEM:
		gai_resp->gai_errno = errno;
		break;
#endif
	default:
		gai_resp->gai_errno = 0;
	}

	gai_resp->ai_count = canons_octets = 0;

	if (!gai_resp->retcode) {
		ai = ai_res;
		while (NULL != ai) {
			gai_resp->ai_count++;
			if (ai->ai_canonname)
				canons_octets += strlen(ai->ai_canonname) + 1;
			ai = ai->ai_next;
		}
		/*
		 * If this query succeeded only after retrying, DNS may have
		 * just become responsive.  Ignore previously-scheduled
		 * retry sleeps once for each pending request, similar to
		 * the way scheduled_sleep() does when its worker_sleep()
		 * is interrupted.
		 */
		if (gai_req->retry > INITIAL_DNS_RETRY) {
			time_now = time(NULL);
			ignore_scheduled_before = time_now;
			next_dns_timeslot = time_now;
			DPRINTF(1, ("DNS success after retry, ignoring sleeps scheduled before now (%s)",
				humantime(time_now)));
		}
	}

	/*
	 * Our response consists of a header, followed by ai_count 
	 * addrinfo structs followed by ai_count sockaddr_storage 
	 * structs followed by the canonical names.
	 */
	gai_resp->octets = sizeof(*gai_resp)
			    + gai_resp->ai_count
				* (sizeof(gai_req->hints)
				   + sizeof(sockaddr_u))
			    + canons_octets;

	resp_octets = sizeof(*resp) + gai_resp->octets;
	resp = erealloc(resp, resp_octets);
	gai_resp = (void *)(resp + 1);

	/* cp serves as our current pointer while serializing */
	cp = (void *)(gai_resp + 1);
	canons_octets = 0;

	if (!gai_resp->retcode) {
		
		ai = ai_res;
		while (NULL != ai) {
			memcpy(cp, ai, sizeof(*ai));
			serialized_ai = (void *)cp;
			cp += sizeof(*ai);

			/* transform ai_canonname into offset */
			if (NULL != serialized_ai->ai_canonname) {
				serialized_ai->ai_canonname = (char *)canons_octets;
				canons_octets += strlen(ai->ai_canonname) + 1;
			}
			
			/* leave fixup of ai_addr pointer for receiver */

			ai = ai->ai_next;
		}

		ai = ai_res;
		while (NULL != ai) {
			NTP_INSIST(ai->ai_addrlen <= sizeof(sockaddr_u));
			memcpy(cp, ai->ai_addr, ai->ai_addrlen);
			cp += sizeof(sockaddr_u);

			ai = ai->ai_next;
		}

		ai = ai_res;
		while (NULL != ai) {
			if (NULL != ai->ai_canonname) {
				this_octets = strlen(ai->ai_canonname) + 1;
				memcpy(cp, ai->ai_canonname, this_octets);
				cp += this_octets;
			}

			ai = ai->ai_next;
		}
	}

	/*
	 * make sure our walk and earlier calc match
	 */
	NTP_INSIST((size_t)(cp - (char *)resp) == resp_octets);

	if (queue_blocking_response(resp, resp_octets, req)) {
		DPRINTF(1, ("blocking_getaddrinfo unable to queue response"));
		return -1;
	}

	return 0;
}


static void
getaddrinfo_sometime_complete(
	blocking_work_req	rtype,
	void *			context,
	size_t			respsize,
	void *			resp
	)
{
	blocking_gai_req *		gai_req;
	blocking_gai_resp *		gai_resp;
	struct addrinfo *		ai;
	struct addrinfo *		next_ai;
	sockaddr_u *			psau;
	char *				node;
	char *				service;
	char *				canon_start;
	int				again;
	int				af;
	const char *			fam_spec;
	int				i;

	gai_req = context;
	gai_resp = resp;

	NTP_REQUIRE(BLOCKING_GETADDRINFO == rtype);
	NTP_REQUIRE(respsize == gai_resp->octets);

	node = (char *)gai_req + sizeof(*gai_req);
	service = node + gai_req->nodesize;

	if (gai_resp->retcode) {
		again = should_retry_dns(gai_resp->retcode, gai_resp->gai_errno);
		/*
		 * exponential backoff of DNS retries to 64s
		 */
		if (gai_req->retry && again) {
			/* log the first retry only */
			if (INITIAL_DNS_RETRY == gai_req->retry)
				NLOG(NLOG_SYSINFO) {
					af = gai_req->hints.ai_family;
					fam_spec = (AF_INET6 == af)
						       ? " (AAAA)"
						       : (AF_INET == af)
							     ? " (A)"
							     : "";
#ifdef EAI_SYSTEM
					if (EAI_SYSTEM == gai_resp->retcode)
						msyslog(LOG_INFO,
							"retrying DNS %s%s: EAI_SYSTEM %s (%d)",
							node, fam_spec,
							strerror(gai_resp->gai_errno),
							gai_resp->gai_errno);
					else
#endif
						msyslog(LOG_INFO,
							"retrying DNS %s%s: %s (%d)",
							node, fam_spec,
							gai_strerror(gai_resp->retcode),
							gai_resp->retcode);
				}
			manage_dns_retry_interval(&gai_req->scheduled,
			    &gai_req->earliest, &gai_req->retry);
			if (!queue_blocking_request(
					BLOCKING_GETADDRINFO,
					gai_req,
					gai_req->octets,
					&getaddrinfo_sometime_complete,
					gai_req))
				return;
			else
				msyslog(LOG_ERR, "unable to retry hostname %s", node);
		}
	}

	/*
	 * fixup pointers in returned addrinfo array
	 */
	ai = (void *)((char *)gai_resp + sizeof(*gai_resp));
	next_ai = NULL;
	for (i = gai_resp->ai_count - 1; i >= 0; i--) {
		ai[i].ai_next = next_ai;
		next_ai = &ai[i];
	}

	psau = (void *)((char *)ai + gai_resp->ai_count * sizeof(*ai));
	canon_start = (char *)psau + gai_resp->ai_count * sizeof(*psau);

	for (i = 0; i < gai_resp->ai_count; i++) {
		if (NULL != ai[i].ai_addr)
			ai[i].ai_addr = &psau->sa;
		psau++;
		if (NULL != ai[i].ai_canonname)
			ai[i].ai_canonname += (size_t)canon_start;
	}

	NTP_ENSURE((char *)psau == canon_start);

	if (!gai_resp->ai_count)
		ai = NULL;
#ifdef DEBUG
	else	/* exercise copy_addrinfo_list() */
		ai = copy_addrinfo_list(ai);
#endif
	
	(*gai_req->callback)(gai_resp->retcode, gai_resp->gai_errno,
			     gai_req->context, node, service, 
			     &gai_req->hints, ai);

#ifdef DEBUG
	/* exercise copy_addrinfo_list() */
	if (NULL != ai)
		free(ai);
#endif
	free(gai_req);
	/* gai_resp is part of block freed by process_blocking_response() */
}


#ifdef TEST_BLOCKING_WORKER
void gai_test_callback(int rescode, int gai_errno, void *context, const char *name, const char *service, const struct addrinfo *hints, const struct addrinfo *ai_res)
{
	sockaddr_u addr;

	if (rescode) {
		DPRINTF(1, ("gai_test_callback context %p error rescode %d %s serv %s\n",
			    context, rescode, name, service));
		return;
	}
	while (!rescode && NULL != ai_res) {
		ZERO_SOCK(&addr);
		memcpy(&addr, ai_res->ai_addr, ai_res->ai_addrlen);
		DPRINTF(1, ("ctx %p fam %d addr %s canon '%s' type %s at %p ai_addr %p ai_next %p\n", 
			    context,
			    AF(&addr),
			    stoa(&addr), 
			    (ai_res->ai_canonname)
				? ai_res->ai_canonname
				: "",
			    (SOCK_DGRAM == ai_res->ai_socktype) 
				? "DGRAM" 
				: (SOCK_STREAM == ai_res->ai_socktype) 
					? "STREAM" 
					: "(other)",
			    ai_res,
			    ai_res->ai_addr,
			    ai_res->ai_next));

		getnameinfo_sometime((sockaddr_u *)ai_res->ai_addr, 128, 32, 0, gni_test_callback, context);

		ai_res = ai_res->ai_next;
	}
}
#endif	/* TEST_BLOCKING_WORKER */


int
getnameinfo_sometime(
	sockaddr_u *		psau,
	size_t			hostoctets,
	size_t			servoctets,
	int			flags,
	gni_sometime_callback	callback,
	void *			context
	)
{
	blocking_gni_req *	gni_req;
	time_t			time_now;
	
	NTP_REQUIRE(hostoctets);
	NTP_REQUIRE(hostoctets + servoctets < 1024);

	gni_req = emalloc(sizeof(*gni_req));
	memset(gni_req, 0, sizeof(*gni_req));

	gni_req->octets = sizeof(*gni_req);
	time_now = time(NULL);
	next_dns_timeslot = max(time_now, next_dns_timeslot);
	gni_req->scheduled = time_now;
	gni_req->earliest = next_dns_timeslot;
	memcpy(&gni_req->socku, psau, SOCKLEN(psau));
	gni_req->hostoctets = hostoctets;
	gni_req->servoctets = servoctets;
	gni_req->flags = flags;
	gni_req->retry = INITIAL_DNS_RETRY;
	gni_req->callback = callback;
	gni_req->context = context;

	if (queue_blocking_request(
		BLOCKING_GETNAMEINFO,
		gni_req,
		sizeof(*gni_req), 
		&getnameinfo_sometime_complete, 
		gni_req)) {

		msyslog(LOG_ERR, "unable to queue getnameinfo request");
		errno = EFAULT;
		return -1;
	}

	return 0;
}


int
blocking_getnameinfo(
	blocking_pipe_header *	req
	)
{
	blocking_gni_req *	gni_req;
	blocking_pipe_header *	resp;
	blocking_gni_resp *	gni_resp;
	size_t			octets;
	size_t			resp_octets;
	char *			host;
	char *			service;
	char *			cp;
	int			rc;
	time_t			time_now;

	gni_req = (void *)((char *)req + sizeof(*req));

	octets = gni_req->hostoctets + gni_req->servoctets;

	/*
	 * Some alloca() implementations are fragile regarding
	 * large allocations.  We only need room for the host
	 * and service names.
	 */
	NTP_REQUIRE(octets < 1024);

#ifndef HAVE_ALLOCA
	host = emalloc(octets);
#else
	host = alloca(octets);
	if (NULL == host) {
		msyslog(LOG_ERR,
			"blocking_getnameinfo unable to allocate %d octets on stack",
			octets);
		exit(1);
	}
#endif
	service = host + gni_req->hostoctets;

	scheduled_sleep(gni_req->scheduled, gni_req->earliest);

#ifdef HAVE_RES_INIT
	/*
	 * This is ad-hoc.  Reload /etc/resolv.conf once per minute
	 * to pick up on changes from the DHCP client.  [Bug 1226]
	 */
	time_now = time(NULL);
	if (next_res_init <= time_now) {
		if (next_res_init)
			res_init();
		next_res_init = time_now + 60;
	}
#endif

	/*
	 * Take a shot at the final size, better to overestimate
	 * then realloc to a smaller size.
	 */

	resp_octets = sizeof(*resp) + sizeof(*gni_resp) + octets;
	resp = emalloc(resp_octets);
	gni_resp = (void *)((char *)resp + sizeof(*resp));

	DPRINTF(2, ("blocking_getnameinfo given addr %s flags 0x%x hostlen %d servlen %d\n",
		    stoa(&gni_req->socku), gni_req->flags,
		    gni_req->hostoctets, gni_req->servoctets));
	
	gni_resp->retcode = getnameinfo(&gni_req->socku.sa,
					SOCKLEN(&gni_req->socku),
					host,
					gni_req->hostoctets,
					service,
					gni_req->servoctets,
					gni_req->flags);

	switch (gni_resp->retcode) {
#ifdef EAI_SYSTEM
	case EAI_SYSTEM:
		gni_resp->gni_errno = errno;
		break;
#endif
	default:
		gni_resp->gni_errno = 0;
	}

	if (gni_resp->retcode) {
		gni_resp->hostoctets = 0;
		gni_resp->servoctets = 0;
	} else {
		gni_resp->hostoctets = strlen(host) + 1;
		gni_resp->servoctets = strlen(service) + 1;
		/*
		 * If this query succeeded only after retrying, DNS may have
		 * just become responsive.  Ignore previously-scheduled
		 * retry sleeps once for each pending request, similar to
		 * the way scheduled_sleep() does when its worker_sleep()
		 * is interrupted.
		 */
		if (gni_req->retry > INITIAL_DNS_RETRY) {
			time_now = time(NULL);
			ignore_scheduled_before = time_now;
			next_dns_timeslot = time_now;
			DPRINTF(1, ("DNS success after retrying, ignoring sleeps scheduled before now (%s)",
				humantime(time_now)));
		}
	}
	octets = gni_resp->hostoctets + gni_resp->servoctets;
	/*
	 * Our response consists of a header, followed by the host and
	 * service strings, each null-terminated.
	 */
	resp_octets = sizeof(*resp) + sizeof(*gni_resp) + octets;

	resp = erealloc(resp, resp_octets);
	gni_resp = (void *)(resp + 1);

	gni_resp->octets = sizeof(*gni_resp) + octets;

	/* cp serves as our current pointer while serializing */
	cp = (void *)(gni_resp + 1);

	if (!gni_resp->retcode) {
		memcpy(cp, host, gni_resp->hostoctets);
		cp += gni_resp->hostoctets;
		memcpy(cp, service, gni_resp->servoctets);
		cp += gni_resp->servoctets;
	}

	NTP_INSIST((size_t)(cp - (char *)resp) == resp_octets);
	NTP_INSIST(resp_octets - sizeof(*resp) == gni_resp->octets);

	rc = queue_blocking_response(resp, resp_octets, req);
	if (rc)
		msyslog(LOG_ERR, "blocking_getnameinfo unable to queue response");
#ifndef HAVE_ALLOCA
	free(host);
#endif
	return rc;
}


static void
getnameinfo_sometime_complete(
	blocking_work_req	rtype,
	void *			context,
	size_t			respsize,
	void *			resp
	)
{
	blocking_gni_req *	gni_req;
	blocking_gni_resp *	gni_resp;
	char *			host;
	char *			service;
	int			again;

	gni_req = context;
	gni_resp = resp;

	NTP_REQUIRE(BLOCKING_GETNAMEINFO == rtype);
	NTP_REQUIRE(respsize == gni_resp->octets);

	if (gni_resp->retcode) {
		again = should_retry_dns(gni_resp->retcode, gni_resp->gni_errno);
		/*
		 * exponential backoff of DNS retries to 64s
		 */
		if (gni_req->retry)
			manage_dns_retry_interval(&gni_req->scheduled,
			    &gni_req->earliest, &gni_req->retry);

		if (gni_req->retry && again) {
			if (!queue_blocking_request(
				BLOCKING_GETNAMEINFO,
				gni_req,
				gni_req->octets, 
				&getnameinfo_sometime_complete, 
				gni_req))
				return;

			msyslog(LOG_ERR, "unable to retry reverse lookup of %s", stoa(&gni_req->socku));
		}
	}

	if (!gni_resp->hostoctets) {
		host = NULL;
		service = NULL;
	} else {
		host = (char *)gni_resp + sizeof(*gni_resp);
		service = (gni_resp->servoctets) 
			      ? host + gni_resp->hostoctets
			      : NULL;
	}

	(*gni_req->callback)(gni_resp->retcode, gni_resp->gni_errno,
			     &gni_req->socku, gni_req->flags, host,
			     service, gni_req->context);

	free(gni_req);
	/* gni_resp is part of block freed by process_blocking_response() */
}


#ifdef TEST_BLOCKING_WORKER
void gni_test_callback(int rescode, int gni_errno, sockaddr_u *psau, int flags, const char *host, const char *service, void *context)
{
	if (!rescode)
		DPRINTF(1, ("gni_test_callback got host '%s' serv '%s' for addr %s context %p\n", 
			    host, service, stoa(psau), context));
	else
		DPRINTF(1, ("gni_test_callback context %p rescode %d gni_errno %d flags 0x%x addr %s\n",
			    context, rescode, gni_errno, flags, stoa(psau)));
}
#endif	/* TEST_BLOCKING_WORKER */


static void
scheduled_sleep(
	time_t scheduled,
	time_t earliest
	)
{
	time_t now;

	if (scheduled < ignore_scheduled_before) {
		DPRINTF(1, ("ignoring sleep until %s scheduled at %s (before %s)\n",
			humantime(earliest), humantime(scheduled),
			humantime(ignore_scheduled_before)));
		return;
	}

	now = time(NULL);

	if (now < earliest) {
		DPRINTF(1, ("sleep until %s scheduled at %s (>= %s)\n",
			humantime(earliest), humantime(scheduled),
			humantime(ignore_scheduled_before)));
		if (-1 == worker_sleep(earliest - now)) {
			/* our sleep was interrupted */
			now = time(NULL);
			ignore_scheduled_before = now;
			next_dns_timeslot = now;
#ifdef HAVE_RES_INIT
			next_res_init = now + 60;
			res_init();
#endif
			DPRINTF(1, ("sleep interrupted by daemon, ignoring sleeps scheduled before now (%s)\n",
				humantime(ignore_scheduled_before)));
		}
	}
}


/*
 * manage_dns_retry_interval is a helper used by
 * getaddrinfo_sometime_complete and getnameinfo_sometime_complete
 * to calculate the new retry interval and schedule the next query.
 */
static void
manage_dns_retry_interval(
	time_t *	pscheduled,
	time_t *	pwhen,
	int *		pretry
	)
{
	time_t	now;
	time_t	when;
	int	retry;
		
	now = time(NULL);
	retry = *pretry;
	when = max(now + retry, next_dns_timeslot);
	next_dns_timeslot = when;
	retry = min(64, retry << 1);

	*pscheduled = now;
	*pwhen = when;
	*pretry = retry;
}

/*
 * should_retry_dns is a helper used by getaddrinfo_sometime_complete
 * and getnameinfo_sometime_complete which implements ntpd's DNS retry
 * policy.
 */
static int
should_retry_dns(
	int	rescode,
	int	res_errno
	)
{
	static int	eai_again_seen;
	int		again;

	/*
	 * If the resolver failed, see if the failure is
	 * temporary. If so, return success.
	 */
	again = 0;

	switch (rescode) {

	case EAI_FAIL:
		again = 1;
		break;

	case EAI_AGAIN:
		again = 1;
		eai_again_seen = 1;		/* [Bug 1178] */
		break;

	case EAI_NONAME:
#if defined(EAI_NODATA) && (EAI_NODATA != EAI_NONAME)
	case EAI_NODATA:
#endif
		again = !eai_again_seen;	/* [Bug 1178] */
		break;

#ifdef EAI_SYSTEM
	case EAI_SYSTEM:
		/* 
		 * EAI_SYSTEM means the real error is in errno.  We should be more
		 * discriminating about which errno values require retrying, but
		 * this matches existing behavior.
		 */
		again = 1;
		DPRINTF(1, ("intres: EAI_SYSTEM errno %d (%s) means try again, right?\n",
			    res_errno, strerror(res_errno)));
		break;
#endif
	}

	DPRINTF(2, ("intres: resolver returned: %s (%d), %sretrying\n",
		gai_strerror(rescode), rescode, again ? "" : "not "));

	return again;
}

#else	/* !WORKER follows */
int ntp_intres_nonempty_compilation_unit;
#endif
