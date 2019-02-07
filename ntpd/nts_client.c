/*
 * nts_client.c - Network Time Security (NTS) client side support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 */
#include "config.h"

#include <unistd.h>

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include <openssl/ssl.h>

#include "ntp_types.h"
#include "ntpd.h"


int open_TCP_socket(const char *hostname);

bool nts_probe(struct peer * peer) {

  SSL_CTX *ctx;
  SSL     *ssl;
  int      server = 0;

  server = open_TCP_socket(peer->hostname);
  if (-1 == server) return false;

  // No error checking yet.
  // Ugly since most SSL routines return 1 on success.

// Fedora 29: 0x1010101fL  1.1.1a
// Fedora 28: 0x1010009fL  1.1.0i
#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  ctx = SSL_CTX_new(TLS_client_method());
#else
  ctx = SSL_CTX_new(TLSv1_2_client_method());
#endif

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  SSL_CTX_set_default_verify_file(ctx);   // Use system root certs
#else
  // FIXME
#endif

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  // FIXME
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
  SSL_CTX_set_max_proto_version(ctx, 0);
#else
  // FIXME
#endif

  ssl = SSL_new(ctx);

  SSL_set_fd(ssl, server);
  SSL_set_tlsext_host_name(ssl, peer->hostname);

  SSL_free(ssl);
  close(server);
  SSL_CTX_free(ctx);

  return false;
}

int open_TCP_socket(const char *hostname) {
  struct addrinfo hints;
  struct addrinfo *answer;
  int gai_rc, err;
  int sockfd;

  res_init();

  ZERO(hints);
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  gai_rc = getaddrinfo(hostname, "ntp", &hints, &answer);
  if (0 != gai_rc) {
    msyslog(LOG_INFO, "DNS: nts_probe: DNS error: %d, %s",
      gai_rc, gai_strerror(gai_rc));
    return -1;
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    msyslog(LOG_INFO, "DNS: nts_probe: no socket: %m");
  } else {
    // Use first answer
    err = connect(sockfd, answer->ai_addr, answer->ai_addrlen);
    if (-1 == err) {
      msyslog(LOG_INFO, "DNS: nts_probe: can't connect: %m");
      close(sockfd);
      sockfd = -1;
    }
  }

  freeaddrinfo(answer);
  return sockfd;
}

/* end */
