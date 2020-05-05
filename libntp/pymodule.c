/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Python binding for selected libntp library functions
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "config.h"

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "timespecops.h"

#include "ntp_config.h"
#include "ntp_assert.h"

#include "ntp_control.h"

#include "ntp_auth.h"
#include <openssl/evp.h>

#include "python_compatibility.h"

const char *progname = "libntpc";

/*
 * Client utility functions
 */

static PyObject *
ntpc_setprogname(PyObject *self, PyObject *args)
{
	char *s;
	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	progname = strdup(s);

	/*
	 * This function is only called from clients.  Therefore
	 * log to stderr rather than syslog, and suppress logfile
	 * impediments.  If we ever want finer-grained control, that
	 * will be easily implemented with additional arguments.
	 */
	syslogit = false;	/* don't log messages to syslog */
	termlogit = true;	/* duplicate to stdout/err */
	termlogit_pid = false;
	msyslog_include_timestamp = false;

	Py_RETURN_NONE;
}

static PyObject *
ntpc_statustoa(PyObject *self, PyObject *args)
{
	int status1, status2;
	char *gs;

	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "ii", &status1, &status2))
		return NULL;
	gs = statustoa(status1, status2);
	return Py_BuildValue("s", gs);
}

static PyObject *
ntpc_prettydate(PyObject *self, PyObject *args)
{
	char *s;
	l_fp ts;

	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	if (hextolfp(s+2, &ts))
		return Py_BuildValue("s", prettydate(ts));
	else {
		PyErr_SetString(PyExc_ValueError, "ill-formed hex date");
		return NULL;
	}
}

static PyObject *
ntpc_lfptofloat(PyObject *self, PyObject *args)
{
	char *s;
	l_fp ts;
	struct timespec tt;

	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	if (!hextolfp(s+2, &ts)) {
		PyErr_SetString(PyExc_ValueError, "ill-formed hex date");
		return NULL;
	}
	tt = lfp_stamp_to_tspec(ts, time(NULL));
	return Py_BuildValue("d", tt.tv_sec + tt.tv_nsec * S_PER_NS);
}

static PyObject *
ntpc_set_tod(PyObject *self, PyObject *args)
{
	struct timespec ts;

	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "ii", &ts.tv_sec, &ts.tv_nsec))
		return NULL;
	return Py_BuildValue("d", ntp_set_tod(&ts));
}

static PyObject *
ntpc_adj_systime(PyObject *self, PyObject *args)
{
	double adjustment;

	UNUSED_ARG(self);
	if (!PyArg_ParseTuple(args, "d", &adjustment))
		return NULL;
	return Py_BuildValue("d", adj_systime(adjustment, adjtime) ? 1 : 0);
}

static PyObject *
ntpc_step_systime(PyObject *self, PyObject *args)
{
	double adjustment;
	doubletime_t full_adjustment;

	UNUSED_ARG(self);
	/*
	 * What we really want is for Python to parse a long double.
	 * As this is, it's a potential source of problems in the Python
	 * utilities if and when the time difference between the Unix epoch
	 * and now exceeds the range of a double.
	 */
	if (!PyArg_ParseTuple(args, "d", &adjustment))
		return NULL;
	full_adjustment = adjustment;
	return Py_BuildValue("d", step_systime(full_adjustment, ntp_set_tod));
}

/* --------------------------------------------------------------- */
/* Hook for CMAC/HMAC
 * Not really part of libntp, but this is a handy place to put it.
 */

/* Slightly older version of OpenSSL */
/* Similar hack in ssl_init.c and attic/digest-timing.c */
#ifndef EVP_MD_CTX_new
#define EVP_MD_CTX_new() EVP_MD_CTX_create()
#endif
#ifndef EVP_MD_CTX_reset
#define EVP_MD_CTX_reset(ctx) EVP_MD_CTX_init(ctx)
#endif

/* Needed on old versions of OpenSSL */
static void SSL_init(void) {
	static bool init_done = false;
	if (init_done)
		return;
	init_done = true;
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
}

/* xx = ntp.ntpc.checkname(name)
 * returns None if algorithm name is invalid. */

static PyObject *
ntpc_checkname(PyObject *self, PyObject *args)
{
	const char *name;
	char upcase[100];
	const EVP_MD *digest;
	const EVP_CIPHER *cipher;
	UNUSED_ARG(self);

	SSL_init();

	if (!PyArg_ParseTuple(args, "s", &name))
		return NULL;
        strlcpy(upcase, name, sizeof(upcase));
	for (int i=0; upcase[i]!=0; i++) {
		upcase[i] = toupper(upcase[i]);
	}

        digest = EVP_get_digestbyname(upcase);
	if (NULL != digest) {
		return Py_BuildValue("i", 1);
        }

        if ((strcmp(upcase, "AES") == 0) || (strcmp(upcase, "AES128CMAC") == 0)) {
                strlcpy(upcase, "AES-128", sizeof(upcase));
        }
        strlcat(upcase, "-CBC", sizeof(upcase));
	cipher = EVP_get_cipherbyname(upcase);
	if (NULL != cipher) {
		int length = EVP_CIPHER_key_length(cipher);
		return Py_BuildValue("i", length);
	}

	Py_RETURN_NONE;
}


/* mac = ntp.ntpc.mac(data, key, name) */

static PyObject *
ntpc_mac(PyObject *self, PyObject *args)
{
	UNUSED_ARG(self);
	uint8_t *data;
	Py_ssize_t datalen;
	uint8_t *key;
	Py_ssize_t keylen;
	char *name;
	uint8_t mac[CMAC_MAX_MAC_LENGTH];
	size_t maclen;
	char upcase[100];
	static EVP_MD_CTX *digest_ctx = NULL;
	static CMAC_CTX *cmac_ctx = NULL;
	const EVP_MD *digest;
	const EVP_CIPHER *cipher;
	int cipherlen;

	SSL_init();

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#y#s",
			&data, &datalen, &key, &keylen, &name))
#else
	if (!PyArg_ParseTuple(args, "s#s#s",
			&data, &datalen, &key, &keylen, &name))
#endif
		Py_RETURN_NONE;

        strlcpy(upcase, name, sizeof(upcase));
	for (int i=0; upcase[i]!=0; i++) {
		upcase[i] = toupper(upcase[i]);
	}

        digest = EVP_get_digestbyname(upcase);
	if (NULL != digest) {
		/* Old digest case, MD5, SHA1 */
		unsigned int maclenint;
		if (NULL == digest_ctx)
			digest_ctx = EVP_MD_CTX_new();
		EVP_MD_CTX_reset(digest_ctx);
		if (!EVP_DigestInit_ex(digest_ctx, digest, NULL))
			Py_RETURN_NONE;
		EVP_DigestUpdate(digest_ctx, key, keylen);
		EVP_DigestUpdate(digest_ctx, data, (unsigned int)datalen);
		EVP_DigestFinal_ex(digest_ctx, mac, &maclenint);
		if (MAX_BARE_MAC_LENGTH < maclenint)
			maclenint = MAX_BARE_MAC_LENGTH;
#if PY_MAJOR_VERSION >= 3
		return Py_BuildValue("y#", &mac, maclenint);
#else
		return Py_BuildValue("s#", &mac, maclenint);
#endif
	}

        if ((strcmp(upcase, "AES") == 0) || (strcmp(upcase, "AES128CMAC") == 0)) {
                strlcpy(upcase, "AES-128", sizeof(upcase));
        }
        strlcat(upcase, "-CBC", sizeof(upcase));

	cipher = EVP_get_cipherbyname(upcase);
	if (NULL == cipher)
		Py_RETURN_NONE;

	cipherlen = EVP_CIPHER_key_length(cipher);
	if (cipherlen < keylen) {
		keylen = cipherlen;		/* truncate */
	} else if (cipherlen > keylen) {
		uint8_t newkey[EVP_MAX_KEY_LENGTH];
		memcpy(newkey, key, keylen);
		while (cipherlen > keylen)
			key[keylen++] = 0;	/* pad with 0s */
		key = newkey;
	}
	if (NULL == cmac_ctx)
		cmac_ctx = CMAC_CTX_new();
	CMAC_resume(cmac_ctx);
        if (!CMAC_Init(cmac_ctx, key, keylen, cipher, NULL)) {
                /* Shouldn't happen.  Does if wrong key_size. */
		Py_RETURN_NONE;
        }
        CMAC_Update(cmac_ctx, data, (unsigned int)datalen);
        CMAC_Final(cmac_ctx, mac, &maclen);
        if (MAX_BARE_MAC_LENGTH < maclen)
                maclen = MAX_BARE_MAC_LENGTH;
#if PY_MAJOR_VERSION >= 3
	return Py_BuildValue("y#", &mac, maclen);
#else
	return Py_BuildValue("s#", &mac, maclen);
#endif
}

/* List of functions defined in the module */

static PyMethodDef ntpc_methods[] = {
    {"setprogname",    ntpc_setprogname,  	METH_VARARGS,
     PyDoc_STR("Set program name for logging purposes.")},
    {"statustoa",      	ntpc_statustoa,  	METH_VARARGS,
     PyDoc_STR("Status string display from peer status word.")},
    {"prettydate",     	ntpc_prettydate,  	METH_VARARGS,
     PyDoc_STR("Convert a time stamp to something readable.")},
    {"lfptofloat",     	ntpc_lfptofloat,  	METH_VARARGS,
     PyDoc_STR("NTP l_fp to Python-style float time.")},
    {"set_tod",     	ntpc_set_tod,   	METH_VARARGS,
     PyDoc_STR("Set time to nanosecond precision.")},
    {"adj_systime",    	ntpc_adj_systime,   	METH_VARARGS,
     PyDoc_STR("Adjust system time by slewing.")},
    {"step_systime",    ntpc_step_systime,   	METH_VARARGS,
     PyDoc_STR("Adjust system time by stepping.")},
    {"checkname",       ntpc_checkname,   	METH_VARARGS,
     PyDoc_STR("Check if name is a valid algorithm name")},
    {"mac",             ntpc_mac,   		METH_VARARGS,
     PyDoc_STR("Compute HMAC or CMAC from data, key, and algorithm name")},
    {NULL,		NULL, 0, NULL}		/* sentinel */
};

PyDoc_STRVAR(module_doc,
"Python wrapper for selected libntp C library routines.\n\
");

/* banishes pointless compiler warnings on various Python versions */
extern PyMODINIT_FUNC initntpc(void);
extern PyMODINIT_FUNC PyInit_ntpc(void);

// cppcheck-suppress unusedFunction
NTPSEC_PY_MODULE_INIT(ntpc)
{
	PyObject *m;

	/* Create the module and add the functions */
	NTPSEC_PY_MODULE_DEF(m, "ntpc", module_doc, ntpc_methods)

	    /* for statustoa() */
	    PyModule_AddIntConstant(m, "TYPE_SYS", TYPE_SYS);
	PyModule_AddIntConstant(m, "TYPE_PEER", TYPE_PEER);
	PyModule_AddIntConstant(m, "TYPE_CLOCK", TYPE_CLOCK);

	if (m == NULL)
		return NTPSEC_PY_MODULE_ERROR_VAL;

	return NTPSEC_PY_MODULE_SUCCESS_VAL(m);
}
