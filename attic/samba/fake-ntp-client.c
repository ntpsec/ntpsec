/*  Last modified on Sat Jan  6 00:11:51 PST 2001 by murray  */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "fake.h"

static char* host = "time.example.com";// First arg
static int packets = 1;		// Second arg
static useconds_t delay = 1000;		// Third arg, microseconds after each packet

static int sock;
static struct ntp_packet ntp_send, ntp_recv;

void bailout(char *msg);

void bailout(char *msg)
{
  int saverrr = errno;
  char timetxt[100];
  time_t nowbin;
  struct tm nowstruct;
  time(&nowbin);
  localtime_r(&nowbin, &nowstruct);
  strftime(timetxt, sizeof(timetxt), "%Y-%b-%d %H:%M", &nowstruct);
  printf("** %s %s: errno = %d, %s\n",
    timetxt, msg, saverrr, strerror(saverrr));
  exit(1);
}

int main (int argc, char *argv[])
{
  struct hostent *target;
  struct sockaddr_in server, client;
  float timeout = 5.0;
  struct timeval tv;
  int addr;
  int i, ec;

  if (argc > 3) delay = atoi(argv[3]);
  if (delay < 0) bailout("Bad delay");
  if (argc > 2) packets = atoi(argv[2]);
  if (packets <= 0) bailout("Bad packet count");

  if (argc <= 1) bailout("Need host name");
  host = argv[1];
  target = gethostbyname(host);
  if (target == NULL)
    bailout("usage: xx <name-addr> [count [delay]]");

  bcopy((char *)target->h_addr, (char *)&addr, sizeof(addr));
  addr = ntohl(addr);
  printf("Connecting to %s=>%d.%d.%d.%d\n",
    host,
    (addr >> 24) & 0xff,
    (addr >> 16) & 0xff,
    (addr >> 8) & 0xff,
    (addr >> 0) & 0xff);

  if ((0)) printf("Sending: len = %zu, header=%8x\n",
    sizeof(ntp_send), ntp_send.header); 

  bzero((char *)&server, sizeof(server));
  bcopy((char *)target->h_addr, (char *)&server.sin_addr, target->h_length);
  server.sin_family = target->h_addrtype;
  server.sin_port = htons(123);

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

  tv.tv_sec = timeout;
  tv.tv_usec = (timeout-tv.tv_sec)*1E6;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));

  ntp_send.header = htonl(0x23000000);
  ntp_send.keyid = 56578;  

  for (i = 0; i < packets; i++) {
    int len;
    struct timespec start, stop;
    double elapsed;
    ntp_send.t3.seconds = htonl(i);
    ntp_send.t3.fraction = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    len = send(sock, &ntp_send, sizeof(ntp_send), 0);
    if (len != sizeof(ntp_send)) bailout("send");
    len = recv(sock, &ntp_recv, sizeof(ntp_recv), 0);
    if (len != sizeof(ntp_recv)) bailout("recv");
    clock_gettime(CLOCK_MONOTONIC, &stop);
    elapsed = (double)(stop.tv_sec-start.tv_sec);
    elapsed += (double)(stop.tv_nsec-start.tv_nsec)/1E9;
    printf("Signing took %.1f us\n", elapsed*1E6);
    usleep(delay);
  }
  close(sock);
  
  return 0;
}
