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
libntpc_statustoa(PyObject *self, PyObject *args)
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
libntpc_prettydate(PyObject *self, PyObject *args)
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

/* List of functions defined in the module */

static PyMethodDef libntpc_methods[] = {
    {"statustoa",      	libntpc_statustoa,  	METH_VARARGS,
     PyDoc_STR("Status string display from peer status word.")},
    {"prettydate",     	libntpc_prettydate,  	METH_VARARGS,
     PyDoc_STR("Status string display from peer status word.")},
    {NULL,		NULL, 0, NULL}		/* sentinel */
};

PyDoc_STRVAR(module_doc,
"Python wrapper for selected libntp C library routines.\n\
");

/* banishes a pointless compiler warning */
extern PyMODINIT_FUNC initlibntpc(void);

// cppcheck-suppress unusedFunction
NTPSEC_PY_MODULE_INIT(libntpc)
{
    PyObject *m;

    /* Create the module and add the functions */
    NTPSEC_PY_MODULE_DEF(m, "libntpc", module_doc, libntpc_methods)

    /* for statustoa() */ 
    PyModule_AddIntConstant(m, "TYPE_SYS", TYPE_SYS);
    PyModule_AddIntConstant(m, "TYPE_PEER", TYPE_PEER);
    PyModule_AddIntConstant(m, "TYPE_CLOCK", TYPE_CLOCK);

    if (m == NULL)
	return NTPSEC_PY_MODULE_ERROR_VAL;

    return NTPSEC_PY_MODULE_SUCCESS_VAL(m);
}
