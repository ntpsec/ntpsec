
/**
 * \file autoopts.c
 *
 *  This file contains all of the routines that must be linked into
 *  an executable to use the generated option processing.  The optional
 *  routines are in separately compiled modules so that they will not
 *  necessarily be linked in.
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

#ifndef PKGDATADIR
#  define PKGDATADIR ""
#endif

static char const   zNil[] = "";
static arg_types_t  argTypes             = { NULL };
static char         line_fmt_buf[32];
static bool         displayEnum          = false;
static char const   pkgdatadir_default[] = PKGDATADIR;
static char const * program_pkgdatadir   = pkgdatadir_default;
static tOptionLoadMode option_load_mode  = OPTION_LOAD_UNCOOKED;
static tePagerState pagerState           = PAGER_STATE_INITIAL;

       FILE *       option_usage_fp      = NULL;

LOCAL void *
ao_malloc(size_t sz)
{
    void * res = malloc(sz);
    if (res == NULL) {
        fprintf(stderr, zAO_Alloc, (int)sz);
        exit(EXIT_FAILURE);
    }
    return res;
}
#undef  malloc
#define malloc(_s)        ao_malloc(_s)

LOCAL void *
ao_realloc(void *p, size_t sz)
{
    void * res = (p == NULL) ? malloc(sz) : realloc(p, sz);
    if (res == NULL) {
        fprintf(stderr, zAO_Realloc, (int)sz, p);
        exit(EXIT_FAILURE);
    }
    return res;
}
#undef  realloc
#define realloc(_p,_s)    ao_realloc(_p,_s)

LOCAL char *
ao_strdup(char const *str)
{
    char * res = strdup(str);
    if (res == NULL) {
        fprintf(stderr, zAO_Strdup, (int)strlen(str));
        exit(EXIT_FAILURE);
    }
    return res;
}
#undef  strdup
#define strdup(_p)        ao_strdup(_p)

#ifndef HAVE_PATHFIND
#  define  pathfind(_p, _n, _m) option_pathfind(_p, _n, _m)
#  include "compat/pathfind.c"
#endif

#ifndef HAVE_SNPRINTF
#  define vsnprintf       option_vsnprintf
#  define snprintf        option_snprintf
#  include "compat/snprintf.c"
#endif

#ifndef HAVE_STRDUP
#  define  strdup(_s)     option_strdup(_s)
#  include "compat/strdup.c"
#endif

#ifndef HAVE_STRCHR
#  define strrchr(_s, _c) option_strrchr(_s, _c)
#  define strchr(_s, _c)  option_strchr(_s, _c)
#  include "compat/strchr.c"
#endif

/**
 *  handle an option.
 *
 *  This routine handles equivalencing, sets the option state flags and
 *  invokes the handler procedure, if any.
 */
LOCAL tSuccess
handle_opt(tOptions * pOpts, tOptState * o_st)
{
    /*
     *  Save a copy of the option procedure pointer.
     *  If this is an equivalence class option, we still want this proc.
     */
    tOptDesc* pOD = o_st->pOD;
    tOptProc* pOP = pOD->pOptProc;
    if (pOD->fOptState & OPTST_ALLOC_ARG)
        AGFREE(pOD->optArg.argString);

    pOD->optArg.argString = o_st->pzOptArg;

    /*
     *  IF we are presetting options, then we will ignore any un-presettable
     *  options.  They are the ones either marked as such.
     */
    if (  ((pOpts->fOptSet & OPTPROC_PRESETTING) != 0)
       && ((pOD->fOptState & OPTST_NO_INIT) != 0)
       )
        return PROBLEM;

    /*
     *  IF this is an equivalence class option,
     *  THEN
     *      Save the option value that got us to this option
     *      entry.  (It may not be pOD->optChar[0], if this is an
     *      equivalence entry.)
     *      set the pointer to the equivalence class base
     */
    if (pOD->optEquivIndex != NO_EQUIVALENT) {
        tOptDesc* p = pOpts->pOptDesc + pOD->optEquivIndex;

        /*
         * IF the current option state has not been defined (set on the
         *    command line), THEN we will allow continued resetting of
         *    the value.  Once "defined", then it must not change.
         */
        if ((pOD->fOptState & OPTST_DEFINED) != 0) {
            /*
             *  The equivalenced-to option has been found on the command
             *  line before.  Make sure new occurrences are the same type.
             *
             *  IF this option has been previously equivalenced and
             *     it was not the same equivalenced-to option,
             *  THEN we have a usage problem.
             */
            if (p->optActualIndex != pOD->optIndex) {
                fprintf(stderr, (char*)zMultiEquiv, p->pz_Name, pOD->pz_Name,
                        (pOpts->pOptDesc + p->optActualIndex)->pz_Name);
                return FAILURE;
            }
        } else {
            /*
             *  Set the equivalenced-to actual option index to no-equivalent
             *  so that we set all the entries below.  This option may either
             *  never have been selected before, or else it was selected by
             *  some sort of "presetting" mechanism.
             */
            p->optActualIndex = NO_EQUIVALENT;
        }

        if (p->optActualIndex != pOD->optIndex) {
            /*
             *  First time through, copy over the state
             *  and add in the equivalence flag
             */
            p->optActualValue = pOD->optValue;
            p->optActualIndex = pOD->optIndex;
            o_st->flags |= OPTST_EQUIVALENCE;
        }

        /*
         *  Copy the most recent option argument.  set membership state
         *  is kept in ``p->optCookie''.  Do not overwrite.
         */
        p->optArg.argString = pOD->optArg.argString;
        pOD = p;

    } else {
        pOD->optActualValue = pOD->optValue;
        pOD->optActualIndex = pOD->optIndex;
    }

    pOD->fOptState &= OPTST_PERSISTENT_MASK;
    pOD->fOptState |= (o_st->flags & ~OPTST_PERSISTENT_MASK);

    /*
     *  Keep track of count only for DEFINED (command line) options.
     *  IF we have too many, build up an error message and bail.
     */
    if (  (pOD->fOptState & OPTST_DEFINED)
       && (++pOD->optOccCt > pOD->optMaxCt)  )  {

        if ((pOpts->fOptSet & OPTPROC_ERRSTOP) != 0) {
            char const * pzEqv =
                (pOD->optEquivIndex != NO_EQUIVALENT) ? zEquiv : zNil;

            fputs(zErrOnly, stderr);

            if (pOD->optMaxCt > 1)
                fprintf(stderr, zAtMost, pOD->optMaxCt, pOD->pz_Name, pzEqv);
            else
                fprintf(stderr, zOnlyOne, pOD->pz_Name, pzEqv);
        }

        return FAILURE;
    }

    /*
     *  If provided a procedure to call, call it
     */
    if (pOP != NULL)
        (*pOP)(pOpts, pOD);

    return SUCCESS;
}

/**
 *  Find the option descriptor and option argument (if any) for the
 *  next command line argument.  DO NOT modify the descriptor.  Put
 *  all the state in the state argument so that the option can be skipped
 *  without consequence (side effect).
 *
 * @param opts the program option descriptor
 * @param o_st  the state of the next found option
 */
LOCAL tSuccess
next_opt(tOptions * opts, tOptState * o_st)
{
    {
        tSuccess res = find_opt(opts, o_st);
        if (! SUCCESSFUL(res))
            return res;
    }

    if (  ((o_st->flags & OPTST_DEFINED) != 0)
       && ((o_st->pOD->fOptState & OPTST_NO_COMMAND) != 0)) {
        fprintf(stderr, zNotCmdOpt, o_st->pOD->pz_Name);
        return FAILURE;
    }

    return get_opt_arg(opts, o_st);
}

/**
 * Process all the options from our current position onward.  (This allows
 * interspersed options and arguments for the few non-standard programs that
 * require it.)  Thus, do not rewind option indexes because some programs
 * choose to re-invoke after a non-option.
 *
 *  @param[in,out] opts   program options descriptor
 *  @returns SUCCESS or FAILURE
 */
LOCAL tSuccess
regular_opts(tOptions * opts)
{
    /* assert:  opts->fOptSet & OPTPROC_IMMEDIATE == 0 */
    for (;;) {
        tOptState opt_st = OPTSTATE_INITIALIZER(DEFINED);

        switch (next_opt(opts, &opt_st)) {
        case FAILURE: goto   failed_option;
        case PROBLEM: return SUCCESS; /* no more args */
        case SUCCESS: break;
        }

        /*
         *  IF this is an immediate action option,
         *  THEN skip it (unless we are supposed to do it a second time).
         */
        if (! DO_NORMALLY(opt_st.flags)) {
            if (! DO_SECOND_TIME(opt_st.flags))
                continue;
            opt_st.pOD->optOccCt--; /* don't count this repetition */
        }

        if (! SUCCESSFUL(handle_opt(opts, &opt_st)))
            break;
    } failed_option:;

    if ((opts->fOptSet & OPTPROC_ERRSTOP) != 0)
        (*opts->pUsageProc)(opts, EXIT_FAILURE);

    return FAILURE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  THESE ROUTINES ARE CALLABLE FROM THE GENERATED OPTION PROCESSING CODE
 */
/*=--subblock=arg=arg_type,arg_name,arg_desc =*/
/*=*
 * library:  opts
 * header:   your-opts.h
 *
 * lib_description:
 *
 *  These are the routines that libopts users may call directly from their
 *  code.  There are several other routines that can be called by code
 *  generated by the libopts option templates, but they are not to be
 *  called from any other user code.  The @file{options.h} header is
 *  fairly clear about this, too.
=*/

/*=export_func optionProcess
 *
 * what: this is the main option processing routine
 *
 * arg:  + tOptions* + opts  + program options descriptor +
 * arg:  + int       + a_ct  + program arg count  +
 * arg:  + char**    + a_v   + program arg vector +
 *
 * ret_type:  int
 * ret_desc:  the count of the arguments processed
 *
 * doc:
 *
 * This is the main entry point for processing options.  It is intended
 * that this procedure be called once at the beginning of the execution of
 * a program.  Depending on options selected earlier, it is sometimes
 * necessary to stop and restart option processing, or to select completely
 * different sets of options.  This can be done easily, but you generally
 * do not want to do this.
 *
 * The number of arguments processed always includes the program name.
 * If one of the arguments is "--", then it is counted and the processing
 * stops.  If an error was encountered and errors are to be tolerated, then
 * the returned value is the index of the argument causing the error.
 * A hyphen by itself ("-") will also cause processing to stop and will
 * @emph{not} be counted among the processed arguments.  A hyphen by itself
 * is treated as an operand.  Encountering an operand stops option
 * processing.
 *
 * err:  Errors will cause diagnostics to be printed.  @code{exit(3)} may
 *       or may not be called.  It depends upon whether or not the options
 *       were generated with the "allow-errors" attribute, or if the
 *       ERRSKIP_OPTERR or ERRSTOP_OPTERR macros were invoked.
=*/
int
optionProcess(tOptions * opts, int a_ct, char ** a_v)
{
    if (! SUCCESSFUL(validate_struct(opts, a_v[0])))
        exit(EX_SOFTWARE);

    /*
     *  Establish the real program name, the program full path,
     *  and do all the presetting the first time thru only.
     */
    if (! ao_initialize(opts, a_ct, a_v))
        return 0;

    /*
     *  IF we are (re)starting,
     *  THEN reset option location
     */
    if (opts->curOptIdx <= 0) {
        opts->curOptIdx = 1;
        opts->pzCurOpt  = NULL;
    }

    if (! SUCCESSFUL(regular_opts(opts)))
        return (int)opts->origArgCt;

    /*
     *  IF    there were no errors
     *    AND we have RC/INI files
     *    AND there is a request to save the files
     *  THEN do that now before testing for conflicts.
     *       (conflicts are ignored in preset options)
     */
    switch (opts->specOptIdx.save_opts) {
    case 0:
    case NO_EQUIVALENT:
        break;
    default:
    {
        tOptDesc * od = opts->pOptDesc + opts->specOptIdx.save_opts;

        if (SELECTED_OPT(od)) {
            optionSaveFile(opts);
            exit(EXIT_SUCCESS);
        }
    }
    }

    /*
     *  IF we are checking for errors,
     *  THEN look for too few occurrences of required options
     */
    if (((opts->fOptSet & OPTPROC_ERRSTOP) != 0)
       && (! is_consistent(opts)))
        (*opts->pUsageProc)(opts, EXIT_FAILURE);

    return (int)opts->curOptIdx;
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * end of autoopts/autoopts.c */
