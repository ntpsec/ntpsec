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


static int create_listener(int port, int family);
static void* nts_ke_listener(void*);
static bool nts_ke_request(SSL *ssl);


static SSL_CTX *server_ctx = NULL;
static int listner4_sock = -1;
static int listner6_sock = -1;

/* Statistics for ntpq */
uint64_t nts_ke_serves = 0;
uint64_t nts_ke_serves_bad = 0;
uint64_t nts_ke_probes = 0;
uint64_t nts_ke_probes_bad = 0;


bool nts_server_init(void) {
    bool ok = true;

    msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
        NTS_KE_PORT);

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    server_ctx = SSL_CTX_new(TLS_server_method());
#else
    OpenSSL_add_all_ciphers();  // FIXME needed on NetBSD
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

    sigfillset(&block_mask);
    pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
    rc = pthread_create(&worker, NULL, nts_ke_listener, &listner4_sock);
    if (rc) {
      msyslog(LOG_ERR, "NTSs: nts_start_server4: error from pthread_create: %s", strerror(errno));
    }
    rc = pthread_create(&worker, NULL, nts_ke_listener, &listner6_sock);
    if (rc) {
      msyslog(LOG_ERR, "NTSs: nts_start_server6: error from pthread_create: %s", strerror(errno));
    }
    pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

    return true;
}

void* nts_ke_listener(void* arg) {
    struct timeval timeout = {.tv_sec = NTS_KE_TIMEOUT, .tv_usec = 0};
    int sock = *(int*)arg;

    while(1) {
        struct sockaddr addr;
        uint len = sizeof(addr);
        SSL *ssl;
        l_fp start, finish;

        int client = accept(sock, &addr, &len);
        if (client < 0) {
            msyslog(LOG_ERR, "NTSs: TCP accept failed: %s", strerror(errno));
            if (EBADF == errno)
                return NULL;
            sleep(1);		/* avoid log clutter on bug */
            continue;
        }
	nts_ke_serves++;
        get_systime(&start);
        msyslog(LOG_INFO, "NTSs: TCP accept-ed from %s",
            sockporttoa((sockaddr_u *)&addr));
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        /* For high volume servers, this should go in a new thread. */
        ssl = SSL_new(server_ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            get_systime(&finish);
            finish -= start;
            msyslog(LOG_ERR, "NTSs: SSL accept from %s failed, %.3Lf sec",
                sockporttoa((sockaddr_u *)&addr), lfptod(finish));
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
        msyslog(LOG_INFO, "NTSs: NTS-KE server took %.3Lf sec", lfptod(finish));

    }
return NULL;
}

bool nts_ke_request(SSL *ssl) {
    /* buff is used for both read and write.
     * RFC 4: servers must accept 1024
     * cookies can be 104, 136, or 168 for IANA_AEAD_AES_SIV_CMAC_xxx
     * 8*104 fits in 1K.  With 168, we only get 5.
     * 8*168 fits comfortably into 2K.
     */
    uint8_t buff[2048];
    int bytes_read, bytes_written;
    uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
    uint8_t cookie[NTS_MAX_COOKIELEN];
    int aead, keylen, cookielen;
    struct BufCtl_t buf;
    int used;

    bytes_read = SSL_read(ssl, buff, sizeof(buff));
    if (0 >= bytes_read) {
        msyslog(LOG_INFO, "NTSs: SSL_read error: %s", strerror(errno));
        nts_log_ssl_error();
        return false;
    }

    // FIXME Ignore request for now
    aead = IANA_AEAD_AES_SIV_CMAC_256;

    buf.next = buff;
    buf.left = sizeof(buff);
    keylen = nts_get_key_length(aead);
    nts_make_keys(ssl, aead, c2s, s2c, keylen);

    /* 4.1.2 Next Protocol, 0 for NTP */
    ke_append_record_uint16(&buf, NTS_CRITICAL+nts_next_protocol_negotiation, 0);
    /* 4.1.5 AEAD Algorithm List */
    ke_append_record_uint16(&buf, nts_algorithm_negotiation, aead);

    for (int i=0; i<NTS_MAX_COOKIES; i++) {
      cookielen = nts_make_cookie(cookie, aead, c2s, s2c, keylen);
      ke_append_record_bytes(&buf, nts_new_cookie, cookie, cookielen);
    }

    /* 4.1.1: End, Critical */
    ke_append_record_null(&buf, NTS_CRITICAL+nts_end_of_message);
    used = sizeof(buff)-buf.left;

    bytes_written = SSL_write(ssl, buff, used);
    if (bytes_written != used) {
        msyslog(LOG_INFO, "NTSs: SSL_write error: %s", strerror(errno));
        nts_log_ssl_error();
        return false;
    }

    msyslog(LOG_INFO, "NTSs: Returned %d bytes", bytes_written);

    return true;
}

int create_listener(int port, int family) {
    int sock = -1;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int on = 1;
    int err;

    switch (family) {
      case AF_INET:
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr= htonl(INADDR_ANY);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
          msyslog(LOG_ERR, "NTSs: Can't create socket4: %s", strerror(errno));
          return -1;
        }
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (0 > err) {
          msyslog(LOG_ERR, "NTSs: can't setsockopt4: %s", strerror(errno));
	  close(sock);
          return -1;
        }
        err = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
        if (0 > err) {
          msyslog(LOG_ERR, "NTSs: can't bind4: %s", strerror(errno));
	  close(sock);
          return -1;
        }
        if (listen(sock, 6) < 0) {
          msyslog(LOG_ERR, "NTSs: can't listen4: %s", strerror(errno));
	  close(sock);
          return -1;
        }
        msyslog(LOG_INFO, "NTSs: listen4 worked");
        break;
      case AF_INET6:
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        addr6.sin6_addr = in6addr_any;
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        if (sock < 0) {
          msyslog(LOG_ERR, "NTSs: Can't create socket6: %s", strerror(errno));
          return -1;
        }
        /* Hack to keep IPV6 from listening on IPV4 too */
        err = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
        if (0 > err) {
          msyslog(LOG_ERR, "NTSs: can't setsockopt6only: %s", strerror(errno));
	  close(sock);
          return -1;
        }
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (0 > err) {
          msyslog(LOG_ERR, "NTSs: can't setsockopt6: %s", strerror(errno));
	  close(sock);
          return -1;
        }
        err = bind(sock, (struct sockaddr*)&addr6, sizeof(addr6));
        if (0 > err) {
          msyslog(LOG_ERR, "NTSs: can't bind6: %s", strerror(errno));
	  close(sock);
          return -1;
        }
        if (listen(sock, 6) < 0) {
          msyslog(LOG_ERR, "NTSs: can't listen6: %s", strerror(errno));
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


/* end */
