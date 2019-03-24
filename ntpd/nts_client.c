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
#include "nts2.h"
#include "ntp_dns.h"

int open_TCP_socket(struct peer* peer);
bool nts_set_cert_search(SSL_CTX *ctx);
bool check_certificate(struct peer* peer, SSL *ssl);
bool nts_client_send_request(struct peer* peer, SSL *ssl);
bool nts_client_process_response(struct peer* peer, SSL *ssl);

static SSL_CTX *client_ctx = NULL;
static sockaddr_u sockaddr;
static bool addrOK;

// Fedora 29:  0x1010102fL  1.1.1b
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
    /* Happens if no ciphers */
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
  SSL_CTX_set_timeout(client_ctx, NTS_KE_TIMEOUT);

  ok &= nts_load_versions(client_ctx);
  ok &= nts_load_ciphers(client_ctx);
  ok &= nts_set_cert_search(client_ctx);

  if (!ok) {
    msyslog(LOG_ERR, "NTSc: Troubles setting up client SSL CTX");
    exit(1);
  };

  return true;
}

bool nts_probe(struct peer * peer) {
  struct timeval timeout = {.tv_sec = NTS_KE_TIMEOUT, .tv_usec = 0};
  SSL     *ssl;
  int      server;
  l_fp     start, finish;
  int      err;

  if (NULL == client_ctx)
    return false;

  nts_ke_probes++;
  addrOK = false;
  get_systime(&start);

  server = open_TCP_socket(peer);
  if (-1 == server) {
    nts_ke_probes_bad++;
    return false;
  }

  err = setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (0 > err) {
    msyslog(LOG_ERR, "NTSc: can't setsockopt: %s", strerror(errno));
    close(server);
    nts_ke_probes_bad++;
    return false;
  }

  // FIXME
  // Not much error checking yet.
  // Ugly since most SSL routines return 1 on success.

  ssl = SSL_new(client_ctx);
  SSL_set_fd(ssl, server);

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
  msyslog(LOG_INFO, "NTSc: Using %s, %s (%d)",
    SSL_get_version(ssl),
    SSL_get_cipher_name(ssl),
    SSL_get_cipher_bits(ssl, NULL));

  if (!check_certificate(peer, ssl))
    goto bail;

  if (!nts_client_send_request(peer, ssl))
    goto bail;
  if (!nts_client_process_response(peer, ssl))
    goto bail;

  /* We are using AEAD_AES_SIV_CMAC_xxx, from RFC 5297
   * key length depends upon which key is selected */
  peer->nts_state.keylen = nts_get_key_length(peer->nts_state.aead);
  if (0 == peer->nts_state.keylen) {
    msyslog(LOG_ERR, "NTSc: Unknown AEAD code: %d", peer->nts_state.aead);
    goto bail;
  }
  nts_make_keys(ssl,
    peer->nts_state.aead,
    peer->nts_state.c2s,
    peer->nts_state.s2c,
    peer->nts_state.keylen);

  addrOK = true;

bail:
  if (!addrOK)
    nts_ke_probes_bad++;
  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(server);

  get_systime(&finish);
  finish -= start;
  msyslog(LOG_INFO, "NTSc: NTS-KE req to %s took %.3Lf sec, %s",
    peer->hostname, lfptod(finish),
    addrOK? "OK" : "fail");

  return addrOK;
}

bool nts_check(struct peer *peer) {
//  msyslog(LOG_INFO, "NTSc: nts_check %s, %d", sockporttoa(&sockaddr), addrOK);
  if (addrOK) {
    dns_take_server(peer, &sockaddr);
    dns_take_status(peer, DNS_good);
  } else
    dns_take_status(peer, DNS_error);
  return addrOK;
}

int open_TCP_socket(struct peer *peer) {
  char host[256], port[32];
  char *tmp;
  struct addrinfo hints;
  struct addrinfo *answer;
  int gai_rc, err;
  int sockfd;
  l_fp start, finish;

  /* copy avoids dancing around const warnings */
  strlcpy(host, peer->hostname, sizeof(host));

  ZERO(hints);
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF(&peer->srcadr);
  tmp = strchr(host, ']');
  if (NULL == tmp) {
    tmp = strchr(host, ':');
  } else {
    /* IPv6 case, start search after ] */
    tmp = strchr(tmp, ':');
  }
  if (NULL == tmp) {
    /* simple case, no : */
    strlcpy(port, "123", sizeof(port));
  } else {
    /* Complicated case, found a : */
    *tmp++ = 0;
    strlcpy(port, tmp, sizeof(port));
  }

  get_systime(&start);
  gai_rc = getaddrinfo(host, port, &hints, &answer);
  if (0 != gai_rc) {
    msyslog(LOG_INFO, "NTSc: nts_probe: DNS error trying to contact %s: %d, %s",
      peer->hostname, gai_rc, gai_strerror(gai_rc));
    return -1;
  }
  get_systime(&finish);
  finish -= start;
  msyslog(LOG_INFO, "NTSc: DNS lookup of %s took %.3Lf sec",
    peer->hostname, lfptod(finish));

  /* Save first answer for NTP */
  memcpy(&sockaddr, answer->ai_addr, answer->ai_addrlen);
  msyslog(LOG_INFO, "NTSc: nts_probe connecting to %s:%s => %s",
    host, port, sockporttoa(&sockaddr));
  /* switch to NTP port in case of server-name:port */
  SET_PORT(&sockaddr, NTP_PORT);
  sockfd = socket(answer->ai_family, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    msyslog(LOG_INFO, "NTSc: nts_probe: no socket: %s", strerror(errno));
  } else {
    // Use first answer
    err = connect(sockfd, answer->ai_addr, answer->ai_addrlen);
    if (-1 == err) {
      msyslog(LOG_INFO, "NTSc: nts_probe: connect failed: %s", strerror(errno));
      close(sockfd);
      sockfd = -1;
    }
  }

  freeaddrinfo(answer);
  return sockfd;
}

bool check_certificate(struct peer* peer, SSL *ssl) {
  X509 *cert = SSL_get_peer_certificate(ssl);
  char host[256], *tmp;

  /* chop off trailing :port */
  strlcpy(host, peer->hostname, sizeof(host));
  tmp = strchr(host, ']');
  if (NULL == tmp)
    tmp = host;			/* not IPv6 [...] format */
  tmp = strchr(tmp, ':');
  if (NULL != tmp)
    *tmp = 0;

// https://wiki.openssl.org/index.php/Hostname_validation
#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  SSL_set1_host(ssl, host);
#elif (OPENSSL_VERSION_NUMBER > 0x1000200fL)
{
  X509_VERIFY_PARAM *param = SSL_get0_param(ssl);
  if (1 != X509_VERIFY_PARAM_set1_host(param, host, strlen(host))) {
      msyslog(LOG_ERR, "NTSc: troubles setting hostflags");
  }
  SSL_set_verify(ssl, SSL_VERIFY_PEER, NULL);
}
#else
  msyslog(LOG_ERR, "NTSc: can't check hostname/certificate");
#endif

  if (NULL == cert) {
    msyslog(LOG_INFO, "NTSc: No certificate");
    if (!(FLAG_NTS_NOVAL & peer->cfg.flags))
      return false;
    return true;
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
      if (!(FLAG_NTS_NOVAL & peer->cfg.flags))
        return false;
    }
  }
  return true;
}

bool nts_make_keys(SSL *ssl, uint16_t aead, uint8_t *c2s, uint8_t *s2c, int keylen) {
  // There is a bug in OpenSSL 1.1.1a
  // Until Mar-23, we were using:
  //    const char *label = "EXPORTER-nts/1";
  // Subject: [Ntp] [NTS4NTP] info for NTS developers
  // From: Martin Langer <mart.langer@ostfalia.de>
  // Date: Tue, 15 Jan 2019 11:40:13 +0100
  // https://mailarchive.ietf.org/arch/msg/ntp/nkc-9n6XOPt5Glgi_ueLvuD9EfY
  const char *label = "EXPORTER-network-time-security/1";
  // FIXME, first 2 bytes, next protocol ID (0)
  unsigned char context[5] = {0x00, 0x00, 0x00, 0x0f, 0x00};
  context[2] = (aead >> 8) & 0xFF;
  context[3] = aead & 0xFF;
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

bool nts_client_send_request(struct peer* peer, SSL *ssl) {
  uint8_t buff[1000];
  int     used, transferred;
  struct  BufCtl_t buf;
  uint16_t aead = NO_AEAD;

  buf.next = buff;
  buf.left = sizeof(buff);

  /* 4.1.2 Next Protocol, 0 for NTP */
  ke_append_record_uint16(&buf, NTS_CRITICAL+nts_next_protocol_negotiation, 0);

  /* 4.1.5 AEAD Algorithm List */
  // FIXME should be : separated list

  if ((NO_AEAD == aead) && (NULL != peer->cfg.nts_cfg.aead))
    aead = nts_string_to_aead(peer->cfg.nts_cfg.aead);
  if ((NO_AEAD == aead) && (NULL != ntsconfig.aead))
    aead = nts_string_to_aead(ntsconfig.aead);
  if (NO_AEAD == aead)
    aead = AEAD_AES_SIV_CMAC_256;
  ke_append_record_uint16(&buf, nts_algorithm_negotiation, aead);

  /* 4.1.1: End, Critical */
  ke_append_record_null(&buf, NTS_CRITICAL+nts_end_of_message);

  used = sizeof(buff)-buf.left;
  if (used >= (int)(sizeof(buff)-10)) {
    msyslog(LOG_ERR, "NTSc: write failed: %d, %ld, %s",
        used, (long)sizeof(buff), strerror(errno));
    return false;
  }
  transferred = SSL_write(ssl, buff, used);
  if (used != transferred) {
    msyslog(LOG_ERR, "NTSc: write failed: %d, %d, %s", used, transferred, strerror(errno));
    nts_log_ssl_error();
    return false;
  }
  return true;
}

bool nts_client_process_response(struct peer* peer, SSL *ssl) {
  uint8_t  buff[2048];  /* RFC 4. says SHOULD be 65K */
  int transferred, idx;
  struct BufCtl_t buf;

  transferred = SSL_read(ssl, buff, sizeof(buff));
  if (0 > transferred) {
    msyslog(LOG_ERR, "NTSc: read failed: %d, %s", transferred, strerror(errno));
    nts_log_ssl_error();
    return false;
  }
  msyslog(LOG_ERR, "NTSc: read %d bytes", transferred);

  peer->nts_state.aead = NO_AEAD;
  peer->nts_state.keylen = 0;
  peer->nts_state.writeIdx = 0;
  peer->nts_state.readIdx = 0;
  peer->nts_state.count = 0;

  buf.next = buff;
  buf.left = transferred;
  while (buf.left > 0) {
    uint16_t type, data;
    bool critical = false;
    int length, keylength;

    type = ke_next_record(&buf, &length);
    if (NTS_CRITICAL & type) {
      critical = true;
      type &= ~NTS_CRITICAL;
    }
    if (0) // Handy for debugging but very verbose
      msyslog(LOG_ERR, "NTSc: Record: T=%d, L=%d, C=%d", type, length, critical);
    switch (type) {
      case nts_error:
        data = next_uint16(&buf);
        if (sizeof(data) != length)
          msyslog(LOG_ERR, "NTSc: wrong length on error: %d", length);
        msyslog(LOG_ERR, "NTSc: error: %d", data);
        return false;
      case nts_next_protocol_negotiation:
        data = next_uint16(&buf);
        if ((sizeof(data) != length) || (data != 0)) {
          msyslog(LOG_ERR, "NTSc: NPN-Wrong length or bad data: %d, %d",
              length, data);
          return false;
        }
        break;
      case nts_algorithm_negotiation:
        data = next_uint16(&buf);
        if (sizeof(data) != length) {
          msyslog(LOG_ERR, "NTSc: AN-Wrong length: %d", length);
          return false;
        }
        keylength = nts_get_key_length(data);
        if (0 == keylength) {
          msyslog(LOG_ERR, "NTSc: AN-Unsupported AEAN type: %d", data);
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
        next_bytes(&buf, (uint8_t*)&peer->nts_state.cookies[idx], length);
        peer->nts_state.writeIdx++;
        peer->nts_state.writeIdx = peer->nts_state.writeIdx % NTS_MAX_COOKIES;
        peer->nts_state.count++;
        break;
      case nts_port_negotiation:
        data = next_uint16(&buf);
        SET_PORT(&sockaddr, data);
        msyslog(LOG_ERR, "NTSc: Using port %d", data);
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
  if (NO_AEAD == peer->nts_state.aead) {
    msyslog(LOG_ERR, "NTSc: No AEAD algorithim.");
    return false;
  }
  if (0 == peer->nts_state.count) {
    msyslog(LOG_ERR, "NTSc: No cookies.");
    return false;
  }

  msyslog(LOG_ERR, "NTSc: Got %d cookies, length %d, aead=%d.",
    peer->nts_state.count, peer->nts_state.cookielen, peer->nts_state.aead);
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
  msyslog(LOG_ERR, "NTSc: can't stat cert dir/file: %s, %s", ntsconfig.ca, strerror(errno));
  return false;
}

/* end */
