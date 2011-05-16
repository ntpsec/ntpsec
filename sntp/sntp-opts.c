/*  
 *  EDIT THIS FILE WITH CAUTION  (sntp-opts.c)
 *  
 *  It has been AutoGen-ed  May 16, 2011 at 07:36:53 AM by AutoGen 5.11.9
 *  From the definitions    sntp-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 35:0:10 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the sntp author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * This source file is copyrighted and licensed under the following terms:
 *
 * sntp copyright (c) 1970-2011 David L. Mills and/or others - all rights reserved
 *
 * see html/copyright.html
 * 
 */

#include <sys/types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define OPTION_CODE_COMPILE 1
#include "sntp-opts.h"

#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
static char const zCopyright[40] =
"sntp 4.2.7p168\n\
see html/copyright.html\n";
static char const zLicenseDescrip[25] =
"see html/copyright.html\n";

extern tUsageProc optionUsage;

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
 *  Ipv4 option description with
 *  "Must also have options" and "Incompatible options":
 */
static char const zIpv4Text[] =
        "Force IPv4 DNS name resolution";
static char const zIpv4_NAME[]               = "IPV4";
static char const zIpv4_Name[]               = "ipv4";
static const int
    aIpv4CantList[] = {
    INDEX_OPT_IPV6, NO_EQUIVALENT };
#define IPV4_FLAGS       (OPTST_DISABLED)

/*
 *  Ipv6 option description with
 *  "Must also have options" and "Incompatible options":
 */
static char const zIpv6Text[] =
        "Force IPv6 DNS name resolution";
static char const zIpv6_NAME[]               = "IPV6";
static char const zIpv6_Name[]               = "ipv6";
static const int
    aIpv6CantList[] = {
    INDEX_OPT_IPV4, NO_EQUIVALENT };
#define IPV6_FLAGS       (OPTST_DISABLED)

/*
 *  Authentication option description:
 */
static char const zAuthenticationText[] =
        "Enable authentication with the key auth-keynumber";
static char const zAuthentication_NAME[]     = "AUTHENTICATION";
static char const zAuthentication_Name[]     = "authentication";
#define AUTHENTICATION_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Bctimeout option description:
 */
static char const zBctimeoutText[] =
        "The number of seconds to wait for broadcasts";
static char const zBctimeout_NAME[]          = "BCTIMEOUT";
static char const zBctimeout_Name[]          = "bctimeout";
#define zBctimeoutDefaultArg         ((char const*)68)
#define BCTIMEOUT_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Broadcast option description:
 */
static char const zBroadcastText[] =
        "Listen to the address specified for broadcast time sync";
static char const zBroadcast_NAME[]          = "BROADCAST";
static char const zBroadcast_Name[]          = "broadcast";
#define BROADCAST_FLAGS       (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Concurrent option description:
 */
static char const zConcurrentText[] =
        "Concurrent query all IPs returned for host-name";
static char const zConcurrent_NAME[]         = "CONCURRENT";
static char const zConcurrent_Name[]         = "concurrent";
#define CONCURRENT_FLAGS       (OPTST_DISABLED | OPTST_STACKED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Debug_Level option description:
 */
static char const zDebug_LevelText[] =
        "Increase output debug message level";
static char const zDebug_Level_NAME[]        = "DEBUG_LEVEL";
static char const zDebug_Level_Name[]        = "debug-level";
#define DEBUG_LEVEL_FLAGS       (OPTST_DISABLED)

/*
 *  Set_Debug_Level option description:
 */
static char const zSet_Debug_LevelText[] =
        "Set the output debug message level";
static char const zSet_Debug_Level_NAME[]    = "SET_DEBUG_LEVEL";
static char const zSet_Debug_Level_Name[]    = "set-debug-level";
#define SET_DEBUG_LEVEL_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Headspace option description:
 */
static char const zHeadspaceText[] =
        "The gap (in milliseconds) between time requests";
static char const zHeadspace_NAME[]          = "HEADSPACE";
static char const zHeadspace_Name[]          = "headspace";
#define zHeadspaceDefaultArg         ((char const*)10)
#define HEADSPACE_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Kod option description:
 */
static char const zKodText[] =
        "KoD history filename";
static char const zKod_NAME[]                = "KOD";
static char const zKod_Name[]                = "kod";
static char const zKodDefaultArg[]             = "/var/db/ntp-kod";
#define KOD_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  Keyfile option description:
 */
static char const zKeyfileText[] =
        "Look in this file for the key specified with -a";
static char const zKeyfile_NAME[]            = "KEYFILE";
static char const zKeyfile_Name[]            = "keyfile";
#define KEYFILE_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  Filelog option description:
 */
static char const zFilelogText[] =
        "Log to specified logfile";
static char const zFilelog_NAME[]            = "FILELOG";
static char const zFilelog_Name[]            = "filelog";
#define FILELOG_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_FILE))

/*
 *  Steplimit option description:
 */
static char const zSteplimitText[] =
        "Adjustments less than steplimit msec will be slewed.";
static char const zSteplimit_NAME[]          = "STEPLIMIT";
static char const zSteplimit_Name[]          = "steplimit";
#define STEPLIMIT_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Ntpversion option description:
 */
static char const zNtpversionText[] =
        "Send <int> as our NTP version";
static char const zNtpversion_NAME[]         = "NTPVERSION";
static char const zNtpversion_Name[]         = "ntpversion";
#define zNtpversionDefaultArg        ((char const*)4)
#define NTPVERSION_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Usereservedport option description:
 */
static char const zUsereservedportText[] =
        "Use the NTP Reserved Port (port 123)";
static char const zUsereservedport_NAME[]    = "USERESERVEDPORT";
static char const zUsereservedport_Name[]    = "usereservedport";
#define USERESERVEDPORT_FLAGS       (OPTST_DISABLED)

/*
 *  Step option description:
 */
static char const zStepText[] =
        "OK to 'step' the time with settimeofday()";
static char const zStep_NAME[]               = "STEP";
static char const zStep_Name[]               = "step";
#define STEP_FLAGS       (OPTST_DISABLED)

/*
 *  Slew option description:
 */
static char const zSlewText[] =
        "OK to 'slew' the time with adjtime()";
static char const zSlew_NAME[]               = "SLEW";
static char const zSlew_Name[]               = "slew";
#define SLEW_FLAGS       (OPTST_DISABLED)

/*
 *  Uctimeout option description:
 */
static char const zUctimeoutText[] =
        "The number of seconds to wait for unicast responses";
static char const zUctimeout_NAME[]          = "UCTIMEOUT";
static char const zUctimeout_Name[]          = "uctimeout";
#define zUctimeoutDefaultArg         ((char const*)5)
#define UCTIMEOUT_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Wait option description:
 */
static char const zWaitText[] =
        "Wait for pending replies (if not setting the time)";
static char const zWait_NAME[]               = "WAIT";
static char const zNotWait_Name[]            = "no-wait";
static char const zNotWait_Pfx[]             = "no";
#define zWait_Name                   (zNotWait_Name + 3)
#define WAIT_FLAGS       (OPTST_INITENABLED)

/*
 *  Help/More_Help/Version option descriptions:
 */
static char const zHelpText[]          = "Display extended usage information and exit";
static char const zHelp_Name[]         = "help";
#ifdef HAVE_WORKING_FORK
#define OPTST_MORE_HELP_FLAGS   (OPTST_IMM | OPTST_NO_INIT)
static char const zMore_Help_Name[]    = "more-help";
static char const zMore_HelpText[]     = "Extended usage information passed thru pager";
#else
#define OPTST_MORE_HELP_FLAGS   (OPTST_OMITTED | OPTST_NO_INIT)
#define zMore_Help_Name   NULL
#define zMore_HelpText    NULL
#endif
#ifdef NO_OPTIONAL_OPT_ARGS
#  define OPTST_VERSION_FLAGS   OPTST_IMM | OPTST_NO_INIT
#else
#  define OPTST_VERSION_FLAGS   OPTST_SET_ARGTYPE(OPARG_TYPE_STRING) | \
                                OPTST_ARG_OPTIONAL | OPTST_IMM | OPTST_NO_INIT
#endif

static char const zVersionText[]       = "Output version information and exit";
static char const zVersion_Name[]      = "version";
static char const zSave_OptsText[]     = "Save the option state to a config file";
static char const zSave_Opts_Name[]    = "save-opts";
static char const zLoad_OptsText[]     = "Load options from a config file";
static char const zLoad_Opts_NAME[]    = "LOAD_OPTS";
static char const zNotLoad_Opts_Name[] = "no-load-opts";
static char const zNotLoad_Opts_Pfx[]  = "no";
#define zLoad_Opts_Name   (zNotLoad_Opts_Name + 3)
/*
 *  Declare option callback procedures
 */
#if defined(TEST_SNTP_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
static tOptProc
    doOptFilelog,    doOptKeyfile,    doOptKod,        doOptNtpversion,
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
    optionBooleanVal,    optionNestedVal,     optionNumericVal,
    optionPagedUsage,    optionPrintVersion,  optionResetOpt,
    optionStackArg,      optionTimeDate,      optionTimeVal,
    optionUnstackArg,    optionVersionStderr;
static tOptProc
    doOptFilelog,         doOptKeyfile,         doOptKod,
    doOptNtpversion,      doOptSet_Debug_Level, doOptSteplimit,
    doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level

#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level
#endif /* defined(TEST_SNTP_OPTS) */
#ifdef TEST_SNTP_OPTS
# define DOVERPROC optionVersionStderr
#else
# define DOVERPROC optionPrintVersion
#endif /* TEST_SNTP_OPTS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Sntp Option Descriptions.
 */
static tOptDesc optDesc[OPTION_CT] = {
  {  /* entry idx, value */ 0, VALUE_OPT_IPV4,
     /* equiv idx, value */ 0, VALUE_OPT_IPV4,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV4_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aIpv4CantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zIpv4Text, zIpv4_NAME, zIpv4_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_IPV6,
     /* equiv idx, value */ 1, VALUE_OPT_IPV6,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV6_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aIpv6CantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zIpv6Text, zIpv6_NAME, zIpv6_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_AUTHENTICATION,
     /* equiv idx, value */ 2, VALUE_OPT_AUTHENTICATION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ AUTHENTICATION_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zAuthenticationText, zAuthentication_NAME, zAuthentication_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_BCTIMEOUT,
     /* equiv idx, value */ 3, VALUE_OPT_BCTIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ BCTIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { zBctimeoutDefaultArg },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zBctimeoutText, zBctimeout_NAME, zBctimeout_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_BROADCAST,
     /* equiv idx, value */ 4, VALUE_OPT_BROADCAST,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ BROADCAST_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ zBroadcastText, zBroadcast_NAME, zBroadcast_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_CONCURRENT,
     /* equiv idx, value */ 5, VALUE_OPT_CONCURRENT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ CONCURRENT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionStackArg,
     /* desc, NAME, name */ zConcurrentText, zConcurrent_NAME, zConcurrent_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 6, VALUE_OPT_DEBUG_LEVEL,
     /* equiv idx, value */ 6, VALUE_OPT_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zDebug_LevelText, zDebug_Level_NAME, zDebug_Level_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 7, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equiv idx, value */ 7, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ SET_DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ SET_DEBUG_LEVEL_OPT_PROC,
     /* desc, NAME, name */ zSet_Debug_LevelText, zSet_Debug_Level_NAME, zSet_Debug_Level_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 8, VALUE_OPT_HEADSPACE,
     /* equiv idx, value */ 8, VALUE_OPT_HEADSPACE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ HEADSPACE_FLAGS, 0,
     /* last opt argumnt */ { zHeadspaceDefaultArg },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zHeadspaceText, zHeadspace_NAME, zHeadspace_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 9, VALUE_OPT_KOD,
     /* equiv idx, value */ 9, VALUE_OPT_KOD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KOD_FLAGS, 0,
     /* last opt argumnt */ { zKodDefaultArg },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptKod,
     /* desc, NAME, name */ zKodText, zKod_NAME, zKod_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 10, VALUE_OPT_KEYFILE,
     /* equiv idx, value */ 10, VALUE_OPT_KEYFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KEYFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptKeyfile,
     /* desc, NAME, name */ zKeyfileText, zKeyfile_NAME, zKeyfile_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 11, VALUE_OPT_FILELOG,
     /* equiv idx, value */ 11, VALUE_OPT_FILELOG,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ FILELOG_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptFilelog,
     /* desc, NAME, name */ zFilelogText, zFilelog_NAME, zFilelog_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 12, VALUE_OPT_STEPLIMIT,
     /* equiv idx, value */ 12, VALUE_OPT_STEPLIMIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ STEPLIMIT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptSteplimit,
     /* desc, NAME, name */ zSteplimitText, zSteplimit_NAME, zSteplimit_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 13, VALUE_OPT_NTPVERSION,
     /* equiv idx, value */ 13, VALUE_OPT_NTPVERSION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NTPVERSION_FLAGS, 0,
     /* last opt argumnt */ { zNtpversionDefaultArg },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptNtpversion,
     /* desc, NAME, name */ zNtpversionText, zNtpversion_NAME, zNtpversion_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 14, VALUE_OPT_USERESERVEDPORT,
     /* equiv idx, value */ 14, VALUE_OPT_USERESERVEDPORT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ USERESERVEDPORT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zUsereservedportText, zUsereservedport_NAME, zUsereservedport_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 15, VALUE_OPT_STEP,
     /* equiv idx, value */ 15, VALUE_OPT_STEP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ STEP_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zStepText, zStep_NAME, zStep_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 16, VALUE_OPT_SLEW,
     /* equiv idx, value */ 16, VALUE_OPT_SLEW,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SLEW_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zSlewText, zSlew_NAME, zSlew_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 17, VALUE_OPT_UCTIMEOUT,
     /* equiv idx, value */ 17, VALUE_OPT_UCTIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ UCTIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { zUctimeoutDefaultArg },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zUctimeoutText, zUctimeout_NAME, zUctimeout_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 18, VALUE_OPT_WAIT,
     /* equiv idx, value */ 18, VALUE_OPT_WAIT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ WAIT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zWaitText, zWait_NAME, zWait_Name,
     /* disablement strs */ zNotWait_Name, zNotWait_Pfx },

  {  /* entry idx, value */ INDEX_OPT_VERSION, VALUE_OPT_VERSION,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_VERSION_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ DOVERPROC,
     /* desc, NAME, name */ zVersionText, NULL, zVersion_Name,
     /* disablement strs */ NULL, NULL },



  {  /* entry idx, value */ INDEX_OPT_HELP, VALUE_OPT_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_IMM | OPTST_NO_INIT, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doUsageOpt,
     /* desc, NAME, name */ zHelpText, NULL, zHelp_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_MORE_HELP, VALUE_OPT_MORE_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_MORE_HELP_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL,  NULL,
     /* option proc      */ optionPagedUsage,
     /* desc, NAME, name */ zMore_HelpText, NULL, zMore_Help_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_SAVE_OPTS, VALUE_OPT_SAVE_OPTS,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_SET_ARGTYPE(OPARG_TYPE_STRING)
                          | OPTST_ARG_OPTIONAL | OPTST_NO_INIT, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL,  NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zSave_OptsText, NULL, zSave_Opts_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_LOAD_OPTS, VALUE_OPT_LOAD_OPTS,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ OPTST_SET_ARGTYPE(OPARG_TYPE_STRING)
			  | OPTST_DISABLE_IMM, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionLoadOpt,
     /* desc, NAME, name */ zLoad_OptsText, zLoad_Opts_NAME, zLoad_Opts_Name,
     /* disablement strs */ zNotLoad_Opts_Name, zNotLoad_Opts_Pfx }
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Sntp Option Environment
 */
static char const zPROGNAME[5] = "SNTP";
static char const zUsageTitle[154] =
"sntp - standard Simple Network Time Protocol program - Ver. 4.2.7p168\n\
USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... \\\n\
\t\t[ hostname-or-IP ...]\n";
static char const zRcName[7] = ".ntprc";
static char const * const apzHomeList[3] = {
    "$HOME",
    ".",
    NULL };

static char const zBugsAddr[34]    = "http://bugs.ntp.org, bugs@ntp.org";
static char const zExplain[] = "\n\n";
static char const zDetail[351] = "\n\
sntp implements the Simple Network Time Protocol and is used to query an\n\
NTP or SNTP server and either display the time or set the local system's\n\
time (given suitable privilege).\n\n\
It can be run interactively from the command line or as a cron job.\n\n\
NTP and SNTP are defined by RFC 5905, which obsoletes RFC 4330 and RFC\n\
1305.\n";
static char const zFullVersion[] = SNTP_FULL_VERSION;
/* extracted from optcode.tlib near line 515 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define sntp_full_usage NULL
#define sntp_short_usage NULL
#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

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

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(tOptions * pOptions, tOptDesc * pOptDesc)
{
    (void)pOptions;
    USAGE(SNTP_EXIT_SUCCESS);
}

#if ! defined(TEST_SNTP_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the set-debug-level option.
 */
static void
doOptSet_Debug_Level(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from debug-opt.def, line 27 */
DESC(DEBUG_LEVEL).optOccCt = atoi( pOptDesc->pzLastArg );
}
#endif /* defined(TEST_SNTP_OPTS) */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the kod option.
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

    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the keyfile option.
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

    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the filelog option.
 */
static void
doOptFilelog(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static teOptFileType const  type =
        FTYPE_MODE_MAY_EXIST + FTYPE_MODE_NO_OPEN;
    static tuFileMode           mode;
#ifndef O_CLOEXEC
#  define O_CLOEXEC 0
#endif
    mode.file_flags = O_CLOEXEC;

    optionFileCheck(pOptions, pOptDesc, type, mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the steplimit option.
 */
static void
doOptSteplimit(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static const struct {long const rmin, rmax;} rng[1] = {
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the ntpversion option.
 */
static void
doOptNtpversion(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static const struct {long const rmin, rmax;} rng[1] = {
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
/* extracted from optmain.tlib near line 128 */

#if defined(TEST_SNTP_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell(tOptions*);

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
/* extracted from optcode.tlib near line 666 */

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
        exit(SNTP_EXIT_FAILURE);
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
