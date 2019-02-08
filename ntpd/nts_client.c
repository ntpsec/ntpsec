/*
 * nts_client.c - Network Time Security (NTS) client side support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 */
#include "config.h"

#include <unistd.h>

#ifdef HAVE_RES_INIT
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "ntp_types.h"
#include "ntpd.h"


int open_TCP_socket(const char *hostname);

bool nts_probe(struct peer * peer) {

  SSL_CTX *ctx;
  SSL     *ssl;
  int      server = 0;
  X509    *cert = NULL;
  uint8_t  buff[1000];
  int      transfered;

  server = open_TCP_socket(peer->hostname);
  if (-1 == server) return false;

  // No error checking yet.
  // Ugly since most SSL routines return 1 on success.

// Fedora 29:  0x1010101fL  1.1.1a
// Fedora 28:  0x1010009fL  1.1.0i
// CentOS 6:   0x1000105fL  1.0.1e
// NetBSD 8:   0x100020bfL  1.0.2k
// FreeBSD 12: 0x1010101fL  1.1.1a-freebsd
#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  ctx = SSL_CTX_new(TLS_client_method());
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);  // FIXME
  SSL_CTX_set_max_proto_version(ctx, 0);
#else
  ctx = SSL_CTX_new(TLSv1_2_client_method());
  if (1) // FIXME if (non-default version request)
    msyslog(LOG_INFO, "NTS: can't set min/max TLS versions.");
#endif

  SSL_CTX_set_default_verify_paths(ctx);   // Use system root certs

  ssl = SSL_new(ctx);

  SSL_set_fd(ssl, server);
  SSL_set_tlsext_host_name(ssl, peer->hostname);

  SSL_connect(ssl);
  SSL_do_handshake(ssl);

  switch (SSL_version(ssl)) {
#ifdef TLS1_3_VERSION
    case TLS1_3_VERSION:
      msyslog(LOG_INFO, "NTS: Using TLS1.3");
      break;
#endif
    case TLS1_2_VERSION:
      msyslog(LOG_INFO, "NTS: Using TLS1.2");
      break;
    default:
      msyslog(LOG_INFO, "NTS: Strange version: %d, \"%s\"",
        SSL_version(ssl), SSL_get_version(ssl));
      break;
    }

  cert = SSL_get_peer_certificate(ssl);
  if (NULL == cert) {
    msyslog(LOG_INFO, "NTS: No certificate");
  } else {
    X509_NAME *certname;
    char name[200];
    int certok;
    certname = X509_get_subject_name(cert);
    X509_NAME_oneline(certname, name, sizeof(name));
    msyslog(LOG_INFO, "NTS: certificate subject name: %s", name);
    certname = X509_get_issuer_name(cert);
    X509_NAME_oneline(certname, name, sizeof(name));
    msyslog(LOG_INFO, "NTS: certificate issuer name: %s", name);
    certok = SSL_get_verify_result(ssl);
    if (X509_V_OK == certok) {
      msyslog(LOG_INFO, "NTS: certificate is valid.");
    } else {
      msyslog(LOG_ERR, "NTS: certificate invalid: %d", certok);
    }

  }

  {
    uint8_t req[16] = {
      0x00, 0x01, 0x00, 0x02,
      0x00, 0x00, 0x00, 0x04,
      0x00, 0x02, 0x00, 0x0f,
      0x80, 0x00, 0x00, 0x00 };
    transfered = SSL_write(ssl, req, sizeof(req));
    if (sizeof(req) != transfered) {
      msyslog(LOG_ERR, "NTS-KE-client: write failed: %d, %m", transfered);
      goto bail;
    }
    transfered = SSL_read(ssl, buff, sizeof(buff));
    if (0 > transfered) {
      msyslog(LOG_ERR, "NTS-KE-client: read failed: %d, %m", transfered);
      goto bail;
    }
    msyslog(LOG_ERR, "NTS-KE-client: read %d bytes", transfered);
  }

bail:
  SSL_free(ssl);
  close(server);
  SSL_CTX_free(ctx);

  return false;
}

int open_TCP_socket(const char *hostname) {
  struct addrinfo hints;
  struct addrinfo *answer;
  sockaddr_u sockaddr;
  int gai_rc, err;
  int sockfd;

#ifdef HAVE_RES_INIT
  res_init();  /* see comment in ntp_dns */
#endif

  ZERO(hints);
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  gai_rc = getaddrinfo(hostname, "8123", &hints, &answer);  // FIXME
  if (0 != gai_rc) {
    msyslog(LOG_INFO, "NTS: nts_probe: DNS error: %d, %s",
      gai_rc, gai_strerror(gai_rc));
    return -1;
  }

  memcpy(&sockaddr, answer->ai_addr, answer->ai_addrlen);
  msyslog(LOG_INFO, "NTS: nts_probe connecting to %s=%s",
    hostname, socktoa(&sockaddr));
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    msyslog(LOG_INFO, "NTS: nts_probe: no socket: %m");
  } else {
    // Use first answer
    err = connect(sockfd, answer->ai_addr, answer->ai_addrlen);
    if (-1 == err) {
      msyslog(LOG_INFO, "NTS: nts_probe: can't connect: %m");
      close(sockfd);
      sockfd = -1;
    }
  }

  freeaddrinfo(answer);
  return sockfd;
}

/* end */
