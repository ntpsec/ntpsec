/* fake-ntp-server.c
 * fake NTP server to exersize fake-samba
 * it won't work with real samba since we don't know any key-IDs
 * so all it is good for is debugging fake-samba
 */
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "fake.h"

static char *name = FAKE_SAMBA_SOCKET;

int main(int argc, char *argv[]) {
	struct timespec start, stop;
	double elapsed;
	int server;
	struct sockaddr_un addr;
	struct to_samba request;
	struct from_samba reply;
	int len;
	uint32_t length, net_length, op;
	((void)(argc));
	((void)(argv));

        clock_gettime(CLOCK_MONOTONIC, &start);

	server = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server == -1) {
	  printf("## socket() failed: %s\n", strerror(errno));
	  return 1;
	}

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, name, sizeof(addr.sun_path));

	if (connect(server, (struct sockaddr *)&addr,
		strlen(addr.sun_path) + sizeof(addr.sun_family)) <0) {
	  printf("## connect() failed: %s\n", strerror(errno));
	  return 2;
	}

	printf("Got connected...\n");

	length = sizeof(request);
	net_length = htonl(length);
	len = write(server, &net_length, sizeof(net_length));
	if (len != sizeof(net_length)) {
	  printf("## write() length failed, len=%d: %s\n", len, strerror(errno));
	  return 2;
	}

	/* Fill in request */
	request.version = 0;
	request.op = 0;
	request.packet_id = HACK_PACKET_ID;
	request.key_id = HACK_KEY_ID;
	for (int i=0; i<LEN_PKT_NOMAC; i++) {
	  request.pkt[i] = i;
	}

	len = write(server, &request, sizeof(request));
	if (len != sizeof(request)) {
	  printf("## write() body failed, len=%d: %s\n", len, strerror(errno));
	  return 2;
	}

	len = read(server, &net_length, sizeof(net_length));
	length = ntohl(net_length);
	if (len != sizeof(net_length)) {
	  printf("## read() length failed, len=%d: %s\n", len, strerror(errno));
	  return 2;
	}
	len = read(server, &reply, sizeof(reply));
	if (len != sizeof(reply)) {
	  printf("## read() body failed, len=%d: %s\n", len, strerror(errno));
	  return 2;
	}

	/* check stuff */
	if (length != sizeof(reply)) {
	  printf("** Wrong length word: %u\n", length);
	}
	op = ntohl(reply.op);
	if (op != 3) {
	  printf("** Wrong op field: %u\n", op);
	}
	if (reply.packet_id != HACK_PACKET_ID) {
	  printf("** Wrong packet_id field: %u\n", reply.packet_id);
	}
	if (reply.key_id != HACK_KEY_ID) {
	  printf("** Wrong key_id field: %u\n", reply.key_id);
	}

	if (memcmp(reply.pkt, request.pkt, sizeof(reply.pkt))) {
	  printf("** pkt body mismatch\n");
	  /* FIXME: print it out */
	}

        clock_gettime(CLOCK_MONOTONIC, &stop);
	elapsed = (double)(stop.tv_sec-start.tv_sec);
	elapsed += (double)(stop.tv_nsec-start.tv_nsec)/1E9;

	printf("Signing took %.1f us\n", elapsed*1E6);
	return 0;
}

