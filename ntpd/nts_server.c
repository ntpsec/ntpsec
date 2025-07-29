/*
 * nts_server.c - Network Time Security (NTS) server side support
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to
 * https://tools.ietf.org/html/rfc8915
 *
 */
#include "config.h"

#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include "ntp.h"
#include "ntpd.h"
#include "ntp_stdlib.h"
#include "nts.h"
#include "nts2.h"
#include "timespecops.h"

/* Beware: bind and accept take type sockaddr, but that's not big
 *         enough for an IPv6 address.
 */


static bool create_listener4(int port);
static bool create_listener6(int port);
static void* nts_ke_listener(void*);
static void nts_ke_request(SSL *ssl, const char **errtxt);
static void nts_ke_accept_fail(char* hostname, double sec, int code);

static void nts_lock_certlock(void);
static void nts_unlock_certlock(void);


static SSL_CTX *server_ctx = NULL;
static int listener4_sock = -1;
static int listener6_sock = -1;

static void nts_ke_setup_send(struct BufCtl_t *buf, int aead,
       uint8_t *c2s, uint8_t *s2c, int keylen);

/* We need a lock to protect reloading our certificate.
 * This seems like overkill, but it doesn't happen often. */
pthread_mutex_t certificate_lock = PTHREAD_MUTEX_INITIALIZER;

static int alpn_select_cb(SSL *ssl,
			  const unsigned char **out,
			  unsigned char *outlen,
			  const unsigned char *in,
			  unsigned int inlen,
			  void *arg)
{
	static const unsigned char alpn[] = {
            'n', 't', 's', 'k', 'e', '/', '1' };
	unsigned i, len;

	UNUSED_ARG(ssl);
	UNUSED_ARG(arg);

	for (i = 0; i < inlen; i += len+1) {
		len = in[i]; // first byte is the length
#if 0
		char foo[256];
		strlcpy(foo, (const char*)in+i+1, len);
		msyslog(LOG_DEBUG, "DEBUG: alpn_select_cb:  %u, %u, %s", inlen-i, len, foo);
#endif
		if (len+1 > inlen-i)
			/* bogus arg: length overlaps end of in buffer */
			return SSL_TLSEXT_ERR_ALERT_FATAL;
		if (len == sizeof(alpn) && !memcmp(in+i+1, alpn, len)) {
			*out = in+i+1;
			*outlen = len;
			return SSL_TLSEXT_ERR_OK;
		}
	}

	return SSL_TLSEXT_ERR_NOACK;
}

bool nts_server_init(void) {
	bool ok = true;

	msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
		NTS_KE_PORT);

	server_ctx = SSL_CTX_new(TLS_server_method());
	if (NULL == server_ctx) {
		/* Happens if no ciphers */
		msyslog(LOG_INFO, "NTSs: NULL server_ctx");
		nts_log_ssl_error();
		return false;
	}

	SSL_CTX_set_alpn_select_cb(server_ctx, alpn_select_cb, NULL);
	SSL_CTX_set_session_cache_mode(server_ctx, SSL_SESS_CACHE_OFF);
	SSL_CTX_set_timeout(server_ctx, NTS_KE_TIMEOUT);  /* session lifetime */

	ok &= nts_load_versions(server_ctx);
	ok &= nts_load_ciphers(server_ctx);
	ok &= nts_load_ecdhcurves(server_ctx);
	ok &= nts_set_cipher_order(server_ctx);

	if (!ok) {
		msyslog(LOG_ERR, "NTSs: Disabling NTS-KE server");
		SSL_CTX_free(server_ctx);
		server_ctx = NULL;
		return false;
	};

	msyslog(LOG_INFO, "NTSs: OpenSSL security level is %d",
		SSL_CTX_get_security_level(server_ctx));

	msyslog(LOG_INFO, "NTSs: starting NTS-KE server listening on port %d",
		NTS_KE_PORT);
	ok &= create_listener4(NTS_KE_PORT);
	ok &= create_listener6(NTS_KE_PORT);

	return ok;
}

bool nts_server_init2(void) {
	pthread_t worker;
	sigset_t block_mask, saved_sig_mask;
	int rc;
	char errbuf[100];

	if (!nts_load_certificate(server_ctx)) {
		return false;
	}

	sigfillset(&block_mask);
	pthread_sigmask(SIG_BLOCK, &block_mask, &saved_sig_mask);
	if (listener4_sock != -1) {
		rc = pthread_create(&worker, NULL, nts_ke_listener, &listener4_sock);
		if (rc) {
			ntp_strerror_r(errno, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSs: nts_start_server4: error from pthread_create: %s", errbuf);
		}
	}
	if (listener6_sock != -1) {
		rc = pthread_create(&worker, NULL, nts_ke_listener, &listener6_sock);
		if (rc) {
			ntp_strerror_r(errno, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSs: nts_start_server6: error from pthread_create: %s", errbuf);
		}
	}
	pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);

	return true;
}

/* called every hour */
void nts_cert_timer(void) {
	check_cert_file();
}

/* call hourly and by SIGHUP */
void check_cert_file(void) {
	if (NULL == server_ctx)
		return;
	nts_lock_certlock();
	nts_reload_certificate(server_ctx);
	nts_unlock_certlock();
}

void nts_lock_certlock(void) {
	int err = pthread_mutex_lock(&certificate_lock);
	if (0 != err) {
		msyslog(LOG_ERR, "ERR: Can't lock certificate_lock: %d", err);
		exit(2);
	}
}

void nts_unlock_certlock(void) {
	int err = pthread_mutex_unlock(&certificate_lock);
	if (0 != err) {
		msyslog(LOG_ERR, "ERR: Can't unlock certificate_lock: %d", err);
		exit(2);
	}
}

/* lfptod goes to long double */
static inline double lfptox(l_fp r) {
/* l_fp to double */
        return ldexp((double)((int64_t)r), -32);
}


void* nts_ke_listener(void* arg) {
	struct timeval timeout = {.tv_sec = NTS_KE_TIMEOUT, .tv_usec = 0};
	int sock = *(int*)arg;
	char errbuf[100];
	char addrbuf[100];
	char usingbuf[100];
	struct timespec start, finish;		/* wall clock */
	l_fp wall;
	const char *errtxt;	/* not NULL if error */
#ifdef RUSAGE_THREAD
	struct timespec start_u, finish_u;	/* CPU user */
	struct timespec start_s, finish_s;	/* CPU system */
	l_fp usr, sys;
	struct rusage usage;
#endif

#ifdef HAVE_SECCOMP_H
        setup_SIGSYS_trap();   /* enable trap for this thread */
#endif

#ifdef RUSAGE_THREAD
	/* NB: start_u and start_s are from near the end of the previous cycle.
	 * Thus usage timing includes the TCP accept and
	 * writing the previous msyslog message.
	 */
	getrusage(RUSAGE_THREAD, &usage);
	start_u = tval_to_tspec(usage.ru_utime);
	start_s = tval_to_tspec(usage.ru_stime);
#endif

	while(1) {
		sockaddr_u addr;
		socklen_t len = sizeof(addr);
		SSL *ssl;
		int client, err;

		sleep(1);			/* FIXME: log clutter/DoS */
		errtxt = NULL;
		client = accept(sock, &addr.sa, &len);
		if (client < 0) {
			ntp_strerror_r(errno, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSs: TCP accept failed: %s", errbuf);
			if (EBADF == errno)
				return NULL;
			continue;
		}
		clock_gettime(CLOCK_MONOTONIC, &start);
		sockporttoa_r(&addr, addrbuf, sizeof(addrbuf));

/* This is disabled in order to reduce clutter in the log file.
 * The client's address is now included in the final message.
 * That works fine in the normal successful case.  There is one line
 * per connection.
 * The failed cases are more complicated.
 * The common fail case is bad guys probing which fails in SSL_accept.
 * That branch has its own error handling.  Again, the common cases
 * have one line per connection and include the client address.
 * Uncommon cases will include two (or more) lines.
 * There are many possible error messages after SSL_accept works.
 * In practice, they don't happen, at least not often enough to notice.
 * They currently get logged without the client's address.  Then they
 * fall into the normal (non-error) path which does include the address.
 * Enabling this might make strange cases easier to understand.
 */
/*		msyslog(LOG_INFO, "NTSs: TCP accept-ed from %s", addrbuf); */

		err = setsockopt(client, SOL_SOCKET, SO_RCVTIMEO,
			&timeout, sizeof(timeout));
		if (0 > err) {
			ntp_strerror_r(errno, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSs: can't set recv timeout: %s", errbuf);
			close(client);
			ntske_cnt.serves_bad++;
			continue;
		}
		err = setsockopt(client, SOL_SOCKET, SO_SNDTIMEO,
			&timeout, sizeof(timeout));
		if (0 > err) {
			ntp_strerror_r(errno, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSs: can't set send timeout: %s", errbuf);
			close(client);
			ntske_cnt.serves_bad++;
			continue;
		}

		/* WARN: For high volume servers, this should go in a new thread. */
		nts_lock_certlock();
		ssl = SSL_new(server_ctx);
		nts_unlock_certlock();
		SSL_set_fd(ssl, client);

		err = SSL_accept(ssl);
		if (0 >= err) {
			int code = SSL_get_error(ssl, err);
			clock_gettime(CLOCK_MONOTONIC, &finish);
			wall = tspec_intv_to_lfp(sub_tspec(finish, start));
			nts_ke_accept_fail(addrbuf, lfptox(wall), code);
			SSL_free(ssl);
			close(client);
			ntske_cnt.serves_nossl++;
			ntske_cnt.serves_nossl_wall += wall;
#ifdef RUSAGE_THREAD
			getrusage(RUSAGE_THREAD, &usage);
			finish_u = tval_to_tspec(usage.ru_utime);
			finish_s = tval_to_tspec(usage.ru_stime);
			usr = tspec_intv_to_lfp(sub_tspec(finish_u, start_u));
			sys = tspec_intv_to_lfp(sub_tspec(finish_s, start_s));
			start_u = finish_u;
			start_s = finish_s;
			ntske_cnt.serves_nossl_cpu += usr;
			ntske_cnt.serves_nossl_cpu += sys;
#endif
			continue;
		}

		/* Save info for final message. */
		snprintf(usingbuf, sizeof(usingbuf), "%s, %s (%d)",
			SSL_get_version(ssl),
			SSL_get_cipher_name(ssl),
			SSL_get_cipher_bits(ssl, NULL));

		nts_ke_request(ssl, &errtxt);

		SSL_shutdown(ssl);
		SSL_free(ssl);
		close(client);

		clock_gettime(CLOCK_MONOTONIC, &finish);
		wall = tspec_intv_to_lfp(sub_tspec(finish, start));
		if (NULL == errtxt) {
			ntske_cnt.serves_good++;
			ntske_cnt.serves_good_wall += wall;
		} else {
			ntske_cnt.serves_bad++;
			ntske_cnt.serves_bad_wall += wall;
		}
#ifdef RUSAGE_THREAD
		getrusage(RUSAGE_THREAD, &usage);
		finish_u = tval_to_tspec(usage.ru_utime);
		finish_s = tval_to_tspec(usage.ru_stime);
		usr = tspec_intv_to_lfp(sub_tspec(finish_u, start_u));
		sys = tspec_intv_to_lfp(sub_tspec(finish_s, start_s));
		start_u = finish_u;
		start_s = finish_s;
		if (NULL == errtxt) {
			ntske_cnt.serves_good_cpu += usr;
			ntske_cnt.serves_good_cpu += sys;
			msyslog(LOG_INFO,
				"NTSs: NTS-KE from %s, OK, Using %s, took %.3f sec, CPU: %.3f+%.3f ms",
				addrbuf, usingbuf, lfptox(wall),
				lfptox(usr*1000), lfptox(sys*1000));
		} else {
			ntske_cnt.serves_bad_cpu += usr;
			ntske_cnt.serves_bad_cpu += sys;
			msyslog(LOG_INFO,
				"NTSs: NTS-KE from %s, Failed, Using %s, took %.3f sec, CPU: %.3f+%.3f ms, %s",
				addrbuf, usingbuf, lfptox(wall),
				lfptox(usr*1000), lfptox(sys*1000),
				errtxt);
		}
#else
		if (NULL == errtxt) {
			msyslog(LOG_INFO,
				"NTSs: NTS-KE from %s, OK, Using %s, took %.3f sec",
				addrbuf, usingbuf, lfptox(wall));
		} else {
			msyslog(LOG_INFO,
				"NTSs: NTS-KE from %s, Failed, Using %s, took %.3f sec, %s",
				addrbuf, usingbuf, lfptox(wall), errtxt);
		}
#endif
	}
	return NULL;
}

/* Analyze failure from SSL_accept
 * print single error message for common cases.
 * Similar code in nts.c, nts_ssl_read() and nts_ssl_write()
 */
void nts_ke_accept_fail(char* hostname, double sec, int code) {
	unsigned long err = ERR_peek_error();
	char errbuf[100];
	char buff[200];
	const char *msg = NULL;
	if (0 == err) {
	  switch (code) {
	    case SSL_ERROR_WANT_READ:
	      msg = "Timeout";
	      break;
	    case SSL_ERROR_SYSCALL:
	      if (ECONNRESET==errno) {
	        msg = "Connection reset1";
	        break;
	      }
	      /* fall through */
	    default:
	      ntp_strerror_r(errno, errbuf, sizeof(errbuf));
	      snprintf(buff, sizeof(buff), "code %d, errno=>%d, %s",
		code, errno, errbuf);
	      msg = buff;
	      break;
	  }
	} else {
	  switch (code) {
	    case SSL_ERROR_SSL:
	      switch (ERR_GET_REASON(err)) {
		case SSL_R_HTTP_REQUEST:
		  msg = "HTTP request";
		  break;
		case SSL_R_NO_SHARED_CIPHER:
                  msg = "no shared cipher";
                  break;
		case SSL_R_WRONG_VERSION_NUMBER:
                  msg = "wrong version number";
                  break;
		case SSL_R_UNSUPPORTED_PROTOCOL:
                  msg = "unsupported protocol";
                  break;
#ifdef SSL_R_UNEXPECTED_EOF_WHILE_READING
/* Not available in OpenSSL 1.1.1w as used by Debian 11 (bullseye), Jul 2025 */
	        case SSL_R_UNEXPECTED_EOF_WHILE_READING:
		  msg = "Connection reset2";
                  break;
#endif
		default:
		  // cc (Debian 8.3.0-6) 8.3.0
		  //   error: label at end of compound statement
		  NULL;
	        /* fall through */
	      }
	      if (NULL != msg) {
		err = 0;
	        break;
	      }
	      /* fall through */
	    default:
	      ntp_strerror_r(errno, errbuf, sizeof(errbuf));
	      snprintf(buff, sizeof(buff), "code %d, errno=>%d, %s, %lx=>%s",
		code, errno, errbuf, err, ERR_reason_error_string(err));
	      msg = buff;
	  }
	}
	msyslog(LOG_INFO, "NTSs: SSL_accept from %s, Failed, took %.3f sec, %s",
		hostname, sec, msg);
	if (err)
	    nts_log_ssl_error();
}

void nts_ke_request(SSL *ssl, const char **errtxt) {
	/* RFC 4: servers must accept 1024
	 * Our cookies can be 104, 136, or 168 for AES_SIV_CMAC_xxx
	 * 8*168 fits comfortably into 2K.
	 */
	uint8_t buff[2048];
	uint8_t c2s[NTS_MAX_KEYLEN], s2c[NTS_MAX_KEYLEN];
	int aead = NO_AEAD, keylen;
	struct BufCtl_t buf;
	int bytes_read, bytes_written;
	int used;

	bytes_read = nts_ssl_read(ssl, buff, sizeof(buff), errtxt);
	if (0 >= bytes_read)
		return;

	buf.next = buff;
	buf.left = bytes_read;
	if (!nts_ke_process_receive(&buf, &aead)) {
		*errtxt = "xx";
		return;
	}

	if ((NO_AEAD == aead) && (NULL != ntsconfig.aead))
		aead = nts_string_to_aead(ntsconfig.aead);
	if (NO_AEAD == aead)
		aead = AEAD_AES_SIV_CMAC_256;    /* default */

	keylen = nts_get_key_length(aead);
	if (!nts_make_keys(ssl, aead, c2s, s2c, keylen)) {
		*errtxt = "Can't make keys";
		return;
	}

	buf.next = buff;
	buf.left = sizeof(buff);
	nts_ke_setup_send(&buf, aead, c2s, s2c, keylen);

	used = sizeof(buff)-buf.left;
	bytes_written = nts_ssl_write(ssl, buff, used, errtxt);
	if (bytes_written != used)
		return;

/* FIXME: Need counters for AEAD */

	return;
}

bool create_listener4(int port) {
	int sock = -1;
	sockaddr_u addr;
	int on = 1;
	int err;
	char errbuf[100];

	addr.sa4.sin_family = AF_INET;
	addr.sa4.sin_port = htons(port);
	addr.sa4.sin_addr.s_addr= htonl(INADDR_ANY);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		if (EAFNOSUPPORT == errno) {
			msyslog(LOG_ERR, "NTSs: No IPv4 support, disabling NTS-KE listener");
			return true;
		}
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: Can't create socket4: %s", errbuf);
		return false;
	}
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't setsockopt4: %s", errbuf);
		close(sock);
		return false;
	}
	err = bind(sock, &addr.sa, sizeof(addr.sa4));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't bind4: %s", errbuf);
		close(sock);
		return false;
	}
	if (listen(sock, 6) < 0) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't listen4: %s", errbuf);
		close(sock);
		return false;
	}
	msyslog(LOG_INFO, "NTSs: listen4 worked");

	listener4_sock = sock;
	return true;
}

bool create_listener6(int port) {
	int sock = -1;
	sockaddr_u addr;
	int on = 1;
	int err;
	char errbuf[100];

	addr.sa6.sin6_family = AF_INET6;
	addr.sa6.sin6_port = htons(port);
	addr.sa6.sin6_addr = in6addr_any;
	sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock < 0) {
		if (EAFNOSUPPORT == errno) {
			msyslog(LOG_ERR, "NTSs: No IPv6 support, disabling NTS-KE listener");
			return true;
		}
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: Can't create socket6: %s", errbuf);
		return false;
	}
	/* Hack to keep IPV6 from listening on IPV4 too */
	err = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't setsockopt6only: %s", errbuf);
		close(sock);
		return false;
	}
	err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't setsockopt6: %s", errbuf);
		close(sock);
		return false;
	}
	err = bind(sock, &addr.sa, sizeof(addr.sa6));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't bind6: %s", errbuf);
		close(sock);
		return false;
	}
	if (listen(sock, 6) < 0) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't listen6: %s", errbuf);
		close(sock);
		return false;
	}
	msyslog(LOG_INFO, "NTSs: listen6 worked");

	listener6_sock = sock;
	return true;
}

bool nts_ke_process_receive(struct BufCtl_t *buf, int *aead) {
	while (buf->left >= NTS_KE_HDR_LNG) {
		uint16_t type, data;
		int length;
		bool critical = false;

		// FIXME: msyslogs need rate limiting
		type = ke_next_record(buf, &length);
                if (length > buf->left){
                        msyslog(LOG_ERR, "NTSs: Chunk too big: 0x%x, %d, %d",
                                type, buf->left, length);
                        return false;
                }
		if (NTS_CRITICAL & type) {
			critical = true;
			type &= ~NTS_CRITICAL;
		}
		if (0) // Handy for debugging but very verbose
			msyslog(LOG_INFO, "NTSs: Record: T=%d, L=%d, C=%d", type, length, critical);
		switch (type) {
		    case nts_error:
			if (sizeof(data) != length) {
				msyslog(LOG_ERR, "NTSs: wrong length on error: %d", length);
				return false;
			}
			data = next_uint16(buf);
			msyslog(LOG_ERR, "NTSs: error: %d", data);
			return false;
		    case nts_next_protocol_negotiation:
			if (sizeof(data) != length) {
				msyslog(LOG_ERR, "NTSs: NPN-Wrong length: %d", length);
				return false;
			}
			data = next_uint16(buf);
			if (data != nts_protocol_NTP) {
				msyslog(LOG_ERR, "NTSs: NPN-Bad data: %d", data);
				return false;
			}
			break;
		    case nts_algorithm_negotiation:
			if (buf->left < length || length % sizeof(uint16_t) > 0) {
				msyslog(LOG_ERR, "NTSs: AN-Wrong length: %d", length);
				return false;
			}
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
			if (critical) {
				return false;
			}
			buf->next += length;
			buf->left -= length;
			break;
		} /* case */
	}   /* while */

// FIXME: check for missing EOM.  Need to read more?
	if (buf->left > 0)
		return false;

	return true;

}

void nts_ke_setup_send(struct BufCtl_t *buf, int aead,
       uint8_t *c2s, uint8_t *s2c, int keylen) {

	/* 4.1.2 Next Protocol */
	ke_append_record_uint16(buf,
				NTS_CRITICAL+nts_next_protocol_negotiation, nts_protocol_NTP);
	/* 4.1.5 AEAD Algorithm List */
	ke_append_record_uint16(buf, nts_algorithm_negotiation, aead);

	if (extra_port)
	        ke_append_record_uint16(buf, nts_port_negotiation, extra_port);


	for (int i=0; i<NTS_MAX_COOKIES; i++) {
		uint8_t cookie[NTS_MAX_COOKIELEN];
		int cookielen = nts_make_cookie(cookie, aead, c2s, s2c, keylen);
		ke_append_record_bytes(buf, nts_new_cookie, cookie, cookielen);
	}

	/* 4.1.1: End, Critical */
	ke_append_record_null(buf, NTS_CRITICAL+nts_end_of_message);
}

/* end */
