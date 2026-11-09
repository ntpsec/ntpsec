/*
 * setup.c - stubs and glue shared by the OSS-Fuzz harnesses in this directory
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntpd.h"
#include "ntp_dns.h"

const char *progname = "ntpsecfuzz";

/* Hack to keep linker happy */
uint16_t extra_port = 0;
struct peer *peer_list = NULL;

void dns_take_server(struct peer *a, sockaddr_u *b);
void dns_take_status(struct peer *a, DNS_Status b);
void dns_take_pool(struct peer *a, sockaddr_u *b);

void dns_take_server(struct peer *a, sockaddr_u *b) {
	UNUSED_ARG(a);
	UNUSED_ARG(b);
	return;
}

void dns_take_status(struct peer *a, DNS_Status b) {
	UNUSED_ARG(a);
	UNUSED_ARG(b);
	return;
}

void dns_take_pool(struct peer *a, sockaddr_u *b) {
	UNUSED_ARG(a);
	UNUSED_ARG(b);
	return;
}

/* Stub the only daemon symbol the NTS parsers reference, to avoid pulling
 * ntp_util/ntp_filegen/ntp_control out of libntpd_lib.a */
void record_ntske_log(NTSKE_Status tag, const char *from, const char *msg,
		      double wall, double usr, double sys, int aead,
		      const char *errbuf)
{
	UNUSED_ARG(tag); UNUSED_ARG(from); UNUSED_ARG(msg);
	UNUSED_ARG(wall); UNUSED_ARG(usr); UNUSED_ARG(sys);
	UNUSED_ARG(aead); UNUSED_ARG(errbuf);
	return;
}

/* Keep fuzzed responses from triggering real DNS lookups inside
 * nts_client.c (see OSS-Fuzz issue 471591901).  The FuzzClient link
 * passes -Wl,--wrap=nts_server_lookup so calls to nts_server_lookup()
 * land here.  It is named __wrap_* because nts_client.o does get pulled
 * out of libntpd_lib.a, so a same-named stub would collide with it. */
bool __wrap_nts_server_lookup(char *server, sockaddr_u *addr, int af);

bool __wrap_nts_server_lookup(char *server, sockaddr_u *addr, int af)
{
	UNUSED_ARG(server); UNUSED_ARG(addr); UNUSED_ARG(af);
	return false;
}
