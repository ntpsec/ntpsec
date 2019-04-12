/*
 * nts_server.c - Network Time Security (NTS) server side support
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 */
#include "config.h"

#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "ntp.h"
#include "ntpd.h"
#include "ntp_stdlib.h"
#include "nts.h"
#include "nts2.h"

/* Beware: bind and accept take type sockaddr, but that's not big
 *         enough for an IPv6 address.
 */

static int create_listener(int port, int family);
static void* nts_ke_listener(void*);
static bool nts_ke_request(SSL *ssl);


static SSL_CTX *server_ctx = NULL;
static int listner4_sock = -1;
static int listner6_sock = -1;

/* Statistics for ntpq */
uint64_t nts_ke_serves_good = 0;
uint64_t nts_ke_serves_bad = 0;
uint64_t nts_ke_probes_good = 0;
uint64_t nts_ke_probes_bad = 0;


bool nts_server_init(void) {
    bool ok = true;

    msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
        NTS_KE_PORT);

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    server_ctx = SSL_CTX_new(TLS_server_method());
#else
    // OpenSSL_add_all_ciphers();  // maybe was needed on NetBSD ??
    server_ctx = SSL_CTX_new(TLSv1_2_server_method());
#endif
    if (NULL == server_ctx) {
      /* Happens if no ciphers */
      msyslog(LOG_INFO, "NTSs: NULL server_ctx");
      nts_log_ssl_error();
      return false;
    }

    SSL_CTX_set_session_cache_mode(server_ctx, SSL_SESS_CACHE_OFF);

    ok &= nts_load_versions(server_ctx);
    ok &= nts_load_ciphers(server_ctx);
    ok &= nts_load_certificate(server_ctx);

    if (!ok) {
      msyslog(LOG_ERR, "NTSs: Disabling NTS-KE server");
      SSL_CTX_free(server_ctx);
      server_ctx = NULL;
      return false;
    };

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    msyslog(LOG_INFO, "NTSs: OpenSSL security level is %d",
        SSL_CTX_get_security_level(server_ctx));
#endif

    listner4_sock = create_listener(NTS_KE_PORT, AF_INET);
    if (listner4_sock < 0) return false;
    listner6_sock = create_listener(NTS_KE_PORT, AF_INET6);
    if (listner6_sock < 0) return false;

    return true;
}

bool nts_server_init2(void) {
    pthread_t worker;
    sigset_t block_mask, saved_sig_mask;
    int rc;
    char errbuf[100];

    sigfillset(&block_mask);
    pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
    rc = pthread_create(&worker, NULL, nts_ke_listener, &listner4_sock);
    if (rc) {
      IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
      msyslog(LOG_ERR, "NTSs: nts_start_server4: error from pthread_create: %s", errbuf);
    }
    rc = pthread_create(&worker, NULL, nts_ke_listener, &listner6_sock);
    if (rc) {
      IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
      msyslog(LOG_ERR, "NTSs: nts_start_server6: error from pthread_create: %s", errbuf);
    }
    pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

    return true;
}

void* nts_ke_listener(void* arg) {
    struct timeval timeout = {.tv_sec = NTS_KE_TIMEOUT, .tv_usec = 0};
    int sock = *(int*)arg;
    char errbuf[100];

    while(1) {
        sockaddr_u addr;
        uint len = sizeof(addr);
        SSL *ssl;
        l_fp start, finish;
        int client, err;

        client = accept(sock, &addr.sa, &len);
        if (client < 0) {
            IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
            msyslog(LOG_ERR, "NTSs: TCP accept failed: %s", errbuf);
            if (EBADF == errno)
                return NULL;
            sleep(1);		/* avoid log clutter on bug */
            continue;
        }
        get_systime(&start);

        sockporttoa_r(&addr, errbuf, sizeof(errbuf));
        msyslog(LOG_INFO, "NTSs: TCP accept-ed from %s", errbuf);
        err = setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        if (0 > err) {
            IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
            msyslog(LOG_ERR, "NTSs: can't setsockopt: %s", errbuf);
            close(client);
            nts_ke_serves_bad++;
            continue;
        }

        /* WARN: For high volume servers, this should go in a new thread. */
        ssl = SSL_new(server_ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            get_systime(&finish);
            finish -= start;
            sockporttoa_r(&addr, errbuf, sizeof(errbuf));
            msyslog(LOG_ERR, "NTSs: SSL accept from %s failed, %.3Lf sec",
                errbuf, lfptod(finish));
            nts_log_ssl_error();
            SSL_free(ssl);
            close(client);
            nts_ke_serves_bad++;
            continue;
        }
        msyslog(LOG_INFO, "NTSs: Using %s, %s (%d)",
            SSL_get_version(ssl),
            SSL_get_cipher_name(ssl),
            SSL_get_cipher_bits(ssl, NULL));


        if (!nts_ke_request(ssl))
	    nts_ke_serves_bad++;

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);

        get_systime(&finish);
        finish -= start;
        nts_ke_serves_good++;
        msyslog(LOG_INFO, "NTSs: NTS-KE server took %.3Lf sec", lfptod(finish));

    }
return NULL;
}

bool nts_ke_request(SSL *ssl) {
    /* RFC 4: servers must accept 1024
     * Our cookies can be 104, 136, or 168 for AES_SIV_CMAC_xxx
     * 8*168 fits comfortably into 2K.
     */
    uint8_t buff[2048];
    uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
    int aead, keylen;
    struct BufCtl_t buf;
    int bytes_read, bytes_written;
    int used;

    bytes_read = nts_ssl_read(ssl, buff, sizeof(buff));
    if (0 > bytes_read)
        return false;

    buf.next = buff;
    buf.left = bytes_read;
    aead = NO_AEAD;
    if (!nts_ke_process_receive(&buf, &aead))
        return false;

    if ((NO_AEAD == aead) && (NULL != ntsconfig.aead))
      aead = nts_string_to_aead(ntsconfig.aead);
    if (NO_AEAD == aead)
      aead = AEAD_AES_SIV_CMAC_256;    /* default */

    keylen = nts_get_key_length(aead);
    if (!nts_make_keys(ssl, aead, c2s, s2c, keylen))
        return false;

    buf.next = buff;
    buf.left = sizeof(buff);
    if (!nts_ke_setup_send(&buf, aead, c2s, s2c, keylen))
        return false;

    used = sizeof(buff)-buf.left;
    bytes_written = nts_ssl_write(ssl, buff, used);
    if (bytes_written != used)
        return false;

    msyslog(LOG_INFO, "NTSs: Read %d, wrote %d bytes.  AEAD=%d",
        bytes_read, bytes_written, aead);

    return true;
}

int create_listener(int port, int family) {
    int sock = -1;
    sockaddr_u addr;
    int on = 1;
    int err;
    char errbuf[100];

    switch (family) {
      case AF_INET:
        addr.sa4.sin_family = AF_INET;
        addr.sa4.sin_port = htons(port);
        addr.sa4.sin_addr.s_addr= htonl(INADDR_ANY);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: Can't create socket4: %s", errbuf);
          return -1;
        }
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (0 > err) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't setsockopt4: %s", errbuf);
	  close(sock);
          return -1;
        }
        err = bind(sock, &addr.sa, sizeof(addr.sa4));
        if (0 > err) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't bind4: %s", errbuf);
	  close(sock);
          return -1;
        }
        if (listen(sock, 6) < 0) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't listen4: %s", errbuf);
	  close(sock);
          return -1;
        }
        msyslog(LOG_INFO, "NTSs: listen4 worked");
        break;
      case AF_INET6:
        addr.sa6.sin6_family = AF_INET6;
        addr.sa6.sin6_port = htons(port);
        addr.sa6.sin6_addr = in6addr_any;
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        if (sock < 0) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: Can't create socket6: %s", errbuf);
          return -1;
        }
        /* Hack to keep IPV6 from listening on IPV4 too */
        err = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
        if (0 > err) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't setsockopt6only: %s", errbuf);
	  close(sock);
          return -1;
        }
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (0 > err) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't setsockopt6: %s", errbuf);
	  close(sock);
          return -1;
        }
        err = bind(sock, &addr.sa, sizeof(addr.sa6));
        if (0 > err) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't bind6: %s", errbuf);
	  close(sock);
          return -1;
        }
        if (listen(sock, 6) < 0) {
          IGNORE(strerror_r(errno, errbuf, sizeof(errbuf)));
          msyslog(LOG_ERR, "NTSs: can't listen6: %s", errbuf);
          close(sock);
          return -1;
        }
        msyslog(LOG_INFO, "NTSs: listen6 worked");
        break;
      default:
        break;
    }

    return sock;
}

bool nts_ke_process_receive(struct BufCtl_t *buf, int *aead) {
    while (buf->left > 0) {
      uint16_t type, data;
      int length;
      bool critical = false;

      type = ke_next_record(buf, &length);
      if (NTS_CRITICAL & type) {
        critical = true;
        type &= ~NTS_CRITICAL;
      }
      if (0) // Handy for debugging but very verbose
        msyslog(LOG_ERR, "NTSs: Record: T=%d, L=%d, C=%d", type, length, critical);
      switch (type) {
        case nts_error:
          data = next_uint16(buf);
          if (sizeof(data) != length)
            msyslog(LOG_ERR, "NTSs: wrong length on error: %d", length);
          msyslog(LOG_ERR, "NTSs: error: %d", data);
          return false;
        case nts_next_protocol_negotiation:
          data = next_uint16(buf);
          if ((sizeof(data) != length) || (data != nts_protocol_NTP)) {
            msyslog(LOG_ERR, "NTSs: NPN-Wrong length or bad data: %d, %d",
                length, data);
            return false;
          }
          break;
        case nts_algorithm_negotiation:
          for (int i=0; i<length; i+=sizeof(uint16_t)) {
            data = next_uint16(buf);
            if (0 == nts_get_key_length(data)) {
              if (0)  /* for debugging */
                msyslog(LOG_ERR, "NTSs: AN-Unsupported AEAN type: %d", data);
              continue;     /* ignore types we don't support */
            }
            if (*aead != NO_AEAD)
              continue;     /* already got one */
            *aead = data;   /* take this one */
          }
          break;
        case nts_end_of_message:
          if ((0 != length) || !critical) {
            msyslog(LOG_ERR, "NTSs: EOM-Wrong length or not Critical: %d, %d",
                length, critical);
            return false;
          }
          if (0 != buf->left) {
            msyslog(LOG_ERR, "NTSs: EOM not at end: %d", buf->left);
            return false;
          }
          break;
        default:
          msyslog(LOG_ERR, "NTSs: received strange type: T=%d, C=%d, L=%d",
            type, critical, length);
          if (critical) return false;
          buf->next += length;
          buf->left -= length;
          break;
      } /* case */
    }   /* while */

  return true;

}

bool nts_ke_setup_send(struct BufCtl_t *buf, int aead,
       uint8_t *c2s, uint8_t *s2c, int keylen) {

    /* 4.1.2 Next Protocol */
    ke_append_record_uint16(buf,
      NTS_CRITICAL+nts_next_protocol_negotiation, nts_protocol_NTP);
    /* 4.1.5 AEAD Algorithm List */
    ke_append_record_uint16(buf, nts_algorithm_negotiation, aead);

    for (int i=0; i<NTS_MAX_COOKIES; i++) {
      uint8_t cookie[NTS_MAX_COOKIELEN];
      int cookielen = nts_make_cookie(cookie, aead, c2s, s2c, keylen);
      ke_append_record_bytes(buf, nts_new_cookie, cookie, cookielen);
    }

    /* 4.1.1: End, Critical */
    ke_append_record_null(buf, NTS_CRITICAL+nts_end_of_message);

    return true;

}

/* end */
