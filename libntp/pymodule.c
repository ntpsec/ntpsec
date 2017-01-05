/*
 * This file is Copyright (c) 2016 by the NTPsec project
 * SPDX-License-Identifier: BSD-2-clause
 *
 * Python binding for selected libntp library functions
 */
#include <Python.h>

#include <config.h>

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_random.h"
#include "ntp_syslog.h"
#include "timespecops.h"

#include "ntp_config.h"
#include "ntp_assert.h"
#include "isc/error.h"
#include "isc/formatcheck.h"

#include "ntp_control.h"

#include "python_compatibility.h"

char *progname = "libntpc";

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
     * impedimenta.  If we ever want finer-grained control, that
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
    tt = lfp_stamp_to_tspec(ts, NULL);
    return Py_BuildValue("d", tt.tv_sec + tt.tv_nsec * 1e-9);
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
    return Py_BuildValue("d", (int)adj_systime(adjustment, adjtime));
}

static PyObject *
ntpc_step_systime(PyObject *self, PyObject *args)
{
    double adjustment;

    UNUSED_ARG(self);
    if (!PyArg_ParseTuple(args, "d", &adjustment))
	return NULL;
    return Py_BuildValue("d", step_systime(adjustment, ntp_set_tod));
}

long ntp_random(void)
/* stub random function for get_systime() */
{
    return 0;
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
    {NULL,		NULL, 0, NULL}		/* sentinel */
};

PyDoc_STRVAR(module_doc,
"Python wrapper for selected libntp C library routines.\n\
");

/* banishes a pointless compiler warning */
extern PyMODINIT_FUNC initntpc(void);

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
