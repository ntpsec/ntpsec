/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"

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

  This module also handles the start of NTS-KE.

  Only one DNS/NTS lookup active at a time

  peer->srcadr holds IPv4/IPv6/UNSPEC flag
  peer->hmode holds DNS retry time (log 2)

  Older versions of this code turned off FLAG_DNS
  so there wasn't any way to do another DNS lookup.
  Now, we turn off FLAG_DNSNTS to indicate success.

  Pool case makes new peer slots.
*/

static struct peer* active = NULL;  /* busy flag */
static pthread_t worker;
static int gai_rc;
static struct addrinfo *answer;

static void* dns_lookup(void* arg);

/* Initially, this was only used for DNS where pp=>hostname was valid.
 * With NTS, it also gets used for numerical IP Addresses.
 */
bool dns_probe(struct peer* pp)
{
	int rc;
        sigset_t        block_mask, saved_sig_mask;
	const char	* busy = "";
	const char	*hostname = pp->hostname;

	/* Comment out the next two lines to get (much) more
	 * printout when we are busy.
	 */
        if (NULL != active)
		return false;

	if (NULL != active) {
		busy = ", busy";
	}
	if (NULL == hostname) {
		hostname = socktoa(&pp->srcadr);
	}

	msyslog(LOG_INFO, "DNS: dns_probe: %s, cast_flags:%x, flags:%x%s",
		hostname, pp->cast_flags, pp->cfg.flags, busy);
        if (NULL != active)	/* normally redundant */
		return false;

	active = pp;

        sigfillset(&block_mask);
        pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
	rc = pthread_create(&worker, NULL, dns_lookup, pp);
        if (rc) {
	  msyslog(LOG_ERR, "DNS: dns_probe: error from pthread_create: %s, %s",
	      hostname, strerror(rc));
          pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);
	  return true;  /* don't try again */
	}
        pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

	return true;
}

void dns_check(void)
{
	int rc;
	struct addrinfo *ai;
	const char      *hostname = active->hostname;
	DNS_Status status;

	if (NULL == hostname) {
		hostname = socktoa(&active->srcadr);
	}
	msyslog(LOG_INFO, "DNS: dns_check: processing %s, %x, %x",
		hostname, active->cast_flags, (unsigned int)active->cfg.flags);

	rc = pthread_join(worker, NULL);
	if (0 != rc) {
		msyslog(LOG_ERR, "DNS: dns_check: join failed %s", strerror(rc));
		return;  /* leaves active set */
	}

#ifndef DISABLE_NTS
	if (active->cfg.flags & FLAG_NTS) {
		nts_check(active);
		active = NULL;
		return;
	}
#endif

	if (0 != gai_rc) {
		msyslog(LOG_INFO, "DNS: dns_check: DNS error: %d, %s",
			gai_rc, gai_strerror(gai_rc));
		answer = NULL;
	}

	for (ai = answer; NULL != ai; ai = ai->ai_next) {
		sockaddr_u sockaddr;
		if (sizeof(sockaddr_u) < ai->ai_addrlen)
			continue;  /* Weird */
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

	if (NULL != answer) {
		freeaddrinfo(answer);
	}
	active = NULL;
}

/* Beware: no calls to msyslog from here.
 * It's not thread safe.
 * This is the only other thread in ntpd.
 */
static void* dns_lookup(void* arg)
{
	struct peer *pp = (struct peer *) arg;
	struct addrinfo hints;

#ifdef HAVE_SECCOMP_H
        setup_SIGSYS_trap();      /* enable trap for this thread */
#endif

#ifdef HAVE_RES_INIT
	/* Reload DNS servers from /etc/resolv.conf in case DHCP has updated it.
	 * We only need to do this occasionally, but it's not expensive
	 * and simpler to do it every time than it is to figure out when
	 * to do it.
	 * This res_init() covers NTS too.
	 */
	res_init();
#endif

	if (pp->cfg.flags & FLAG_NTS) {
#ifndef DISABLE_NTS
		nts_probe(pp);
#endif
	} else {
		ZERO(hints);
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_family = AF(&pp->srcadr);
		gai_rc = getaddrinfo(pp->hostname, NTP_PORTA, &hints, &answer);
	}

	kill(getpid(), SIGDNS);
	pthread_exit(NULL);

	/* Prevent compiler warning.
	 * More portable than an attribute or directive
	 */
	return (void *)NULL;
}

