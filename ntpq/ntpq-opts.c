/*  
 *  EDIT THIS FILE WITH CAUTION  (ntpq-opts.c)
 *  
 *  It has been AutoGen-ed  March 23, 2012 at 11:22:42 AM by AutoGen 5.14
 *  From the definitions    ntpq-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 36:1:11 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the ntpq author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The ntpq program is copyrighted and licensed
 * under the following terms:
 *
 *  Copyright (C) 1970-2012 The University of Delaware, all rights reserved.
 *  This is free software. It is licensed for use, modification and
 *  redistribution under the terms of the NTP License, copies of which
 *  can be seen at:
 *    <http://ntp.org/license>
 *    <http://opensource.org/licenses/ntp-license.php>
 *
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted,
 *  provided that the above copyright notice appears in all copies and that
 *  both the copyright notice and this permission notice appear in
 *  supporting documentation, and that the name The University of Delaware not be used in
 *  advertising or publicity pertaining to distribution of the software
 *  without specific, written prior permission. The University of Delaware makes no
 *  representations about the suitability this software for any purpose. It
 *  is provided "as is" without express or implied warranty.
 */

#include <sys/types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define OPTION_CODE_COMPILE 1
#include "ntpq-opts.h"
#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
#define zCopyright      (ntpq_opt_strs+0)
#define zLicenseDescrip (ntpq_opt_strs+314)

extern tUsageProc optionUsage;
/*
 *  global included definitions
 */#ifdef __windows
  extern int atoi(const char*);
#else
# include <stdlib.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

/*
 *  ntpq option static const strings
 */
static char const ntpq_opt_strs[1831] =
/*     0 */ "ntpq 4.2.7p267\n"
            "Copyright (C) 1970-2012 The University of Delaware, all rights reserved.\n"
            "This is free software. It is licensed for use, modification and\n"
            "redistribution under the terms of the NTP License, copies of which\n"
            "can be seen at:\n"
            "  <http://ntp.org/license>\n"
            "  <http://opensource.org/licenses/ntp-license.php>\n\0"
/*   314 */ "Permission to use, copy, modify, and distribute this software and its\n"
            "documentation for any purpose with or without fee is hereby granted,\n"
            "provided that the above copyright notice appears in all copies and that\n"
            "both the copyright notice and this permission notice appear in supporting\n"
            "documentation, and that the name The University of Delaware not be used in\n"
            "advertising or publicity pertaining to distribution of the software\n"
            "without specific, written prior permission. The University of Delaware\n"
            "makes no representations about the suitability this software for any\n"
            "purpose. It is provided \"as is\" without express or implied warranty.\n\0"
/*   952 */ "Force IPv4 DNS name resolution\0"
/*   983 */ "IPV4\0"
/*   988 */ "ipv4\0"
/*   993 */ "Force IPv6 DNS name resolution\0"
/*  1024 */ "IPV6\0"
/*  1029 */ "ipv6\0"
/*  1034 */ "run a command and exit\0"
/*  1057 */ "COMMAND\0"
/*  1065 */ "command\0"
/*  1073 */ "Increase debug verbosity level\0"
/*  1104 */ "DEBUG_LEVEL\0"
/*  1116 */ "debug-level\0"
/*  1128 */ "Set the debug verbosity level\0"
/*  1158 */ "SET_DEBUG_LEVEL\0"
/*  1174 */ "set-debug-level\0"
/*  1190 */ "Print a list of the peers\0"
/*  1216 */ "PEERS\0"
/*  1222 */ "peers\0"
/*  1228 */ "Force ntpq to operate in interactive mode\0"
/*  1270 */ "INTERACTIVE\0"
/*  1282 */ "interactive\0"
/*  1294 */ "numeric host addresses\0"
/*  1317 */ "NUMERIC\0"
/*  1325 */ "numeric\0"
/*  1333 */ "Always output status line with readvar\0"
/*  1372 */ "OLD_RV\0"
/*  1379 */ "old-rv\0"
/*  1386 */ "Display extended usage information and exit\0"
/*  1430 */ "help\0"
/*  1435 */ "Extended usage information passed thru pager\0"
/*  1480 */ "more-help\0"
/*  1490 */ "Output version information and exit\0"
/*  1526 */ "version\0"
/*  1534 */ "Save the option state to a config file\0"
/*  1573 */ "save-opts\0"
/*  1583 */ "Load options from a config file\0"
/*  1615 */ "LOAD_OPTS\0"
/*  1625 */ "no-load-opts\0"
/*  1638 */ "no\0"
/*  1641 */ "NTPQ\0"
/*  1646 */ "ntpq - standard NTP query program - Ver. 4.2.7p267\n"
            "USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... [ host ...]\n\0"
/*  1767 */ ".ntprc\0"
/*  1774 */ "$HOME\0"
/*  1780 */ ".\0"
/*  1782 */ "http://bugs.ntp.org, bugs@ntp.org\0"
/*  1816 */ "ntpq 4.2.7p267";

/*
 *  ipv4 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV4_DESC      (ntpq_opt_strs+952)
#define IPV4_NAME      (ntpq_opt_strs+983)
#define IPV4_name      (ntpq_opt_strs+988)
static int const aIpv4CantList[] = {
    INDEX_OPT_IPV6, NO_EQUIVALENT };
#define IPV4_FLAGS     (OPTST_DISABLED)

/*
 *  ipv6 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV6_DESC      (ntpq_opt_strs+993)
#define IPV6_NAME      (ntpq_opt_strs+1024)
#define IPV6_name      (ntpq_opt_strs+1029)
static int const aIpv6CantList[] = {
    INDEX_OPT_IPV4, NO_EQUIVALENT };
#define IPV6_FLAGS     (OPTST_DISABLED)

/*
 *  command option description:
 */
#define COMMAND_DESC      (ntpq_opt_strs+1034)
#define COMMAND_NAME      (ntpq_opt_strs+1057)
#define COMMAND_name      (ntpq_opt_strs+1065)
#define COMMAND_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  debug-level option description:
 */
#define DEBUG_LEVEL_DESC      (ntpq_opt_strs+1073)
#define DEBUG_LEVEL_NAME      (ntpq_opt_strs+1104)
#define DEBUG_LEVEL_name      (ntpq_opt_strs+1116)
#define DEBUG_LEVEL_FLAGS     (OPTST_DISABLED)

/*
 *  set-debug-level option description:
 */
#define SET_DEBUG_LEVEL_DESC      (ntpq_opt_strs+1128)
#define SET_DEBUG_LEVEL_NAME      (ntpq_opt_strs+1158)
#define SET_DEBUG_LEVEL_name      (ntpq_opt_strs+1174)
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  peers option description with
 *  "Must also have options" and "Incompatible options":
 */
#define PEERS_DESC      (ntpq_opt_strs+1190)
#define PEERS_NAME      (ntpq_opt_strs+1216)
#define PEERS_name      (ntpq_opt_strs+1222)
static int const aPeersCantList[] = {
    INDEX_OPT_INTERACTIVE, NO_EQUIVALENT };
#define PEERS_FLAGS     (OPTST_DISABLED)

/*
 *  interactive option description with
 *  "Must also have options" and "Incompatible options":
 */
#define INTERACTIVE_DESC      (ntpq_opt_strs+1228)
#define INTERACTIVE_NAME      (ntpq_opt_strs+1270)
#define INTERACTIVE_name      (ntpq_opt_strs+1282)
static int const aInteractiveCantList[] = {
    INDEX_OPT_COMMAND,
    INDEX_OPT_PEERS, NO_EQUIVALENT };
#define INTERACTIVE_FLAGS     (OPTST_DISABLED)

/*
 *  numeric option description:
 */
#define NUMERIC_DESC      (ntpq_opt_strs+1294)
#define NUMERIC_NAME      (ntpq_opt_strs+1317)
#define NUMERIC_name      (ntpq_opt_strs+1325)
#define NUMERIC_FLAGS     (OPTST_DISABLED)

/*
 *  old-rv option description:
 */
#define OLD_RV_DESC      (ntpq_opt_strs+1333)
#define OLD_RV_NAME      (ntpq_opt_strs+1372)
#define OLD_RV_name      (ntpq_opt_strs+1379)
#define OLD_RV_FLAGS     (OPTST_DISABLED)

/*
 *  Help/More_Help/Version option descriptions:
 */
#define HELP_DESC       (ntpq_opt_strs+1386)
#define HELP_name       (ntpq_opt_strs+1430)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (ntpq_opt_strs+1435)
#define MORE_HELP_name  (ntpq_opt_strs+1480)
#define MORE_HELP_FLAGS (OPTST_IMM | OPTST_NO_INIT)
#else
#define MORE_HELP_DESC  NULL
#define MORE_HELP_name  NULL
#define MORE_HELP_FLAGS (OPTST_OMITTED | OPTST_NO_INIT)
#endif
#ifdef NO_OPTIONAL_OPT_ARGS
#  define VER_FLAGS     (OPTST_IMM | OPTST_NO_INIT)
#else
#  define VER_FLAGS     (OPTST_SET_ARGTYPE(OPARG_TYPE_STRING) | \
                         OPTST_ARG_OPTIONAL | OPTST_IMM | OPTST_NO_INIT)
#endif
#define VER_DESC        (ntpq_opt_strs+1490)
#define VER_name        (ntpq_opt_strs+1526)
#define SAVE_OPTS_DESC  (ntpq_opt_strs+1534)
#define SAVE_OPTS_name  (ntpq_opt_strs+1573)
#define LOAD_OPTS_DESC     (ntpq_opt_strs+1583)
#define LOAD_OPTS_NAME     (ntpq_opt_strs+1615)
#define NO_LOAD_OPTS_name  (ntpq_opt_strs+1625)
#define LOAD_OPTS_pfx      (ntpq_opt_strs+1638)
#define LOAD_OPTS_name     (NO_LOAD_OPTS_name + 3)
/*
 *  Declare option callback procedures
 */
#if defined(TEST_NTPQ_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
extern tOptProc
    optionStackArg;
static tOptProc
    doUsageOpt;

/*
 *  #define map the "normal" callout procs to the test ones...
 */
#define COMMAND_OPT_PROC optionStackArg
#define SET_DEBUG_LEVEL_OPT_PROC optionStackArg
#define PEERS_OPT_PROC optionStackArg


#else /* NOT defined TEST_NTPQ_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    ntpOptionPrintVersion,   ntpq_custom_opt_handler, optionBooleanVal,
    optionNestedVal,         optionNumericVal,        optionPagedUsage,
    optionResetOpt,          optionStackArg,          optionTimeDate,
    optionTimeVal,           optionUnstackArg,        optionVendorOption,
    optionVersionStderr;
static tOptProc
    doOptSet_Debug_Level, doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define COMMAND_OPT_PROC ntpq_custom_opt_handler
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level
#define PEERS_OPT_PROC ntpq_custom_opt_handler

#define COMMAND_OPT_PROC ntpq_custom_opt_handler
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level
#define PEERS_OPT_PROC ntpq_custom_opt_handler
#endif /* defined(TEST_NTPQ_OPTS) */
#define VER_PROC        ntpOptionPrintVersion

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntpq Option Descriptions.
 */
static tOptDesc optDesc[OPTION_CT] = {
  {  /* entry idx, value */ 0, VALUE_OPT_IPV4,
     /* equiv idx, value */ 0, VALUE_OPT_IPV4,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV4_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --ipv4 */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aIpv4CantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ IPV4_DESC, IPV4_NAME, IPV4_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_IPV6,
     /* equiv idx, value */ 1, VALUE_OPT_IPV6,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV6_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --ipv6 */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aIpv6CantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ IPV6_DESC, IPV6_NAME, IPV6_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_COMMAND,
     /* equiv idx, value */ 2, VALUE_OPT_COMMAND,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ COMMAND_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --command */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ COMMAND_OPT_PROC,
     /* desc, NAME, name */ COMMAND_DESC, COMMAND_NAME, COMMAND_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_DEBUG_LEVEL,
     /* equiv idx, value */ 3, VALUE_OPT_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ DEBUG_LEVEL_DESC, DEBUG_LEVEL_NAME, DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equiv idx, value */ 4, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ SET_DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --set-debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ SET_DEBUG_LEVEL_OPT_PROC,
     /* desc, NAME, name */ SET_DEBUG_LEVEL_DESC, SET_DEBUG_LEVEL_NAME, SET_DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_PEERS,
     /* equiv idx, value */ 5, VALUE_OPT_PEERS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PEERS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --peers */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aPeersCantList,
     /* option proc      */ PEERS_OPT_PROC,
     /* desc, NAME, name */ PEERS_DESC, PEERS_NAME, PEERS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 6, VALUE_OPT_INTERACTIVE,
     /* equiv idx, value */ 6, VALUE_OPT_INTERACTIVE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ INTERACTIVE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --interactive */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aInteractiveCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ INTERACTIVE_DESC, INTERACTIVE_NAME, INTERACTIVE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 7, VALUE_OPT_NUMERIC,
     /* equiv idx, value */ 7, VALUE_OPT_NUMERIC,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NUMERIC_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --numeric */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ NUMERIC_DESC, NUMERIC_NAME, NUMERIC_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 8, VALUE_OPT_OLD_RV,
     /* equiv idx, value */ 8, VALUE_OPT_OLD_RV,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OLD_RV_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --old-rv */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ OLD_RV_DESC, OLD_RV_NAME, OLD_RV_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_VERSION, VALUE_OPT_VERSION,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_VERSION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ VER_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ VER_PROC,
     /* desc, NAME, name */ VER_DESC, NULL, VER_name,
     /* disablement strs */ NULL, NULL },



  {  /* entry idx, value */ INDEX_OPT_HELP, VALUE_OPT_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_HELP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_IMM | OPTST_NO_INIT, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doUsageOpt,
     /* desc, NAME, name */ HELP_DESC, NULL, HELP_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_MORE_HELP, VALUE_OPT_MORE_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_MORE_HELP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MORE_HELP_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL,  NULL,
     /* option proc      */ optionPagedUsage,
     /* desc, NAME, name */ MORE_HELP_DESC, NULL, MORE_HELP_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_SAVE_OPTS, VALUE_OPT_SAVE_OPTS,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_SAVE_OPTS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_SET_ARGTYPE(OPARG_TYPE_STRING)
                          | OPTST_ARG_OPTIONAL | OPTST_NO_INIT, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL,  NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SAVE_OPTS_DESC, NULL, SAVE_OPTS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_LOAD_OPTS, VALUE_OPT_LOAD_OPTS,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_LOAD_OPTS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ OPTST_SET_ARGTYPE(OPARG_TYPE_STRING)
			  | OPTST_DISABLE_IMM, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionLoadOpt,
     /* desc, NAME, name */ LOAD_OPTS_DESC, LOAD_OPTS_NAME, LOAD_OPTS_name,
     /* disablement strs */ NO_LOAD_OPTS_name, LOAD_OPTS_pfx }
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntpq Option Environment
 */
#define zPROGNAME       (ntpq_opt_strs+1641)
#define zUsageTitle     (ntpq_opt_strs+1646)
#define zRcName         (ntpq_opt_strs+1767)
static char const * const apzHomeList[3] = {
    ntpq_opt_strs+1774,
    ntpq_opt_strs+1780,
    NULL };
#define zBugsAddr       (ntpq_opt_strs+1782)
#define zExplain        (NULL)
#define zDetail         (NULL)
#define zFullVersion    (ntpq_opt_strs+1816)
/* extracted from optcode.tlib near line 315 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define ntpq_full_usage (NULL)

#define ntpq_short_usage (NULL)

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(tOptions * pOptions, tOptDesc * pOptDesc)
{
    (void)pOptions;
    USAGE(NTPQ_EXIT_SUCCESS);
}

#if ! defined(TEST_NTPQ_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the set-debug-level option.
 */
static void
doOptSet_Debug_Level(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from debug-opt.def, line 26 */
DESC(DEBUG_LEVEL).optOccCt = atoi( pOptDesc->pzLastArg );
}
#endif /* defined(TEST_NTPQ_OPTS) */
/* extracted from optmain.tlib near line 128 */

#if defined(TEST_NTPQ_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell(tOptions*);

int
main(int argc, char ** argv)
{
    int res = NTPQ_EXIT_SUCCESS;
    (void)optionProcess(&ntpqOptions, argc, argv);
    optionPutShell(&ntpqOptions);
    res = ferror(stdout);
    if (res != 0)
        fputs("output error writing to stdout\n", stderr);
    return res;
}
#endif  /* defined TEST_NTPQ_OPTS */
/* extracted from optmain.tlib near line 1148 */

#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

#ifndef  WITH_PACKAGER
# define ntpq_packager_info NULL
#else
static char const ntpq_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport ntpq bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif

tOptions ntpqOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_ENVIRON
    + OPTPROC_MISUSE ),
    0, NULL,                    /* current option index, current option */
    NULL,         NULL,         zPROGNAME,
    zRcName,      zCopyright,   zLicenseDescrip,
    zFullVersion, apzHomeList,  zUsageTitle,
    zExplain,     zDetail,      optDesc,
    zBugsAddr,                  /* address to send bugs to */
    NULL, NULL,                 /* extensions/saved state  */
    optionUsage, /* usage procedure */
    translate_option_strings,   /* translation procedure */
    /*
     *  Indexes to special options
     */
    { INDEX_OPT_MORE_HELP, /* more-help option index */
      INDEX_OPT_SAVE_OPTS, /* save option index */
      NO_EQUIVALENT, /* '-#' option index */
      NO_EQUIVALENT /* index of default opt */
    },
    14 /* full option count */, 9 /* user option count */,
    ntpq_full_usage, ntpq_short_usage,
    NULL, NULL,
    PKGDATADIR, ntpq_packager_info
};

#if ENABLE_NLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <autoopts/usage-txt.h>

static char* AO_gettext(char const* pz);
static void  coerce_it(void** s);

static char*
AO_gettext(char const* pz)
{
    char* pzRes;
    if (pz == NULL)
        return NULL;
    pzRes = _(pz);
    if (pzRes == pz)
        return pzRes;
    pzRes = strdup(pzRes);
    if (pzRes == NULL) {
        fputs(_("No memory for duping translated strings\n"), stderr);
        exit(NTPQ_EXIT_FAILURE);
    }
    return pzRes;
}

static void coerce_it(void** s) { *s = AO_gettext(*s);
}

/*
 *  This invokes the translation code (e.g. gettext(3)).
 */
static void
translate_option_strings(void)
{
    tOptions * const pOpt = &ntpqOptions;

    /*
     *  Guard against re-translation.  It won't work.  The strings will have
     *  been changed by the first pass through this code.  One shot only.
     */
    if (option_usage_text.field_ct != 0) {
        /*
         *  Do the translations.  The first pointer follows the field count
         *  field.  The field count field is the size of a pointer.
         */
        tOptDesc * pOD = pOpt->pOptDesc;
        char **    ppz = (char**)(void*)&(option_usage_text);
        int        ix  = option_usage_text.field_ct;

        do {
            ppz++;
            *ppz = AO_gettext(*ppz);
        } while (--ix > 0);

        coerce_it((void*)&(pOpt->pzCopyright));
        coerce_it((void*)&(pOpt->pzCopyNotice));
        coerce_it((void*)&(pOpt->pzFullVersion));
        coerce_it((void*)&(pOpt->pzUsageTitle));
        coerce_it((void*)&(pOpt->pzExplain));
        coerce_it((void*)&(pOpt->pzDetail));
        coerce_it((void*)&(pOpt->pzPackager));
        option_usage_text.field_ct = 0;

        for (ix = pOpt->optCt; ix > 0; ix--, pOD++)
            coerce_it((void*)&(pOD->pzText));
    }

    if ((pOpt->fOptSet & OPTPROC_NXLAT_OPT_CFG) == 0) {
        tOptDesc * pOD = pOpt->pOptDesc;
        int        ix;

        for (ix = pOpt->optCt; ix > 0; ix--, pOD++) {
            coerce_it((void*)&(pOD->pz_Name));
            coerce_it((void*)&(pOD->pz_DisableName));
            coerce_it((void*)&(pOD->pz_DisablePfx));
        }
        /* prevent re-translation */
        ntpqOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* ntpq-opts.c ends here */
