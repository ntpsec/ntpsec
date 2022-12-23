/*
 * nts.c - Network Time Security (NTS) support
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to
 * https://tools.ietf.org/html/draft-ietf-ntp-using-nts-for-ntp-15
 *
 * This module exposes mostly functions and structure pointers (not
 * structures) so that the NTS implementation can be sealed off from
 * the rest of the code. It supports both the client and server sides.
 *
 * The exception is client configuration, for which various bits have
 * to be set by the config parser.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <openssl/err.h>

#include "ntp_types.h"
#include "ntp_stdlib.h"
#include "ntpd.h"
#include "nts.h"
#include "nts2.h"

struct ntsconfig_t ntsconfig = {
	.ntsenable = false,
	.mintls = NULL,
	.maxtls = NULL,
	.tlsciphersuites = NULL,
	.cert = NULL,
	.key = NULL,
	.KI = NULL,
	.ca = NULL,
	.aead = NULL
};

void nts_log_version(void);

/*****************************************************/

/* More SSL initialization in ssl_init() from libntp/ssl_init.c */

void nts_init(void) {
	bool ok = true;
	nts_log_version();
	if (ntsconfig.ntsenable) {
		ok &= nts_server_init();
	}
	ok &= nts_client_init();
	ok &= nts_cookie_init();
	ok &= extens_init();
	if (!ok) {
		msyslog(LOG_ERR, "NTS: troubles during init.  Bailing.");
		exit(1);
	}
}

void nts_init2(void) {
	bool ok = true;
	if (ntsconfig.ntsenable) {
		ok &= nts_server_init2();
		ok &= nts_cookie_init2();
	}
	if (!ok) {
		msyslog(LOG_ERR, "NTS: troubles during init2.  Bailing.");
		exit(1);
	}
}

/* There are 2 cases:
 *  1: match, log version
 *  2: mismatch, log both build and run
 */
void nts_log_version(void) {
	unsigned long buildVersion = OPENSSL_VERSION_NUMBER;
	const char * text = OpenSSL_version(OPENSSL_VERSION);
	bool match = (buildVersion == OpenSSL_version_num()) &&
		(0 == strcmp(OPENSSL_VERSION_TEXT, text));
	if (match) {
		/* Case 1 */
		msyslog(LOG_INFO, "INIT: %s, %lx",
			OPENSSL_VERSION_TEXT, buildVersion);
	} else {
                /* Case 2 */
		msyslog(LOG_INFO, "INIT: Built with %s, %lx",
			OPENSSL_VERSION_TEXT, buildVersion);
		msyslog(LOG_INFO, "INIT: Running with %s, %lx",
			OpenSSL_version(OPENSSL_VERSION),
			OpenSSL_version_num());
		if (buildVersion > OpenSSL_version_num()) {
			msyslog(LOG_ERR, "INIT: Old OpenSSL library, bailing");
			exit(1);
		}
	}
	/*
	 * If the runtime OpenSSL is 1.1.1a, then bail, since we'll run into errors with the
	 * TLSv1.3 maximum label length
	 */
	if (OpenSSL_version_num() == 0x1010101fL) {
		msyslog(LOG_ERR, "INIT: OpenSSL 1.1.1a has a maximum label length bug, bailing");
		exit(1);
	}
}

/*****************************************************/

void nts_timer(void) {
	nts_cert_timer();
	nts_cookie_timer();
}

/*****************************************************/

/* 0 is default, -1 is error */
int nts_translate_version(const char *arg) {
	if (NULL == arg) {
		return 0;
	}
	if (0 == strcmp(arg, "TLS1.3")) {
		return TLS1_3_VERSION;
	}
	msyslog(LOG_ERR, "NTS: TLS unrecognized version string: %s.", arg);
	return -1;
}

/* Translate text to AEAD code.  NO_AEAD for none/error */
uint16_t nts_string_to_aead(const char* text) {
	if (false) {
	} else if (0 == strcmp(text, "AES_SIV_CMAC_256")) {
		return AEAD_AES_SIV_CMAC_256;
	} else if (0 == strcmp(text, "AES_SIV_CMAC_384")) {
		return AEAD_AES_SIV_CMAC_384;
	} else if (0 == strcmp(text, "AES_SIV_CMAC_512")) {
		return AEAD_AES_SIV_CMAC_512;
	} else {
		return NO_AEAD;
	}
}

/* returns key length, 0 if unknown arg */
int nts_get_key_length(uint16_t aead) {
	switch (aead) {
	    case AEAD_AES_SIV_CMAC_256:
		return AEAD_AES_SIV_CMAC_256_KEYLEN;
	    case AEAD_AES_SIV_CMAC_384:
		return AEAD_AES_SIV_CMAC_384_KEYLEN;
	    case AEAD_AES_SIV_CMAC_512:
		return AEAD_AES_SIV_CMAC_512_KEYLEN;
	    default:
		return 0;
	}
}


/*****************************************************/

bool nts_load_versions(SSL_CTX *ctx) {
	int minver, maxver;
	minver = nts_translate_version(ntsconfig.mintls);
	maxver = nts_translate_version(ntsconfig.maxtls);
	if ((-1 == minver) || (-1 == maxver))
		return false;
	if(0 == minver) minver = TLS1_3_VERSION;   // 3.
	SSL_CTX_set_min_proto_version(ctx, minver);
	SSL_CTX_set_max_proto_version(ctx, maxver);
	return true;
}

bool nts_load_ciphers(SSL_CTX *ctx) {
	/* SSL set_ciphers(uites) ignores typos or ciphers it doesn't support.
	 * There is no SSL_CTX_get_cipher_list, so we can't easily read back
	 * the ciphers to see what it took.
	 * We could make a dummy SSL, read the list, then free it.
	 */
	if (NULL != ntsconfig.tlsciphersuites) {
		if (1 != SSL_CTX_set_ciphersuites(ctx, ntsconfig.tlsciphersuites)) {
			msyslog(LOG_ERR, "NTS: troubles setting ciphersuites.");
			return false;
		} else {
			msyslog(LOG_INFO, "NTS: set ciphersuites.");
		}
	}
	return true;
}


static struct stat certfile_stat;

void nts_reload_certificate(SSL_CTX *ctx) {
	struct stat temp_stat;
	const char *cert = NTS_CERT_FILE;

	if (NULL != ntsconfig.cert)
		cert = ntsconfig.cert;

	if (0 != stat(cert, &temp_stat)) {
		return;
	}

	if ((certfile_stat.st_mtime == temp_stat.st_mtime)
            && (certfile_stat.st_ctime == temp_stat.st_ctime)) {
		return;  /* avoid clutter in log file */
	}

	nts_load_certificate(ctx);

}

bool nts_load_certificate(SSL_CTX *ctx) {
	const char *cert = NTS_CERT_FILE;
	const char *key = NTS_KEY_FILE;
        char errbuf[100];

	if (NULL != ntsconfig.cert)
		cert = ntsconfig.cert;
	if (NULL != ntsconfig.key)
		key = ntsconfig.key;

	/* for reload checking */
	if (0 != stat(cert, &certfile_stat)) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_ERR, "NTSs: can't stat certificate (chain) from %s: %s", cert, errbuf);
		return false;
	}

	if (1 != SSL_CTX_use_certificate_chain_file(ctx, cert)) {
		msyslog(LOG_ERR, "NTSs: can't load certificate (chain) from %s", cert);
		nts_log_ssl_error();
		return false;
	} else {
		msyslog(LOG_ERR, "NTSs: loaded certificate (chain) from %s", cert);
	}
	if (1 != SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM)) {
		msyslog(LOG_ERR, "NTSs: can't load private key from %s", key);
		nts_log_ssl_error();
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


int nts_ssl_read(SSL *ssl, uint8_t *buff, int buff_length) {
	int bytes_read;
	char errbuf[100];
	bytes_read = SSL_read(ssl, buff, buff_length);
	if (0 >= bytes_read) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTS: SSL_read error: %s", errbuf);
		nts_log_ssl_error();
		return -1;
	}
	return bytes_read;
}

int nts_ssl_write(SSL *ssl, uint8_t *buff, int buff_length) {
	int bytes_written;
	char errbuf[100];
	bytes_written = SSL_write(ssl, buff, buff_length);
	if (0 >= bytes_written) {
		ntp_strerror_r(errno, errbuf, sizeof(errbuf));
		msyslog(LOG_INFO, "NTS: SSL_write error: %s", errbuf);
		nts_log_ssl_error();
		return -1;
	}
	return bytes_written;
}

void nts_log_ssl_error(void) {
	char buff[256];
	int err = ERR_get_error();
	SSL_load_error_strings();        /* Needed on NetBSD */
	while (0 != err) {
		ERR_error_string_n(err, buff, sizeof(buff));
		msyslog(LOG_INFO, "NTS: %s", buff);
		err = ERR_get_error();
	}
}

/*****************************************************/

/* NB: KE length is body length, Extension length includes header. */

/* Troubles with signed/unsigned compares when using sizeof() */

void ke_append_record_null(BufCtl* buf, uint16_t type) {
	append_header(buf, type, 0);
}

void ke_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data) {
	if (NTS_KE_HDR_LNG+NTS_KE_U16_LNG > buf->left)
		return;
	append_header(buf, type, NTS_KE_U16_LNG);
	append_uint16(buf, data);
}

void ke_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length) {
	if (NTS_KE_HDR_LNG+length > buf->left)
		return;
	append_header(buf, type, length);
	append_bytes(buf, data, length);
}

void ex_append_record_null(BufCtl* buf, uint16_t type) {
	append_header(buf, type, NTS_KE_HDR_LNG);
}

void ex_append_record_uint16(BufCtl* buf, uint16_t type, uint16_t data) {
	if (NTS_KE_HDR_LNG+NTS_KE_U16_LNG > buf->left)
		return;
	append_header(buf, type, NTS_KE_HDR_LNG+NTS_KE_U16_LNG);
	append_uint16(buf, data);
}

void ex_append_record_bytes(BufCtl* buf, uint16_t type, uint8_t *data, int length) {
	if (NTS_KE_HDR_LNG+length > buf->left)
		return;
	append_header(buf, type, NTS_KE_HDR_LNG+length);
	append_bytes(buf, data, length);
}

void ex_append_header(BufCtl* buf, uint16_t type, uint16_t length) {
	append_header(buf, type, NTS_KE_HDR_LNG+length);
}

void append_header(BufCtl* buf, uint16_t type, uint16_t length) {
	uint16_t * ptr = (uint16_t *)buf->next;
	if (NTS_KE_HDR_LNG > buf->left)
		return;
	*ptr++ = htons(type);
	*ptr++ = htons(length);
	buf->next += NTS_KE_HDR_LNG;
	buf->left -= NTS_KE_HDR_LNG;
	/* leaves buf pointing to where data will go */
}

void append_uint16(BufCtl* buf, uint16_t data) {
	uint16_t * ptr = (uint16_t *)buf->next;
	if (NTS_KE_U16_LNG > buf->left)
		return;
	*ptr++ = htons(data);
	buf->next += NTS_KE_U16_LNG;
	buf->left -= NTS_KE_U16_LNG;
}

void append_bytes(BufCtl* buf, uint8_t *data, int length) {
	if (length > buf->left)
		return;
	memcpy(buf->next, data, length);
	buf->next += length;
	buf->left -= length;
}

/* Reads type and length of the next record, and moves cursor to the data */
uint16_t ke_next_record(BufCtl* buf, int *length) {
	uint16_t *ptr = (uint16_t *)buf->next;
	uint16_t type = ntohs(*ptr++);
	*length = ntohs(*ptr++);
	buf->next += NTS_KE_HDR_LNG;
	buf->left -= NTS_KE_HDR_LNG;
	return type;
}

uint16_t ex_next_record(BufCtl* buf, int *length) {
	uint16_t *ptr = (uint16_t *)buf->next;
	uint16_t type = ntohs(*ptr++);
	*length = ntohs(*ptr++)-NTS_KE_HDR_LNG;
	buf->next += NTS_KE_HDR_LNG;
	buf->left -= NTS_KE_HDR_LNG;
	return type;
}

/* Reads a uint16 from the record and advances to the next data */
uint16_t next_uint16(BufCtl* buf) {
	uint16_t *ptr = (uint16_t *)buf->next;
	uint16_t data = ntohs(*ptr++);
	buf->next += NTS_KE_U16_LNG;
	buf->left -= NTS_KE_U16_LNG;
	return data;
}

/* Reads a string of bytes from the record and advances to the next data */
uint16_t next_bytes(BufCtl* buf, uint8_t *data, int length) {
	memcpy(data, buf->next, length);
	buf->next += length;
	buf->left -= length;
	return length;
}


/* end */
