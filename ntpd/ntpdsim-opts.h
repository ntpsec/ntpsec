/*   -*- buffer-read-only: t -*- vi: set ro:
 *  
 *  DO NOT EDIT THIS FILE   (ntpdsim-opts.h)
 *  
 *  It has been AutoGen-ed  Sunday August 20, 2006 at 08:09:50 PM EDT
 *  From the definitions    ntpdsim-opts.def
 *  and the template file   options
 */

/*
 *  This file was produced by an AutoOpts template.  AutoOpts is a
 *  copyrighted work.  This header file is not encumbered by AutoOpts
 *  licensing, but is provided under the licensing terms chosen by the
 *  ntpdsim author or copyright holder.  AutoOpts is licensed under
 *  the terms of the LGPL.  The redistributable library (``libopts'') is
 *  licensed under the terms of either the LGPL or, at the users discretion,
 *  the BSD license.  See the AutoOpts and/or libopts sources for details.
 *
 * This source file is copyrighted and licensed under the following terms:
 *
 * ntpdsim copyright 1970-2006 ntp.org - all rights reserved
 *
 * see html/copyright.html
 */
/*
 *  This file contains the programmatic interface to the Automated
 *  Options generated for the ntpdsim program.
 *  These macros are documented in the AutoGen info file in the
 *  "AutoOpts" chapter.  Please refer to that doc for usage help.
 */
#ifndef AUTOOPTS_NTPDSIM_OPTS_H_GUARD
#define AUTOOPTS_NTPDSIM_OPTS_H_GUARD
#include "config.h"
#include <autoopts/options.h>

/*
 *  Ensure that the library used for compiling this generated header is at
 *  least as new as the version current when the header template was released
 *  (not counting patch version increments).  Also ensure that the oldest
 *  tolerable version is at least as old as what was current when the header
 *  template was released.
 */
#define AO_TEMPLATE_VERSION 110592
#if (AO_TEMPLATE_VERSION < OPTIONS_MINIMUM_VERSION) \
 || (AO_TEMPLATE_VERSION > OPTIONS_STRUCT_VERSION)
# error option template version mismatches autoopts/options.h header
  Choke Me.
#endif

/*
 *  Enumeration of each option:
 */
typedef enum {
        INDEX_OPT_IPV4             =  0,
        INDEX_OPT_IPV6             =  1,
        INDEX_OPT_AUTHREQ          =  2,
        INDEX_OPT_AUTHNOREQ        =  3,
        INDEX_OPT_BCASTSYNC        =  4,
        INDEX_OPT_SIMBROADCASTDELAY =  5,
        INDEX_OPT_CONFIGFILE       =  6,
        INDEX_OPT_PHASENOISE       =  7,
        INDEX_OPT_DEBUG_LEVEL      =  8,
        INDEX_OPT_SET_DEBUG_LEVEL  =  9,
        INDEX_OPT_DRIFTFILE        = 10,
        INDEX_OPT_PANICGATE        = 11,
        INDEX_OPT_SIMSLEW          = 12,
        INDEX_OPT_JAILDIR          = 13,
        INDEX_OPT_INTERFACE        = 14,
        INDEX_OPT_KEYFILE          = 15,
        INDEX_OPT_LOGFILE          = 16,
        INDEX_OPT_NOVIRTUALIPS     = 17,
        INDEX_OPT_MODIFYMMTIMER    = 18,
        INDEX_OPT_NOFORK           = 19,
        INDEX_OPT_NICE             = 20,
        INDEX_OPT_SERVERTIME       = 21,
        INDEX_OPT_PIDFILE          = 22,
        INDEX_OPT_PRIORITY         = 23,
        INDEX_OPT_QUIT             = 24,
        INDEX_OPT_PROPAGATIONDELAY = 25,
        INDEX_OPT_UPDATEINTERVAL   = 26,
        INDEX_OPT_STATSDIR         = 27,
        INDEX_OPT_ENDSIMTIME       = 28,
        INDEX_OPT_TRUSTEDKEY       = 29,
        INDEX_OPT_FREQERR          = 30,
        INDEX_OPT_WALKNOISE        = 31,
        INDEX_OPT_USER             = 32,
        INDEX_OPT_VAR              = 33,
        INDEX_OPT_DVAR             = 34,
        INDEX_OPT_SLEW             = 35,
        INDEX_OPT_NDELAY           = 36,
        INDEX_OPT_PDELAY           = 37,
        INDEX_OPT_VERSION          = 38,
        INDEX_OPT_HELP             = 39,
        INDEX_OPT_MORE_HELP        = 40,
        INDEX_OPT_SAVE_OPTS        = 41,
        INDEX_OPT_LOAD_OPTS        = 42
} teOptIndex;

#define OPTION_CT    43
#define NTPDSIM_VERSION       "4.2.3p34"
#define NTPDSIM_FULL_VERSION  "ntpdsim - NTP daemon simulation program - Ver. 4.2.3p34"

/*
 *  Interface defines for all options.  Replace "n" with
 *  the UPPER_CASED option name (as in the teOptIndex
 *  enumeration above).  e.g. HAVE_OPT( IPV4 )
 */
#define        DESC(n) ntpdsimOptions.pOptDesc[INDEX_OPT_ ## n]
#define     HAVE_OPT(n) (! UNUSED_OPT(& DESC(n)))
#define      OPT_ARG(n) (DESC(n).pzLastArg)
#define    STATE_OPT(n) (DESC(n).fOptState & OPTST_SET_MASK)
#define    COUNT_OPT(n) (DESC(n).optOccCt)
#define    ISSEL_OPT(n) (SELECTED_OPT(&DESC(n)))
#define ISUNUSED_OPT(n) (UNUSED_OPT(& DESC(n)))
#define  ENABLED_OPT(n) (! DISABLED_OPT(& DESC(n)))
#define  STACKCT_OPT(n) (((tArgList*)(DESC(n).optCookie))->useCt)
#define STACKLST_OPT(n) (((tArgList*)(DESC(n).optCookie))->apzArgs)
#define    CLEAR_OPT(n) STMTS( \
                DESC(n).fOptState &= OPTST_PERSISTENT;   \
                if ( (DESC(n).fOptState & OPTST_INITENABLED) == 0) \
                    DESC(n).fOptState |= OPTST_DISABLED; \
                DESC(n).optCookie = NULL )

/*
 *  Interface defines for specific options.
 */
#define VALUE_OPT_IPV4           '4'
#define WHICH_OPT_IPV4           (DESC(IPV4).optActualValue)
#define WHICH_IDX_IPV4           (DESC(IPV4).optActualIndex)
#define VALUE_OPT_IPV6           '6'
#define VALUE_OPT_AUTHREQ        'a'
#define VALUE_OPT_AUTHNOREQ      'A'
#define VALUE_OPT_BCASTSYNC      'b'
#define VALUE_OPT_SIMBROADCASTDELAY 'B'
#define VALUE_OPT_CONFIGFILE     'c'
#define VALUE_OPT_PHASENOISE     'C'
#ifdef DEBUG
#define VALUE_OPT_DEBUG_LEVEL    'd'
#endif /* DEBUG */
#ifdef DEBUG
#define VALUE_OPT_SET_DEBUG_LEVEL 'D'
#endif /* DEBUG */
#define VALUE_OPT_DRIFTFILE      'f'
#define VALUE_OPT_PANICGATE      'g'
#define VALUE_OPT_SIMSLEW        'H'
#define VALUE_OPT_JAILDIR        'i'
#define VALUE_OPT_INTERFACE      'I'
#define VALUE_OPT_KEYFILE        'k'
#define VALUE_OPT_LOGFILE        'l'
#define VALUE_OPT_NOVIRTUALIPS   'L'
#ifdef SYS_WINNT
#define VALUE_OPT_MODIFYMMTIMER  'M'
#endif /* SYS_WINNT */
#define VALUE_OPT_NOFORK         'n'
#define VALUE_OPT_NICE           'N'
#define VALUE_OPT_SERVERTIME     'O'
#define VALUE_OPT_PIDFILE        'p'
#define VALUE_OPT_PRIORITY       'P'
#define OPT_VALUE_PRIORITY       (*(unsigned long*)(&DESC(PRIORITY).pzLastArg))
#define VALUE_OPT_QUIT           'q'
#define VALUE_OPT_PROPAGATIONDELAY 'r'
#define VALUE_OPT_UPDATEINTERVAL 'U'
#define OPT_VALUE_UPDATEINTERVAL (*(unsigned long*)(&DESC(UPDATEINTERVAL).pzLastArg))
#define VALUE_OPT_STATSDIR       's'
#define VALUE_OPT_ENDSIMTIME     'S'
#define VALUE_OPT_TRUSTEDKEY     't'
#define VALUE_OPT_FREQERR        'T'
#define VALUE_OPT_WALKNOISE      'W'
#define VALUE_OPT_USER           'u'
#define VALUE_OPT_VAR            'v'
#define VALUE_OPT_DVAR           'V'
#define VALUE_OPT_SLEW           'x'
#define VALUE_OPT_NDELAY         'Y'
#define VALUE_OPT_PDELAY         'Z'

#define VALUE_OPT_VERSION       'v'
#define VALUE_OPT_SAVE_OPTS     '>'
#define VALUE_OPT_LOAD_OPTS     '<'
#define VALUE_OPT_HELP          '?'
#define VALUE_OPT_MORE_HELP     '!'
#define SET_OPT_SAVE_OPTS(a)   STMTS( \
        DESC(SAVE_OPTS).fOptState &= OPTST_PERSISTENT; \
        DESC(SAVE_OPTS).fOptState |= OPTST_SET; \
        DESC(SAVE_OPTS).pzLastArg  = (const char*)(a) )
/*
 *  Interface defines not associated with particular options
 */
#define ERRSKIP_OPTERR  STMTS( ntpdsimOptions.fOptSet &= ~OPTPROC_ERRSTOP )
#define ERRSTOP_OPTERR  STMTS( ntpdsimOptions.fOptSet |= OPTPROC_ERRSTOP )
#define RESTART_OPT(n)  STMTS( \
                ntpdsimOptions.curOptIdx = (n); \
                ntpdsimOptions.pzCurOpt  = NULL )
#define START_OPT       RESTART_OPT(1)
#define USAGE(c)        (*ntpdsimOptions.pUsageProc)( &ntpdsimOptions, c )
/* extracted from opthead near line 288 */

/* * * * * *
 *
 *  Declare the ntpdsim option descriptor.
 */
#ifdef  __cplusplus
extern "C" {
#endif

extern tOptions   ntpdsimOptions;

#ifndef _
#  if ENABLE_NLS
#    include <stdio.h>
     static inline char* aoGetsText( const char* pz ) {
         if (pz == NULL) return NULL;
         return (char*)gettext( pz );
     }
#    define _(s)  aoGetsText(s)
#  else  /* ENABLE_NLS */
#    define _(s)  s
#  endif /* ENABLE_NLS */
#endif

#ifdef  __cplusplus
}
#endif
#endif /* AUTOOPTS_NTPDSIM_OPTS_H_GUARD */
/*
 * Local Variables:
 * Mode: C
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * options.h ends here */
