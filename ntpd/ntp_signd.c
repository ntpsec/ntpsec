/* Copyright Red Hat, Inc.
   Copyright Andrew Tridgell.
   Copyright the NTPsec Contributors
   Licensed under the same terms as NTP itself.
 */
#include "config.h"
#include "ntp.h"
#include <sys/socket.h>

#ifdef ENABLE_MSSNTP

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"
#include "timespecops.h"

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/un.h>

/* socket routines by tridge - from junkcode.samba.org */
/*
 * Dependency on NTP packet structure removed by ESR.
 * This code now only knows about the length of an NTP packet header,
 * not its content. Note that the signing technique never handled anything
 * but unextended and MACless packet headers, so it can't be used with NTS.
 */

struct mssntp_counters mssntp_cnt, old_mssntp_cnt;

#define ERR_BUF_LEN 96 // arbitary length for error buffer (6x16) -- JamesB192

static struct do_we_log do_we_log_signd = {
	.c_decay=2,	// What is the period length in hours
	.c_limit=4,	// How many messages/period
	.score=0,	// Zero the score to begin
	.last=0,	// Zero the last to begin
};

static uint32_t signd_count = 1;

/*
  connect to a unix domain socket
*/
static int
ux_socket_connect(const char *name)
{
	struct timeval tv = { .tv_sec=2, .tv_usec=400000};
	int fd;
	struct sockaddr_un addr;
	if (!name) {
		return -1;
	}

	ZERO(addr);
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, name, sizeof(addr.sun_path));

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		maybe_log(&do_we_log_signd, LOG_ERR,
			"SIGND: can not create socket: %s",
			errbuf);
		return -1;
	}

	// FIXME: Chutzpah code ... always works so well.
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv, sizeof(tv));

	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		maybe_log(&do_we_log_signd, LOG_ERR,
			"SIGND: can not connect socket '%s': %s",
			name, errbuf);
		close(fd);
		return -1;
	}

	return fd;
}


/*
  keep writing until its all sent
*/
static size_t
write_all(int fd, const void *buf, size_t len)
{
	size_t total = 0;
	while (len) {
		ssize_t n = write(fd, buf, len);
		if (n <= 0) return total;
		buf = n + (const char *)buf;
		len -= (unsigned int)n;
		total += (unsigned int)n;
	}
	return total;
}

/*
  keep reading until its all read
*/
static size_t
read_all(int fd, void *buf, size_t len)
{
	size_t total = 0;
	while (len) {
		ssize_t n = read(fd, buf, len);
		if (n <= 0) return total;
		buf = n + (char *)buf;
		len -= (unsigned int)n;
		total += (unsigned int)n;
	}
	return total;
}

/*
  send a packet in length prefix format
*/
static int
send_packet(int fd, const char *buf, uint32_t len)
{
	uint32_t net_len = htonl(len);
	if (write_all(fd, &net_len, sizeof(net_len)) != sizeof(net_len)) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: can not send length: %s",
			errbuf);
		return -1;
	}
	if (write_all(fd, buf, len) != (size_t)len) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: can not send packet to samba: %s", errbuf);
		return -1;
	}
	return 0;
}

/*
  receive a packet in length prefix format
*/
static int
recv_packet(int fd, char **buf, uint32_t *len)
{
	if (read_all(fd, len, sizeof(*len)) != sizeof(*len)) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: can not receive length: %s",
			errbuf);
		return -1;
	}
	// There are constraints on length.
	// anything not in 64 <= len <= 1512 is probably garbage
	// 12 is metadata len before the signed packet
	// 48 is packet len, and 4 the keyID len
	// anything > 1512 will unacceptably fragment on ethernet 
	// FIXME: Word is the one true length is 80 (12+48+4+16)
	//        and all others are as an abominataton unto MS-SNTP
	*len = ntohl(*len);
	if (1512 < *len) {
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: rejecting too long packet: got %u, max is 1512",
			*len);
		return -1;
	}
	if (12 > *len) {
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: rejecting too short packet: got %u, min is 12",
			*len);
		return -1;
	}
	(*buf) = emalloc(*len);
	if (read_all(fd, *buf, *len) != (size_t)*len) {
		char errbuf[ERR_BUF_LEN];
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		free(*buf);
		maybe_log(&do_we_log_signd, LOG_DEBUG,
			"SIGND: can not receive signed packet from samba: %s",
			errbuf);
		return -1;
	}
	return 0;
}

void
send_via_ntp_signd(
	struct recvbuf *rbufp,	/* receive packet pointer */
	void *xpkt
	)
{
	/* We are here because it was detected that the client
	 * sent an all-zero signature, and we therefore know
	 * it's windows trying to talk to an AD server
	 *
	 * Because we don't want to dive into Samba's secrets
	 * database just to find the long-term kerberos key
	 * that is re-used as the NTP key, we instead hand the
	 * packet over to Samba to sign, and return to us.
	 *
	 * The signing method Samba will use is described by
	 * Microsoft in MS-SNTP, previously found here:
	 * http://msdn.microsoft.com/en-us/library/cc212930.aspx
	 *
	 * which now seems to be located at
	 * https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-sntp/8106cb73-ab3a-4542-8bc8-784dd32031cc
	 */

	struct timespec start, finish;
	l_fp wall;

	bool quit = false;
	int fd;
	size_t sendlen;
	struct samba_key_in {
		uint32_t version;
		uint32_t op;
		uint32_t packet_id;
		uint32_t key_id_le;
		char pkt[LEN_PKT_NOMAC];
	} samba_pkt;

	struct samba_key_out {
		uint32_t version;
		uint32_t op;
		uint32_t packet_id;
	} samba_reply;

	uint32_t header_length = sizeof(samba_reply);
	char full_socket[256];

	char *reply = NULL;
	uint32_t reply_len;

	mssntp_cnt.serves++;
	clock_gettime(CLOCK_MONOTONIC, &start);

	ZERO(samba_pkt);
	samba_pkt.op = 0; /* Sign message */
	/* This will be echoed into the reply - a different
	 * implementation might want multiple packets
	 * awaiting signing */

	samba_pkt.packet_id = signd_count++;

	/* Swap the byte order back - it's actually little
	 * endian on the wire, but it was read above as
	 * network byte order */
	samba_pkt.key_id_le = htonl(rbufp->keyid);
	memcpy(&samba_pkt.pkt, xpkt, sizeof(samba_pkt.pkt));

	snprintf(full_socket, sizeof(full_socket), "%s/socket", ntp_signd_socket);

	fd = ux_socket_connect(full_socket);
	/* Only continue with this if we can talk to Samba */
	if (fd < 4) {
		mssntp_cnt.serves_no++;
		goto signd_cleanup;
	}
	
	/* Send old packet to Samba, expect response */
	/* Packet to Samba is quite simple:
	   All values BIG endian except key ID as noted
	   [packet size as BE] - 4 bytes
	   [protocol version (0)] - 4 bytes
	   [packet ID] - 4 bytes
	   [operation (sign message=0)] - 4 bytes
	   [key id] - LITTLE endian (as on wire) - 4 bytes
	   [message to sign] - as marshalled, without signature
	*/

	if (send_packet(fd, (char *)&samba_pkt, offsetof(struct samba_key_in, pkt) + LEN_PKT_NOMAC) != 0) {
		/* Huh?  could not talk to Samba... */
		mssntp_cnt.serves_err++;
		goto signd_cleanup;
	}

	if (recv_packet(fd, &reply, &reply_len) != 0) {
		mssntp_cnt.serves_err++;
		goto signd_cleanup;
	}
	/* Return packet is also simple:
	   [packet size] - network byte order - 4 bytes
	   [protocol version (0)] network byte order - - 4 bytes
	   [operation (signed success=3, failure=4)] network byte order - - 4 byte
	   (optional) [signed message] - as provided before, with signature appended
	*/

	{
		uint32_t op_reply = 0;
		memcpy(&samba_reply, reply, header_length > reply_len ? reply_len: header_length);
		op_reply = ntohl(samba_reply.op);
		if (reply_len < header_length) {
			maybe_log(&do_we_log_signd, LOG_INFO,
				  "SIGND: bad samba reply length %u <= %u reply header length.\n",
				  reply_len, header_length);
			quit = true;
		}
		if (3 != op_reply) {
			maybe_log(&do_we_log_signd, LOG_INFO,
				   "SIGND: bad Samba repy op want 3, got %u.\n",
				   op_reply);
			quit = true;
		}
		if (quit) {
			mssntp_cnt.serves_bad++;
			clock_gettime(CLOCK_MONOTONIC, &finish);
			wall = tspec_intv_to_lfp(sub_tspec(finish, start));
			mssntp_cnt.serves_bad_wall += wall;
			if (wall > mssntp_cnt.serves_bad_slowest)
			  mssntp_cnt.serves_bad_slowest = wall;
			goto signd_cleanup;
		}
	}

	sendlen = reply_len - header_length;
	xpkt = reply + header_length;
	sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, xpkt, (uint32_t)sendlen);

	mssntp_cnt.serves_good++;
	clock_gettime(CLOCK_MONOTONIC, &finish);
	wall = tspec_intv_to_lfp(sub_tspec(finish, start));
	mssntp_cnt.serves_good_wall += wall;
	if (wall > mssntp_cnt.serves_good_slowest)
	  mssntp_cnt.serves_good_slowest = wall;

	DPRINT(1, ("transmit ntp_signd packet: at %u %s->%s keyid %08x len %zu\n",
		   current_time, socktoa(&rbufp->dstadr->sin),
		   socktoa(&rbufp->recv_srcadr), rbufp->keyid, sendlen));

signd_cleanup:
	if (reply) {
		free(reply);
		reply = NULL;
	}
	close(fd);
}
#endif
