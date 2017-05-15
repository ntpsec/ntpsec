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
        pthread_attr_t  thr_attr;
        sigset_t        block_mask, saved_sig_mask;

	msyslog(LOG_INFO, "dns_probe: %s, 0x%x", pp->hostname, pp->cast_flags);
        if (NULL != active)
		return false;
	active = pp;

        pthread_attr_init(&thr_attr);
	/* FIXME-DNS: stack size? */
        sigfillset(&block_mask);
        pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
	rc = pthread_create(&worker, &thr_attr, dns_lookup, pp);
        if (rc) {
		msyslog(LOG_ERR, "dns_probe: error from pthread_create: %s, %m", pp->hostname);
		return true;  /* don't try again */
	}
        pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);
        pthread_attr_destroy(&thr_attr);

	return true;
};

void dns_check(void)
{
	int rc;
	struct addrinfo *ai;
	sockaddr_u sockaddr;

	msyslog(LOG_INFO, "dns_check: lookup for %s", active->hostname);
	rc = pthread_join(worker, NULL);
	if (0 != rc) {
		msyslog(LOG_ERR, "dns_check: join failed %m");
		return;  /* leaves active set */
	}
	if (0 != gai_rc) {
		msyslog(LOG_INFO, "dns_check: DNS error %s",
			gai_strerror(gai_rc));
		/* FIXME-DNS callback with null? */
		active = NULL;
		return;
	}

	for (ai = answer; NULL != ai; ai = ai->ai_next) {
		sockaddr.sa = *ai->ai_addr;
		msyslog(LOG_INFO, "dns_check: found %s=%s",
			socktoa(&sockaddr), ai->ai_canonname);
	}

	msyslog(LOG_INFO, "dns_check: processing %s, %x",
        	active->hostname, active->cast_flags);

	if (active->cast_flags & MDF_POOL)
		pool_take_dns(active, answer);
	else
		server_take_dns(active, answer);

	active = NULL;
};

static void* dns_lookup(void* arg)
{
	struct peer *pp = (struct peer *) arg;
	struct addrinfo hints;

	// FIXME-DNS need occasional res_init()
	ZERO(hints);
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF(&pp->srcadr);
	// FIXME-DNS
	// hints.ai_family = my_node->addr->type;
	gai_rc = getaddrinfo(pp->hostname, "ntp", &hints, &answer);

	kill(getpid(), SIGDNS);
	pthread_exit(NULL);
};



#endif /* ENABLE_DNS_LOOKUP */

