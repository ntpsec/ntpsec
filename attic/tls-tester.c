/* Hack to generate various NTS-KE server SSL errors.
 * Build with:
 *     cc -g -Wall -lssl -o tls-tester tls-tester.c
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>			/* Needed on FreeBSD */
#include <netdb.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>

typedef union {                         /* On Linux, these come from: */
        struct sockaddr         sa;     /* /usr/include/bits/socket.h */
        struct sockaddr_in      sa4;    /* /usr/include/linux/in.h */
        struct sockaddr_in6     sa6;    /* /usr/include/linux/in6.h */
} sockaddr_u;

const char *host = "localhost";
const char *port = "4460";

bool early_disconnect = false;  // before TLS
bool early_timeout = false;
bool TLS_disconnect = false;	// after TLS
bool TLS_timeout = false;

int main (int argc, char *argv[]) {
  int err;
  struct addrinfo hints;
  int gai_rc; 
  struct addrinfo *answer, *worker;
  sockaddr_u  address;
  int address_len = 0;
  int  server;
  SSL_CTX *ctx;
  SSL *ssl;

  if (1 < argc) {
    if (false) {
	NULL;
    } else if (0 == strcmp(argv[1], "-early-disconnect")) {
      early_disconnect = true;
      argc--;
    } else if (0 == strcmp(argv[1], "-early-timeout")) {
      early_timeout = true;
      argc--;
    } else if (0 == strcmp(argv[1], "-TLS-disconnect")) {
      TLS_disconnect = true;
      argc--;
    } else if (0 == strcmp(argv[1], "-TLS-timeout")) {
      TLS_timeout = true;
      argc--;
    }
  }

  if (1 < argc) {
    host = argv[1];
  }
  if (2 < argc) {
    port = argv[2];
  }

  printf("### testing %s:%s\n", host, port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  gai_rc = getaddrinfo(host, port, &hints, &answer);
  if (0 != gai_rc) {
    printf("### DNS error: %s:%s => %s\n", host, port, gai_strerror(gai_rc));
    exit(1);
  }

  for (worker=answer; NULL!=worker; worker=worker->ai_next) {
    char buff[100];
    const char *text;
    if (sizeof(address) < worker->ai_addrlen) {
      printf("!!! Oops.  Address size: %u, %lu\n",
        worker->ai_addrlen, (unsigned long)sizeof(address));
      exit(1);
    }
    address_len = worker->ai_addrlen;
    memcpy(&address, worker->ai_addr, worker->ai_addrlen);
    switch (worker->ai_family) {
      case AF_INET:
        text = inet_ntop(AF_INET,
          &((struct sockaddr_in *)worker->ai_addr)->sin_addr,
          buff, sizeof(buff));
        break;
      case AF_INET6:
        text = inet_ntop(AF_INET6,
          &((struct sockaddr_in6 *)worker->ai_addr)->sin6_addr,
          buff, sizeof(buff));
        break;
      default:
        printf("!!! Oops.  Strange network family %d\n", worker->ai_family);
        exit(1);
    }
    // check for NULL??
    printf("# Found %s\n", text);
  }

  server = socket(address.sa4.sin_family, SOCK_STREAM, 0);
  if (-1 == server) {
    printf("!!! Oops.  Can't make socket: %s\n", strerror(errno));
    exit(1);
  }
  err = connect(server, &address.sa, address_len);
  if (-1 == err) {
    printf("!!! Oops.  Can't connect: %s\n", strerror(errno));
    exit(1);
  }

  printf("### No SSL yet.\n");
  if (early_disconnect) {
    printf("### Closing...\n");
    close(server);
    exit(0);
    // SSL_accept => code 1, err 0A000126   1=>SSL_ERROR_SSL
  }
  if (early_timeout) {
    printf("### Sleeping...\n");
    sleep(5);
    exit(0);
    // SSL_accept => code 2, err 0   2=>SSL_ERROR_WANT_READ
  }

  ctx = SSL_CTX_new(TLS_client_method());
  if (NULL == ctx) {
    printf("!!! Oops.  SSL_CTX_new failed.\n");
    exit(1);
  }
  ssl = SSL_new(ctx);
  if (NULL == ssl) {
    printf("!!! Oops.  SSL_new failed.\n");
    exit(1);
  }
  SSL_set_fd(ssl, server);

  /* We aren't checking certificates */
  if (1 != SSL_connect(ssl)) {
    printf("!!! Oops.  SSL_connect failed.\n");
    exit(1);
  }

  printf("### SSL connected.\n");
  if (TLS_disconnect) {
    printf("### Closing...\n");
    close(server);
    exit(0);
    // SSL_accept => code 1, err 0A000126   1=>SSL_ERROR_SSL
  }
  if (TLS_timeout) {
    printf("### Sleeping...\n");
    sleep(5);
    exit(0);
    // SSL_accept => code 2, err 0   2=>SSL_ERROR_WANT_READ
  }


  freeaddrinfo(answer);

  return 0;
}
