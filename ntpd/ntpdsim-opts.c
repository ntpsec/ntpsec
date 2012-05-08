/*  
 *  EDIT THIS FILE WITH CAUTION  (ntpdsim-opts.c)
 *  
 *  It has been AutoGen-ed  May  1, 2012 at 09:18:07 AM by AutoGen 5.16pre31
 *  From the definitions    ntpdsim-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 36:4:11 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the ntpdsim author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The ntpdsim program is copyrighted and licensed
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

#define OPTION_CODE_COMPILE 1
#include "ntpdsim-opts.h"
#include <sys/types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
#define zCopyright      (ntpdsim_opt_strs+0)
#define zLicenseDescrip (ntpdsim_opt_strs+317)

/*
 *  global included definitions
 */
#ifdef __windows
  extern int atoi(const char *);
#else
# include <stdlib.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

/*
 *  ntpdsim option static const strings
 */
static char const ntpdsim_opt_strs[3123] =
/*     0 */ "ntpdsim 4.2.7p272\n"
            "Copyright (C) 1970-2012 The University of Delaware, all rights reserved.\n"
            "This is free software. It is licensed for use, modification and\n"
            "redistribution under the terms of the NTP License, copies of which\n"
            "can be seen at:\n"
            "  <http://ntp.org/license>\n"
            "  <http://opensource.org/licenses/ntp-license.php>\n\0"
/*   317 */ "Permission to use, copy, modify, and distribute this software and its\n"
            "documentation for any purpose with or without fee is hereby granted,\n"
            "provided that the above copyright notice appears in all copies and that\n"
            "both the copyright notice and this permission notice appear in supporting\n"
            "documentation, and that the name The University of Delaware not be used in\n"
            "advertising or publicity pertaining to distribution of the software without\n"
            "specific, written prior permission.  The University of Delaware makes no\n"
            "representations about the suitability this software for any purpose.  It is\n"
            "provided \"as is\" without express or implied warranty.\n\0"
/*   957 */ "Force IPv4 DNS name resolution\0"
/*   988 */ "IPV4\0"
/*   993 */ "ipv4\0"
/*   998 */ "Force IPv6 DNS name resolution\0"
/*  1029 */ "IPV6\0"
/*  1034 */ "ipv6\0"
/*  1039 */ "Require crypto authentication\0"
/*  1069 */ "AUTHREQ\0"
/*  1077 */ "authreq\0"
/*  1085 */ "Do not require crypto authentication\0"
/*  1122 */ "AUTHNOREQ\0"
/*  1132 */ "authnoreq\0"
/*  1142 */ "Allow us to sync to broadcast servers\0"
/*  1180 */ "BCASTSYNC\0"
/*  1190 */ "bcastsync\0"
/*  1200 */ "configuration file name\0"
/*  1224 */ "CONFIGFILE\0"
/*  1235 */ "configfile\0"
/*  1246 */ "Increase output debug message level\0"
/*  1282 */ "DEBUG_LEVEL\0"
/*  1294 */ "debug-level\0"
/*  1306 */ "this package was built using 'configure --disable--debug'\0"
/*  1364 */ "Set the output debug message level\0"
/*  1399 */ "SET_DEBUG_LEVEL\0"
/*  1415 */ "set-debug-level\0"
/*  1431 */ "frequency drift file name\0"
/*  1457 */ "DRIFTFILE\0"
/*  1467 */ "driftfile\0"
/*  1477 */ "Allow the first adjustment to be Big\0"
/*  1514 */ "PANICGATE\0"
/*  1524 */ "panicgate\0"
/*  1534 */ "Jail directory\0"
/*  1549 */ "JAILDIR\0"
/*  1557 */ "jaildir\0"
/*  1565 */ "built without --enable-clockctl or --enable-linuxcaps\0"
/*  1619 */ "Listen on an interface name or address\0"
/*  1658 */ "INTERFACE\0"
/*  1668 */ "interface\0"
/*  1678 */ "path to symmetric keys\0"
/*  1701 */ "KEYFILE\0"
/*  1709 */ "keyfile\0"
/*  1717 */ "path to the log file\0"
/*  1738 */ "LOGFILE\0"
/*  1746 */ "logfile\0"
/*  1754 */ "Do not listen to virtual interfaces\0"
/*  1790 */ "NOVIRTUALIPS\0"
/*  1803 */ "novirtualips\0"
/*  1816 */ "Modify Multimedia Timer (Windows only)\0"
/*  1855 */ "MODIFYMMTIMER\0"
/*  1869 */ "modifymmtimer\0"
/*  1883 */ "Do not fork\0"
/*  1895 */ "NOFORK\0"
/*  1902 */ "nofork\0"
/*  1909 */ "Run at high priority\0"
/*  1930 */ "NICE\0"
/*  1935 */ "nice\0"
/*  1940 */ "path to the PID file\0"
/*  1961 */ "PIDFILE\0"
/*  1969 */ "pidfile\0"
/*  1977 */ "Process priority\0"
/*  1994 */ "PRIORITY\0"
/*  2003 */ "priority\0"
/*  2012 */ "Set the time and quit\0"
/*  2034 */ "QUIT\0"
/*  2039 */ "quit\0"
/*  2044 */ "Broadcast/propagation delay\0"
/*  2072 */ "PROPAGATIONDELAY\0"
/*  2089 */ "propagationdelay\0"
/*  2106 */ "Save parsed configuration and quit\0"
/*  2141 */ "SAVECONFIGQUIT\0"
/*  2156 */ "saveconfigquit\0"
/*  2171 */ "Statistics file location\0"
/*  2196 */ "STATSDIR\0"
/*  2205 */ "statsdir\0"
/*  2214 */ "Trusted key number\0"
/*  2233 */ "TRUSTEDKEY\0"
/*  2244 */ "trustedkey\0"
/*  2255 */ "Run as userid (or userid:groupid)\0"
/*  2289 */ "USER\0"
/*  2294 */ "user\0"
/*  2299 */ "interval in seconds between scans for new or dropped interfaces\0"
/*  2363 */ "UPDATEINTERVAL\0"
/*  2378 */ "updateinterval\0"
/*  2393 */ "make ARG an ntp variable (RW)\0"
/*  2423 */ "VAR\0"
/*  2427 */ "var\0"
/*  2431 */ "make ARG an ntp variable (RW|DEF)\0"
/*  2465 */ "DVAR\0"
/*  2470 */ "dvar\0"
/*  2475 */ "Seconds to wait for first clock sync\0"
/*  2512 */ "WAIT_SYNC\0"
/*  2522 */ "wait-sync\0"
/*  2532 */ "Slew up to 600 seconds\0"
/*  2555 */ "SLEW\0"
/*  2560 */ "slew\0"
/*  2565 */ "Use CPU cycle counter (Windows only)\0"
/*  2602 */ "USEPCC\0"
/*  2609 */ "usepcc\0"
/*  2616 */ "Force CPU cycle counter use (Windows only)\0"
/*  2659 */ "PCCFREQ\0"
/*  2667 */ "pccfreq\0"
/*  2675 */ "Display extended usage information and exit\0"
/*  2719 */ "help\0"
/*  2724 */ "Extended usage information passed thru pager\0"
/*  2769 */ "more-help\0"
/*  2779 */ "Output version information and exit\0"
/*  2815 */ "version\0"
/*  2823 */ "Save the option state to a config file\0"
/*  2862 */ "save-opts\0"
/*  2872 */ "Load options from a config file\0"
/*  2904 */ "LOAD_OPTS\0"
/*  2914 */ "no-load-opts\0"
/*  2927 */ "no\0"
/*  2930 */ "NTPDSIM\0"
/*  2938 */ "ntpdsim - NTP daemon simulation program - Ver. 4.2.7p272\n"
            "USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]...\n\0"
/*  3053 */ "$HOME\0"
/*  3059 */ ".\0"
/*  3061 */ ".ntprc\0"
/*  3068 */ "http://bugs.ntp.org, bugs@ntp.org\0"
/*  3102 */ "\n\n\0"
/*  3105 */ "ntpdsim 4.2.7p272";

/*
 *  ipv4 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV4_DESC      (ntpdsim_opt_strs+957)
#define IPV4_NAME      (ntpdsim_opt_strs+988)
#define IPV4_name      (ntpdsim_opt_strs+993)
static int const aIpv4CantList[] = {
    INDEX_OPT_IPV6, NO_EQUIVALENT };
#define IPV4_FLAGS     (OPTST_DISABLED)

/*
 *  ipv6 option description with
 *  "Must also have options" and "Incompatible options":
 */
#define IPV6_DESC      (ntpdsim_opt_strs+998)
#define IPV6_NAME      (ntpdsim_opt_strs+1029)
#define IPV6_name      (ntpdsim_opt_strs+1034)
static int const aIpv6CantList[] = {
    INDEX_OPT_IPV4, NO_EQUIVALENT };
#define IPV6_FLAGS     (OPTST_DISABLED)

/*
 *  authreq option description with
 *  "Must also have options" and "Incompatible options":
 */
#define AUTHREQ_DESC      (ntpdsim_opt_strs+1039)
#define AUTHREQ_NAME      (ntpdsim_opt_strs+1069)
#define AUTHREQ_name      (ntpdsim_opt_strs+1077)
static int const aAuthreqCantList[] = {
    INDEX_OPT_AUTHNOREQ, NO_EQUIVALENT };
#define AUTHREQ_FLAGS     (OPTST_DISABLED)

/*
 *  authnoreq option description with
 *  "Must also have options" and "Incompatible options":
 */
#define AUTHNOREQ_DESC      (ntpdsim_opt_strs+1085)
#define AUTHNOREQ_NAME      (ntpdsim_opt_strs+1122)
#define AUTHNOREQ_name      (ntpdsim_opt_strs+1132)
static int const aAuthnoreqCantList[] = {
    INDEX_OPT_AUTHREQ, NO_EQUIVALENT };
#define AUTHNOREQ_FLAGS     (OPTST_DISABLED)

/*
 *  bcastsync option description:
 */
#define BCASTSYNC_DESC      (ntpdsim_opt_strs+1142)
#define BCASTSYNC_NAME      (ntpdsim_opt_strs+1180)
#define BCASTSYNC_name      (ntpdsim_opt_strs+1190)
#define BCASTSYNC_FLAGS     (OPTST_DISABLED)

/*
 *  configfile option description:
 */
#define CONFIGFILE_DESC      (ntpdsim_opt_strs+1200)
#define CONFIGFILE_NAME      (ntpdsim_opt_strs+1224)
#define CONFIGFILE_name      (ntpdsim_opt_strs+1235)
#define CONFIGFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  debug-level option description:
 */
#ifdef DEBUG
#define DEBUG_LEVEL_DESC      (ntpdsim_opt_strs+1246)
#define DEBUG_LEVEL_NAME      (ntpdsim_opt_strs+1282)
#define DEBUG_LEVEL_name      (ntpdsim_opt_strs+1294)
#define DEBUG_LEVEL_FLAGS     (OPTST_DISABLED)

#else   /* disable debug-level */
#define DEBUG_LEVEL_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define DEBUG_LEVEL_NAME      NULL
#define DEBUG_LEVEL_DESC      (ntpdsim_opt_strs+1306)
#define DEBUG_LEVEL_name      (ntpdsim_opt_strs+1294)
#endif  /* DEBUG */

/*
 *  set-debug-level option description:
 */
#ifdef DEBUG
#define SET_DEBUG_LEVEL_DESC      (ntpdsim_opt_strs+1364)
#define SET_DEBUG_LEVEL_NAME      (ntpdsim_opt_strs+1399)
#define SET_DEBUG_LEVEL_name      (ntpdsim_opt_strs+1415)
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable set-debug-level */
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define SET_DEBUG_LEVEL_NAME      NULL
#define SET_DEBUG_LEVEL_DESC      (ntpdsim_opt_strs+1306)
#define SET_DEBUG_LEVEL_name      (ntpdsim_opt_strs+1415)
#endif  /* DEBUG */

/*
 *  driftfile option description:
 */
#define DRIFTFILE_DESC      (ntpdsim_opt_strs+1431)
#define DRIFTFILE_NAME      (ntpdsim_opt_strs+1457)
#define DRIFTFILE_name      (ntpdsim_opt_strs+1467)
#define DRIFTFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  panicgate option description:
 */
#define PANICGATE_DESC      (ntpdsim_opt_strs+1477)
#define PANICGATE_NAME      (ntpdsim_opt_strs+1514)
#define PANICGATE_name      (ntpdsim_opt_strs+1524)
#define PANICGATE_FLAGS     (OPTST_DISABLED)

/*
 *  jaildir option description:
 */
#ifdef HAVE_DROPROOT
#define JAILDIR_DESC      (ntpdsim_opt_strs+1534)
#define JAILDIR_NAME      (ntpdsim_opt_strs+1549)
#define JAILDIR_name      (ntpdsim_opt_strs+1557)
#define JAILDIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable jaildir */
#define JAILDIR_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define JAILDIR_NAME      NULL
#define JAILDIR_DESC      (ntpdsim_opt_strs+1565)
#define JAILDIR_name      (ntpdsim_opt_strs+1557)
#endif  /* HAVE_DROPROOT */

/*
 *  interface option description:
 */
#define INTERFACE_DESC      (ntpdsim_opt_strs+1619)
#define INTERFACE_NAME      (ntpdsim_opt_strs+1658)
#define INTERFACE_name      (ntpdsim_opt_strs+1668)
#define INTERFACE_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  keyfile option description:
 */
#define KEYFILE_DESC      (ntpdsim_opt_strs+1678)
#define KEYFILE_NAME      (ntpdsim_opt_strs+1701)
#define KEYFILE_name      (ntpdsim_opt_strs+1709)
#define KEYFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  logfile option description:
 */
#define LOGFILE_DESC      (ntpdsim_opt_strs+1717)
#define LOGFILE_NAME      (ntpdsim_opt_strs+1738)
#define LOGFILE_name      (ntpdsim_opt_strs+1746)
#define LOGFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  novirtualips option description:
 */
#define NOVIRTUALIPS_DESC      (ntpdsim_opt_strs+1754)
#define NOVIRTUALIPS_NAME      (ntpdsim_opt_strs+1790)
#define NOVIRTUALIPS_name      (ntpdsim_opt_strs+1803)
#define NOVIRTUALIPS_FLAGS     (OPTST_DISABLED)

/*
 *  modifymmtimer option description:
 */
#ifdef SYS_WINNT
#define MODIFYMMTIMER_DESC      (ntpdsim_opt_strs+1816)
#define MODIFYMMTIMER_NAME      (ntpdsim_opt_strs+1855)
#define MODIFYMMTIMER_name      (ntpdsim_opt_strs+1869)
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
#define NOFORK_DESC      (ntpdsim_opt_strs+1883)
#define NOFORK_NAME      (ntpdsim_opt_strs+1895)
#define NOFORK_name      (ntpdsim_opt_strs+1902)
static int const aNoforkCantList[] = {
    INDEX_OPT_WAIT_SYNC, NO_EQUIVALENT };
#define NOFORK_FLAGS     (OPTST_DISABLED)

/*
 *  nice option description:
 */
#define NICE_DESC      (ntpdsim_opt_strs+1909)
#define NICE_NAME      (ntpdsim_opt_strs+1930)
#define NICE_name      (ntpdsim_opt_strs+1935)
#define NICE_FLAGS     (OPTST_DISABLED)

/*
 *  pidfile option description:
 */
#define PIDFILE_DESC      (ntpdsim_opt_strs+1940)
#define PIDFILE_NAME      (ntpdsim_opt_strs+1961)
#define PIDFILE_name      (ntpdsim_opt_strs+1969)
#define PIDFILE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  priority option description:
 */
#define PRIORITY_DESC      (ntpdsim_opt_strs+1977)
#define PRIORITY_NAME      (ntpdsim_opt_strs+1994)
#define PRIORITY_name      (ntpdsim_opt_strs+2003)
#define PRIORITY_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  quit option description with
 *  "Must also have options" and "Incompatible options":
 */
#define QUIT_DESC      (ntpdsim_opt_strs+2012)
#define QUIT_NAME      (ntpdsim_opt_strs+2034)
#define QUIT_name      (ntpdsim_opt_strs+2039)
static int const aQuitCantList[] = {
    INDEX_OPT_SAVECONFIGQUIT,
    INDEX_OPT_WAIT_SYNC, NO_EQUIVALENT };
#define QUIT_FLAGS     (OPTST_DISABLED)

/*
 *  propagationdelay option description:
 */
#define PROPAGATIONDELAY_DESC      (ntpdsim_opt_strs+2044)
#define PROPAGATIONDELAY_NAME      (ntpdsim_opt_strs+2072)
#define PROPAGATIONDELAY_name      (ntpdsim_opt_strs+2089)
#define PROPAGATIONDELAY_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  saveconfigquit option description with
 *  "Must also have options" and "Incompatible options":
 */
#ifdef SAVECONFIG
#define SAVECONFIGQUIT_DESC      (ntpdsim_opt_strs+2106)
#define SAVECONFIGQUIT_NAME      (ntpdsim_opt_strs+2141)
#define SAVECONFIGQUIT_name      (ntpdsim_opt_strs+2156)
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
#define STATSDIR_DESC      (ntpdsim_opt_strs+2171)
#define STATSDIR_NAME      (ntpdsim_opt_strs+2196)
#define STATSDIR_name      (ntpdsim_opt_strs+2205)
#define STATSDIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  trustedkey option description:
 */
#define TRUSTEDKEY_DESC      (ntpdsim_opt_strs+2214)
#define TRUSTEDKEY_NAME      (ntpdsim_opt_strs+2233)
#define TRUSTEDKEY_name      (ntpdsim_opt_strs+2244)
#define TRUSTEDKEY_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  user option description:
 */
#ifdef HAVE_DROPROOT
#define USER_DESC      (ntpdsim_opt_strs+2255)
#define USER_NAME      (ntpdsim_opt_strs+2289)
#define USER_name      (ntpdsim_opt_strs+2294)
#define USER_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable user */
#define USER_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define USER_NAME      NULL
#define USER_DESC      (ntpdsim_opt_strs+1565)
#define USER_name      (ntpdsim_opt_strs+2294)
#endif  /* HAVE_DROPROOT */

/*
 *  updateinterval option description:
 */
#define UPDATEINTERVAL_DESC      (ntpdsim_opt_strs+2299)
#define UPDATEINTERVAL_NAME      (ntpdsim_opt_strs+2363)
#define UPDATEINTERVAL_name      (ntpdsim_opt_strs+2378)
#define UPDATEINTERVAL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  var option description:
 */
#define VAR_DESC      (ntpdsim_opt_strs+2393)
#define VAR_NAME      (ntpdsim_opt_strs+2423)
#define VAR_name      (ntpdsim_opt_strs+2427)
#define VAR_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  dvar option description:
 */
#define DVAR_DESC      (ntpdsim_opt_strs+2431)
#define DVAR_NAME      (ntpdsim_opt_strs+2465)
#define DVAR_name      (ntpdsim_opt_strs+2470)
#define DVAR_FLAGS     (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  wait-sync option description with
 *  "Must also have options" and "Incompatible options":
 */
#ifdef HAVE_WORKING_FORK
#define WAIT_SYNC_DESC      (ntpdsim_opt_strs+2475)
#define WAIT_SYNC_NAME      (ntpdsim_opt_strs+2512)
#define WAIT_SYNC_name      (ntpdsim_opt_strs+2522)
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
#define SLEW_DESC      (ntpdsim_opt_strs+2532)
#define SLEW_NAME      (ntpdsim_opt_strs+2555)
#define SLEW_name      (ntpdsim_opt_strs+2560)
#define SLEW_FLAGS     (OPTST_DISABLED)

/*
 *  usepcc option description:
 */
#ifdef SYS_WINNT
#define USEPCC_DESC      (ntpdsim_opt_strs+2565)
#define USEPCC_NAME      (ntpdsim_opt_strs+2602)
#define USEPCC_name      (ntpdsim_opt_strs+2609)
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
#define PCCFREQ_DESC      (ntpdsim_opt_strs+2616)
#define PCCFREQ_NAME      (ntpdsim_opt_strs+2659)
#define PCCFREQ_name      (ntpdsim_opt_strs+2667)
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
#define HELP_DESC       (ntpdsim_opt_strs+2675)
#define HELP_name       (ntpdsim_opt_strs+2719)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (ntpdsim_opt_strs+2724)
#define MORE_HELP_name  (ntpdsim_opt_strs+2769)
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
#define VER_DESC        (ntpdsim_opt_strs+2779)
#define VER_name        (ntpdsim_opt_strs+2815)
#define SAVE_OPTS_DESC  (ntpdsim_opt_strs+2823)
#define SAVE_OPTS_name  (ntpdsim_opt_strs+2862)
#define LOAD_OPTS_DESC     (ntpdsim_opt_strs+2872)
#define LOAD_OPTS_NAME     (ntpdsim_opt_strs+2904)
#define NO_LOAD_OPTS_name  (ntpdsim_opt_strs+2914)
#define LOAD_OPTS_pfx      (ntpdsim_opt_strs+2927)
#define LOAD_OPTS_name     (NO_LOAD_OPTS_name + 3)
/*
 *  Declare option callback procedures
 */
#ifdef DEBUG
  static tOptProc doOptSet_Debug_Level;
#else /* not DEBUG */
# define doOptSet_Debug_Level NULL
#endif /* def/not DEBUG */
#if defined(TEST_NTPDSIM_OPTS)
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


#else /* NOT defined TEST_NTPDSIM_OPTS */
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

#endif /* defined(TEST_NTPDSIM_OPTS) */
#define VER_PROC        ntpOptionPrintVersion

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the ntpdsim Option Descriptions.
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
 *  Define the ntpdsim Option Environment
 */
#define zPROGNAME       (ntpdsim_opt_strs+2930)
#define zUsageTitle     (ntpdsim_opt_strs+2938)
#define zRcName         (ntpdsim_opt_strs+3061)
static char const * const apzHomeList[3] = {
    ntpdsim_opt_strs+3053,
    ntpdsim_opt_strs+3059,
    NULL };
#define zBugsAddr       (ntpdsim_opt_strs+3068)
#define zExplain        (NULL)
#define zDetail         (ntpdsim_opt_strs+3102)
#define zFullVersion    (ntpdsim_opt_strs+3105)
/* extracted from optcode.tlib near line 349 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define ntpdsim_full_usage (NULL)

#define ntpdsim_short_usage (NULL)

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
    optionUsage(&ntpdsimOptions, NTPDSIM_EXIT_SUCCESS);
    /* NOTREACHED */
    (void)pOptDesc;
    (void)pOptions;
}

#if ! defined(TEST_NTPDSIM_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the set-debug-level option, when DEBUG is #define-d.
 *
 * @param pOptions the ntpdsim options data structure
 * @param pOptDesc the option descriptor for this option.
 */
#ifdef DEBUG
static void
doOptSet_Debug_Level(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from ntpdbase-opts.def, line 100 */
DESC(DEBUG_LEVEL).optOccCt = atoi( pOptDesc->pzLastArg );
    (void)pOptions;
}
#endif /* defined DEBUG */
#endif /* defined(TEST_NTPDSIM_OPTS) */
/* extracted from optmain.tlib near line 35 */

#if defined(TEST_NTPDSIM_OPTS) /* TEST MAIN PROCEDURE: */

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
    int res = NTPDSIM_EXIT_SUCCESS;
    (void)optionProcess(&ntpdsimOptions, argc, argv);
    optionPutShell(&ntpdsimOptions);
    res = ferror(stdout);
    if (res != 0)
        fputs("output error writing to stdout\n", stderr);
    return res;
}
#endif  /* defined TEST_NTPDSIM_OPTS */
/* extracted from optmain.tlib near line 1113 */

#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

#ifndef  WITH_PACKAGER
# define ntpdsim_packager_info NULL
#else
static char const ntpdsim_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport ntpdsim bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif

tOptions ntpdsimOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_ENVIRON
    + OPTPROC_NO_ARGS
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
    38 /* full option count */, 33 /* user option count */,
    ntpdsim_full_usage, ntpdsim_short_usage,
    NULL, NULL,
    PKGDATADIR, ntpdsim_packager_info
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
        exit(NTPDSIM_EXIT_FAILURE);
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
    tOptions * const pOpt = &ntpdsimOptions;

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
        ntpdsimOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* ntpdsim-opts.c ends here */
