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
#include "nts_lib.h"

static int create_listener(int port);
static void* nts_ke_listener(void*);
static void nts_ke_request(SSL *ssl);

int nts_ke_port = 123;

void nts_start_server(void) {
    SSL_CTX *ctx;
    pthread_t worker;
    sigset_t block_mask, saved_sig_mask;
    int rc;

    msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
        nts_ke_port);

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);  // FIXME
    SSL_CTX_set_max_proto_version(ctx, 0);
#else
    /* Older versions of OpenSSL don't support min/max version requests.
     * That's OK, since we don't want anything older than 1.2 and
     * they don't support anything newer.
     * There is similar code in nts_probe(). */
    ctx = SSL_CTX_new(TLSv1_2_server_method());
    SSL_CTX_set_options(ctx, NO_OLD_VERSIONS);
#endif

#if (OPENSSL_VERSION_NUMBER > 0x1010000fL)
    msyslog(LOG_INFO, "NTSs: OpenSSL security level is %d",
        SSL_CTX_get_security_level(ctx));
#endif

    if (1 != SSL_CTX_use_certificate_chain_file(ctx, "/etc/ntp/cert-chain.pem")) {
        // FIXME log SSL errors
        msyslog(LOG_ERR, "NTSs: can't load cert-chain");
    }

    if (1 != SSL_CTX_use_PrivateKey_file(ctx, "/etc/ntp/key.pem", SSL_FILETYPE_PEM)) {
        // FIXME log SSL errors
        msyslog(LOG_ERR, "NTSs: can't load private key");
    }
    if (1 != SSL_CTX_check_private_key(ctx)) {
        msyslog(LOG_ERR, "NTSs: Private Key doesn't work ******");
    } else {
        msyslog(LOG_INFO, "NTSs: Private Key OK");
    }

    sigfillset(&block_mask);
    pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
    rc = pthread_create(&worker, NULL, nts_ke_listener, ctx);
    if (rc) {
      msyslog(LOG_ERR, "NTSs: nts_start_server: error from pthread_create: %m");
    }
    pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

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
        msyslog(LOG_INFO, "NTSs: TCP accept-ed from %s",
            socktoa((sockaddr_u *)&addr));

        /* This could/should go in a new thread. */  // FIXME
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            msyslog(LOG_ERR, "NTSs: SSL accept failed: %m");
            close(client);
            continue;
        }
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
    uint8_t cookie[NTS_COOKIELEN];
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
    keylen = get_key_length(aead);
    nts_make_keys(ssl, c2s, s2c, keylen);

    /* 4.1.2 Next Protocol, 0 for NTP */
    nts_append_record_uint16(&buf, next_protocol_negotiation, 0);
    /* 4.1.5 AEAD Algorithm List */
    nts_append_record_uint16(&buf, algorithm_negotiation, aead);

    for (int i=0; i<NTS_MAX_COOKIES; i++) {
      cookielen = make_cookie(cookie, aead, c2s, s2c, keylen);
      nts_append_record_bytes(&buf, new_cookie, cookie, cookielen);
    }
    
    /* 4.1.1: End, Critical */
    nts_append_record_null(&buf, CRITICAL+end_of_message);
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

int get_key_length(int aead) {
  switch (aead) {
    case IANA_AEAD_AES_SIV_CMAC_256:
      return AEAD_AES_SIV_CMAC_256_KEYLEN;
    default:
      msyslog(LOG_ERR, "NTS: Strange AEAD code: %d", aead);
      return 16;
  }
}

// FIXME - this is a total hack to test pack/unpack
/* returns actual length */
int make_cookie(uint8_t *cookie,
  uint16_t aead,
  uint8_t *c2s, uint8_t *s2c, int keylen) {

  int length = NTS_COOKIELEN/2;

  if (keylen < length)
    length = keylen;
  *cookie = aead & 0xFF;
  for (int i=0; i<length; i++) {
    *cookie++ = *c2s++^*s2c++;
  }

  return length;
}


/* end */
