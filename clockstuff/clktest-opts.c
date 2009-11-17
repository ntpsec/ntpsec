/*  
 *  EDIT THIS FILE WITH CAUTION  (clktest-opts.c)
 *  
 *  It has been AutoGen-ed  November 17, 2009 at 06:37:09 AM by AutoGen 5.10
 *  From the definitions    clktest-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 33:0:8 templates.
 */

/*
 *  This file was produced by an AutoOpts template.  AutoOpts is a
 *  copyrighted work.  This source file is not encumbered by AutoOpts
 *  licensing, but is provided under the licensing terms chosen by the
 *  clktest author or copyright holder.  AutoOpts is licensed under
 *  the terms of the LGPL.  The redistributable library (``libopts'') is
 *  licensed under the terms of either the LGPL or, at the users discretion,
 *  the BSD license.  See the AutoOpts and/or libopts sources for details.
 *
 * This source file is copyrighted and licensed under the following terms:
 *
 * clktest copyright (c) 1970-2009 David L. Mills and/or others - all rights reserved
 *
 * see html/copyright.html
 */

#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
extern FILE * option_usage_fp;
#define OPTION_CODE_COMPILE 1
#include "clktest-opts.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
tSCC zCopyright[] =
       "clktest copyright (c) 1970-2009 David L. Mills and/or others, all rights reserved"
/* extracted from /deacon/backroom/ntp-dev/include/copyright.def near line 8 */
;
tSCC zCopyrightNotice[24] =
"see html/copyright.html";

extern tUsageProc optionUsage;

/*
 *  global included definitions
 */
extern int speed;
extern int ttflags;

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
 *  Speed option description:
 */
tSCC    zSpeedText[] =
        "Set the rate according to the speed";
tSCC    zSpeed_NAME[]              = "SPEED";
tSCC    zSpeed_Name[]              = "speed";
#define SPEED_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Debug option description:
 */
tSCC    zDebugText[] =
        "Turn on debugging output";
tSCC    zDebug_NAME[]              = "DEBUG";
tSCC    zDebug_Name[]              = "debug";
#define DEBUG_FLAGS       (OPTST_DISABLED)

/*
 *  Crmod option description:
 */
tSCC    zCrmodText[] =
        "Turn on CRMOD flag";
tSCC    zCrmod_NAME[]              = "CRMOD";
tSCC    zCrmod_Name[]              = "crmod";
#define CRMOD_FLAGS       (OPTST_DISABLED)

/*
 *  Command option description with
 *  "Must also have options" and "Incompatible options":
 */
tSCC    zCommandText[] =
        "";
tSCC    zCommand_NAME[]            = "COMMAND";
tSCC    zCommand_Name[]            = "command";
static const int
    aCommandMustList[] = {
    INDEX_OPT_TIMEOUT, NO_EQUIVALENT };
#define COMMAND_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Timeout option description:
 */
tSCC    zTimeoutText[] =
        "Limit wait for command completion";
tSCC    zTimeout_NAME[]            = "TIMEOUT";
tSCC    zTimeout_Name[]            = "timeout";
#define TIMEOUT_FLAGS       (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/*
 *  Help/More_Help/Version option descriptions:
 */
tSCC zHelpText[]          = "Display extended usage information and exit";
tSCC zHelp_Name[]         = "help";
#ifdef HAVE_WORKING_FORK
#define OPTST_MORE_HELP_FLAGS   (OPTST_IMM | OPTST_NO_INIT)
tSCC zMore_Help_Name[]    = "more-help";
tSCC zMore_HelpText[]     = "Extended usage information passed thru pager";
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

tSCC zVersionText[]       = "Output version information and exit";
tSCC zVersion_Name[]      = "version";
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
#if defined(TEST_CLKTEST_OPTS)
/*
 *  Under test, omit argument processing, or call optionStackArg,
 *  if multiple copies are allowed.
 */
extern tOptProc
    optionPagedUsage, optionVersionStderr;
static tOptProc
    doOptTimeout, doUsageOpt;

/*
 *  #define map the "normal" callout procs to the test ones...
 */
#define SPEED_OPT_PROC optionStackArg
#define CRMOD_OPT_PROC optionStackArg


#else /* NOT defined TEST_CLKTEST_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    optionPagedUsage, optionPrintVersion;
static tOptProc
    doOptCrmod, doOptSpeed, doOptTimeout, doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define SPEED_OPT_PROC doOptSpeed
#define CRMOD_OPT_PROC doOptCrmod

#define SPEED_OPT_PROC doOptSpeed
#define CRMOD_OPT_PROC doOptCrmod
#endif /* defined(TEST_CLKTEST_OPTS) */
#ifdef TEST_CLKTEST_OPTS
# define DOVERPROC optionVersionStderr
#else
# define DOVERPROC optionPrintVersion
#endif /* TEST_CLKTEST_OPTS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Clktest Option Descriptions.
 */
static tOptDesc optDesc[ OPTION_CT ] = {
  {  /* entry idx, value */ 0, VALUE_OPT_SPEED,
     /* equiv idx, value */ 0, VALUE_OPT_SPEED,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SPEED_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ SPEED_OPT_PROC,
     /* desc, NAME, name */ zSpeedText, zSpeed_NAME, zSpeed_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_DEBUG,
     /* equiv idx, value */ 1, VALUE_OPT_DEBUG,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ DEBUG_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zDebugText, zDebug_NAME, zDebug_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_CRMOD,
     /* equiv idx, value */ 2, VALUE_OPT_CRMOD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ CRMOD_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ CRMOD_OPT_PROC,
     /* desc, NAME, name */ zCrmodText, zCrmod_NAME, zCrmod_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_COMMAND,
     /* equiv idx, value */ 3, VALUE_OPT_COMMAND,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ COMMAND_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ aCommandMustList, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ zCommandText, zCommand_NAME, zCommand_Name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_TIMEOUT,
     /* equiv idx, value */ 4, VALUE_OPT_TIMEOUT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ TIMEOUT_FLAGS, 0,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptTimeout,
     /* desc, NAME, name */ zTimeoutText, zTimeout_NAME, zTimeout_Name,
     /* disablement strs */ NULL, NULL },

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
 *  Define the Clktest Option Environment
 */
tSCC   zPROGNAME[]   = "CLKTEST";
tSCC   zUsageTitle[] =
"clktest - test the clock line discipline - Ver. 4.2.5p245-RC\n\
USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]... <tty_device>\n";
tSCC   zRcName[]     = ".ntprc";
tSCC*  apzHomeList[] = {
       "$HOME",
       ".",
       NULL };

tSCC   zBugsAddr[]    = "http://bugs.ntp.org, bugs@ntp.org";
#define zExplain NULL
tSCC    zDetail[]     = "\n\
Clktest can be used to test the clock line discipline (CLKLDISC,\n\
it must be available), and to take a look at radio clocks attached\n\
to a serial port.\n";
tSCC    zFullVersion[] = CLKTEST_FULL_VERSION;
/* extracted from /usr/local/gnu/share/autogen/optcode.tpl near line 495 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define clktest_full_usage NULL
#define clktest_short_usage NULL
tOptions clktestOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_REQ_OPT
    + OPTPROC_ENVIRON
    + OPTPROC_ARGS_REQ ),
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
    { INDEX_OPT_MORE_HELP, /* more-help option index */
      INDEX_OPT_SAVE_OPTS, /* save option index */
      NO_EQUIVALENT, /* '-#' option index */
      NO_EQUIVALENT /* index of default opt */
    },
    10 /* full option count */, 5 /* user option count */,
    clktest_full_usage, clktest_short_usage,
    NULL, NULL
};

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(
    tOptions*   pOptions,
    tOptDesc*   pOptDesc )
{
    (void)pOptions;
    USAGE( EXIT_SUCCESS );
}

#if ! defined(TEST_CLKTEST_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the speed option.
 */
static void
doOptSpeed(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from clktest-opts.def, line 130 */
    /*
     * speed table
     */
    static struct speeds {
            const unsigned int bps;
            const unsigned int rate;
    } *spd, speedtab[] = {
        { 300,          B300 },
        { 1200,         B1200 },
        { 2400,         B2400 },
        { 4800,         B4800 },
        { 9600,         B9600 },
        { 19200,        EXTA },
        { 38400,        EXTB },
        { 0,            0 }
    };
    unsigned long tmp;

    if (!atouint(pOptDesc->pzLastArg, &tmp)) {
        fprintf(stderr, "%s error:  not a number:  ``%s''\n",
                pOptions->pzProgName, pOptDesc->pzLastArg );
        USAGE( EXIT_FAILURE );
    }

    for (spd = speedtab;;) {
        if (tmp == spd->bps)
            break;
        spd++;
        if (spd->bps == 0) {
            fprintf(stderr, "%s: speed %lu is unsupported\n",
                    pOptions->pzProgName, tmp);
            USAGE( EXIT_FAILURE );
        }
    }

    speed = spd->rate;
}
#endif /* defined(TEST_CLKTEST_OPTS) */

#if ! defined(TEST_CLKTEST_OPTS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the crmod option.
 */
static void
doOptCrmod(tOptions* pOptions, tOptDesc* pOptDesc)
{
    /* extracted from clktest-opts.def, line 184 */
    ttflags |= CRMOD;
}
#endif /* defined(TEST_CLKTEST_OPTS) */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the timeout option.
 */
static void
doOptTimeout(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static const struct {long const rmin, rmax;} rng[1] = {
        { 1, 600 } };
    long val;
    int  ix;
    char * pzEnd;

    if (pOptions <= OPTPROC_EMIT_LIMIT)
        goto emit_ranges;

    errno = 0;
    val = strtol(pOptDesc->optArg.argString, &pzEnd, 0);
    if ((pOptDesc->optArg.argString == pzEnd) || (errno != 0))
        goto bad_value;

    if (*pzEnd != '\0')
        goto bad_value;
    for (ix = 0; ix < 1; ix++) {
        if (val < rng[ix].rmin)
            continue;  /* ranges need not be ordered. */
        if (val == rng[ix].rmin)
            goto valid_return;
        if (rng[ix].rmax == LONG_MIN)
            continue;
        if (val <= rng[ix].rmax)
            goto valid_return;
    }

  bad_value:

    option_usage_fp = stderr;

  emit_ranges:
    optionShowRange(pOptions, pOptDesc, (void *)rng, 1);
    return;

  valid_return:
    if ((pOptDesc->fOptState & OPTST_ALLOC_ARG) != 0) {
        free((void *)pOptDesc->optArg.argString);
        pOptDesc->fOptState &= ~OPTST_ALLOC_ARG;
    }
    pOptDesc->optArg.argInt = val;
}
/* extracted from /usr/local/gnu/share/autogen/optmain.tpl near line 109 */

#if defined(TEST_CLKTEST_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell( tOptions* );

int
main(int argc, char** argv)
{
    int res = EXIT_SUCCESS;
    (void)optionProcess( &clktestOptions, argc, argv );
    optionPutShell( &clktestOptions );
    return res;
}
#endif  /* defined TEST_CLKTEST_OPTS */
/* extracted from /usr/local/gnu/share/autogen/optcode.tpl near line 627 */

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
  coerce_it((void*)&(clktestOptions._f))

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
    if (option_usage_text.field_ct != 0) {

        /*
         *  Do the translations.  The first pointer follows the field count
         *  field.  The field count field is the size of a pointer.
         */
        tOptDesc* pOD = clktestOptions.pOptDesc;
        char**    ppz = (char**)(void*)&(option_usage_text);
        int       ix  = option_usage_text.field_ct;

        do {
            ppz++;
            *ppz = AO_gettext(*ppz);
        } while (--ix > 0);

        COERSION(pzCopyright);
        COERSION(pzCopyNotice);
        COERSION(pzFullVersion);
        COERSION(pzUsageTitle);
        COERSION(pzExplain);
        COERSION(pzDetail);
        option_usage_text.field_ct = 0;

        for (ix = clktestOptions.optCt; ix > 0; ix--, pOD++)
            coerce_it((void*)&(pOD->pzText));
    }

    if ((clktestOptions.fOptSet & OPTPROC_NXLAT_OPT_CFG) == 0) {
        tOptDesc* pOD = clktestOptions.pOptDesc;
        int       ix;

        for (ix = clktestOptions.optCt; ix > 0; ix--, pOD++) {
            coerce_it((void*)&(pOD->pz_Name));
            coerce_it((void*)&(pOD->pz_DisableName));
            coerce_it((void*)&(pOD->pz_DisablePfx));
        }
        /* prevent re-translation */
        clktestOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* clktest-opts.c ends here */
