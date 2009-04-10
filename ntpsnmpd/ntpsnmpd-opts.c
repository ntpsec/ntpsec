/*  
 *  EDIT THIS FILE WITH CAUTION  (ntpsnmpd-opts.c)
 *  
 *  It has been AutoGen-ed  Friday April 10, 2009 at 07:49:07 AM EDT
 *  From the definitions    ntpsnmpd-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 29:0:4 templates.
 */

/*
 *  This file was produced by an AutoOpts template.  AutoOpts is a
 *  copyrighted work.  This source file is not encumbered by AutoOpts
 *  licensing, but is provided under the licensing terms chosen by the
 *  ntpsnmpd author or copyright holder.  AutoOpts is licensed under
 *  the terms of the LGPL.  The redistributable library (``libopts'') is
 *  licensed under the terms of either the LGPL or, at the users discretion,
 *  the BSD license.  See the AutoOpts and/or libopts sources for details.
 *
 * This source file is copyrighted and licensed under the following terms:
 *
 * ntpsnmpd copyright 1970-2008 David L. Mills and/or others - all rights reserved
 *
 * see html/copyright.html
 */


#include <limits.h>

#define OPTION_CODE_COMPILE 1
#include "ntpsnmpd-opts.h"

#ifdef  __cplusplus
extern "C" {
#endif
tSCC zCopyright[] =
       "ntpsnmpd copyright (c) 1970-2008 David L. Mills and/or others, all rights reserved";
tSCC zCopyrightNotice[] =
       
/* extracted from ../include/copyright.def near line 8 */
"see html/copyright.html";
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
 *  Nofork option description:
 */
tSCC    zNoforkText[] =
        "Do not fork";
tSCC    zNofork_NAME[]             = "NOFORK";
tSCC    zNofork_Name[]             = "nofork";
#define NOFORK_FLAGS       (OPTST_DISABLED)

/*
 *  Syslog option description:
 */
tSCC    zSyslogText[] =
        "Log to syslog()";
tSCC    zSyslog_NAME[]             = "SYSLOG";
tSCC    zSyslog_Name[]             = "syslog";
#define SYSLOG_FLAGS       (OPTST_DISABLED)

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
#if defined(TEST_NTPSNMPD_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
extern tOptProc
    optionPagedUsage, optionVersionStderr;
static tOptProc
    doUsageOpt;

#else /* NOT defined TEST_NTPSNMPD_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    optionPagedUsage, optionPrintVersion;
static tOptProc
    doUsageOpt;
#endif /* defined(TEST_NTPSNMPD_OPTS) */
#ifdef TEST_NTPSNMPD_OPTS
# define DOVERPROC optionVersionStderr
#else
# define DOVERPROC optionPrintVersion
#endif /* TEST_NTPSNMPD_OPTS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntpsnmpd Option Descriptions.
 */
static tOptDesc optDesc[ OPTION_CT ] = {
  {  /* entry idx, value */ 0, VALUE_OPT_NOFORK,
     /* equiv idx, value */ 0, VALUE_OPT_NOFORK,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ NOFORK_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zNoforkText, zNofork_NAME, zNofork_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_SYSLOG,
     /* equiv idx, value */ 1, VALUE_OPT_SYSLOG,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SYSLOG_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zSyslogText, zSyslog_NAME, zSyslog_Name,
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
 *  Define the Ntpsnmpd Option Environment
 */
tSCC   zPROGNAME[]   = "NTPSNMPD";
tSCC   zUsageTitle[] =
"ntpsnmpd - NTP SNMP MIB agent - Ver. 4.2.5p163\n\
USAGE:  %s [ -<flag> | --<name> ]...\n";
tSCC   zRcName[]     = ".ntprc";
tSCC*  apzHomeList[] = {
       "$HOME",
       ".",
       NULL };

tSCC   zBugsAddr[]    = "http://bugs.ntp.org, bugs@ntp.org";
#define zExplain NULL
#define zDetail         NULL
tSCC    zFullVersion[] = NTPSNMPD_FULL_VERSION;
/* extracted from /usr/local/gnu/share/autogen/optcode.tpl near line 408 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */

tOptions ntpsnmpdOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_ENVIRON
    + OPTPROC_NO_ARGS
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
    7 /* full option count */, 2 /* user option count */
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

#if defined(TEST_NTPSNMPD_OPTS) /* TEST MAIN PROCEDURE: */

int
main( int argc, char** argv )
{
    int res = EXIT_SUCCESS;
    (void)optionProcess( &ntpsnmpdOptions, argc, argv );
    {
        void optionPutShell( tOptions* );
        optionPutShell( &ntpsnmpdOptions );
    }
    return res;
}
#endif  /* defined TEST_NTPSNMPD_OPTS */
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
  coerce_it((void*)&(ntpsnmpdOptions._f))

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
        tOptDesc* pOD = ntpsnmpdOptions.pOptDesc;
        int       ix  = ntpsnmpdOptions.optCt;

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
/* ntpsnmpd-opts.c ends here */
