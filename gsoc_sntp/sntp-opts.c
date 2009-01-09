/*  
 *  EDIT THIS FILE WITH CAUTION  (sntp-opts.c)
 *  
 *  It has been AutoGen-ed  Friday January  9, 2009 at 07:17:35 AM EST
 *  From the definitions    sntp-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 29:0:4 templates.
 */

/*
 *  This file was produced by an AutoOpts template.  AutoOpts is a
 *  copyrighted work.  This source file is not encumbered by AutoOpts
 *  licensing, but is provided under the licensing terms chosen by the
 *  sntp author or copyright holder.  AutoOpts is licensed under
 *  the terms of the LGPL.  The redistributable library (``libopts'') is
 *  licensed under the terms of either the LGPL or, at the users discretion,
 *  the BSD license.  See the AutoOpts and/or libopts sources for details.
 *
 * This source file is copyrighted and licensed under the following terms:
 *
 * sntp copyright 2008 ntp.org - all rights reserved
 *
 * 
 */


#include <limits.h>

#define OPTION_CODE_COMPILE 1
#include "sntp-opts.h"

#ifdef  __cplusplus
extern "C" {
#endif
tSCC zCopyright[] =
       "sntp copyright (c) 2008 ntp.org, all rights reserved";
tSCC zCopyrightNotice[] =
       
/* extracted from sntp-opts.def near line 17 */
"";
extern tUsageProc optionUsage;

#ifndef NULL
#  define NULL 0
#endif
#ifndef EXIT_SUCCESS
#  define  EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#  define  EXIT_FAILURE 1
#endif
/*
 *  Ipv4 option description:
 */
tSCC    zIpv4Text[] =
        "Force IPv4 DNS name resolution";
tSCC    zIpv4_NAME[]               = "IPV4";
tSCC    zIpv4_Name[]               = "ipv4";
#define IPV4_FLAGS       (OPTST_DISABLED)

/*
 *  Ipv6 option description:
 */
tSCC    zIpv6Text[] =
        "Force IPv6 DNS name resolution";
tSCC    zIpv6_NAME[]               = "IPV6";
tSCC    zIpv6_Name[]               = "ipv6";
#define IPV6_FLAGS       (OPTST_DISABLED)

/*
 *  Normalverbose option description:
 */
tSCC    zNormalverboseText[] =
        "Normal verbose";
tSCC    zNormalverbose_NAME[]      = "NORMALVERBOSE";
tSCC    zNormalverbose_Name[]      = "normalverbose";
#define NORMALVERBOSE_FLAGS       (OPTST_DISABLED)

/*
 *  Kod option description:
 */
tSCC    zKodText[] =
        "Specify a file for the KOD packet storage";
tSCC    zKod_NAME[]                = "KOD";
tSCC    zKod_Name[]                = "kod";
#define KOD_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Syslog option description with
 *  "Must also have options" and "Incompatible options":
 */
tSCC    zSyslogText[] =
        "Logging with syslog";
tSCC    zSyslog_NAME[]             = "SYSLOG";
tSCC    zSyslog_Name[]             = "syslog";
static const int
    aSyslogCantList[] = {
    INDEX_OPT_FILELOG, NO_EQUIVALENT };
#define SYSLOG_FLAGS       (OPTST_DISABLED)

/*
 *  Filelog option description with
 *  "Must also have options" and "Incompatible options":
 */
tSCC    zFilelogText[] =
        "Logging to specified logfile";
tSCC    zFilelog_NAME[]            = "FILELOG";
tSCC    zFilelog_Name[]            = "filelog";
static const int
    aFilelogCantList[] = {
    INDEX_OPT_SYSLOG, NO_EQUIVALENT };
#define FILELOG_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Settod option description with
 *  "Must also have options" and "Incompatible options":
 */
tSCC    zSettodText[] =
        "Set (step) the time with settimeofday()";
tSCC    zSettod_NAME[]             = "SETTOD";
tSCC    zSettod_Name[]             = "settod";
static const int
    aSettodCantList[] = {
    INDEX_OPT_ADJTIME, NO_EQUIVALENT };
#define SETTOD_FLAGS       (OPTST_DISABLED)

/*
 *  Adjtime option description with
 *  "Must also have options" and "Incompatible options":
 */
tSCC    zAdjtimeText[] =
        "Set (slew) the time with adjtime()";
tSCC    zAdjtime_NAME[]            = "ADJTIME";
tSCC    zAdjtime_Name[]            = "adjtime";
static const int
    aAdjtimeCantList[] = {
    INDEX_OPT_SETTOD, NO_EQUIVALENT };
#define ADJTIME_FLAGS       (OPTST_DISABLED)

/*
 *  Broadcast option description:
 */
tSCC    zBroadcastText[] =
        "Use broadcast packages from the broadcast address specified for synchronisation";
tSCC    zBroadcast_NAME[]          = "BROADCAST";
tSCC    zBroadcast_Name[]          = "broadcast";
#define BROADCAST_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Timeout option description:
 */
tSCC    zTimeoutText[] =
        "Specify the number of seconds until SNTP times out when waiting for broadcast packets";
tSCC    zTimeout_NAME[]            = "TIMEOUT";
tSCC    zTimeout_Name[]            = "timeout";
#define TIMEOUT_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Authentication option description:
 */
tSCC    zAuthenticationText[] =
        "Enable authentication with the key keyno. This option is used as -a keyno";
tSCC    zAuthentication_NAME[]     = "AUTHENTICATION";
tSCC    zAuthentication_Name[]     = "authentication";
#define AUTHENTICATION_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Keyfile option description:
 */
tSCC    zKeyfileText[] =
        "Specify a keyfile. SNTP will look in this file for the key specified with -a";
tSCC    zKeyfile_NAME[]            = "KEYFILE";
tSCC    zKeyfile_Name[]            = "keyfile";
#define KEYFILE_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Help/More_Help/Version option descriptions:
 */
tSCC zHelpText[]       = "Display usage information and exit";
tSCC zHelp_Name[]      = "help";

tSCC zMore_HelpText[]  = "Extended usage information passed thru pager";
tSCC zMore_Help_Name[] = "more-help";

tSCC zVersionText[]    = "Output version information and exit";
tSCC zVersion_Name[]   = "version";

/*
 *  Save/Load_Opts option description:
 */
tSCC zSave_OptsText[]     = "Save the option state to a config file";
tSCC zSave_Opts_Name[]    = "save-opts";

tSCC zLoad_OptsText[]     = "Load options from a config file";
tSCC zLoad_Opts_NAME[]    = "LOAD_OPTS";

tSCC zNotLoad_Opts_Name[] = "no-load-opts";
tSCC zNotLoad_Opts_Pfx[]  = "no";
#define zLoad_Opts_Name   (zNotLoad_Opts_Name + 3)
/*
 *  Declare option callback procedures
 */
#if defined(TEST_SNTP_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
extern tOptProc
    optionNumericVal, optionPagedUsage, optionVersionStderr;
static tOptProc
    doUsageOpt;

#else /* NOT defined TEST_SNTP_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    optionNumericVal, optionPagedUsage, optionPrintVersion;
static tOptProc
    doUsageOpt;
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
static tOptDesc optDesc[ OPTION_CT ] = {
  {  /* entry idx, value */ 0, VALUE_OPT_IPV4,
     /* equiv idx, value */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV4_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zIpv4Text, zIpv4_NAME, zIpv4_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_IPV6,
     /* equiv idx, value */ NOLIMIT, NOLIMIT,
     /* equivalenced to  */ INDEX_OPT_IPV4,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IPV6_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zIpv6Text, zIpv6_NAME, zIpv6_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_NORMALVERBOSE,
     /* equiv idx, value */ 2, VALUE_OPT_NORMALVERBOSE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NORMALVERBOSE_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zNormalverboseText, zNormalverbose_NAME, zNormalverbose_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_KOD,
     /* equiv idx, value */ 3, VALUE_OPT_KOD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KOD_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zKodText, zKod_NAME, zKod_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_SYSLOG,
     /* equiv idx, value */ 4, VALUE_OPT_SYSLOG,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SYSLOG_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aSyslogCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zSyslogText, zSyslog_NAME, zSyslog_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_FILELOG,
     /* equiv idx, value */ 5, VALUE_OPT_FILELOG,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ FILELOG_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aFilelogCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zFilelogText, zFilelog_NAME, zFilelog_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 6, VALUE_OPT_SETTOD,
     /* equiv idx, value */ 6, VALUE_OPT_SETTOD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SETTOD_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aSettodCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zSettodText, zSettod_NAME, zSettod_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 7, VALUE_OPT_ADJTIME,
     /* equiv idx, value */ 7, VALUE_OPT_ADJTIME,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ ADJTIME_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, aAdjtimeCantList,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zAdjtimeText, zAdjtime_NAME, zAdjtime_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 8, VALUE_OPT_BROADCAST,
     /* equiv idx, value */ 8, VALUE_OPT_BROADCAST,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ BROADCAST_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zBroadcastText, zBroadcast_NAME, zBroadcast_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 9, VALUE_OPT_TIMEOUT,
     /* equiv idx, value */ 9, VALUE_OPT_TIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ TIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zTimeoutText, zTimeout_NAME, zTimeout_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 10, VALUE_OPT_AUTHENTICATION,
     /* equiv idx, value */ 10, VALUE_OPT_AUTHENTICATION,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ AUTHENTICATION_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ zAuthenticationText, zAuthentication_NAME, zAuthentication_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 11, VALUE_OPT_KEYFILE,
     /* equiv idx, value */ 11, VALUE_OPT_KEYFILE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ KEYFILE_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zKeyfileText, zKeyfile_NAME, zKeyfile_Name,
     /* disablement strs */ NULL, NULL },

#ifdef NO_OPTIONAL_OPT_ARGS
#  define VERSION_OPT_FLAGS     OPTST_IMM | OPTST_NO_INIT
#else
#  define VERSION_OPT_FLAGS     OPTST_SET_ARGTYPE(OPARG_TYPE_STRING) | \
                                OPTST_ARG_OPTIONAL | OPTST_IMM | OPTST_NO_INIT
#endif

  {  /* entry idx, value */ INDEX_OPT_VERSION, VALUE_OPT_VERSION,
     /* equiv idx value  */ NO_EQUIVALENT, 0,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ VERSION_OPT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ DOVERPROC,
     /* desc, NAME, name */ zVersionText, NULL, zVersion_Name,
     /* disablement strs */ NULL, NULL },

#undef VERSION_OPT_FLAGS


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
     /* opt state flags  */ OPTST_IMM | OPTST_NO_INIT, 0,
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
     /* opt state flags  */ OPTST_SET_ARGTYPE(OPARG_TYPE_STRING) \
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
tSCC   zPROGNAME[]   = "SNTP";
tSCC   zUsageTitle[] =
"sntp - standard SNTP program - Ver. 4.2.5p153\n\
USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... ...\n";
tSCC   zRcName[]     = ".ntprc";
tSCC*  apzHomeList[] = {
       "$HOME",
       ".",
       NULL };

tSCC   zBugsAddr[]    = "http://bugs.ntp.org, bugs@ntp.org";
#define zExplain NULL
tSCC    zDetail[]     = "\n\
.I sntp\n\
can be used as a SNTP client to query a NTP or SNTP server and either display\n\
the time or set the local system's time (given suitable privilege).  It can be\n\
run as an interactive command or in a\n\
.I cron\n\
job.\n\
NTP is the Network Time Protocol (RFC 1305) and SNTP is the\n\
Simple Network Time Protocol (RFC 2030, which supersedes RFC 1769).\n";
tSCC    zFullVersion[] = SNTP_FULL_VERSION;
/* extracted from /usr/local/gnu/share/autogen/optcode.tpl near line 408 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */

tOptions sntpOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_ENVIRON
    + OPTPROC_ARGS_REQ
    + OPTPROC_HAS_IMMED ),
    0, NULL,                    /* current option index, current option */
    NULL,         NULL,         zPROGNAME,
    zRcName,      zCopyright,   zCopyrightNotice,
    zFullVersion, apzHomeList,  zUsageTitle,
    zExplain,     zDetail,      optDesc,
    zBugsAddr,                  /* address to send bugs to */
    NULL, NULL,                 /* extensions/saved state  */
    optionUsage,       /* usage procedure */
    translate_option_strings,   /* translation procedure */
    /*
     *  Indexes to special options
     */
    { INDEX_OPT_MORE_HELP,
      INDEX_OPT_SAVE_OPTS,
      NO_EQUIVALENT /* index of '-#' option */,
      NO_EQUIVALENT /* index of default opt */
    },
    17 /* full option count */, 12 /* user option count */
};

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(
    tOptions*   pOptions,
    tOptDesc*   pOptDesc )
{
    USAGE( EXIT_SUCCESS );
}
/* extracted from /usr/local/gnu/share/autogen/optmain.tpl near line 92 */

#if defined(TEST_SNTP_OPTS) /* TEST MAIN PROCEDURE: */

int
main( int argc, char** argv )
{
    int res = EXIT_SUCCESS;
    (void)optionProcess( &sntpOptions, argc, argv );
    {
        void optionPutShell( tOptions* );
        optionPutShell( &sntpOptions );
    }
    return res;
}
#endif  /* defined TEST_SNTP_OPTS */
/* extracted from /usr/local/gnu/share/autogen/optcode.tpl near line 514 */

#if ENABLE_NLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <autoopts/usage-txt.h>

static char* AO_gettext( char const* pz );
static void  coerce_it(void** s);

static char*
AO_gettext( char const* pz )
{
    char* pzRes;
    if (pz == NULL)
        return NULL;
    pzRes = _(pz);
    if (pzRes == pz)
        return pzRes;
    pzRes = strdup( pzRes );
    if (pzRes == NULL) {
        fputs( _("No memory for duping translated strings\n"), stderr );
        exit( EXIT_FAILURE );
    }
    return pzRes;
}

static void coerce_it(void** s) { *s = AO_gettext(*s); }
#define COERSION(_f) \
  coerce_it((void*)&(sntpOptions._f))

/*
 *  This invokes the translation code (e.g. gettext(3)).
 */
static void
translate_option_strings( void )
{
    /*
     *  Guard against re-translation.  It won't work.  The strings will have
     *  been changed by the first pass through this code.  One shot only.
     */
    if (option_usage_text.field_ct == 0)
        return;
    /*
     *  Do the translations.  The first pointer follows the field count field.
     *  The field count field is the size of a pointer.
     */
    {
        char** ppz = (char**)(void*)&(option_usage_text);
        int    ix  = option_usage_text.field_ct;

        do {
            ppz++;
            *ppz = AO_gettext(*ppz);
        } while (--ix > 0);
    }
    option_usage_text.field_ct = 0;

    {
        tOptDesc* pOD = sntpOptions.pOptDesc;
        int       ix  = sntpOptions.optCt;

        for (;;) {
            pOD->pzText           = AO_gettext(pOD->pzText);
            pOD->pz_NAME          = AO_gettext(pOD->pz_NAME);
            pOD->pz_Name          = AO_gettext(pOD->pz_Name);
            pOD->pz_DisableName   = AO_gettext(pOD->pz_DisableName);
            pOD->pz_DisablePfx    = AO_gettext(pOD->pz_DisablePfx);
            if (--ix <= 0)
                break;
            pOD++;
        }
    }
    COERSION(pzCopyright);
    COERSION(pzCopyNotice);
    COERSION(pzFullVersion);
    COERSION(pzUsageTitle);
    COERSION(pzExplain);
    COERSION(pzDetail);
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* sntp-opts.c ends here */
