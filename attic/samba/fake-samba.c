/*
 * fake-samba.c -- fake samba server to exercise MS-SNTP option.
 */
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <sys/stat.h>


#include "fake.h"

static char *name = FAKE_SAMBA_SOCKET;

int main(int argc, char *argv[]) {
	int listener;
	struct sockaddr_un addr;
	uint32_t length, net_length;
	((void)(argc));
	((void)(argv));

	unlink(name);  /* Don't care if it fails. */

	listener = socket(AF_UNIX, SOCK_STREAM, 0);
	if (listener == -1) {
	  printf("## socket() failed: %s\n", strerror(errno));
	  return 1;
	}
	
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, name, sizeof(addr.sun_path));

	if (bind(listener, (struct sockaddr *)&addr,
		strlen(addr.sun_path) + sizeof(addr.sun_family)) <0) {
	  printf("## bind() failed: %s\n", strerror(errno));
	  return 2;
	}
	/* change mode so ntpd as user NTP can write to it */
	if (chmod(name, 0777) < 0) {
	  printf("## chmod() failed: %s\n", strerror(errno));
	  return 3;
	}


	if (listen(listener, 6) < 0) {
	  printf("## listen() failed: %s\n", strerror(errno));
	  return 4;
	}

	for (;;) {
	  struct sockaddr_un who;
	  socklen_t len = sizeof(who);
	  int client;
	  char buffer[100];
	  struct to_samba request;
	  struct from_samba reply;
	  printf("## Waiting for a connection...\n");
	  client = accept(listener, (struct sockaddr *)&who, &len);
	  if (client < 0) {
	    printf("## accept() failed: %s\n", strerror(errno));
	    return 5;
	  }
	  printf("## Got one...  len=%d\n", len);

	  len = read(client, &net_length, sizeof(net_length));
	  if (len <0) {
	    printf("## read() length failed: len=%d, %s\n", len, strerror(errno));
	    return 6;
	  }
	  if (len != sizeof(net_length)) {
	    printf("** Wrong length body read(), len=%d\n", len);
	    close(client);
	    printf("\n");
	    continue;
	  } else {
	    printf("## read %d bytes\n", len);
	    length = ntohl(net_length);
	  }
	  if (length != sizeof(request)) {
	    printf("** Wrong length word: %u\n", length);
	  }
	  len = read(client, buffer, sizeof(buffer));
	  if (len <0) {
	    printf("## read() body failed: len=%d, %s\n", len, strerror(errno));
	    return 6;
	  }
	  if (len != sizeof(request)) {
	    printf("** Wrong length body read(), len=%d\n", len);
	    close(client);
	    printf("\n");
	    continue;
	  } else {
	    printf("## read %d bytes\n", len);
	    memcpy(&request, buffer, sizeof(request));
	  }
	  if (request.op != 0) {
	    printf("** Wrong op field: %u\n", request.op);
	  }

	  /* construct reply */
	  reply.version = request.version;
	  reply.op = htonl(3);
	  reply.packet_id = request.packet_id;
	  memcpy(reply.pkt, request.pkt, sizeof(reply.pkt));
	  reply.key_id = request.key_id;
	  memcpy(reply.mac, HACK_KEY, sizeof(reply.mac));

	  length = sizeof(reply);
	  net_length = htonl(length);
	  len = write(client, &net_length, sizeof(net_length));
	  if (len <0) {
	    printf("## write() length failed: len=%d, %s\n", len, strerror(errno));
	    return 7;
	  }
	  if (len != sizeof(net_length)) {
	    printf("## Wrong length write(),  len=%d\n", len);
	  } else {
	    printf("## wrote %d bytes\n", len);
          }

	  len = write(client, &reply, sizeof(reply));
	  if (len <0) {
	    printf("## write() body failed: len=%d, %s\n", len, strerror(errno));
	    return 7;
	  }
	  if (len != sizeof(reply)) {
	    printf("## Wrong length write(),  len=%d\n", len);
	  } else {
	    printf("## wrote %d bytes\n", len);
          }
	  close(client);
	  printf("packet_id: %u, key_id: %d\n",
	    request.packet_id, request.key_id);
	  printf("\n");
	}

	printf("Working so far...\n");
	return 0;
}

