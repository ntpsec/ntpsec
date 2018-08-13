/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include "config.h"

#ifdef ENABLE_DNS_LOOKUP

#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_RES_INIT
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#include "ntpd.h"
#include "ntp_dns.h"


/* Notes:

  Only one DNS lookup active at a time

  peer->srcadr holds IPv4/IPv6/UNSPEC flag
  peer->hmode holds DNS retry time (log 2)
  FLAG_DNS used in server case to indicate need DNS

  Server can't lookup again after finding an answer
    answer uses same peer slot, turns off FLAG_DNS
    srcadr+hmode changed by normal code
    server can't lookup again if answer stops responding
  Pool case makes new peer slots, pool slot unchanged
    so OK for another lookup
*/

static struct peer* active = NULL;  /* busy flag */
static pthread_t worker;
static int gai_rc;
static struct addrinfo *answer;

static void* dns_lookup(void* arg);


bool dns_probe(struct peer* pp)
{
	int rc;
        sigset_t        block_mask, saved_sig_mask;
	const char	* busy = "";

	/* Comment out the next two lines to get (much) more
	 * printout when we are busy.
	 */
        if (NULL != active)
		return false;

	if (NULL != active)
		busy = ", busy";
	msyslog(LOG_INFO, "DNS: dns_probe: %s, cast_flags:%x, flags:%x%s",
		pp->hostname, pp->cast_flags, pp->cfg.flags, busy);
        if (NULL != active)	/* normally redundant */
		return false;

	active = pp;

        sigfillset(&block_mask);
        pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
	rc = pthread_create(&worker, NULL, dns_lookup, pp);
        if (rc) {
		errno = rc;
		msyslog(LOG_ERR, "DNS: dns_probe: error from pthread_create: %s, %m", pp->hostname);
		return true;  /* don't try again */
	}
        pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

	return true;
};

void dns_check(void)
{
	int rc;
	struct addrinfo *ai;
	DNS_Status status;

	msyslog(LOG_INFO, "DNS: dns_check: processing %s, %x, %x",
		active->hostname, active->cast_flags, (unsigned int)active->cfg.flags);

	rc = pthread_join(worker, NULL);
	if (0 != rc) {
		msyslog(LOG_ERR, "DNS: dns_check: join failed %m");
		return;  /* leaves active set */
	}
	if (0 != gai_rc) {
		msyslog(LOG_INFO, "DNS: dns_check: DNS error: %d, %s",
			gai_rc, gai_strerror(gai_rc));
		answer = NULL;
	}

	for (ai = answer; NULL != ai; ai = ai->ai_next) {
		sockaddr_u sockaddr;
		memcpy(&sockaddr, ai->ai_addr, ai->ai_addrlen);
		/* Both dns_take_pool and dns_take_server log something. */
		// msyslog(LOG_INFO, "DNS: Take %s=>%s",
		//		socktoa(ai->ai_addr), socktoa(&sockaddr));
		if (active->cast_flags & MDF_POOL)
			dns_take_pool(active, &sockaddr);
		else
			dns_take_server(active, &sockaddr);
	}

	switch (gai_rc) {
		case 0:
			status = DNS_good;
			break;

		case EAI_AGAIN:
			status = DNS_temp;
			break;

		/* Treat all other errors as permanent.
		 * Some values from man page weren't in headers.
		 */
		default:
			status = DNS_error;
	}

	dns_take_status(active, status);

	if (NULL != answer)
		freeaddrinfo(answer);
	active = NULL;
};

/* Beware: no calls to msyslog from here.
 * It's not thread safe.
 * This is the only other thread in ntpd.
 */
static void* dns_lookup(void* arg)
{
	struct peer *pp = (struct peer *) arg;
	struct addrinfo hints;

#ifdef HAVE_RES_INIT
	/* Reload DNS servers from /etc/resolv.conf in case DHCP has updated it.
	 * We only need to do this occasionally, but it's not expensive
	 * and simpler to do it every time than it is to figure out when
	 * to do it.
	 */
	res_init();
#endif

	ZERO(hints);
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF(&pp->srcadr);
	gai_rc = getaddrinfo(pp->hostname, "ntp", &hints, &answer);

	kill(getpid(), SIGDNS);
	pthread_exit(NULL);

	/* Prevent compiler warning.
	 * More portable than an attribute or directive
	 */
	return (void *)NULL;
};



#endif /* ENABLE_DNS_LOOKUP */

