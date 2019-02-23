/*
 * nts_client.c - Network Time Security (NTS) client side support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 */
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_RES_INIT
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "ntp_types.h"
#include "ntpd.h"
#include "nts.h"

#define PORT "123"

int open_TCP_socket(const char *hostname);
bool nts_set_cert_search(SSL_CTX *ctx);
bool nts_client_build_request(struct peer* peer, SSL *ssl);
bool nts_client_process_response(struct peer* peer, SSL *ssl);

SSL_CTX *client_ctx = NULL;

// Fedora 29:  0x1010101fL  1.1.1a
// Fedora 28:  0x1010009fL  1.1.0i
// Debian 9:   0x101000afL  1.1.0j
// Debian 8:   0x1000114fL  1.0.1t
// CentOS 7:   0x100020bfL  1.0.2k
// CentOS 6:   0x1000105fL  1.0.1e
// NetBSD 8:   0x100020bfL  1.0.2k
// NetBSD 7:   0x1000115fL  1.0.1u
// FreeBSD 12: 0x1010101fL  1.1.1a-freebsd
// FreeBSD 11: 0x100020ffL  1.0.2o-freebsd

bool nts_client_init(void) {
  bool     ok = true;

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  client_ctx = SSL_CTX_new(TLS_client_method());
#else
  OpenSSL_add_all_ciphers();  // FIXME needed on NetBSD
  client_ctx = SSL_CTX_new(TLSv1_2_client_method());
#endif
  if (NULL == client_ctx) {
    // ?? Happens on NetBSD - says no ciphers
    msyslog(LOG_INFO, "NTSs: NULL client_ctx");
    nts_log_ssl_error();
    return false;
  }

#if (OPENSSL_VERSION_NUMBER > 0x1000200fL)
  {
  // 4., ALPN, RFC 7301
  static unsigned char alpn [] = { 7, 'n', 't', 's', 'k', 'e', '/', '1' };
  SSL_CTX_set_alpn_protos(client_ctx, alpn, sizeof(alpn));
  }
#endif

  SSL_CTX_set_session_cache_mode(client_ctx, SSL_SESS_CACHE_OFF);

  ok &= nts_load_versions(client_ctx);
  ok &= nts_load_ciphers(client_ctx);
  ok &= nts_set_cert_search(client_ctx);

  if (!ok) {
    msyslog(LOG_ERR, "NTSc: Troubles setting up client SSL CTX");
    if (1) {
      msyslog(LOG_ERR, "NTSc: Maybe should bail.");   // FIXME
      return true;
    }
    SSL_CTX_free(client_ctx);
    client_ctx = NULL;
    return false;
  };

  return true;
}

bool nts_probe(struct peer * peer) {
  SSL     *ssl;
  int      server = 0;
  X509    *cert = NULL;

  if (NULL == client_ctx)
    return false;

  nts_ke_probes++;

  server = open_TCP_socket(peer->hostname);
  if (-1 == server)
    return false;

  // FIXME
  // Not much error checking yet.
  // Ugly since most SSL routines return 1 on success.

  ssl = SSL_new(client_ctx);
  SSL_set_fd(ssl, server);

// https://wiki.openssl.org/index.php/Hostname_validation
#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  // SSL_set_hostflags(ssl, X509_CHECK_FLAG_NO_WILDCARDS);
  SSL_set1_host(ssl, peer->hostname);
#elif (OPENSSL_VERSION_NUMBER > 0x1000200fL)
{
  X509_VERIFY_PARAM *param = SSL_get0_param(ssl);
  // X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_WILDCARDS);
  if (1 != X509_VERIFY_PARAM_set1_host(param,
          peer->hostname, strlen(peer->hostname))) {
      msyslog(LOG_ERR, "NTSc: troubles setting hostflags");
  }
  SSL_set_verify(ssl, SSL_VERIFY_PEER, NULL);
}
#else
  msyslog(LOG_ERR, "NTSc: can't check hostname/certificate");
#endif

  // SSL_set_timeout(SSL_get_session(ssl), 2);  // FIXME
  if (1 != SSL_connect(ssl)) {
    msyslog(LOG_INFO, "NTSc: SSL_connect failed");
    nts_log_ssl_error();
    goto bail;
  }
  if (1 != SSL_do_handshake(ssl)) {
    msyslog(LOG_INFO, "NTSc: SSL_do_handshake failed");
    nts_log_ssl_error();
    goto bail;
  }

  /* This may be clutter, but this is how to do it. */
  msyslog(LOG_INFO, "NTSc: Using %s, %s with %d secret bits",
    SSL_get_version(ssl),
    SSL_get_cipher_name(ssl),
    SSL_get_cipher_bits(ssl, NULL));

  cert = SSL_get_peer_certificate(ssl);
  if (NULL == cert) {
    msyslog(LOG_INFO, "NTSc: No certificate");
  } else {
    X509_NAME *certname;
    char name[200];
    int certok;
    certname = X509_get_subject_name(cert);
    X509_NAME_oneline(certname, name, sizeof(name));
    msyslog(LOG_INFO, "NTSc: certificate subject name: %s", name);
    certname = X509_get_issuer_name(cert);
    X509_NAME_oneline(certname, name, sizeof(name));
    msyslog(LOG_INFO, "NTSc: certificate issuer name: %s", name);
    certok = SSL_get_verify_result(ssl);
    if (X509_V_OK == certok) {
      msyslog(LOG_INFO, "NTSc: certificate is valid.");
    } else {
      msyslog(LOG_ERR, "NTSc: certificate invalid: %d=>%s",
          certok, X509_verify_cert_error_string(certok));
    }
  }

  if (!nts_client_build_request(peer, ssl))
    goto bail;
  if (!nts_client_process_response(peer, ssl))
    goto bail;

  // FIXME
  /* We are using AEAD_AES_SIV_CMAC_256, from RFC 5297
   * There are no alternatives and no clean API yet.
   */
  peer->nts_state.keylen = nts_get_key_length(AEAD_AES_SIV_CMAC_256);
  nts_make_keys(ssl,
    peer->nts_state.c2s,
    peer->nts_state.s2c,
    peer->nts_state.keylen);

bail:
  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(server);

  return false;
}

int open_TCP_socket(const char *hostname) {
  struct addrinfo hints;
  struct addrinfo *answer;
  sockaddr_u sockaddr;
  int gai_rc, err;
  int sockfd;

  ZERO(hints);
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  gai_rc = getaddrinfo(hostname, PORT, &hints, &answer);  // FIXME
  if (0 != gai_rc) {
    msyslog(LOG_INFO, "NTSc: nts_probe: DNS error: %d, %s",
      gai_rc, gai_strerror(gai_rc));
    return -1;
  }

  memcpy(&sockaddr, answer->ai_addr, answer->ai_addrlen);
  msyslog(LOG_INFO, "NTSc: nts_probe connecting to %s=%s, port %s",
    hostname, socktoa(&sockaddr), PORT);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    msyslog(LOG_INFO, "NTSc: nts_probe: no socket: %m");
  } else {
    // Use first answer
    err = connect(sockfd, answer->ai_addr, answer->ai_addrlen);
    if (-1 == err) {
      msyslog(LOG_INFO, "NTSc: nts_probe: connect failed: %m");
      close(sockfd);
      sockfd = -1;
    }
  }

  freeaddrinfo(answer);
  return sockfd;
}

// FIXME - context shouldn't be magic
bool nts_make_keys(SSL *ssl, uint8_t *c2s, uint8_t *s2c, int keylen) {
  // char *label = "EXPORTER-network-time-security/1";
  // Subject: [Ntp] [NTS4NTP] info for NTS developers
  // From: Martin Langer <mart.langer@ostfalia.de>
  // Date: Tue, 15 Jan 2019 11:40:13 +0100
  // bug in OpenSSL 1.1.1a
  const char *label = "EXPORTER-nts/1";
  unsigned char context[5] = {0x00, 0x00, 0x00, 0x0f, 0x00};
  if (1 != SSL_export_keying_material(ssl, c2s, keylen,
        label, strlen(label),
        context, 5, 1)) {
     msyslog(LOG_ERR, "NTS: Error making c2s\n");
     nts_log_ssl_error();
     return false;
  }
  context[4] = 0x01;
  if (1 != SSL_export_keying_material(ssl, s2c, keylen,
        label, strlen(label),
        context, 5, 1)) {
     msyslog(LOG_ERR, "NTS: Error making s2c\n");
     nts_log_ssl_error();
     return false;
  }
  return true;
}

bool nts_client_build_request(struct peer* peer, SSL *ssl) {
  uint8_t buff[1000];
  int     used, transfered;
  struct BufCtl_t buf;

  UNUSED_ARG(peer);

  buf.next = buff;
  buf.left = sizeof(buff);

  /* 4.1.2 Next Protocol, 0 for NTP */
  nts_append_record_uint16(&buf, NTS_CRITICAL+nts_next_protocol_negotiation, 0);

  /* 4.1.5 AEAD Algorithm List
   * AEAD_AES_SIV_CMAC_256 is the only one for now */
  nts_append_record_uint16(&buf, nts_algorithm_negotiation, AEAD_AES_SIV_CMAC_256);

  /* 4.1.1: End, Critical */
  nts_append_record_null(&buf, NTS_CRITICAL+nts_end_of_message);

  used = sizeof(buff)-buf.left;
  if (used >= (int)(sizeof(buff)-10)) {
    msyslog(LOG_ERR, "NTSc: write failed: %d, %ld, %m",
        used, (long)sizeof(buff));
    return false;
  }
  transfered = SSL_write(ssl, buff, used);
  if (used != transfered) {
    msyslog(LOG_ERR, "NTSc: write failed: %d, %d, %m", used, transfered);
    return false;
  }
  return true;
}

bool nts_client_process_response(struct peer* peer, SSL *ssl) {
  uint8_t  buff[2000];
  int transfered, idx;
  struct BufCtl_t buf;

  transfered = SSL_read(ssl, buff, sizeof(buff));
  if (0 > transfered) {
    msyslog(LOG_ERR, "NTSc: read failed: %d, %m", transfered);
    return false;
  }
  msyslog(LOG_ERR, "NTSc: read %d bytes", transfered);

  peer->nts_state.aead = -1;
  peer->nts_state.keylen = 0;
  peer->nts_state.writeIdx = 0;
  peer->nts_state.readIdx = 0;
  peer->nts_state.count = 0;

  buf.next = buff;
  buf.left = transfered;
  while (buf.left > 0) {
    uint16_t type, data;
    bool critical = false;
    int length;

    type = nts_next_record(&buf, &length);
    if (NTS_CRITICAL & type) {
      critical = true;
      type &= ~NTS_CRITICAL;
    }
    if (0) // Handy for debugging but very verbose
      msyslog(LOG_ERR, "NTSc: Record: T=%d, L=%d, C=%d", type, length, critical);
    switch (type) {
      case nts_error:
        data = nts_next_uint16(&buf);
        if (sizeof(data) != length)
          msyslog(LOG_ERR, "NTSc: wrong length on error: %d", length);
        msyslog(LOG_ERR, "NTSc: error: %d", data);
        return false;
      case nts_next_protocol_negotiation:
        data = nts_next_uint16(&buf);
        if ((sizeof(data) != length) || (data != 0)) {
          msyslog(LOG_ERR, "NTSc: NPN-Wrong length or bad data: %d, %d",
              length, data);
          return false;
        }
        break;
      case nts_algorithm_negotiation:
        data = nts_next_uint16(&buf);
        if ((sizeof(data) != length) || (data != AEAD_AES_SIV_CMAC_256)) {
          msyslog(LOG_ERR, "NTSc: AN-Wrong length or bad data: %d, %d",
              length, data);
          return false;
        }
        peer->nts_state.aead = data;
        break;
      case nts_new_cookie:
        if (NTS_MAX_COOKIELEN < length) {
          msyslog(LOG_ERR, "NTSc: NC cookie too big: %d", length);
          return false;
        }
        if (0 == peer->nts_state.cookielen)
           peer->nts_state.cookielen = length;
        if (length != peer->nts_state.cookielen) {
          msyslog(LOG_ERR, "NTSc: Cookie length mismatch %d, %d.",
            length, peer->nts_state.cookielen);
          return false;
        }
        idx = peer->nts_state.writeIdx;
        if (NTS_MAX_COOKIES <= peer->nts_state.count) {
          msyslog(LOG_ERR, "NTSc: Extra cookie ignored.");
          break;
        }
        nts_next_bytes(&buf, (uint8_t*)&peer->nts_state.cookies[idx], length);
        peer->nts_state.writeIdx++;
        peer->nts_state.writeIdx = peer->nts_state.writeIdx % NTS_MAX_COOKIES;
        peer->nts_state.count++;
        break;
      case nts_end_of_message:
        if ((0 != length) || !critical) {
          msyslog(LOG_ERR, "NTSc: EOM-Wrong length or not Critical: %d, %d",
              length, critical);
          return false;
        }
        if (0 != buf.left) {
          msyslog(LOG_ERR, "NTSc: EOM not at end: %d", buf.left);
          return false;
        }
       // FIXME check for no more
        break;
      default:
        msyslog(LOG_ERR, "NTSc: received strange type: T=%d, C=%d, L=%d",
          type, critical, length);
        if (critical) return false;
        buf.next += length;
        buf.left -= length;
        break;
    } /* case */
  }   /* while */

  // FIXME lots of other checks
  if (-1 == peer->nts_state.aead) {
    msyslog(LOG_ERR, "NTSc: No AEAD algorithim.");
    return false;
  }
  if (0 == peer->nts_state.count) {
    msyslog(LOG_ERR, "NTSc: No cookies.");
    return false;
  }

  msyslog(LOG_ERR, "NTSc: Got %d cookies, length %d.",
    peer->nts_state.count, peer->nts_state.cookielen);
  return true;
}

bool nts_set_cert_search(SSL_CTX *ctx) {
  struct stat statbuf;
  if (NULL == ntsconfig.ca) {
    msyslog(LOG_INFO, "NTSc: Using system default root certificates.");
    SSL_CTX_set_default_verify_paths(ctx);   // Use system root certs
    return true;
  }
  if (0 == stat(ntsconfig.ca, &statbuf)) {
    if (S_ISDIR(statbuf.st_mode)) {
      msyslog(LOG_INFO, "NTSc: Using dir %s for root certificates.", ntsconfig.ca);
      SSL_CTX_load_verify_locations(ctx, NULL, ntsconfig.ca);
      return true;
    }
    if (S_ISREG(statbuf.st_mode)) {
      msyslog(LOG_INFO, "NTSc: Using file %s for root certificates.", ntsconfig.ca);
      SSL_CTX_load_verify_locations(ctx, ntsconfig.ca, NULL);
      return true;
    }
    msyslog(LOG_ERR, "NTSc: cert dir/file isn't dir or file: %s. mode 0x%x",
        ntsconfig.ca, statbuf.st_mode);
    return false;
  }
  msyslog(LOG_ERR, "NTSc: can't stat cert dir/file: %s, %m", ntsconfig.ca);
  return false;
}

/* end */
