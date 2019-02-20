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

/* default file names */
#define NTS_CERT_FILE "/etc/ntp/cert-chain.pem"
#define NTS_KEY_FILE "/etc/ntp/key.pem"

int nts_ke_port = 123;

static bool nts_load_certificate(SSL_CTX *ctx);
static int create_listener(int port);
static void* nts_ke_listener(void*);
static void nts_ke_request(SSL *ssl);
static int nts_translate_version(const char *arg);

static SSL_CTX *server_ctx = NULL;

void nts_init(void) {
    bool ok = true;
    ok &= nts_cookie_init();
    if (ntsconfig.ntsenable)
        ok &= nts_server_init();
    ok &= nts_client_init();
    ok &= extens_init();
    if (!ok) {
      msyslog(LOG_ERR, "NTS: troubles during init.  Bailing.");
      exit(1);
    }
}

bool nts_server_init(void) {
    pthread_t worker;
    sigset_t block_mask, saved_sig_mask;
    int rc;
    bool ok = true;

    msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
        nts_ke_port);

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    server_ctx = SSL_CTX_new(TLS_server_method());
#else
    OpenSSL_add_all_ciphers();  // FIXME needed on NetBSD
    server_ctx = SSL_CTX_new(TLSv1_2_server_method());
#endif
    if (NULL == server_ctx) {
      // ?? Happens on NetBSD - says no ciphers
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

    sigfillset(&block_mask);
    pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
    rc = pthread_create(&worker, NULL, nts_ke_listener, server_ctx);
    if (rc) {
      msyslog(LOG_ERR, "NTSs: nts_start_server: error from pthread_create: %m");
    }
    pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

    return true;
}

void* nts_ke_listener(void* arg) {
    SSL_CTX *ctx = (SSL_CTX *)arg;
    int sock;

    // FIXME - need IPv6 too
    sock = create_listener(nts_ke_port);
    if (sock < 0) return NULL;

    while(1) {
        struct sockaddr addr;
        uint len = sizeof(addr);
        SSL *ssl;

        int client = accept(sock, &addr, &len);
        if (client < 0) {
            msyslog(LOG_ERR, "NTSs: TCP accept failed: %m");
            continue;
        }
	nts_ke_serves++;
        msyslog(LOG_INFO, "NTSs: TCP accept-ed from %s",
            socktoa((sockaddr_u *)&addr));

        /* This could/should go in a new thread. */  // FIXME
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            msyslog(LOG_ERR, "NTSs: SSL accept failed");
            nts_log_ssl_error();
            close(client);
            continue;
        }
        SSL_set_timeout(SSL_get_session(ssl), 2);  // FIXME
        msyslog(LOG_INFO, "NTSs: SSL accept-ed from %s",
            socktoa((sockaddr_u *)&addr));
        msyslog(LOG_INFO, "NTSs: Using TLS version %s, cipher %s with %d secret bits",
            SSL_get_version(ssl),
            SSL_get_cipher_name(ssl),
            SSL_get_cipher_bits(ssl, NULL));


        nts_ke_request(ssl);

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }
}

void nts_ke_request(SSL *ssl) {
    uint8_t buff[1000];
    int bytes_read, bytes_written;
    uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
    uint8_t cookie[NTS_MAX_COOKIELEN];
    int aead, keylen, cookielen;
    struct BufCtl_t buf;
    int used;

    bytes_read = SSL_read(ssl, buff, sizeof(buff));
    if (0 >= bytes_read) {
        msyslog(LOG_INFO, "NTSs: SSL_read error");
        return;
    }

    // FIXME Ignore request for now
    aead = IANA_AEAD_AES_SIV_CMAC_256;

    buf.next = buff;
    buf.left = sizeof(buff);
    keylen = nts_get_key_length(aead);
    nts_make_keys(ssl, c2s, s2c, keylen);

    /* 4.1.2 Next Protocol, 0 for NTP */
    nts_append_record_uint16(&buf, NTS_CRITICAL+nts_next_protocol_negotiation, 0);
    /* 4.1.5 AEAD Algorithm List */
    nts_append_record_uint16(&buf, nts_algorithm_negotiation, aead);

    for (int i=0; i<NTS_MAX_COOKIES; i++) {
      cookielen = nts_make_cookie(cookie, aead, c2s, s2c, keylen);
      nts_append_record_bytes(&buf, nts_new_cookie, cookie, cookielen);
    }
    
    /* 4.1.1: End, Critical */
    nts_append_record_null(&buf, NTS_CRITICAL+nts_end_of_message);
    used = sizeof(buff)-buf.left;

    bytes_written = SSL_write(ssl, buff, used);
    if (bytes_written != used) {
        msyslog(LOG_INFO, "NTSs: SSL_write error");
        return;
    }

    msyslog(LOG_INFO, "NTSs: Returned %d bytes", bytes_written);
    
    return;
}

int create_listener(int port) {
    int sock;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        msyslog(LOG_ERR, "NTSs: Can't create socket: %m");
        return -1;
    }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        msyslog(LOG_ERR, "NTSs: can't bind: %m");
        return -1;
    }

    if (listen(sock, 1) < 0) {
        msyslog(LOG_ERR, "NTSs: can't listen: %m");
        return -1;
    }
    return sock;
}

/* returns key length, 0 if unknown arg */
int nts_get_key_length(int aead) {
  switch (aead) {
    case IANA_AEAD_AES_SIV_CMAC_256:
      return AEAD_AES_SIV_CMAC_256_KEYLEN;
    case IANA_AEAD_AES_SIV_CMAC_384:
      return AEAD_AES_SIV_CMAC_384_KEYLEN;
    case IANA_AEAD_AES_SIV_CMAC_512:
      return AEAD_AES_SIV_CMAC_512_KEYLEN;
    default:
      msyslog(LOG_ERR, "NTS: Strange AEAD code: %d", aead);
      return 0;
  }
}

bool nts_load_versions(SSL_CTX *ctx) {
  int minver, maxver;
  minver = nts_translate_version(ntsconfig.mintls);
  maxver = nts_translate_version(ntsconfig.maxtls);
  if ((-1 == minver) || (-1 == maxver))
    return false;
#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
  if(0 == minver) minver = TLS1_2_VERSION;   // 3.
  SSL_CTX_set_min_proto_version(ctx, minver);
  SSL_CTX_set_max_proto_version(ctx, maxver);
#else
  /* Older versions of OpenSSL don't support min/max version requests.
   * That's OK, since we don't want anything older than 1.2 and
   * they don't support anything newer. */
  SSL_CTX_set_options(ctx, NO_OLD_VERSIONS);
#endif
  return true;
}

/* 0 is default, -1 is error */
int nts_translate_version(const char *arg) {
  if (NULL == arg)
    return 0;
  if (0 == strcmp(arg, "TLS1.2"))
    return TLS1_2_VERSION;
  if (0 == strcmp(arg, "TLS1.3")) {
#ifdef TLS1_3_VERSION
    return TLS1_3_VERSION;
#else
    msyslog(LOG_ERR, "NTS: TLS1.3 not supported by this version of OpenSSL.");
    return -1;
#endif
  }
  msyslog(LOG_ERR, "NTS: unreconized version string: %s.", arg);
  return -1;
}

bool nts_load_ciphers(SSL_CTX *ctx) {
  /* SSL set_ciphers(uites) ignores typos or ciphers it doesn't support.
   * There is no SSL_CTX_get_cipher_list, so we can't easily read back
   * the ciphers to see what it took.
   * We could make a dummy SSL, read the list, then free it.
   */
  if (NULL != ntsconfig.tlsciphers) {
    if (1 != SSL_CTX_set_cipher_list(ctx, ntsconfig.tlsciphers)) {
      msyslog(LOG_ERR, "NTS: troubles setting ciphers.");
      return false;
    } else {
      msyslog(LOG_INFO, "NTS: set ciphers.");
    }
  }
  if (NULL != ntsconfig.tlsciphersuites) {
#ifdef TLS1_3_VERSION
    if (1 != SSL_CTX_set_ciphersuites(ctx, ntsconfig.tlsciphersuites)) {
      msyslog(LOG_ERR, "NTS: troubles setting ciphersuites.");
      return false;
    } else {
      msyslog(LOG_INFO, "NTS: set ciphersuites.");
    }
#else
    msyslog(LOG_ERR, "NTS: ciphersuites not supported on this version of OpenSSL.");
#endif
  }
  return true;
}

bool nts_load_certificate(SSL_CTX *ctx) {
    const char *cert = NTS_CERT_FILE;
    const char *key = NTS_KEY_FILE;

    if (NULL != ntsconfig.cert)
       cert = ntsconfig.cert;
    if (NULL != ntsconfig.key)
       key = ntsconfig.key;

    if (1 != SSL_CTX_use_certificate_chain_file(ctx, cert)) {
        // FIXME log SSL errors
        msyslog(LOG_ERR, "NTSs: can't load certificate (chain) from %s", cert);
        return false;
    } else {
        msyslog(LOG_ERR, "NTSs: loaded certificate (chain) from %s", cert);
    }

    if (1 != SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM)) {
        // FIXME log SSL errors
        msyslog(LOG_ERR, "NTSs: can't load private key from %s", key);
        return false;
    } else {
        msyslog(LOG_ERR, "NTSs: loaded private key from %s", key);
    }

    if (1 != SSL_CTX_check_private_key(ctx)) {
        msyslog(LOG_ERR, "NTSs: Private Key doesn't work ******");
        return false;
    } else {
        msyslog(LOG_INFO, "NTSs: Private Key OK");
    }
    return true;
}

/* end */
