/*  
 *  EDIT THIS FILE WITH CAUTION  (sntp-opts.c)
 *  
 *  It has been AutoGen-ed  August 31, 2012 at 02:54:19 AM by AutoGen 5.16.2
 *  From the definitions    sntp-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 36:5:11 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the sntp author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The sntp program is copyrighted and licensed
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

#ifndef __doxygen__
#define OPTION_CODE_COMPILE 1
#include "sntp-opts.h"
#include <sys/types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
#define zCopyright      (sntp_opt_strs+0)
#define zLicenseDescrip (sntp_opt_strs+314)

/*
 *  global included definitions
 */
#ifdef __windows
  extern int atoi(const char*);
#else
# include <stdlib.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

/*
 *  sntp option static const strings
 */
static char const sntp_opt_strs[2568] =
/*     0 */ "sntp 4.2.7p296\n"
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
            "advertising or publicity pertaining to distribution of the software without\n"
            "specific, written prior permission.  The University of Delaware makes no\n"
            "representations about the suitability this software for any purpose.  It is\n"
            "provided \"as is\" without express or implied warranty.\n\0"
/*   954 */ "Force IPv4 DNS name resolution\0"
/*   985 */ "IPV4\0"
/*   990 */ "ipv4\0"
/*   995 */ "Force IPv6 DNS name resolution\0"
/*  1026 */ "IPV6\0"
/*  1031 */ "ipv6\0"
/*  1036 */ "Enable authentication with the key auth-keynumber\0"
/*  1086 */ "AUTHENTICATION\0"
/*  1101 */ "authentication\0"
/*  1116 */ "The number of seconds to wait for broadcasts\0"
/*  1161 */ "BCTIMEOUT\0"
/*  1171 */ "bctimeout\0"
/*  1181 */ "Listen to the address specified for broadcast time sync\0"
/*  1237 */ "BROADCAST\0"
/*  1247 */ "broadcast\0"
/*  1257 */ "Concurrently query all IPs returned for host-name\0"
/*  1307 */ "CONCURRENT\0"
/*  1318 */ "concurrent\0"
/*  1329 */ "Increase debug verbosity level\0"
/*  1360 */ "DEBUG_LEVEL\0"
/*  1372 */ "debug-level\0"
/*  1384 */ "Set the debug verbosity level\0"
/*  1414 */ "SET_DEBUG_LEVEL\0"
/*  1430 */ "set-debug-level\0"
/*  1446 */ "The gap (in milliseconds) between time requests\0"
/*  1494 */ "GAP\0"
/*  1498 */ "gap\0"
/*  1502 */ "KoD history filename\0"
/*  1523 */ "KOD\0"
/*  1527 */ "kod\0"
/*  1531 */ "/var/db/ntp-kod\0"
/*  1547 */ "Look in this file for the key specified with -a\0"
/*  1595 */ "KEYFILE\0"
/*  1603 */ "keyfile\0"
/*  1611 */ "Log to specified logfile\0"
/*  1636 */ "LOGFILE\0"
/*  1644 */ "logfile\0"
/*  1652 */ "Adjustments less than steplimit msec will be slewed\0"
/*  1704 */ "STEPLIMIT\0"
/*  1714 */ "steplimit\0"
/*  1724 */ "Send int as our NTP version\0"
/*  1752 */ "NTPVERSION\0"
/*  1763 */ "ntpversion\0"
/*  1774 */ "Use the NTP Reserved Port (port 123)\0"
/*  1811 */ "USERESERVEDPORT\0"
/*  1827 */ "usereservedport\0"
/*  1843 */ "OK to 'step' the time with settimeofday(2)\0"
/*  1886 */ "STEP\0"
/*  1891 */ "step\0"
/*  1896 */ "OK to 'slew' the time with adjtime(2)\0"
/*  1934 */ "SLEW\0"
/*  1939 */ "slew\0"
/*  1944 */ "The number of seconds to wait for unicast responses\0"
/*  1996 */ "UCTIMEOUT\0"
/*  2006 */ "uctimeout\0"
/*  2016 */ "Wait for pending replies (if not setting the time)\0"
/*  2067 */ "WAIT\0"
/*  2072 */ "no-wait\0"
/*  2080 */ "no\0"
/*  2083 */ "Display extended usage information and exit\0"
/*  2127 */ "help\0"
/*  2132 */ "Extended usage information passed thru pager\0"
/*  2177 */ "more-help\0"
/*  2187 */ "Output version information and exit\0"
/*  2223 */ "version\0"
/*  2231 */ "Save the option state to a config file\0"
/*  2270 */ "save-opts\0"
/*  2280 */ "Load options from a config file\0"
/*  2312 */ "LOAD_OPTS\0"
/*  2322 */ "no-load-opts\0"
/*  2335 */ "SNTP\0"
/*  2340 */ "sntp - standard Simple Network Time Protocol client program - Ver. 4.2.7p296\n"
            "USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... \\\n"
            "\t\t[ hostname-or-IP ...]\n\0"
/*  2501 */ "$HOME\0"
/*  2507 */ ".\0"
/*  2509 */ ".ntprc\0"
/*  2516 */ "http://bugs.ntp.org, bugs@ntp.org\0"
/*  2550 */ "\n\n\0"
/*  2553 */ "sntp 4.2.7p296";

/*
 *  ipv4 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV4_DESC      (sntp_opt_strs+954)
#define IPV4_NAME      (sntp_opt_strs+985)
#define IPV4_name      (sntp_opt_strs+990)
static int const aIpv4CantList[] = {
    INDEX_OPT_IPV6, NO_EQUIVALENT };
#define IPV4_FLAGS     (OPTST_DISABLED)

/*
 *  ipv6 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV6_DESC      (sntp_opt_strs+995)
#define IPV6_NAME      (sntp_opt_strs+1026)
#define IPV6_name      (sntp_opt_strs+1031)
static int const aIpv6CantList[] = {
    INDEX_OPT_IPV4, NO_EQUIVALENT };
#define IPV6_FLAGS     (OPTST_DISABLED)

/*
 *  authentication option description:
 */
#define AUTHENTICATION_DESC      (sntp_opt_strs+1036)
#define AUTHENTICATION_NAME      (sntp_opt_strs+1086)
#define AUTHENTICATION_name      (sntp_opt_strs+1101)
#define AUTHENTICATION_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  bctimeout option description:
 */
#define BCTIMEOUT_DESC      (sntp_opt_strs+1116)
#define BCTIMEOUT_NAME      (sntp_opt_strs+1161)
#define BCTIMEOUT_name      (sntp_opt_strs+1171)
#define BCTIMEOUT_DFT_ARG   ((char const*)68)
#define BCTIMEOUT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  broadcast option description:
 */
#define BROADCAST_DESC      (sntp_opt_strs+1181)
#define BROADCAST_NAME      (sntp_opt_strs+1237)
#define BROADCAST_name      (sntp_opt_strs+1247)
#define BROADCAST_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  concurrent option description:
 */
#define CONCURRENT_DESC      (sntp_opt_strs+1257)
#define CONCURRENT_NAME      (sntp_opt_strs+1307)
#define CONCURRENT_name      (sntp_opt_strs+1318)
#define CONCURRENT_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  debug-level option description:
 */
#define DEBUG_LEVEL_DESC      (sntp_opt_strs+1329)
#define DEBUG_LEVEL_NAME      (sntp_opt_strs+1360)
#define DEBUG_LEVEL_name      (sntp_opt_strs+1372)
#define DEBUG_LEVEL_FLAGS     (OPTST_DISABLED)

/*
 *  set-debug-level option description:
 */
#define SET_DEBUG_LEVEL_DESC      (sntp_opt_strs+1384)
#define SET_DEBUG_LEVEL_NAME      (sntp_opt_strs+1414)
#define SET_DEBUG_LEVEL_name      (sntp_opt_strs+1430)
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  gap option description:
 */
#define GAP_DESC      (sntp_opt_strs+1446)
#define GAP_NAME      (sntp_opt_strs+1494)
#define GAP_name      (sntp_opt_strs+1498)
#define GAP_DFT_ARG   ((char const*)50)
#define GAP_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  kod option description:
 */
#define KOD_DESC      (sntp_opt_strs+1502)
#define KOD_NAME      (sntp_opt_strs+1523)
#define KOD_name      (sntp_opt_strs+1527)
#define KOD_DFT_ARG   (sntp_opt_strs+1531)
#define KOD_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  keyfile option description:
 */
#define KEYFILE_DESC      (sntp_opt_strs+1547)
#define KEYFILE_NAME      (sntp_opt_strs+1595)
#define KEYFILE_name      (sntp_opt_strs+1603)
#define KEYFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  logfile option description:
 */
#define LOGFILE_DESC      (sntp_opt_strs+1611)
#define LOGFILE_NAME      (sntp_opt_strs+1636)
#define LOGFILE_name      (sntp_opt_strs+1644)
#define LOGFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  steplimit option description:
 */
#define STEPLIMIT_DESC      (sntp_opt_strs+1652)
#define STEPLIMIT_NAME      (sntp_opt_strs+1704)
#define STEPLIMIT_name      (sntp_opt_strs+1714)
#define STEPLIMIT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  ntpversion option description:
 */
#define NTPVERSION_DESC      (sntp_opt_strs+1724)
#define NTPVERSION_NAME      (sntp_opt_strs+1752)
#define NTPVERSION_name      (sntp_opt_strs+1763)
#define NTPVERSION_DFT_ARG   ((char const*)4)
#define NTPVERSION_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  usereservedport option description:
 */
#define USERESERVEDPORT_DESC      (sntp_opt_strs+1774)
#define USERESERVEDPORT_NAME      (sntp_opt_strs+1811)
#define USERESERVEDPORT_name      (sntp_opt_strs+1827)
#define USERESERVEDPORT_FLAGS     (OPTST_DISABLED)

/*
 *  step option description:
 */
#define STEP_DESC      (sntp_opt_strs+1843)
#define STEP_NAME      (sntp_opt_strs+1886)
#define STEP_name      (sntp_opt_strs+1891)
#define STEP_FLAGS     (OPTST_DISABLED)

/*
 *  slew option description:
 */
#define SLEW_DESC      (sntp_opt_strs+1896)
#define SLEW_NAME      (sntp_opt_strs+1934)
#define SLEW_name      (sntp_opt_strs+1939)
#define SLEW_FLAGS     (OPTST_DISABLED)

/*
 *  uctimeout option description:
 */
#define UCTIMEOUT_DESC      (sntp_opt_strs+1944)
#define UCTIMEOUT_NAME      (sntp_opt_strs+1996)
#define UCTIMEOUT_name      (sntp_opt_strs+2006)
#define UCTIMEOUT_DFT_ARG   ((char const*)5)
#define UCTIMEOUT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  wait option description:
 */
#define WAIT_DESC      (sntp_opt_strs+2016)
#define WAIT_NAME      (sntp_opt_strs+2067)
#define NOT_WAIT_name  (sntp_opt_strs+2072)
#define NOT_WAIT_PFX   (sntp_opt_strs+2080)
#define WAIT_name      (NOT_WAIT_name + 3)
#define WAIT_FLAGS     (OPTST_INITENABLED)

/*
 *  Help/More_Help/Version option descriptions:
 */
#define HELP_DESC       (sntp_opt_strs+2083)
#define HELP_name       (sntp_opt_strs+2127)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (sntp_opt_strs+2132)
#define MORE_HELP_name  (sntp_opt_strs+2177)
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
#define VER_DESC        (sntp_opt_strs+2187)
#define VER_name        (sntp_opt_strs+2223)
#define SAVE_OPTS_DESC  (sntp_opt_strs+2231)
#define SAVE_OPTS_name  (sntp_opt_strs+2270)
#define LOAD_OPTS_DESC     (sntp_opt_strs+2280)
#define LOAD_OPTS_NAME     (sntp_opt_strs+2312)
#define NO_LOAD_OPTS_name  (sntp_opt_strs+2322)
#define LOAD_OPTS_pfx      (sntp_opt_strs+2080)
#define LOAD_OPTS_name     (NO_LOAD_OPTS_name + 3)
/*
 *  Declare option callback procedures
 */
#if defined(TEST_SNTP_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
static tOptProc
    doOptKeyfile,    doOptKod,        doOptLogfile,    doOptNtpversion,
    doOptSteplimit,  doUsageOpt;

/*
 *  #define map the "normal" callout procs to the test ones...
 */
#define SET_DEBUG_LEVEL_OPT_PROC optionStackArg


#else /* NOT defined TEST_SNTP_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    ntpOptionPrintVersion, optionBooleanVal,      optionNestedVal,
    optionNumericVal,      optionPagedUsage,      optionResetOpt,
    optionStackArg,        optionTimeDate,        optionTimeVal,
    optionUnstackArg,      optionVendorOption,    optionVersionStderr;
static tOptProc
    doOptKeyfile,         doOptKod,             doOptLogfile,
    doOptNtpversion,      doOptSet_Debug_Level, doOptSteplimit,
    doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level

#endif /* defined(TEST_SNTP_OPTS) */
#define VER_PROC        ntpOptionPrintVersion

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 *  Define the sntp Option Descriptions.
 * This is an array of OPTION_CT entries, one for each
 * option that the sntp program responds to.
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

  {  /* entry idx, value */ 2, VALUE_OPT_AUTHENTICATION,
     /* equiv idx, value */ 2, VALUE_OPT_AUTHENTICATION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ AUTHENTICATION_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --authentication */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ AUTHENTICATION_DESC, AUTHENTICATION_NAME, AUTHENTICATION_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_BCTIMEOUT,
     /* equiv idx, value */ 3, VALUE_OPT_BCTIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ BCTIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { BCTIMEOUT_DFT_ARG },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ BCTIMEOUT_DESC, BCTIMEOUT_NAME, BCTIMEOUT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_BROADCAST,
     /* equiv idx, value */ 4, VALUE_OPT_BROADCAST,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ BROADCAST_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --broadcast */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ BROADCAST_DESC, BROADCAST_NAME, BROADCAST_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_CONCURRENT,
     /* equiv idx, value */ 5, VALUE_OPT_CONCURRENT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ CONCURRENT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --concurrent */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ CONCURRENT_DESC, CONCURRENT_NAME, CONCURRENT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 6, VALUE_OPT_DEBUG_LEVEL,
     /* equiv idx, value */ 6, VALUE_OPT_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ DEBUG_LEVEL_DESC, DEBUG_LEVEL_NAME, DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 7, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equiv idx, value */ 7, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ SET_DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --set-debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ SET_DEBUG_LEVEL_OPT_PROC,
     /* desc, NAME, name */ SET_DEBUG_LEVEL_DESC, SET_DEBUG_LEVEL_NAME, SET_DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 8, VALUE_OPT_GAP,
     /* equiv idx, value */ 8, VALUE_OPT_GAP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ GAP_FLAGS, 0,
     /* last opt argumnt */ { GAP_DFT_ARG },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ GAP_DESC, GAP_NAME, GAP_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 9, VALUE_OPT_KOD,
     /* equiv idx, value */ 9, VALUE_OPT_KOD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KOD_FLAGS, 0,
     /* last opt argumnt */ { KOD_DFT_ARG },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptKod,
     /* desc, NAME, name */ KOD_DESC, KOD_NAME, KOD_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 10, VALUE_OPT_KEYFILE,
     /* equiv idx, value */ 10, VALUE_OPT_KEYFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KEYFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --keyfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptKeyfile,
     /* desc, NAME, name */ KEYFILE_DESC, KEYFILE_NAME, KEYFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 11, VALUE_OPT_LOGFILE,
     /* equiv idx, value */ 11, VALUE_OPT_LOGFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ LOGFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --logfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptLogfile,
     /* desc, NAME, name */ LOGFILE_DESC, LOGFILE_NAME, LOGFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 12, VALUE_OPT_STEPLIMIT,
     /* equiv idx, value */ 12, VALUE_OPT_STEPLIMIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ STEPLIMIT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --steplimit */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptSteplimit,
     /* desc, NAME, name */ STEPLIMIT_DESC, STEPLIMIT_NAME, STEPLIMIT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 13, VALUE_OPT_NTPVERSION,
     /* equiv idx, value */ 13, VALUE_OPT_NTPVERSION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NTPVERSION_FLAGS, 0,
     /* last opt argumnt */ { NTPVERSION_DFT_ARG },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptNtpversion,
     /* desc, NAME, name */ NTPVERSION_DESC, NTPVERSION_NAME, NTPVERSION_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 14, VALUE_OPT_USERESERVEDPORT,
     /* equiv idx, value */ 14, VALUE_OPT_USERESERVEDPORT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ USERESERVEDPORT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --usereservedport */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ USERESERVEDPORT_DESC, USERESERVEDPORT_NAME, USERESERVEDPORT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 15, VALUE_OPT_STEP,
     /* equiv idx, value */ 15, VALUE_OPT_STEP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ STEP_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --step */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ STEP_DESC, STEP_NAME, STEP_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 16, VALUE_OPT_SLEW,
     /* equiv idx, value */ 16, VALUE_OPT_SLEW,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SLEW_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --slew */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SLEW_DESC, SLEW_NAME, SLEW_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 17, VALUE_OPT_UCTIMEOUT,
     /* equiv idx, value */ 17, VALUE_OPT_UCTIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ UCTIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { UCTIMEOUT_DFT_ARG },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ UCTIMEOUT_DESC, UCTIMEOUT_NAME, UCTIMEOUT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 18, VALUE_OPT_WAIT,
     /* equiv idx, value */ 18, VALUE_OPT_WAIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ WAIT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --wait */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ WAIT_DESC, WAIT_NAME, WAIT_name,
     /* disablement strs */ NOT_WAIT_name, NOT_WAIT_PFX },

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
 *  Define the sntp Option Environment
 */
#define zPROGNAME       (sntp_opt_strs+2335)
#define zUsageTitle     (sntp_opt_strs+2340)
#define zRcName         (sntp_opt_strs+2509)
static char const * const apzHomeList[3] = {
    sntp_opt_strs+2501,
    sntp_opt_strs+2507,
    NULL };
#define zBugsAddr       (sntp_opt_strs+2516)
#define zExplain        (sntp_opt_strs+2550)
#define zDetail         (NULL)
#define zFullVersion    (sntp_opt_strs+2553)
/* extracted from optcode.tlib near line 350 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define sntp_full_usage (NULL)

#define sntp_short_usage (NULL)

#endif /* not defined __doxygen__ */

/*
 *  Create the static procedure(s) declared above.
 */
/**
 * The callout function that invokes the optionUsage function.
 *
 * @param pOptions the AutoOpts option description structure
 * @param pOptDesc the descriptor for the "help" (usage) option.
 * @noreturn
 */
static void
doUsageOpt(tOptions * pOptions, tOptDesc * pOptDesc)
{
    optionUsage(&sntpOptions, SNTP_EXIT_SUCCESS);
    /* NOTREACHED */
    (void)pOptDesc;
    (void)pOptions;
}

#if ! defined(TEST_SNTP_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the set-debug-level option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptSet_Debug_Level(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from debug-opt.def, line 26 */
DESC(DEBUG_LEVEL).optOccCt = atoi( pOptDesc->pzLastArg );
    (void)pOptions;
}
#endif /* defined(TEST_SNTP_OPTS) */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the kod option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptKod(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static teOptFileType const  type =
        FTYPE_MODE_MAY_EXIST + FTYPE_MODE_NO_OPEN;
    static tuFileMode           mode;
#ifndef O_CLOEXEC
#  define O_CLOEXEC 0
#endif
    mode.file_flags = O_CLOEXEC;

    /*
     * This function handles special invalid values for "pOptions"
     */
    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the keyfile option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptKeyfile(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static teOptFileType const  type =
        FTYPE_MODE_MAY_EXIST + FTYPE_MODE_NO_OPEN;
    static tuFileMode           mode;
#ifndef O_CLOEXEC
#  define O_CLOEXEC 0
#endif
    mode.file_flags = O_CLOEXEC;

    /*
     * This function handles special invalid values for "pOptions"
     */
    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the logfile option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptLogfile(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static teOptFileType const  type =
        FTYPE_MODE_MAY_EXIST + FTYPE_MODE_NO_OPEN;
    static tuFileMode           mode;
#ifndef O_CLOEXEC
#  define O_CLOEXEC 0
#endif
    mode.file_flags = O_CLOEXEC;

    /*
     * This function handles special invalid values for "pOptions"
     */
    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the steplimit option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptSteplimit(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static struct {long rmin, rmax;} const rng[1] = {
        { 0, LONG_MAX } };
    int  ix;

    if (pOptions <= OPTPROC_EMIT_LIMIT)
        goto emit_ranges;
    optionNumericVal(pOptions, pOptDesc);

    for (ix = 0; ix < 1; ix++) {
        if (pOptDesc->optArg.argInt < rng[ix].rmin)
            continue;  /* ranges need not be ordered. */
        if (pOptDesc->optArg.argInt == rng[ix].rmin)
            return;
        if (rng[ix].rmax == LONG_MIN)
            continue;
        if (pOptDesc->optArg.argInt <= rng[ix].rmax)
            return;
    }

    option_usage_fp = stderr;

emit_ranges:
    optionShowRange(pOptions, pOptDesc, (void *)rng, 1);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the ntpversion option.
 *
 * @param pOptions the sntp options data structure
 * @param pOptDesc the option descriptor for this option.
 */
static void
doOptNtpversion(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static struct {long rmin, rmax;} const rng[1] = {
        { 0, 7 } };
    int  ix;

    if (pOptions <= OPTPROC_EMIT_LIMIT)
        goto emit_ranges;
    optionNumericVal(pOptions, pOptDesc);

    for (ix = 0; ix < 1; ix++) {
        if (pOptDesc->optArg.argInt < rng[ix].rmin)
            continue;  /* ranges need not be ordered. */
        if (pOptDesc->optArg.argInt == rng[ix].rmin)
            return;
        if (rng[ix].rmax == LONG_MIN)
            continue;
        if (pOptDesc->optArg.argInt <= rng[ix].rmax)
            return;
    }

    option_usage_fp = stderr;

emit_ranges:
    optionShowRange(pOptions, pOptDesc, (void *)rng, 1);
}
/* extracted from optmain.tlib near line 48 */

#if defined(TEST_SNTP_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell(tOptions*);

/**
 * Generated main procedure.  This will emit text that a Bourne shell can
 * process to handle its command line arguments.
 *
 * @param argc argument count
 * @param argv argument vector
 * @returns program exit code
 */
int
main(int argc, char ** argv)
{
    int res = SNTP_EXIT_SUCCESS;
    (void)optionProcess(&sntpOptions, argc, argv);
    optionPutShell(&sntpOptions);
    res = ferror(stdout);
    if (res != 0)
        fputs("output error writing to stdout\n", stderr);
    return res;
}
#endif  /* defined TEST_SNTP_OPTS */
/* extracted from optmain.tlib near line 1146 */

/**
 * The directory containing the data associated with sntp.
 */
#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

/**
 * Information about the person or institution that packaged sntp
 * for the current distribution.
 */
#ifndef  WITH_PACKAGER
# define sntp_packager_info NULL
#else
static char const sntp_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport sntp bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif
#ifndef __doxygen__

#endif /* __doxygen__ */
/**
 * The option definitions for sntp.  The one structure that
 * binds them all.
 */
tOptions sntpOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_NEGATIONS
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
    24 /* full option count */, 19 /* user option count */,
    sntp_full_usage, sntp_short_usage,
    NULL, NULL,
    PKGDATADIR, sntp_packager_info
};

#if ENABLE_NLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <autoopts/usage-txt.h>

static char* AO_gettext(char const* pz);
static void  coerce_it(void** s);

/**
 * AutoGen specific wrapper function for gettext.
 * It relies on the macro _() to convert from English to the target
 * language, then strdup-duplicates the result string.
 *
 * @param[in] pz the input text used as a lookup key.
 * @returns the translated text (if there is one),
 *   or the original text (if not).
 */
static char *
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
        exit(SNTP_EXIT_FAILURE);
    }
    return pzRes;
}

static void coerce_it(void** s) { *s = AO_gettext(*s);
}

/**
 * Translate all the translatable strings in the sntpOptions
 * structure defined above.  This is done only once.
 */
static void
translate_option_strings(void)
{
    tOptions * const pOpt = &sntpOptions;

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
        sntpOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* sntp-opts.c ends here */
