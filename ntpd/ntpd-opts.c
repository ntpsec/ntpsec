/*  
 *  EDIT THIS FILE WITH CAUTION  (ntpd-opts.c)
 *  
 *  It has been AutoGen-ed  February 27, 2012 at 11:44:12 AM by AutoGen 5.14
 *  From the definitions    ntpd-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 36:1:11 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the ntpd author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The ntpd program is copyrighted and licensed
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
#include "ntpd-opts.h"
#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
#define zCopyright      (ntpd_opt_strs+0)
#define zLicenseDescrip (ntpd_opt_strs+314)

extern tUsageProc optionUsage;
/*
 *  global included definitions
 */#ifdef __windows
  extern int atoi(const char *);
#else
# include <stdlib.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

/*
 *  ntpd option static const strings
 */
static char const ntpd_opt_strs[3008] =
/*     0 */ "ntpd 4.2.7p261\n"
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
/*  1034 */ "Require crypto authentication\0"
/*  1064 */ "AUTHREQ\0"
/*  1072 */ "authreq\0"
/*  1080 */ "Do not require crypto authentication\0"
/*  1117 */ "AUTHNOREQ\0"
/*  1127 */ "authnoreq\0"
/*  1137 */ "Allow us to sync to broadcast servers\0"
/*  1175 */ "BCASTSYNC\0"
/*  1185 */ "bcastsync\0"
/*  1195 */ "configuration file name\0"
/*  1219 */ "CONFIGFILE\0"
/*  1230 */ "configfile\0"
/*  1241 */ "Increase output debug message level\0"
/*  1277 */ "DEBUG_LEVEL\0"
/*  1289 */ "debug-level\0"
/*  1301 */ "this package was built using 'configure --disable--debug'\0"
/*  1359 */ "Set the output debug message level\0"
/*  1394 */ "SET_DEBUG_LEVEL\0"
/*  1410 */ "set-debug-level\0"
/*  1426 */ "frequency drift file name\0"
/*  1452 */ "DRIFTFILE\0"
/*  1462 */ "driftfile\0"
/*  1472 */ "Allow the first adjustment to be Big\0"
/*  1509 */ "PANICGATE\0"
/*  1519 */ "panicgate\0"
/*  1529 */ "Jail directory\0"
/*  1544 */ "JAILDIR\0"
/*  1552 */ "jaildir\0"
/*  1560 */ "built without --enable-clockctl or --enable-linuxcaps\0"
/*  1614 */ "Listen on an interface name or address\0"
/*  1653 */ "INTERFACE\0"
/*  1663 */ "interface\0"
/*  1673 */ "path to symmetric keys\0"
/*  1696 */ "KEYFILE\0"
/*  1704 */ "keyfile\0"
/*  1712 */ "path to the log file\0"
/*  1733 */ "LOGFILE\0"
/*  1741 */ "logfile\0"
/*  1749 */ "Do not listen to virtual interfaces\0"
/*  1785 */ "NOVIRTUALIPS\0"
/*  1798 */ "novirtualips\0"
/*  1811 */ "Modify Multimedia Timer (Windows only)\0"
/*  1850 */ "MODIFYMMTIMER\0"
/*  1864 */ "modifymmtimer\0"
/*  1878 */ "Do not fork\0"
/*  1890 */ "NOFORK\0"
/*  1897 */ "nofork\0"
/*  1904 */ "Run at high priority\0"
/*  1925 */ "NICE\0"
/*  1930 */ "nice\0"
/*  1935 */ "path to the PID file\0"
/*  1956 */ "PIDFILE\0"
/*  1964 */ "pidfile\0"
/*  1972 */ "Process priority\0"
/*  1989 */ "PRIORITY\0"
/*  1998 */ "priority\0"
/*  2007 */ "Set the time and quit\0"
/*  2029 */ "QUIT\0"
/*  2034 */ "quit\0"
/*  2039 */ "Broadcast/propagation delay\0"
/*  2067 */ "PROPAGATIONDELAY\0"
/*  2084 */ "propagationdelay\0"
/*  2101 */ "Save parsed configuration and quit\0"
/*  2136 */ "SAVECONFIGQUIT\0"
/*  2151 */ "saveconfigquit\0"
/*  2166 */ "Statistics file location\0"
/*  2191 */ "STATSDIR\0"
/*  2200 */ "statsdir\0"
/*  2209 */ "Trusted key number\0"
/*  2228 */ "TRUSTEDKEY\0"
/*  2239 */ "trustedkey\0"
/*  2250 */ "Run as userid (or userid:groupid)\0"
/*  2284 */ "USER\0"
/*  2289 */ "user\0"
/*  2294 */ "interval in seconds between scans for new or dropped interfaces\0"
/*  2358 */ "UPDATEINTERVAL\0"
/*  2373 */ "updateinterval\0"
/*  2388 */ "make ARG an ntp variable (RW)\0"
/*  2418 */ "VAR\0"
/*  2422 */ "var\0"
/*  2426 */ "make ARG an ntp variable (RW|DEF)\0"
/*  2460 */ "DVAR\0"
/*  2465 */ "dvar\0"
/*  2470 */ "Seconds to wait for first clock sync\0"
/*  2507 */ "WAIT_SYNC\0"
/*  2517 */ "wait-sync\0"
/*  2527 */ "Slew up to 600 seconds\0"
/*  2550 */ "SLEW\0"
/*  2555 */ "slew\0"
/*  2560 */ "Use CPU cycle counter (Windows only)\0"
/*  2597 */ "USEPCC\0"
/*  2604 */ "usepcc\0"
/*  2611 */ "Force CPU cycle counter use (Windows only)\0"
/*  2654 */ "PCCFREQ\0"
/*  2662 */ "pccfreq\0"
/*  2670 */ "Display extended usage information and exit\0"
/*  2714 */ "help\0"
/*  2719 */ "Extended usage information passed thru pager\0"
/*  2764 */ "more-help\0"
/*  2774 */ "Output version information and exit\0"
/*  2810 */ "version\0"
/*  2818 */ "NTPD\0"
/*  2823 */ "ntpd - NTP daemon program - Ver. 4.2.7p261\n"
            "USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... \\\n"
            "\t\t[ <server1> ... <serverN> ]\n\0"
/*  2956 */ "http://bugs.ntp.org, bugs@ntp.org\0"
/*  2990 */ "\n\n\0"
/*  2993 */ "ntpd 4.2.7p261";

/*
 *  ipv4 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV4_DESC      (ntpd_opt_strs+952)
#define IPV4_NAME      (ntpd_opt_strs+983)
#define IPV4_name      (ntpd_opt_strs+988)
static int const aIpv4CantList[] = {
    INDEX_OPT_IPV6, NO_EQUIVALENT };
#define IPV4_FLAGS     (OPTST_DISABLED)

/*
 *  ipv6 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV6_DESC      (ntpd_opt_strs+993)
#define IPV6_NAME      (ntpd_opt_strs+1024)
#define IPV6_name      (ntpd_opt_strs+1029)
static int const aIpv6CantList[] = {
    INDEX_OPT_IPV4, NO_EQUIVALENT };
#define IPV6_FLAGS     (OPTST_DISABLED)

/*
 *  authreq option description with
 *  "Must also have options" and "Incompatible options":
 */
#define AUTHREQ_DESC      (ntpd_opt_strs+1034)
#define AUTHREQ_NAME      (ntpd_opt_strs+1064)
#define AUTHREQ_name      (ntpd_opt_strs+1072)
static int const aAuthreqCantList[] = {
    INDEX_OPT_AUTHNOREQ, NO_EQUIVALENT };
#define AUTHREQ_FLAGS     (OPTST_DISABLED)

/*
 *  authnoreq option description with
 *  "Must also have options" and "Incompatible options":
 */
#define AUTHNOREQ_DESC      (ntpd_opt_strs+1080)
#define AUTHNOREQ_NAME      (ntpd_opt_strs+1117)
#define AUTHNOREQ_name      (ntpd_opt_strs+1127)
static int const aAuthnoreqCantList[] = {
    INDEX_OPT_AUTHREQ, NO_EQUIVALENT };
#define AUTHNOREQ_FLAGS     (OPTST_DISABLED)

/*
 *  bcastsync option description:
 */
#define BCASTSYNC_DESC      (ntpd_opt_strs+1137)
#define BCASTSYNC_NAME      (ntpd_opt_strs+1175)
#define BCASTSYNC_name      (ntpd_opt_strs+1185)
#define BCASTSYNC_FLAGS     (OPTST_DISABLED)

/*
 *  configfile option description:
 */
#define CONFIGFILE_DESC      (ntpd_opt_strs+1195)
#define CONFIGFILE_NAME      (ntpd_opt_strs+1219)
#define CONFIGFILE_name      (ntpd_opt_strs+1230)
#define CONFIGFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  debug-level option description:
 */
#ifdef DEBUG
#define DEBUG_LEVEL_DESC      (ntpd_opt_strs+1241)
#define DEBUG_LEVEL_NAME      (ntpd_opt_strs+1277)
#define DEBUG_LEVEL_name      (ntpd_opt_strs+1289)
#define DEBUG_LEVEL_FLAGS     (OPTST_DISABLED)

#else   /* disable debug-level */
#define DEBUG_LEVEL_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define DEBUG_LEVEL_NAME      NULL
#define DEBUG_LEVEL_DESC      (ntpd_opt_strs+1301)
#define DEBUG_LEVEL_name      (ntpd_opt_strs+1289)
#endif  /* DEBUG */

/*
 *  set-debug-level option description:
 */
#ifdef DEBUG
#define SET_DEBUG_LEVEL_DESC      (ntpd_opt_strs+1359)
#define SET_DEBUG_LEVEL_NAME      (ntpd_opt_strs+1394)
#define SET_DEBUG_LEVEL_name      (ntpd_opt_strs+1410)
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable set-debug-level */
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define SET_DEBUG_LEVEL_NAME      NULL
#define SET_DEBUG_LEVEL_DESC      (ntpd_opt_strs+1301)
#define SET_DEBUG_LEVEL_name      (ntpd_opt_strs+1410)
#endif  /* DEBUG */

/*
 *  driftfile option description:
 */
#define DRIFTFILE_DESC      (ntpd_opt_strs+1426)
#define DRIFTFILE_NAME      (ntpd_opt_strs+1452)
#define DRIFTFILE_name      (ntpd_opt_strs+1462)
#define DRIFTFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  panicgate option description:
 */
#define PANICGATE_DESC      (ntpd_opt_strs+1472)
#define PANICGATE_NAME      (ntpd_opt_strs+1509)
#define PANICGATE_name      (ntpd_opt_strs+1519)
#define PANICGATE_FLAGS     (OPTST_DISABLED)

/*
 *  jaildir option description:
 */
#ifdef HAVE_DROPROOT
#define JAILDIR_DESC      (ntpd_opt_strs+1529)
#define JAILDIR_NAME      (ntpd_opt_strs+1544)
#define JAILDIR_name      (ntpd_opt_strs+1552)
#define JAILDIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable jaildir */
#define JAILDIR_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define JAILDIR_NAME      NULL
#define JAILDIR_DESC      (ntpd_opt_strs+1560)
#define JAILDIR_name      (ntpd_opt_strs+1552)
#endif  /* HAVE_DROPROOT */

/*
 *  interface option description:
 */
#define INTERFACE_DESC      (ntpd_opt_strs+1614)
#define INTERFACE_NAME      (ntpd_opt_strs+1653)
#define INTERFACE_name      (ntpd_opt_strs+1663)
#define INTERFACE_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  keyfile option description:
 */
#define KEYFILE_DESC      (ntpd_opt_strs+1673)
#define KEYFILE_NAME      (ntpd_opt_strs+1696)
#define KEYFILE_name      (ntpd_opt_strs+1704)
#define KEYFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  logfile option description:
 */
#define LOGFILE_DESC      (ntpd_opt_strs+1712)
#define LOGFILE_NAME      (ntpd_opt_strs+1733)
#define LOGFILE_name      (ntpd_opt_strs+1741)
#define LOGFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  novirtualips option description:
 */
#define NOVIRTUALIPS_DESC      (ntpd_opt_strs+1749)
#define NOVIRTUALIPS_NAME      (ntpd_opt_strs+1785)
#define NOVIRTUALIPS_name      (ntpd_opt_strs+1798)
#define NOVIRTUALIPS_FLAGS     (OPTST_DISABLED)

/*
 *  modifymmtimer option description:
 */
#ifdef SYS_WINNT
#define MODIFYMMTIMER_DESC      (ntpd_opt_strs+1811)
#define MODIFYMMTIMER_NAME      (ntpd_opt_strs+1850)
#define MODIFYMMTIMER_name      (ntpd_opt_strs+1864)
#define MODIFYMMTIMER_FLAGS     (OPTST_DISABLED)

#else   /* disable modifymmtimer */
#define MODIFYMMTIMER_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define MODIFYMMTIMER_NAME      NULL
#define MODIFYMMTIMER_DESC      NULL
#define MODIFYMMTIMER_name      NULL
#endif  /* SYS_WINNT */

/*
 *  nofork option description with
 *  "Must also have options" and "Incompatible options":
 */
#define NOFORK_DESC      (ntpd_opt_strs+1878)
#define NOFORK_NAME      (ntpd_opt_strs+1890)
#define NOFORK_name      (ntpd_opt_strs+1897)
static int const aNoforkCantList[] = {
    INDEX_OPT_WAIT_SYNC, NO_EQUIVALENT };
#define NOFORK_FLAGS     (OPTST_DISABLED)

/*
 *  nice option description:
 */
#define NICE_DESC      (ntpd_opt_strs+1904)
#define NICE_NAME      (ntpd_opt_strs+1925)
#define NICE_name      (ntpd_opt_strs+1930)
#define NICE_FLAGS     (OPTST_DISABLED)

/*
 *  pidfile option description:
 */
#define PIDFILE_DESC      (ntpd_opt_strs+1935)
#define PIDFILE_NAME      (ntpd_opt_strs+1956)
#define PIDFILE_name      (ntpd_opt_strs+1964)
#define PIDFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  priority option description:
 */
#define PRIORITY_DESC      (ntpd_opt_strs+1972)
#define PRIORITY_NAME      (ntpd_opt_strs+1989)
#define PRIORITY_name      (ntpd_opt_strs+1998)
#define PRIORITY_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  quit option description with
 *  "Must also have options" and "Incompatible options":
 */
#define QUIT_DESC      (ntpd_opt_strs+2007)
#define QUIT_NAME      (ntpd_opt_strs+2029)
#define QUIT_name      (ntpd_opt_strs+2034)
static int const aQuitCantList[] = {
    INDEX_OPT_SAVECONFIGQUIT,
    INDEX_OPT_WAIT_SYNC, NO_EQUIVALENT };
#define QUIT_FLAGS     (OPTST_DISABLED)

/*
 *  propagationdelay option description:
 */
#define PROPAGATIONDELAY_DESC      (ntpd_opt_strs+2039)
#define PROPAGATIONDELAY_NAME      (ntpd_opt_strs+2067)
#define PROPAGATIONDELAY_name      (ntpd_opt_strs+2084)
#define PROPAGATIONDELAY_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  saveconfigquit option description with
 *  "Must also have options" and "Incompatible options":
 */
#ifdef SAVECONFIG
#define SAVECONFIGQUIT_DESC      (ntpd_opt_strs+2101)
#define SAVECONFIGQUIT_NAME      (ntpd_opt_strs+2136)
#define SAVECONFIGQUIT_name      (ntpd_opt_strs+2151)
static int const aSaveconfigquitCantList[] = {
    INDEX_OPT_QUIT,
    INDEX_OPT_WAIT_SYNC, NO_EQUIVALENT };
#define SAVECONFIGQUIT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable saveconfigquit */
#define SAVECONFIGQUIT_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define aSaveconfigquitCantList   NULL
#define SAVECONFIGQUIT_NAME      NULL
#define SAVECONFIGQUIT_DESC      NULL
#define SAVECONFIGQUIT_name      NULL
#endif  /* SAVECONFIG */

/*
 *  statsdir option description:
 */
#define STATSDIR_DESC      (ntpd_opt_strs+2166)
#define STATSDIR_NAME      (ntpd_opt_strs+2191)
#define STATSDIR_name      (ntpd_opt_strs+2200)
#define STATSDIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  trustedkey option description:
 */
#define TRUSTEDKEY_DESC      (ntpd_opt_strs+2209)
#define TRUSTEDKEY_NAME      (ntpd_opt_strs+2228)
#define TRUSTEDKEY_name      (ntpd_opt_strs+2239)
#define TRUSTEDKEY_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  user option description:
 */
#ifdef HAVE_DROPROOT
#define USER_DESC      (ntpd_opt_strs+2250)
#define USER_NAME      (ntpd_opt_strs+2284)
#define USER_name      (ntpd_opt_strs+2289)
#define USER_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable user */
#define USER_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define USER_NAME      NULL
#define USER_DESC      (ntpd_opt_strs+1560)
#define USER_name      (ntpd_opt_strs+2289)
#endif  /* HAVE_DROPROOT */

/*
 *  updateinterval option description:
 */
#define UPDATEINTERVAL_DESC      (ntpd_opt_strs+2294)
#define UPDATEINTERVAL_NAME      (ntpd_opt_strs+2358)
#define UPDATEINTERVAL_name      (ntpd_opt_strs+2373)
#define UPDATEINTERVAL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  var option description:
 */
#define VAR_DESC      (ntpd_opt_strs+2388)
#define VAR_NAME      (ntpd_opt_strs+2418)
#define VAR_name      (ntpd_opt_strs+2422)
#define VAR_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  dvar option description:
 */
#define DVAR_DESC      (ntpd_opt_strs+2426)
#define DVAR_NAME      (ntpd_opt_strs+2460)
#define DVAR_name      (ntpd_opt_strs+2465)
#define DVAR_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  wait-sync option description with
 *  "Must also have options" and "Incompatible options":
 */
#ifdef HAVE_WORKING_FORK
#define WAIT_SYNC_DESC      (ntpd_opt_strs+2470)
#define WAIT_SYNC_NAME      (ntpd_opt_strs+2507)
#define WAIT_SYNC_name      (ntpd_opt_strs+2517)
static int const aWait_SyncCantList[] = {
    INDEX_OPT_NOFORK,
    INDEX_OPT_QUIT,
    INDEX_OPT_SAVECONFIGQUIT, NO_EQUIVALENT };
#define WAIT_SYNC_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

#else   /* disable wait-sync */
#define WAIT_SYNC_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define aWait_SyncCantList   NULL
#define WAIT_SYNC_NAME      NULL
#define WAIT_SYNC_DESC      NULL
#define WAIT_SYNC_name      NULL
#endif  /* HAVE_WORKING_FORK */

/*
 *  slew option description:
 */
#define SLEW_DESC      (ntpd_opt_strs+2527)
#define SLEW_NAME      (ntpd_opt_strs+2550)
#define SLEW_name      (ntpd_opt_strs+2555)
#define SLEW_FLAGS     (OPTST_DISABLED)

/*
 *  usepcc option description:
 */
#ifdef SYS_WINNT
#define USEPCC_DESC      (ntpd_opt_strs+2560)
#define USEPCC_NAME      (ntpd_opt_strs+2597)
#define USEPCC_name      (ntpd_opt_strs+2604)
#define USEPCC_FLAGS     (OPTST_DISABLED)

#else   /* disable usepcc */
#define USEPCC_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define USEPCC_NAME      NULL
#define USEPCC_DESC      NULL
#define USEPCC_name      NULL
#endif  /* SYS_WINNT */

/*
 *  pccfreq option description:
 */
#ifdef SYS_WINNT
#define PCCFREQ_DESC      (ntpd_opt_strs+2611)
#define PCCFREQ_NAME      (ntpd_opt_strs+2654)
#define PCCFREQ_name      (ntpd_opt_strs+2662)
#define PCCFREQ_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable pccfreq */
#define PCCFREQ_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define PCCFREQ_NAME      NULL
#define PCCFREQ_DESC      NULL
#define PCCFREQ_name      NULL
#endif  /* SYS_WINNT */

/*
 *  Help/More_Help/Version option descriptions:
 */
#define HELP_DESC       (ntpd_opt_strs+2670)
#define HELP_name       (ntpd_opt_strs+2714)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (ntpd_opt_strs+2719)
#define MORE_HELP_name  (ntpd_opt_strs+2764)
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
#define VER_DESC        (ntpd_opt_strs+2774)
#define VER_name        (ntpd_opt_strs+2810)
/*
 *  Declare option callback procedures
 */
#ifdef DEBUG
  static tOptProc doOptSet_Debug_Level;
#else /* not DEBUG */
# define doOptSet_Debug_Level NULL
#endif /* def/not DEBUG */
#if defined(TEST_NTPD_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
static tOptProc
    doUsageOpt;

/*
 *  #define map the "normal" callout procs to the test ones...
 */
#define SET_DEBUG_LEVEL_OPT_PROC optionStackArg


#else /* NOT defined TEST_NTPD_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    ntpOptionPrintVersion, optionBooleanVal,      optionNestedVal,
    optionNumericVal,      optionPagedUsage,      optionResetOpt,
    optionStackArg,        optionTimeDate,        optionTimeVal,
    optionUnstackArg,      optionVendorOption,    optionVersionStderr;
static tOptProc
    doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level

#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level
#endif /* defined(TEST_NTPD_OPTS) */
#define VER_PROC        ntpOptionPrintVersion

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntpd Option Descriptions.
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

  {  /* entry idx, value */ 2, VALUE_OPT_AUTHREQ,
     /* equiv idx, value */ 2, VALUE_OPT_AUTHREQ,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ AUTHREQ_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --authreq */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aAuthreqCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ AUTHREQ_DESC, AUTHREQ_NAME, AUTHREQ_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_AUTHNOREQ,
     /* equiv idx, value */ 3, VALUE_OPT_AUTHNOREQ,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ AUTHNOREQ_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --authnoreq */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aAuthnoreqCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ AUTHNOREQ_DESC, AUTHNOREQ_NAME, AUTHNOREQ_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_BCASTSYNC,
     /* equiv idx, value */ 4, VALUE_OPT_BCASTSYNC,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ BCASTSYNC_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --bcastsync */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ BCASTSYNC_DESC, BCASTSYNC_NAME, BCASTSYNC_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_CONFIGFILE,
     /* equiv idx, value */ 5, VALUE_OPT_CONFIGFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ CONFIGFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --configfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ CONFIGFILE_DESC, CONFIGFILE_NAME, CONFIGFILE_name,
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

  {  /* entry idx, value */ 8, VALUE_OPT_DRIFTFILE,
     /* equiv idx, value */ 8, VALUE_OPT_DRIFTFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ DRIFTFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --driftfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ DRIFTFILE_DESC, DRIFTFILE_NAME, DRIFTFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 9, VALUE_OPT_PANICGATE,
     /* equiv idx, value */ 9, VALUE_OPT_PANICGATE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ PANICGATE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --panicgate */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PANICGATE_DESC, PANICGATE_NAME, PANICGATE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 10, VALUE_OPT_JAILDIR,
     /* equiv idx, value */ 10, VALUE_OPT_JAILDIR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ JAILDIR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --jaildir */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ JAILDIR_DESC, JAILDIR_NAME, JAILDIR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 11, VALUE_OPT_INTERFACE,
     /* equiv idx, value */ 11, VALUE_OPT_INTERFACE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ INTERFACE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --interface */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ INTERFACE_DESC, INTERFACE_NAME, INTERFACE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 12, VALUE_OPT_KEYFILE,
     /* equiv idx, value */ 12, VALUE_OPT_KEYFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KEYFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --keyfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ KEYFILE_DESC, KEYFILE_NAME, KEYFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 13, VALUE_OPT_LOGFILE,
     /* equiv idx, value */ 13, VALUE_OPT_LOGFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ LOGFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --logfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ LOGFILE_DESC, LOGFILE_NAME, LOGFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 14, VALUE_OPT_NOVIRTUALIPS,
     /* equiv idx, value */ 14, VALUE_OPT_NOVIRTUALIPS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NOVIRTUALIPS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --novirtualips */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ NOVIRTUALIPS_DESC, NOVIRTUALIPS_NAME, NOVIRTUALIPS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 15, VALUE_OPT_MODIFYMMTIMER,
     /* equiv idx, value */ 15, VALUE_OPT_MODIFYMMTIMER,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MODIFYMMTIMER_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --modifymmtimer */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ MODIFYMMTIMER_DESC, MODIFYMMTIMER_NAME, MODIFYMMTIMER_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 16, VALUE_OPT_NOFORK,
     /* equiv idx, value */ 16, VALUE_OPT_NOFORK,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NOFORK_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --nofork */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aNoforkCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ NOFORK_DESC, NOFORK_NAME, NOFORK_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 17, VALUE_OPT_NICE,
     /* equiv idx, value */ 17, VALUE_OPT_NICE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NICE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --nice */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ NICE_DESC, NICE_NAME, NICE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 18, VALUE_OPT_PIDFILE,
     /* equiv idx, value */ 18, VALUE_OPT_PIDFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PIDFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --pidfile */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PIDFILE_DESC, PIDFILE_NAME, PIDFILE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 19, VALUE_OPT_PRIORITY,
     /* equiv idx, value */ 19, VALUE_OPT_PRIORITY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PRIORITY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --priority */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ PRIORITY_DESC, PRIORITY_NAME, PRIORITY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 20, VALUE_OPT_QUIT,
     /* equiv idx, value */ 20, VALUE_OPT_QUIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ QUIT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --quit */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aQuitCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ QUIT_DESC, QUIT_NAME, QUIT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 21, VALUE_OPT_PROPAGATIONDELAY,
     /* equiv idx, value */ 21, VALUE_OPT_PROPAGATIONDELAY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PROPAGATIONDELAY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --propagationdelay */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PROPAGATIONDELAY_DESC, PROPAGATIONDELAY_NAME, PROPAGATIONDELAY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 22, VALUE_OPT_SAVECONFIGQUIT,
     /* equiv idx, value */ 22, VALUE_OPT_SAVECONFIGQUIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SAVECONFIGQUIT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --saveconfigquit */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aSaveconfigquitCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SAVECONFIGQUIT_DESC, SAVECONFIGQUIT_NAME, SAVECONFIGQUIT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 23, VALUE_OPT_STATSDIR,
     /* equiv idx, value */ 23, VALUE_OPT_STATSDIR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ STATSDIR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --statsdir */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ STATSDIR_DESC, STATSDIR_NAME, STATSDIR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 24, VALUE_OPT_TRUSTEDKEY,
     /* equiv idx, value */ 24, VALUE_OPT_TRUSTEDKEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ TRUSTEDKEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --trustedkey */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ TRUSTEDKEY_DESC, TRUSTEDKEY_NAME, TRUSTEDKEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 25, VALUE_OPT_USER,
     /* equiv idx, value */ 25, VALUE_OPT_USER,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ USER_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --user */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ USER_DESC, USER_NAME, USER_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 26, VALUE_OPT_UPDATEINTERVAL,
     /* equiv idx, value */ 26, VALUE_OPT_UPDATEINTERVAL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ UPDATEINTERVAL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --updateinterval */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ UPDATEINTERVAL_DESC, UPDATEINTERVAL_NAME, UPDATEINTERVAL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 27, VALUE_OPT_VAR,
     /* equiv idx, value */ 27, VALUE_OPT_VAR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ VAR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --var */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ VAR_DESC, VAR_NAME, VAR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 28, VALUE_OPT_DVAR,
     /* equiv idx, value */ 28, VALUE_OPT_DVAR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ DVAR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --dvar */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ DVAR_DESC, DVAR_NAME, DVAR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 29, VALUE_OPT_WAIT_SYNC,
     /* equiv idx, value */ 29, VALUE_OPT_WAIT_SYNC,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ WAIT_SYNC_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --wait-sync */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aWait_SyncCantList,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ WAIT_SYNC_DESC, WAIT_SYNC_NAME, WAIT_SYNC_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 30, VALUE_OPT_SLEW,
     /* equiv idx, value */ 30, VALUE_OPT_SLEW,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SLEW_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --slew */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SLEW_DESC, SLEW_NAME, SLEW_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 31, VALUE_OPT_USEPCC,
     /* equiv idx, value */ 31, VALUE_OPT_USEPCC,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ USEPCC_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --usepcc */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ USEPCC_DESC, USEPCC_NAME, USEPCC_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 32, VALUE_OPT_PCCFREQ,
     /* equiv idx, value */ 32, VALUE_OPT_PCCFREQ,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PCCFREQ_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --pccfreq */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PCCFREQ_DESC, PCCFREQ_NAME, PCCFREQ_name,
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
     /* disablement strs */ NULL, NULL }
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntpd Option Environment
 */
#define zPROGNAME       (ntpd_opt_strs+2818)
#define zUsageTitle     (ntpd_opt_strs+2823)
#define zRcName         NULL
#define apzHomeList     NULL
#define zBugsAddr       (ntpd_opt_strs+2956)
#define zExplain        (ntpd_opt_strs+2990)
#define zDetail         (NULL)
#define zFullVersion    (ntpd_opt_strs+2993)
/* extracted from optcode.tlib near line 315 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define ntpd_full_usage (NULL)

#define ntpd_short_usage (NULL)

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(tOptions * pOptions, tOptDesc * pOptDesc)
{
    (void)pOptions;
    USAGE(NTPD_EXIT_SUCCESS);
}

#if ! defined(TEST_NTPD_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the set-debug-level option, when DEBUG is #define-d.
 */
#ifdef DEBUG
static void
doOptSet_Debug_Level(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from ntpdbase-opts.def, line 100 */
DESC(DEBUG_LEVEL).optOccCt = atoi( pOptDesc->pzLastArg );
}
#endif /* defined DEBUG */
#endif /* defined(TEST_NTPD_OPTS) */
/* extracted from optmain.tlib near line 128 */

#if defined(TEST_NTPD_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell(tOptions*);

int
main(int argc, char ** argv)
{
    int res = NTPD_EXIT_SUCCESS;
    (void)optionProcess(&ntpdOptions, argc, argv);
    optionPutShell(&ntpdOptions);
    res = ferror(stdout);
    if (res != 0)
        fputs("output error writing to stdout\n", stderr);
    return res;
}
#endif  /* defined TEST_NTPD_OPTS */
/* extracted from optmain.tlib near line 1148 */

#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

#ifndef  WITH_PACKAGER
# define ntpd_packager_info NULL
#else
static char const ntpd_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport ntpd bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif

tOptions ntpdOptions = {
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
      NO_EQUIVALENT, /* save option index */
      NO_EQUIVALENT, /* '-#' option index */
      NO_EQUIVALENT /* index of default opt */
    },
    36 /* full option count */, 33 /* user option count */,
    ntpd_full_usage, ntpd_short_usage,
    NULL, NULL,
    PKGDATADIR, ntpd_packager_info
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
        exit(NTPD_EXIT_FAILURE);
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
    tOptions * const pOpt = &ntpdOptions;

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
        ntpdOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* ntpd-opts.c ends here */
