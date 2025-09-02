/* Hack to generate various NTS-KE server SSL errors.
 * Build with:
 *     cc -g -Wall -lcrypto -lssl -o tls-tester tls-tester.c
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
#include <openssl/err.h>

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
bool send_chunks = false;

/* working finger into a buffer - updated by append/unpack routines */
struct BufCtl_t {
    uint8_t *next;  /* pointer to next data/space */
    int left;       /* data left or space available */
};
typedef struct BufCtl_t BufCtl;

void append_record16(BufCtl* buf, uint16_t type, uint16_t data);
void append_header(BufCtl* buf, uint16_t type, uint16_t length);
void append_uint16(BufCtl* buf, uint16_t data);


static void do_args (int argc, char *argv[]) {
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
    } else if (0 == strcmp(argv[1], "-send-chunks")) {
      send_chunks = true;
      argc--;
    }
  }

  if (1 < argc) {
    host = argv[1];
  }
  if (2 < argc) {
    port = argv[2];
  }
}

static int do_connect (void) {
  int err;
  struct addrinfo hints;
  int gai_rc; 
  struct addrinfo *answer, *worker;
  sockaddr_u  address;
  int address_len = 0;
  int server;

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
  freeaddrinfo(answer);
  return server;
}

static void print_buf(uint8_t *buff, int bytes) {
  while (0 < bytes) {
    uint16_t *buff16 = (uint16_t *)buff;
    uint16_t type, length;
    type = ntohs(*buff16++);  buff+= 2;
    length = ntohs(*buff16++);  buff+= 2;
    bytes -=4;
    buff += length;
    bytes -= length;
    printf("  type=0x%04x, lng=%d", type, length);
    if (20<length) {
      printf("\n");
      continue;
    }
    while (0<length) {
      uint16_t data = ntohs(*buff16++);
      length-=2;
      printf(", data=0x%0x(%d)", data, data);
    }
    printf("\n");
  }
}

/* send request in chunks to test collector */
static int chunkify(SSL *ssl, uint8_t *buff, int bytes) {
  int total = 0;
  while (0<bytes) {
    int to_send, sent;
    to_send = random() % (bytes+1);
    if (0==total && to_send==bytes) {
      /* avoid all in one chunk */
      continue;
    }
    if (to_send<3) {
      to_send=bytes;
    }
    sent = SSL_write(ssl, buff, to_send);
    printf("Sent %d=>%d bytes\n", to_send, sent);
    if (0>=sent) return sent;
    buff += sent;
    bytes -= sent;
    total += sent;
  }
  return total;
}


int main (int argc, char *argv[]) {
  int bytes, wrote, read;
  int server;
  uint8_t buff[1000];
  struct  BufCtl_t buf;
  SSL_CTX *ctx;
  SSL *ssl;

  srandom(time(NULL));

  /* I'm testing newer versions of OpenSSL.
   * Fedora's config file needs something not in the default OpenSSL build.
   * This avoids crashing.
   */
  OPENSSL_init_crypto(OPENSSL_INIT_NO_LOAD_CONFIG, NULL);

  do_args(argc, argv);

  printf("### testing %s:%s\n", host, port);

  server = do_connect();

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

  errno = 0;
  ctx = SSL_CTX_new(TLS_client_method());
  if (NULL == ctx) {
    printf("!!! Oops.  SSL_CTX_new failed.: %s\n", strerror(errno));
    ERR_print_errors_fp(stderr);
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

  buf.next = buff;
  buf.left = sizeof(buff);
  append_record16(&buf, 0x8001, 0);  // next protocol
  append_record16(&buf, 4, 15);      // algorithm_negotiation
  append_header(&buf, 0x8000, 0);    // end_of_message
  bytes = sizeof(buff)-buf.left;
  printf("Sending %d bytes\n", bytes);
  print_buf(buff, bytes);
  if (send_chunks) {
    wrote = chunkify(ssl, buff, bytes);
  } else {
    wrote = SSL_write(ssl, buff, bytes);
  }
  if (wrote != bytes) {
    printf("!!! error from SSL_write: %d=>%d, %s\n",
      bytes, wrote, strerror(errno));
    exit(1);
  }
  read = SSL_read(ssl, buff, sizeof(buff));
  if (-1 == read) {
    printf("!!! error from SSL_read: %d, %s\n", read, strerror(errno));
    exit(1);
  }
  printf("Received %d bytes\n", read);
  print_buf(buff, read);
  return 0;
}

/* no error checking, length excludes header */
void append_record16(BufCtl* buf, uint16_t type, uint16_t data) {
	append_header(buf, type, 2);
	append_uint16(buf, data);
}

void append_header(BufCtl* buf, uint16_t type, uint16_t length) {
        uint16_t *ptr = (uint16_t *)buf->next;
        *ptr++ = htons(type);
        *ptr++ = htons(length);
        buf->next += 4;
        buf->left -= 4;
}

void append_uint16(BufCtl* buf, uint16_t data) {
        uint16_t *ptr = (uint16_t *)buf->next;
        *ptr++ = htons(data);
        buf->next += 2;
        buf->left -= 2;
}

