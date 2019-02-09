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

int create_listener(int port);
void* nts_ke_listener(void*);
void nts_ke_request(SSL *ssl);

void nts_start_server(void) {
    SSL_CTX *ctx;
    pthread_t worker;
    sigset_t block_mask, saved_sig_mask;
    int rc;

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
    if (1) // FIXME if (non-default version request)
      msyslog(LOG_INFO, "NTSc: can't set min/max TLS versions.");
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

void nts_ke_request(SSL *ssl);
void* nts_ke_listener(void* arg) {
    SSL_CTX *ctx = (SSL_CTX *)arg;
    int sock;

    sock = create_listener(123);
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
    size_t bytes_read, bytes_written;
    uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
    int keylen = AEAD_AES_SIV_CMAC_256_KEYLEN;


    bytes_read = SSL_read(ssl, buff, sizeof(buff));
    if (0 >= bytes_read) {
        msyslog(LOG_INFO, "NTSs: SSL_read error");
        return;
    }

    // Hack, echo it back
    bytes_written = SSL_write(ssl, buff, bytes_read);
    if (bytes_written != bytes_read) {
        msyslog(LOG_INFO, "NTSs: SSL_write error");
        return;
    }

    if (!nts_make_keys(ssl, c2s, s2c, keylen))
      return;

}

int create_listener(int port)
{
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


/* end */
