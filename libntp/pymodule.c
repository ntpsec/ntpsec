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
#include "timespecops.h"

#include "ntp_config.h"
#include "ntp_assert.h"
#include "isc/error.h"
#include "isc/formatcheck.h"

#include "ntp_control.h"

#include "python_compatibility.h"

/*
 * Client utility functions
 */

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
	return Py_BuildValue("s", prettydate(&ts));
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

/* List of functions defined in the module */

static PyMethodDef ntpc_methods[] = {
    {"statustoa",      	ntpc_statustoa,  	METH_VARARGS,
     PyDoc_STR("Status string display from peer status word.")},
    {"prettydate",     	ntpc_prettydate,  	METH_VARARGS,
     PyDoc_STR("Convert a time stamp to something readable.")},
    {"lfptofloat",     	ntpc_lfptofloat,  	METH_VARARGS,
     PyDoc_STR("NTP l_fp to Python-style float time.")},
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
