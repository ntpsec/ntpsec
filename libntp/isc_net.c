/*
 * Copyright (C) 2004, 2005, 2007, 2008, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#if defined(HAVE_SYS_SYSCTL_H)
#include <sys/sysctl.h>
#endif

/* hack to ignore GCC Unused Result */
#define IGNORE(r) do{if(r){}}while(0)

#include <errno.h>
#include <unistd.h>
#include <string.h>

/* for struct sockaddr on *BSD */
#include <sys/socket.h>                /* Contractual promise. */
#include <netinet/in.h>                /* Contractual promise. */

#include "isc_netaddr.h"
#include "isc_error.h"
#include "isc_result.h"

static isc_result_t	ipv4_result = ISC_R_NOTFOUND;
static isc_result_t	ipv6_result = ISC_R_NOTFOUND;
static isc_result_t	ipv6only_result = ISC_R_NOTFOUND;

static isc_result_t
try_proto(int domain) {
	int s;
	isc_result_t result = ISC_R_SUCCESS;
	char strbuf[BUFSIZ];

	s = socket(domain, SOCK_STREAM, 0);
	if (s == -1) {
		switch (errno) {
#ifdef EAFNOSUPPORT
		case EAFNOSUPPORT:
#endif
#ifdef EPROTONOSUPPORT
		case EPROTONOSUPPORT:
#endif
#ifdef EINVAL
		case EINVAL:
#endif
			return (ISC_R_NOTFOUND);
		default:
			IGNORE(strerror_r(errno, strbuf, sizeof(strbuf)));
			UNEXPECTED_ERROR("socket() failed: %s", strbuf);
			return (ISC_R_UNEXPECTED);
		}
	}

	if (domain == PF_INET6) {
		struct sockaddr_in6 sin6;
		socklen_t len;

		/*
		 * Check to see if IPv6 is broken, as is common on Linux.
		 */
		len = sizeof(sin6);
		if (getsockname(s, (struct sockaddr *)&sin6, &len) < 0)
		{
/*			isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
				      ISC_LOGMODULE_SOCKET, ISC_LOG_ERROR,
				      "retrieving the address of an IPv6 "
				      "socket from the kernel failed.");
			isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
				      ISC_LOGMODULE_SOCKET, ISC_LOG_ERROR,
				      "IPv6 is not supported.");
*/
			result = ISC_R_NOTFOUND;
		} else {
			if (len == sizeof(struct sockaddr_in6))
				result = ISC_R_SUCCESS;
			else {
/*				isc_log_write(isc_lctx,
					      ISC_LOGCATEGORY_GENERAL,
					      ISC_LOGMODULE_SOCKET,
					      ISC_LOG_ERROR,
					      "IPv6 structures in kernel and "
					      "user space do not match.");
				isc_log_write(isc_lctx,
					      ISC_LOGCATEGORY_GENERAL,
					      ISC_LOGMODULE_SOCKET,
					      ISC_LOG_ERROR,
					      "IPv6 is not supported.");
*/
				result = ISC_R_NOTFOUND;
			}
		}
	}

	(void)close(s);

	return (result);
}

static void
initialize(void) {
	static bool once = false;

	if (once) return;
	once = true;
	ipv4_result = try_proto(PF_INET);
	ipv6_result = try_proto(PF_INET6);
}

bool
isc_net_probeipv4_bool(void) {
    initialize();
    return (ISC_R_SUCCESS == ipv4_result);
}

bool
isc_net_probeipv6_bool(void) {
  return (ISC_R_SUCCESS == isc_net_probeipv6());
}

isc_result_t
isc_net_probeipv6(void) {
	initialize();
	return (ipv6_result);
}

static void
initialize_ipv6only(void) {
        static bool once_ipv6only = false;
#ifdef IPV6_V6ONLY
	int s, on;
	char strbuf[BUFSIZ];
#endif

        if (once_ipv6only) return;
        once_ipv6only = true;
	isc_result_t result;

	result = isc_net_probeipv6();
	if (result != ISC_R_SUCCESS) {
		ipv6only_result = result;
		return;
	}

#ifndef IPV6_V6ONLY
	ipv6only_result = ISC_R_NOTFOUND;
	return;
#else
	/* check for TCP sockets */
	s = socket(PF_INET6, SOCK_STREAM, 0);
	if (s == -1) {
		IGNORE(strerror_r(errno, strbuf, sizeof(strbuf)));
		UNEXPECTED_ERROR("socket() failed: %s", strbuf);
		ipv6only_result = ISC_R_UNEXPECTED;
		return;
	}

	on = 1;
	if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
		ipv6only_result = ISC_R_NOTFOUND;
		goto close;
	}

	close(s);

	/* check for UDP sockets */
	s = socket(PF_INET6, SOCK_DGRAM, 0);
	if (s == -1) {
	    IGNORE(strerror_r(errno, strbuf, sizeof(strbuf)));
		UNEXPECTED_ERROR("socket() failed: %s", strbuf);
		ipv6only_result = ISC_R_UNEXPECTED;
		return;
	}

	on = 1;
	if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
		ipv6only_result = ISC_R_NOTFOUND;
		goto close;
	}

	ipv6only_result = ISC_R_SUCCESS;

close:
	close(s);
	return;
#endif /* IPV6_V6ONLY */
}

bool
isc_net_probe_ipv6only_bool(void) {
    initialize_ipv6only();
    return (ISC_R_SUCCESS == ipv6only_result);
}
