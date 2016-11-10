/*
 * python_compatibility.h -- macros for Python 2/3 compatibility
 *
 * This file is Copyright (c) 2016 by the NTPsec project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 *
 * Definitions based on examples in "Supporting Python 3 - The Book Site"
 *     http://python3porting.com/cextensions.html
 */

#ifndef GUARD_PYTHON_COMPATIBILITY_H
#define GUARD_PYTHON_COMPATIBILITY_H

#include <Python.h>

#if PY_MAJOR_VERSION >= 3

#define NTPSEC_PY_MODULE_INIT(name) PyMODINIT_FUNC PyInit_##name(void)

#define NTPSEC_PY_MODULE_DEF(mod, name, doc, methods) \
    static struct PyModuleDef moduledef = { \
        PyModuleDef_HEAD_INIT, name, doc, -1, methods, NULL, NULL, NULL, NULL}; \
    mod = PyModule_Create(&moduledef);

#define NTPSEC_PY_MODULE_ERROR_VAL NULL
#define NTPSEC_PY_MODULE_SUCCESS_VAL(val) val

#define NTPSEC_PY_BYTE_FORMAT "y#"

#else /* !Python 3 */

#define NTPSEC_PY_MODULE_INIT(name) PyMODINIT_FUNC init##name(void)

#define NTPSEC_PY_MODULE_DEF(mod, name, doc, methods) \
    mod = Py_InitModule3(name, methods, doc);

#define NTPSEC_PY_MODULE_ERROR_VAL
#define NTPSEC_PY_MODULE_SUCCESS_VAL(val)

#define NTPSEC_PY_BYTE_FORMAT "s#"

#endif /* !Python 3 */

#endif /* GUARD_PYTHON_COMPATIBILITY_H */
