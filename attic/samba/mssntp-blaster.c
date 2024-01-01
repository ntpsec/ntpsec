/*
 * mssntp-blaster.c:
 *     hack to generate mssntp traffic so you can measure throughput.
 * ntpq has an mssntpinfo command.
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "fake.h"

char* host = "localhost";	// First arg
int keyid = 6;			// Second arg
int delay = 15;			// Third arg
int packets = 10;		// Fourth arg

int sock;

struct ntp_packet ntp_send, ntp_recv;

int ntp_port_number = 123;

/* NTP time starts 1900, unix time starts 1970 */
int ntp_to_unix = 0x7c558180;
int unix_to_ntp = 0x83aa7e80;

#define DATELENGTH 100
static void bailout(char *msg)
{
  int saverrr = errno;
  char timetxt[DATELENGTH];
  time_t nowbin;
  struct tm nowstruct;
  time(&nowbin);
  localtime_r(&nowbin, &nowstruct);
  strftime(timetxt, DATELENGTH, "%Y-%b-%d %H:%M", &nowstruct);
  printf("** %s %s: errno = %d, %s\n",
    timetxt, msg, saverrr, strerror(saverrr));
  sleep(10);  /* Beware of loops in scripts. */
  exit(1);
};

int main (int argc, char *argv[])
{
  char *hostname;
  struct hostent *target;
  struct sockaddr_in server, client;
  struct timespec delay_spec, now;
  float timeout = 5.0;
  int addr;
  int i, ec, len, saved_errno;

  if (argc > 4) packets = atoi(argv[4]);
  if (packets < 0) bailout("Bad packets");

  if (argc > 3) delay = atoi(argv[3]);
  if (delay < 0) bailout("Bad delay");

  if (argc > 2) keyid = atoi(argv[2]);
  if (keyid < 0 || keyid > 0xFFFFFFFF) bailout("Bad key_id");

  hostname = "localhost";
  if (argc > 1) hostname = argv[1];
  target = gethostbyname(hostname);
  if (target == NULL) bailout("Bad hostname");

  bcopy((char *)target->h_addr, (char *)&addr, sizeof(addr));
  addr = ntohl(addr);
  printf("Connecting to %s=>%d.%d.%d.%d\n",
    hostname,
    (addr >> 24) & 0xff,
    (addr >> 16) & 0xff,
    (addr >> 8) & 0xff,
    (addr >> 0) & 0xff);

  if (0) printf("Sending: len = %ld, header=%8x\n",
    sizeof(ntp_send), ntp_send.header); 

  bzero((char *)&server, sizeof(server));
  bcopy((char *)target->h_addr, (char *)&server.sin_addr, target->h_length);
  server.sin_family = target->h_addrtype;
  server.sin_port = htons(ntp_port_number);

  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock < 0) bailout("socket");

  bzero((char *)&client, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = htons(0);
  
  ec = bind(sock, (struct sockaddr *)&client, sizeof(client) );
  if (ec) bailout("bind");
  
  ec = connect(sock, (struct sockaddr *)&server, sizeof(server) );
  if (ec) bailout("connect");

  delay_spec.tv_sec = (int)timeout;
  delay_spec.tv_nsec = (timeout-delay_spec.tv_sec)*1E9;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, \
    (char*)&delay_spec, sizeof(delay_spec));

  ntp_send.header = htonl(0x23000000);
  ntp_send.keyid = keyid;

  for (i = 0; i<packets; i++) {
    clock_gettime(CLOCK_REALTIME, &now);
    ntp_send.t3.seconds = htonl(now.tv_sec+unix_to_ntp);
    ntp_send.t3.fraction = 0;
    len = send(sock, &ntp_send, sizeof(ntp_send), 0);
    if (len != sizeof(ntp_send)) bailout("send");
    printf("Sent one\n");
    len = recv(sock, &ntp_recv, sizeof(ntp_recv), 0);
    saved_errno = errno;
    if (len == -1) {
      /* probably timeout: EAGIN or EWOULDBLOCK */
      printf("Error from recv: %s\n", strerror(saved_errno));
    } else if (len != sizeof(ntp_recv)) {
      printf("Wrong length from recv: %d\n", len);
    } else {
      printf("Got one.\n");
    }
    nanosleep(&delay_spec, NULL);
  }
  close(sock);
  
  return 0;
}
