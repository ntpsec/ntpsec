/*
 * nts_client.c - Network Time Security (NTS) client side support
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 */
#include "config.h"

#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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
#include "ntp_stdlib.h"
#include "timespecops.h"

SSL_CTX* make_ssl_client_ctx(const char *filename);
int open_TCP_socket(struct peer *peer, const char *hostname);
struct addrinfo * find_best_addr(struct addrinfo *answer);
bool connect_TCP_socket(int sockfd, struct addrinfo *addr);
bool nts_set_cert_search(SSL_CTX *ctx, const char *filename);
void set_hostname(SSL *ssl, struct peer *peer, const char *hostname);
bool check_certificate(SSL *ssl, struct peer *peer);
bool check_alpn(SSL *ssl, struct peer *peer, const char *hostname);
bool nts_client_send_request(SSL *ssl, struct peer *peer);
bool nts_client_send_request_core(uint8_t *buff, int buf_size, int *used, struct peer* peer);
bool nts_client_process_response(SSL *ssl, struct peer *peer);
bool nts_client_process_response_core(uint8_t *buff, int transferred, struct peer* peer);
bool nts_server_lookup(char *server, sockaddr_u *addr, int af);

static SSL_CTX *client_ctx = NULL;

/* Ugly global variables passed from worker thread back to main thread. */
static sockaddr_u sockaddr;
static bool addrOK;


bool nts_client_init(void) {

	client_ctx = make_ssl_client_ctx(ntsconfig.ca);


/* Ugly global variables passed from worker thread back to main thread. */
	return true;
}

bool nts_probe(struct peer * peer) {
	struct timeval timeout = {.tv_sec = NTS_KE_TIMEOUT, .tv_usec = 0};
	const char *hostname = peer->hostname;
	char hostbuf[100];
	char errbuf[100];
	SSL     *ssl;
	int      server;
	struct timespec start, finish;
	int      err;

	if (NULL == client_ctx)
		return false;

	addrOK = false;
	clock_gettime(CLOCK_REALTIME, &start);

	if (NULL == hostname) {
		/* IP Address case */
		int af = AF(&peer->srcadr);
		switch (af) {
		    case AF_INET:
			inet_ntop(af, PSOCK_ADDR4(&peer->srcadr), hostbuf, sizeof(hostbuf));
			break;
		    case AF_INET6:
			inet_ntop(af, PSOCK_ADDR6(&peer->srcadr), hostbuf, sizeof(hostbuf));
			break;
		    default:
			return false;
		}
		hostname = hostbuf;
	}

	server = open_TCP_socket(peer, hostname);
	if (-1 == server) {
		nts_ke_probes_bad++;
		return false;
	}

	err = setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	if (0 > err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSc: can't setsockopt: %s", errbuf);
		close(server);
		nts_ke_probes_bad++;
		return false;
	}

	if (NULL == peer->cfg.nts_cfg.ca)
		ssl = SSL_new(client_ctx);
	else {
		SSL_CTX *ctx;
		ctx = make_ssl_client_ctx(peer->cfg.nts_cfg.ca);
		if (NULL == ctx) {
			close(server);
			return false;
		}
		ssl = SSL_new(ctx);
		SSL_CTX_free(ctx);
	}
	set_hostname(ssl, peer, hostname);
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

	if (!check_certificate(ssl, peer))
		goto bail;
	if (!check_alpn(ssl, peer, hostname))
		goto bail;

	if (!nts_client_send_request(ssl, peer))
		goto bail;
	if (!nts_client_process_response(ssl, peer))
		goto bail;

	/* We are using AEAD_AES_SIV_CMAC_xxx, from RFC 5297
	 * key length depends upon which key is selected */
	peer->nts_state.keylen = nts_get_key_length(peer->nts_state.aead);
	if (0 == peer->nts_state.keylen) {
		msyslog(LOG_ERR, "NTSc: Unknown AEAD code: %d", peer->nts_state.aead);
		goto bail;
	}
	if (!nts_make_keys(ssl,
			   peer->nts_state.aead,
			   peer->nts_state.c2s,
			   peer->nts_state.s2c,
			   peer->nts_state.keylen))
		goto bail;

	addrOK = true;
	nts_ke_probes_good++;

  bail:
	if (!addrOK) {
		nts_ke_probes_bad++;
		peer->nts_state.count = -1;
	}
	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(server);

	clock_gettime(CLOCK_REALTIME, &finish);
	finish = sub_tspec(finish, start);
	msyslog(LOG_INFO, "NTSc: NTS-KE req to %s took %.3f sec, %s",
		hostname, tspec_to_d(finish),
		addrOK? "OK" : "fail");

	return addrOK;
}

bool nts_check(struct peer *peer) {
	if (0) {
		char errbuf[100];
		sockporttoa_r(&sockaddr, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: nts_check %s, %d", errbuf, addrOK);
	}
	if (addrOK) {
		dns_take_server(peer, &sockaddr);
		dns_take_status(peer, DNS_good);
	} else
		dns_take_status(peer, DNS_error);
	return addrOK;
}

SSL_CTX* make_ssl_client_ctx(const char * filename) {
	bool ok = true;
	SSL_CTX *ctx;

	ctx = SSL_CTX_new(TLS_client_method());
	if (NULL == ctx) {
		/* Happens if no ciphers */
		msyslog(LOG_ERR, "NTSc: NULL ctx");
		nts_log_ssl_error();
		return NULL;
	}

	{
		// 4., ALPN, RFC 7301
		static unsigned char alpn [] = { 7, 'n', 't', 's', 'k', 'e', '/', '1' };
		SSL_CTX_set_alpn_protos(ctx, alpn, sizeof(alpn));
	}

	SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
	SSL_CTX_set_timeout(ctx, NTS_KE_TIMEOUT);   /* session lifetime */

	ok &= nts_load_versions(ctx);
	ok &= nts_load_ciphers(ctx);
	ok &= nts_set_cert_search(ctx, filename);

	if (!ok) {
		msyslog(LOG_ERR, "NTSc: Troubles setting up client SSL CTX");
		SSL_CTX_free(ctx);
		return NULL;
	};

	return ctx;
}

/* return -1 on error */
int open_TCP_socket(struct peer *peer, const char *hostname) {
	char host[256], port[32];
	char errbuf[100];
	char *tmp;
	struct addrinfo hints;
	struct addrinfo *answer, *worker;
	int gai_rc;
	int sockfd;
	struct timespec start, finish;

	/* copy avoids dancing around const warnings */
	strlcpy(host, hostname, sizeof(host));

	/* handle xxx:port case */
	tmp = strchr(host, ']');
	if (NULL == tmp) {
		tmp = strchr(host, ':');
	} else {
		/* IPv6 case, start search after ] */
		tmp = strchr(tmp, ':');
	}
	if (NULL == tmp) {
		/* simple case, no : */
		strlcpy(port, NTS_KE_PORTA, sizeof(port));
	} else {
		/* Complicated case, found a : */
		*tmp++ = 0;
		strlcpy(port, tmp, sizeof(port));
	}

	ZERO(hints);
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF(&peer->srcadr);  /* -4, -6 switch */
	clock_gettime(CLOCK_REALTIME, &start);
	gai_rc = getaddrinfo(host, port, &hints, &answer);
	if (0 != gai_rc) {
		msyslog(LOG_INFO, "NTSc: open_TCP_socket: DNS error trying to contact %s: %d, %s",
			hostname, gai_rc, gai_strerror(gai_rc));
		return -1;
	}
	clock_gettime(CLOCK_REALTIME, &finish);
	finish = sub_tspec(finish, start);
	msyslog(LOG_INFO, "NTSc: DNS lookup of %s took %.3f sec",
		hostname, tspec_to_d(finish));

	/* Use first answer
	 * sockaddr is global for NTP address
	 * also use as temp for printing here
	 */
	worker = find_best_addr(answer);
	memcpy(&sockaddr, worker->ai_addr, worker->ai_addrlen);
	sockporttoa_r(&sockaddr, errbuf, sizeof(errbuf));
	msyslog(LOG_INFO, "NTSc: connecting to %s:%s => %s",
		host, port, errbuf);

	/* setup default NTP port now
	 *   in case of server-name:port later on
	 */
	SET_PORT(&sockaddr, NTP_PORT);
	sockfd = socket(worker->ai_family, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: open_TCP_socket: no socket: %s", errbuf);
	} else {
		/* Use first IP Address */
		if (!connect_TCP_socket(sockfd, worker)) {
			close(sockfd);
			sockfd = -1;
		}
	}

	freeaddrinfo(answer);
	return sockfd;

}

struct addrinfo *find_best_addr(struct addrinfo *answer) {
	/* default to first one */
        return(answer);
}


/* This kludgery is needed to get a sane timeout.
 * The default is unspecified but long.
 * On Linux, man connect gets man 2 which doesn't mention O_NONBLOCK
 * Use man 3 connect.
 */
bool connect_TCP_socket(int sockfd, struct addrinfo *addr) {
	char errbuf[100];
        int err;
	fd_set fdset;
	struct timeval timeout;
	int so_error;
	socklen_t so_len = sizeof(so_error);

	err = fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (-1 == err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: can't set O_NONBLOCK %s", errbuf);
		return false;
	}
	err = connect(sockfd, addr->ai_addr, addr->ai_addrlen);
	/* The normal case is -1 and errno == EINPROGRESS
	 * Getting connected should be possible if the scheduler
	 * avoids us for long enough.
	 * Other errors may be possible.  No route?
	 * I haven't seen that yet.  HGM, 2020 Jan 19
	 */
	if (-1 != err || EINPROGRESS != errno) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: connect_TCP_socket: connect failed: %s", errbuf);
		return false;
	}

	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	timeout.tv_sec = NTS_KE_TIMEOUT;
	timeout.tv_usec = 0;

	if (0 == select(sockfd + 1, NULL, &fdset, NULL, &timeout)) {
		msyslog(LOG_INFO, "NTSc: connect_TCP_socket: timeout");
		return false;
	}

	/* It's ready, either connected or error. */
	if (-1 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &so_len)) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: connect_TCP_socket: getsockopt failed: %s", errbuf);
		return false;
	}

        if (0 != so_error) {
		ntp_strerror_r(so_error, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: connect_TCP_socket: connect failed: %s", errbuf);
		return false;
	}

	err = fcntl(sockfd, F_SETFL, 0); /* turn off O_NONBLOCK */
	if (-1 == err) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTSc: can't unset O_NONBLOCK %s", errbuf);
		return false;
	}

	return true;
}


void set_hostname(SSL *ssl, struct peer *peer, const char *hostname) {
	char host[256], *tmp;

	/* chop off trailing :port */
	strlcpy(host, hostname, sizeof(host));
	tmp = strchr(host, ']');
	if (NULL == tmp) {
		tmp = host;			/* not IPv6 [...] format */
	}
	tmp = strchr(tmp, ':');
	if (NULL != tmp) {
		*tmp = 0;
	}

// https://wiki.openssl.org/index.php/Hostname_validation
	UNUSED_ARG(peer);
	SSL_set_hostflags(ssl, X509_CHECK_FLAG_NO_WILDCARDS);
	SSL_set1_host(ssl, host);
	msyslog(LOG_DEBUG, "NTSc: set cert host: %s", host);

}

bool check_certificate(SSL *ssl, struct peer* peer) {
	X509 *cert = SSL_get_peer_certificate(ssl);

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
		X509_free(cert);
		certok = SSL_get_verify_result(ssl);
		if (X509_V_OK == certok) {
			msyslog(LOG_INFO, "NTSc: certificate is valid.");
		} else {
			msyslog(LOG_ERR, "NTSc: certificate invalid: %d=>%s",
				certok, X509_verify_cert_error_string(certok));
			if (FLAG_NTS_NOVAL & peer->cfg.flags) {
				msyslog(LOG_INFO, "NTSc: noval - accepting invalid cert.");
				return true;
			}
			return false;
		}
	}
	return true;
}

bool check_alpn(SSL *ssl, struct peer* peer, const char *hostname) {
	UNUSED_ARG(peer);
	const unsigned char *data;
	unsigned int len;
	SSL_get0_alpn_selected(ssl, &data, &len);
	if (0 == len) {
		msyslog(LOG_DEBUG, "NTSc: No ALPN from %s (%s)",
			hostname, SSL_get_version(ssl));
		return false;
	}
	/* For now, we only support one version.
	 * This will get more complicated when version 2 arrives. */
	if (len != 7 ||
	    0 != memcmp(data, "ntske/1", len)) {
		/* copy data over so we can print it. */
		/* don't read past end of data */
		unsigned int i, l;
		char buff [16];
		l = min(len, sizeof(buff)-1);
		memcpy(buff, data, l);
		buff[l] = '\0';
		for (i=0; i<l; i++) {
			if (!isgraph((int)buff[i])) {
			  buff[i] = '*'; /* fix non-printing crap */
			}
		}
		msyslog(LOG_DEBUG, "NTSc: Strange ALPN %s (%u) from %s",
			buff, len, hostname);
		return false;
	}
	msyslog(LOG_DEBUG, "NTSc: Good ALPN from %s", hostname);

	return true;
}

bool nts_make_keys(SSL *ssl, uint16_t aead, uint8_t *c2s, uint8_t *s2c, int keylen) {
	const char *label = "EXPORTER-network-time-security";
	unsigned char context[5];
	context[0] = (nts_protocol_NTP >> 8) & 0xFF;
	context[1] = nts_protocol_NTP & 0xFF;
	context[2] = (aead >> 8) & 0xFF;
	context[3] = aead & 0xFF;
	context[4] = 0x00;
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

bool nts_client_send_request(SSL *ssl, struct peer* peer) {
	uint8_t buff[1000];
	int     used, transferred;
	bool    success;

	success = nts_client_send_request_core(buff, sizeof(buff), &used, peer);
	if (!success) {
		return false;
	}

	transferred = nts_ssl_write(ssl, buff, used);
	if (used != transferred)
		return false;

	return true;
}

bool nts_client_send_request_core(uint8_t *buff, int buf_size, int *used, struct peer* peer) {
	struct  BufCtl_t buf;
	uint16_t aead = NO_AEAD;

	buf.next = buff;
	buf.left = buf_size;

	/* 4.1.2 Next Protocol, 0 for NTP */
	ke_append_record_uint16(&buf,
				NTS_CRITICAL+nts_next_protocol_negotiation, nts_protocol_NTP);

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

	*used = buf_size-buf.left;
	if (*used >= (int)(buf_size - 10)) {
		msyslog(LOG_ERR, "ERR-NTSc: buffer overflow: %d, %ld",
			*used, (long)buf_size);
		exit(2);
	}
	return true;
}

bool nts_client_process_response(SSL *ssl, struct peer* peer) {
	uint8_t  buff[2048];  /* RFC 4. says SHOULD be 65K */
	int transferred;

	transferred = nts_ssl_read(ssl, buff, sizeof(buff));
	if (0 > transferred)
		return false;
	msyslog(LOG_ERR, "NTSc: read %d bytes", transferred);

	return nts_client_process_response_core(buff, transferred, peer);
}

bool nts_client_process_response_core(uint8_t *buff, int transferred, struct peer* peer) {
	int idx;
	struct BufCtl_t buf;

	peer->nts_state.aead = NO_AEAD;
	peer->nts_state.keylen = 0;
	peer->nts_state.writeIdx = 0;
	peer->nts_state.readIdx = 0;
	peer->nts_state.count = 0;

	buf.next = buff;
	buf.left = transferred;
	while (buf.left > 0) {
		uint16_t type, data, port;
		bool critical = false;
		int length, keylength;
		char errbuf[100];
#define MAX_SERVER 100
		char server[MAX_SERVER];

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
			if ((sizeof(data) != length) || (data != nts_protocol_NTP)) {
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
		    case nts_server_negotiation:
			if (MAX_SERVER < (length+1)) {
				msyslog(LOG_ERR, "NTSc: server string too long %d.", length);
				return false;
			}
			next_bytes(&buf, (uint8_t *)server, length);
			server[length] = '\0';
			/* save port in case port specified before server */
			port = SRCPORT(&sockaddr);
			if (!nts_server_lookup(server, &sockaddr, AF(&peer->srcadr)))
				return false;
			SET_PORT(&sockaddr, port);
			socktoa_r(&sockaddr, errbuf, sizeof(errbuf));
			msyslog(LOG_ERR, "NTSc: Using server %s=>%s", server, errbuf);
			break;
		    case nts_port_negotiation:
			if (sizeof(port) != length) {
				msyslog(LOG_ERR, "NTSc: PN-Wrong length: %d, %d",
					length, critical);
				return false;
			}
			port = next_uint16(&buf);
			SET_PORT(&sockaddr, port);
			msyslog(LOG_ERR, "NTSc: Using port %d", port);
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
			if (critical) {
				return false;
			}
			buf.next += length;
			buf.left -= length;
			break;
		} /* case */
	}   /* while */

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

bool nts_set_cert_search(SSL_CTX *ctx, const char *filename) {
	struct stat statbuf;
	char errbuf[100];
	if (NULL == filename) {
		msyslog(LOG_INFO, "NTSc: Using system default root certificates.");
		SSL_CTX_set_default_verify_paths(ctx);   // Use system root certs
		return true;
	}
	if (0 == stat(filename, &statbuf)) {
		if (S_ISDIR(statbuf.st_mode)) {
			if (1 != SSL_CTX_load_verify_locations(
				ctx, NULL, filename)) {
			msyslog(LOG_INFO, "NTSc: Can't use %s as dir for root certificates.", filename);
			    nts_log_ssl_error();
			    return false;
			}
			msyslog(LOG_INFO, "NTSc: Using dir %s for root certificates.", filename);
			return true;
		}
		if (S_ISREG(statbuf.st_mode)) {
			if (1 != SSL_CTX_load_verify_locations(
				ctx, filename, NULL)) {
			    msyslog(LOG_INFO, "NTSc: Can't use %s as file for root certificates.", filename);
			    nts_log_ssl_error();
			    return false;
			}
			msyslog(LOG_INFO, "NTSc: Using file %s for root certificates.", filename);
			return true;
		}
		msyslog(LOG_ERR, "NTSc: cert dir/file isn't dir or file: %s. mode 0x%x",
			filename, statbuf.st_mode);
		return false;
	}
	ntp_strerror_r(errno, errbuf, sizeof(errbuf));
	msyslog(LOG_ERR, "NTSc: can't stat cert dir/file: %s, %s",
		filename, errbuf);
	return false;
}
/* The -4/-6 option is used for both the NTS-KE server and the NTP server.
 * That will break if the KE server returns a name that returns only an
 * address of the other type.
 * We could fix that by trying again with AF_UNSPEC.
 */
bool nts_server_lookup(char *server, sockaddr_u *addr, int af) {
	struct addrinfo hints;
	struct addrinfo *answer;
	int gai_rc;

	ZERO(hints);
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = af;

	gai_rc = getaddrinfo(server, NTS_KE_PORTA, &hints, &answer);
	if (0 != gai_rc) {
		msyslog(LOG_INFO, "NTSc: DNS error trying to lookup %s: %d, %s",
			server, gai_rc, gai_strerror(gai_rc));
		return false;
	}

	if (NULL == answer)
		return false;

	if (sizeof(sockaddr_u) >= answer->ai_addrlen)
		memcpy(addr, answer->ai_addr, answer->ai_addrlen);

	freeaddrinfo(answer);

	return true;
}

/* end */
