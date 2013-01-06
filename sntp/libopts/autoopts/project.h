
/**
 * \file project.h
 *
 *  This file is part of AutoOpts, a companion to AutoGen.
 *  AutoOpts is free software.
 *  AutoOpts is Copyright (C) 1992-2013 by Bruce Korb - all rights reserved
 *
 *  AutoOpts is available under any one of two licenses.  The license
 *  in use must be one of these two and the choice is under the control
 *  of the user of the license.
 *
 *   The GNU Lesser General Public License, version 3 or later
 *      See the files "COPYING.lgplv3" and "COPYING.gplv3"
 *
 *   The Modified Berkeley Software Distribution License
 *      See the file "COPYING.mbsd"
 *
 *  These files have the following md5sums:
 *
 *  43b91e8ca915626ed3818ffb1b71248b pkg/libopts/COPYING.gplv3
 *  06a1a2e4760c90ea5e1dad8dfaac4d39 pkg/libopts/COPYING.lgplv3
 *  66a5cedaf62c4b2637025f049f9b826f pkg/libopts/COPYING.mbsd
 */

#ifndef AUTOGEN_PROJECT_H
#define AUTOGEN_PROJECT_H

#include "config.h"
#include "compat/compat.h"
#include "ag-char-map.h"

/*
 *  Procedure success codes
 *
 *  USAGE:  define procedures to return "tSuccess".  Test their results
 *          with the SUCCEEDED, FAILED and HADGLITCH macros.
 *
 *  Microsoft sticks its nose into user space here, so for Windows' sake,
 *  make sure all of these are undefined.
 */
#undef  SUCCESS
#undef  FAILURE
#undef  PROBLEM
#undef  SUCCEEDED
#undef  SUCCESSFUL
#undef  FAILED
#undef  HADGLITCH

#define SUCCESS  ((tSuccess) 0)
#define FAILURE  ((tSuccess)-1)
#define PROBLEM  ((tSuccess) 1)

typedef int tSuccess;

#define SUCCEEDED(p)    ((p) == SUCCESS)
#define SUCCESSFUL(p)   SUCCEEDED(p)
#define FAILED(p)       ((p) <  SUCCESS)
#define HADGLITCH(p)    ((p) >  SUCCESS)

#ifndef STR
#  define __STR(s)      #s
#  define STR(s)        __STR(s)
#endif

#ifdef DEFINING
#  define VALUE(s)      = s
#  define MODE
#else
#  define VALUE(s)
#  define MODE extern
#endif

#endif /* AUTOGEN_PROJECT_H */
/* end of project.h */
