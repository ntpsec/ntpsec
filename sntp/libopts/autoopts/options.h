/*   -*- buffer-read-only: t -*- vi: set ro:
 *  
 *  DO NOT EDIT THIS FILE   (options.h)
 *  
 *  It has been AutoGen-ed  Sunday April  9, 2006 at 11:49:19 AM PDT
 *  From the definitions    funcs.def
 *  and the template file   options_h
 *
 *  This file defines all the global structures and special values
 *  used in the automated option processing library.
 *
 *  Automated Options copyright 1992-Y Bruce Korb
 *
 *  AutoOpts is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  AutoOpts is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with AutoOpts.  If not, write to:
 *  	The Free Software Foundation, Inc.,
 *  	51 Franklin Street, Fifth Floor
 *  	Boston, MA  02110-1301, USA.
 */
#ifndef AUTOOPTS_OPTIONS_H_GUARD
#define AUTOOPTS_OPTIONS_H_GUARD
#include <sys/types.h>

#if defined(HAVE_STDINT_H)
# include <stdint.h>
#elif defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#endif /* HAVE_STDINT/INTTYPES_H */

#if defined(HAVE_LIMITS_H)
# include <limits.h>
#elif defined(HAVE_SYS_LIMITS_H)
# include <sys/limits.h>
#endif /* HAVE_LIMITS/SYS_LIMITS_H */

/*
 *  PUBLIC DEFINES
 *
 *  The following defines may be used in applications that need to test the
 *  state of an option.  To test against these masks and values, a pointer
 *  to an option descriptor must be obtained.  There are two ways:
 *
 *  1. inside an option processing procedure, it is the second argument,
 *  conventionally "tOptDesc* pOD".
 *
 *  2.  Outside of an option procedure (or to reference a different option
 *  descriptor), use either "&DESC( opt_name )" or "&pfx_DESC( opt_name )".
 *
 *  See the relevant generated header file to determine which and what
 *  values for "opt_name" are available.
 */

typedef enum {
    OPARG_TYPE_NONE             = 0,
    OPARG_TYPE_STRING           = 1,    /* default type/ vanilla string      */
    OPARG_TYPE_ENUMERATION      = 2,    /* opt arg is an enum (keyword list) */
    OPARG_TYPE_BOOLEAN          = 3,    /* opt arg is boolean-valued         */
    OPARG_TYPE_MEMBERSHIP       = 4,    /* opt arg sets set membership bits  */
    OPARG_TYPE_NUMERIC          = 5,    /* opt arg has numeric value         */
    OPARG_TYPE_HIERARCHY        = 6     /* option arg is hierarchical value  */
} teOptArgType;

typedef struct optionValue {
    teOptArgType        valType;
    char*               pzName;
    union {
        char            strVal[1];      /* OPARG_TYPE_STRING      */
        int             enumVal;        /* OPARG_TYPE_ENUMERATION */
        int             boolVal;        /* OPARG_TYPE_BOOLEAN     */
        long            setVal;         /* OPARG_TYPE_MEMBERSHIP  */
        long            longVal;        /* OPARG_TYPE_NUMERIC     */
        void*           nestVal;        /* OPARG_TYPE_HIERARCHY   */
    } v;
} tOptionValue;

#define OPTST_SET_ARGTYPE(n) ((n) << 12)
#define OPTST_GET_ARGTYPE(f) (((f) & OPTST_ARG_TYPE_MASK) >> 12)

/*
 *  Bits in the fOptState option descriptor field.
 */
#define OPTST_INIT           0x0000000  /* Initial compiled value            */
#define OPTST_SET            0x0000001  /* Set via the "SET_OPT()" macro     */
#define OPTST_PRESET         0x0000002  /* Set via an RC/INI file            */
#define OPTST_DEFINED        0x0000004  /* Set via a command line option     */

#define OPTST_SET_MASK       0x0000007  /* mask of flags that show set state */

#define OPTST_EQUIVALENCE    0x0000010  /* selected by equiv'ed option       */
#define OPTST_DISABLED       0x0000020  /* option is in disabled state       */

#define OPTST_NO_INIT        0x0000100  /* option cannot be preset           */
#define OPTST_NUMBER_OPT     0x0000200  /* opt value (flag) is any digit     */
#define OPTST_STACKED        0x0000400  /* opt uses optionStackArg procedure */
#define OPTST_INITENABLED    0x0000800  /* option defaults to enabled        */
#define OPTST_ARG_TYPE_MASK  0x000F000  /* bits used to specify opt arg type */
#define OPTST_ARG_OPTIONAL   0x0010000  /* the option argument not required  */
#define OPTST_IMM            0x0020000  /* process option on first pass      */
#define OPTST_DISABLE_IMM    0x0040000  /* process disablement on first pass */
#define OPTST_OMITTED        0x0080000  /* compiled out of program           */
#define OPTST_MUST_SET       0x0100000  /* must be set or pre-set            */
#define OPTST_DOCUMENT       0x0200000  /* opt is for documentation only     */
#define OPTST_TWICE          0x0400000  /* process option twice - imm + reg  */
#define OPTST_DISABLE_TWICE  0x0800000  /* process disabled option twice     */

#define OPTST_PERSISTENT     0xFFFFF00  /* mask of flags that do not change  */

#define SELECTED_OPT( pod )  ( (pod)->fOptState & (OPTST_SET | OPTST_DEFINED))
#define UNUSED_OPT(   pod )  (((pod)->fOptState & OPTST_SET_MASK) == 0)
#define DISABLED_OPT( pod )  ( (pod)->fOptState & OPTST_DISABLED)
#define OPTION_STATE( pod )  ((pod)->fOptState)

/*
 *  PRIVATE INTERFACES
 *
 *  The following values are used in the generated code to communicate
 *  with the option library procedures.  They are not for public use
 *  and may be subject to change.
 */

/*
 *  Define any special processing flags
 */
#define OPTPROC_NONE        0x000000
#define OPTPROC_LONGOPT     0x000001 /* Process long style options      */
#define OPTPROC_SHORTOPT    0x000002 /* Process short style "flags"     */
#define OPTPROC_ERRSTOP     0x000004 /* Stop on argument errors         */
#define OPTPROC_DISABLEDOPT 0x000008 /* Current option is disabled      */
#define OPTPROC_NO_REQ_OPT  0x000010 /* no options are required         */
#define OPTPROC_NUM_OPT     0x000020 /* there is a number option        */
#define OPTPROC_INITDONE    0x000040 /* have initializations been done? */
#define OPTPROC_NEGATIONS   0x000080 /* any negation options?           */
#define OPTPROC_ENVIRON     0x000100 /* check environment?              */
#define OPTPROC_NO_ARGS     0x000200 /* Disallow remaining arguments    */
#define OPTPROC_ARGS_REQ    0x000400 /* Require arguments after options */
#define OPTPROC_REORDER     0x000800 /* reorder arguments after options */
#define OPTPROC_GNUUSAGE    0x001000 /* emit usage in GNU style         */
#define OPTPROC_TRANSLATE   0x002000 /* Translate strings in tOptions   */
#define OPTPROC_HAS_IMMED   0x004000 /* program defines immed options   */
#define OPTPROC_PRESETTING  0x800000 /* opt processing in preset state  */

#define STMTS(s)  do { s; } while (0)

/*
 *  The following must be #defined instead of typedef-ed
 *  because "static const" cannot both be applied to a type,
 *  tho each individually can...so they all are
 */
#define tSCC        static const char
#define tCC         const char
#define tAoSC       static char
#define tAoUC       unsigned char
#define tAoUI       unsigned int
#define tAoUL       unsigned long
#define tAoUS       unsigned short

/*
 *  It is so disgusting that there must be so many ways
 *  of specifying TRUE and FALSE.
 */
typedef enum { AG_FALSE = 0, AG_TRUE } ag_bool;

/*
 *  Define a structure that describes each option and
 *  a pointer to the procedure that handles it.
 *  The argument is the count of this flag previously seen.
 */
typedef struct options  tOptions;
typedef struct optDesc  tOptDesc;
typedef struct optNames tOptNames;

/*
 *  The option procedures do the special processing for each
 *  option flag that needs it.
 */
typedef void (tOptProc)( tOptions*  pOpts, tOptDesc* pOptDesc );
typedef tOptProc*  tpOptProc;

/*
 *  The usage procedure will never return.  It calls "exit(2)"
 *  with the "exitCode" argument passed to it.
 */
typedef void (tUsageProc)( tOptions* pOpts, int exitCode );
typedef tUsageProc* tpUsageProc;

/*
 *  Special definitions.  "NOLIMIT" is the 'max' value to use when
 *  a flag may appear multiple times without limit.  "NO_EQUIVALENT"
 *  is an illegal value for 'optIndex' (option description index).
 */
#define NOLIMIT          USHRT_MAX
#define OPTION_LIMIT     SHRT_MAX
#define NO_EQUIVALENT    (OPTION_LIMIT+1)

/*
 *  Special values for optValue.  It must not be generatable from the
 *  computation "optIndex +96".  Since "optIndex" is limited to 100, ...
 */
#define NUMBER_OPTION    '#'

typedef struct argList tArgList;
#define MIN_ARG_ALLOC_CT   6
#define INCR_ARG_ALLOC_CT  8
struct argList {
    int             useCt;
    int             allocCt;
    tCC*            apzArgs[ MIN_ARG_ALLOC_CT ];
};

/*
 *  Descriptor structure for each option.
 *  Only the fields marked "PUBLIC" are for public use.
 */
struct optDesc {
    tAoUS           optIndex;         /* PUBLIC */
    tAoUS           optValue;         /* PUBLIC */
    tAoUS           optActualIndex;   /* PUBLIC */
    tAoUS           optActualValue;   /* PUBLIC */

    tAoUS           optEquivIndex;    /* PUBLIC */
    tAoUS           optMinCt;
    tAoUS           optMaxCt;
    tAoUS           optOccCt;         /* PUBLIC */

    tAoUI           fOptState;        /* PUBLIC */
    tAoUI           reserved;
    tCC*            pzLastArg;        /* PUBLIC */
    void*           optCookie;        /* PUBLIC */

    const int *     pOptMust;
    const int *     pOptCant;
    tpOptProc       pOptProc;
    const char*     pzText;

    const char*     pz_NAME;
    const char*     pz_Name;
    const char*     pz_DisableName;
    const char*     pz_DisablePfx;
};

/*
 *  Some options need special processing, so we store their
 *  indexes in a known place:
 */
typedef struct optSpecIndex tOptSpecIndex;
struct optSpecIndex {
    tAoUS           more_help;
    tAoUS           save_opts;
    tAoUS           number_option;
    tAoUS           default_opt;
};

#define  OPTIONS_STRUCT_VERSION  110594
#define  OPTIONS_VERSION_STRING  "27:1:2"
#define  OPTIONS_MINIMUM_VERSION 102400
#define  OPTIONS_MIN_VER_STRING  "25:0:0"

/*
 *  The procedure generated for translating option text
 */
typedef void (tOptionXlateProc)(void);

struct options {
    const int         structVersion;
    int               origArgCt;
    char**            origArgVect;
    unsigned int      fOptSet;
    unsigned int      curOptIdx;
    char*             pzCurOpt;

    const char*       pzProgPath;
    const char*       pzProgName;
    const char*       pzPROGNAME;
    const char*       pzRcName;
    const char*       pzCopyright;
    const char*       pzCopyNotice;
    const char*       pzFullVersion;
    const char**      papzHomeList;
    const char*       pzUsageTitle;
    const char*       pzExplain;
    const char*       pzDetail;
    tOptDesc*         pOptDesc;
    const char*       pzBugAddr;

    void*             pExtensions;
    void*             pSavedState;

    tpUsageProc       pUsageProc;
    tOptionXlateProc* pTransProc;

    tOptSpecIndex     specOptIdx;
    const int         optCt;
    const int         presetOptCt;
};

/*
 *  "token list" structure returned by "string_tokenize()"
 */
typedef struct {
    unsigned long   tkn_ct;
    unsigned char*  tkn_list[1];
} token_list_t;

/*
 *  Hide the interface - it pollutes a POSIX claim, but leave it for
 *  anyone #include-ing this header
 */
#define strneqvcmp      option_strneqvcmp
#define streqvcmp       option_streqvcmp
#define streqvmap       option_streqvmap
#define strequate       option_strequate
#define strtransform    option_strtransform

/*
 *  This is an output only structure used by text_mmap and text_munmap.
 *  Clients must not alter the contents and must provide it to both
 *  the text_mmap and text_munmap procedures.  BE ADVISED: if you are
 *  mapping the file with PROT_WRITE the NUL byte at the end MIGHT NOT
 *  BE WRITABLE.  In any event, that byte is not be written back
 *  to the source file.  ALSO: if "txt_data" is valid and "txt_errno"
 *  is not zero, then there *may* not be a terminating NUL.
 */
typedef struct {
    void*       txt_data;      /* text file data   */
    size_t      txt_size;      /* actual file size */
    size_t      txt_full_size; /* mmaped mem size  */
    int         txt_fd;        /* file descriptor  */
    int         txt_zero_fd;   /* fd for /dev/zero */
    int         txt_errno;     /* warning code     */
    int         txt_prot;      /* "prot" flags     */
    int         txt_flags;     /* mapping type     */
    int         txt_alloc;     /* if we malloced memory */
} tmap_info_t;

#define TEXT_MMAP_FAILED_ADDR(a)  ((void*)(a) ==  (void*)MAP_FAILED)

/*
 *  When loading a line (or block) of text as an option, the value can
 *  be processed in any of several modes:
 *
 *  @table @samp
 *  @item keep
 *  Every part of the value between the delimiters is saved.
 *
 *  @item uncooked
 *  Even if the value begins with quote characters, do not do quote processing.
 *
 *  @item cooked
 *  If the value looks like a quoted string, then process it.
 *  Double quoted strings are processed the way strings are in "C" programs,
 *  except they are treated as regular characters if the following character
 *  is not a well-established escape sequence.
 *  Single quoted strings (quoted with apostrophies) are handled the way
 *  strings are handled in shell scripts, *except* that backslash escapes
 *  are honored before backslash escapes and apostrophies.
 *  @end table
 */
typedef enum {
    OPTION_LOAD_COOKED,
    OPTION_LOAD_UNCOOKED,
    OPTION_LOAD_KEEP
} tOptionLoadMode;

#ifdef  __cplusplus
extern "C" {
#define CPLUSPLUS_CLOSER }
#else
#define CPLUSPLUS_CLOSER
#endif

/*
 *  The following routines may be coded into AutoOpts client code:
 */

/* From: tokenize.c line 115
 *
 * ao_string_tokenize - tokenize an input string
 *
 * Arguments:
 *   string       string to be tokenized
 *
 * Returns: token_list_t* - pointer to a structure that lists each token
 *
 *  This function will convert one input string into a list of strings.
 *  The list of strings is derived by separating the input based on
 *  white space separation.  However, if the input contains either single
 *  or double quote characters, then the text after that character up to
 *  a matching quote will become the string in the list.
 *  
 *  The returned pointer should be deallocated with @code{free(3C)} when
 *  are done using the data.  The data are placed in a single block of
 *  allocated memory.  Do not deallocate individual token/strings.
 *  
 *  The structure pointed to will contain at least these two fields:
 *  @table @samp
 *  @item tkn_ct
 *  The number of tokens found in the input string.
 *  @item tok_list
 *  An array of @code{tkn_ct + 1} pointers to substring tokens, with
 *  the last pointer set to NULL.
 *  @end table
 *  
 *  There are two types of quoted strings: single quoted (@code{'}) and
 *  double quoted (@code{"}).  Singly quoted strings are fairly raw in that
 *  escape characters (@code{\\}) are simply another character, except when
 *  preceding the following characters:
 *  @example
 *  @code{\\}  double backslashes reduce to one
 *  @code{'}   incorporates the single quote into the string
 *  @code{\n}  suppresses both the backslash and newline character
 *  @end example
 *  
 *  Double quote strings are formed according to the rules of string
 *  constants in ANSI-C programs.
 */
extern token_list_t* ao_string_tokenize( const char* );


/* From: configfile.c line 113
 *
 * configFileLoad - parse a configuration file
 *
 * Arguments:
 *   pzFile       the file to load
 *
 * Returns: const tOptionValue* - An allocated, compound value structure
 *
 *  This routine will load a named configuration file and parse the
 *  text as a hierarchically valued option.  The option descriptor
 *  created from an option definition file is not used via this interface.
 *  The returned value is "named" with the input file name and is of
 *  type "@code{OPARG_TYPE_HIERARCHY}".  It may be used in calls to
 *  @code{optionGetValue()}, @code{optionNextValue()} and
 *  @code{optionUnloadNested()}.
 */
extern const tOptionValue* configFileLoad( const char* );


/* From: configfile.c line 869
 *
 * optionFileLoad - Load the locatable config files, in order
 *
 * Arguments:
 *   pOpts        program options descriptor
 *   pzProg       program name
 *
 * Returns: int - 0 -> SUCCESS, -1 -> FAILURE
 *
 *  This function looks in all the specified directories for a configuration
 *  file ("rc" file or "ini" file) and processes any found twice.  The first
 *  time through, they are processed in reverse order (last file first).  At
 *  that time, only "immediate action" configurables are processed.  For
 *  example, if the last named file specifies not processing any more
 *  configuration files, then no more configuration files will be processed.
 *  Such an option in the @strong{first} named directory will have no effect.
 *  
 *  Once the immediate action configurables have been handled, then the
 *  directories are handled in normal, forward order.  In that way, later
 *  config files can override the settings of earlier config files.
 *  
 *  See the AutoOpts documentation for a thorough discussion of the
 *  config file format.
 *  
 *  Configuration files not found or not decipherable are simply ignored.
 */
extern int optionFileLoad( tOptions*, const char* );


/* From: configfile.c line 241
 *
 * optionFindNextValue - find a hierarcicaly valued option instance
 *
 * Arguments:
 *   pOptDesc     an option with a nested arg type
 *   pPrevVal     the last entry
 *   name         name of value to find
 *   value        the matching value
 *
 * Returns: const tOptionValue* - a compound value structure
 *
 *  This routine will find the next entry in a nested value option or
 *  configurable.  It will search through the list and return the next entry
 *  that matches the criteria.
 */
extern const tOptionValue* optionFindNextValue( const tOptDesc*, const tOptionValue*, const char*, const char* );


/* From: configfile.c line 166
 *
 * optionFindValue - find a hierarcicaly valued option instance
 *
 * Arguments:
 *   pOptDesc     an option with a nested arg type
 *   name         name of value to find
 *   value        the matching value
 *
 * Returns: const tOptionValue* - a compound value structure
 *
 *  This routine will find an entry in a nested value option or configurable.
 *  It will search through the list and return a matching entry.
 */
extern const tOptionValue* optionFindValue( const tOptDesc*, const char*, const char* );


/* From: restore.c line 157
 *
 * optionFree - free allocated option processing memory
 *
 * Arguments:
 *   pOpts        program options descriptor
 *
 *  AutoOpts sometimes allocates memory and puts pointers to it in the
 *  option state structures.  This routine deallocates all such memory.
 */
extern void optionFree( tOptions* );


/* From: configfile.c line 310
 *
 * optionGetValue - get a specific value from a hierarcical list
 *
 * Arguments:
 *   pOptValue    a hierarchcal value
 *   valueName    name of value to get
 *
 * Returns: const tOptionValue* - a compound value structure
 *
 *  This routine will find an entry in a nested value option or configurable.
 *  If "valueName" is NULL, then the first entry is returned.  Otherwise,
 *  the first entry with a name that exactly matches the argument will be
 *  returned.
 */
extern const tOptionValue* optionGetValue( const tOptionValue*, const char* );


/* From: load.c line 477
 *
 * optionLoadLine - process a string for an option name and value
 *
 * Arguments:
 *   pOpts        program options descriptor
 *   pzLine       NUL-terminated text
 *
 *  This is a client program callable routine for setting options from, for
 *  example, the contents of a file that they read in.  Only one option may
 *  appear in the text.  It will be treated as a normal (non-preset) option.
 *  
 *  When passed a pointer to the option struct and a string, it will find
 *  the option named by the first token on the string and set the option
 *  argument to the remainder of the string.  The caller must NUL terminate
 *  the string.  Any embedded new lines will be included in the option
 *  argument.  If the input looks like one or more quoted strings, then the
 *  input will be "cooked".  The "cooking" is identical to the string
 *  formation used in AutoGen definition files (@pxref{basic expression}),
 *  except that you may not use backquotes.
 */
extern void optionLoadLine( tOptions*, const char* );


/* From: configfile.c line 369
 *
 * optionNextValue - get the next value from a hierarchical list
 *
 * Arguments:
 *   pOptValue    a hierarchcal list value
 *   pOldValue    a value from this list
 *
 * Returns: const tOptionValue* - a compound value structure
 *
 *  This routine will return the next entry after the entry passed in.  At the
 *  end of the list, NULL will be returned.  If the entry is not found on the
 *  list, NULL will be returned and "@var{errno}" will be set to EINVAL.
 *  The "@var{pOldValue}" must have been gotten from a prior call to this
 *  routine or to "@code{opitonGetValue()}".
 */
extern const tOptionValue* optionNextValue( const tOptionValue*, const tOptionValue* );


/* From: usage.c line 128
 *
 * optionOnlyUsage - Print usage text for just the options
 *
 * Arguments:
 *   pOpts        program options descriptor
 *   ex_code      exit code for calling exit(3)
 *
 *  This routine will print only the usage for each option.
 *  This function may be used when the emitted usage must incorporate
 *  information not available to AutoOpts.
 */
extern void optionOnlyUsage( tOptions*, int );


/* From: autoopts.c line 934
 *
 * optionProcess - this is the main option processing routine
 *
 * Arguments:
 *   pOpts        program options descriptor
 *   argc         program arg count
 *   argv         program arg vector
 *
 * Returns: int - the count of the arguments processed
 *
 *  This is the main entry point for processing options.  It is intended
 *  that this procedure be called once at the beginning of the execution of
 *  a program.  Depending on options selected earlier, it is sometimes
 *  necessary to stop and restart option processing, or to select completely
 *  different sets of options.  This can be done easily, but you generally
 *  do not want to do this.
 *  
 *  The number of arguments processed always includes the program name.
 *  If one of the arguments is "--", then it is counted and the processing
 *  stops.  If an error was encountered and errors are to be tolerated, then
 *  the returned value is the index of the argument causing the error.
 *  A hyphen by itself ("-") will also cause processing to stop and will
 *  @emph{not} be counted among the processed arguments.  A hyphen by itself
 *  is treated as an operand.  Encountering an operand stops option
 *  processing.
 */
extern int optionProcess( tOptions*, int, char** );


/* From: restore.c line 121
 *
 * optionRestore - restore option state from memory copy
 *
 * Arguments:
 *   pOpts        program options descriptor
 *
 *  Copy back the option state from saved memory.
 *  The allocated memory is left intact, so this routine can be
 *  called repeatedly without having to call optionSaveState again.
 *  If you are restoring a state that was saved before the first call
 *  to optionProcess(3AO), then you may change the contents of the
 *  argc/argv parameters to optionProcess.
 */
extern void optionRestore( tOptions* );


/* From: save.c line 325
 *
 * optionSaveFile - saves the option state to a file
 *
 * Arguments:
 *   pOpts        program options descriptor
 *
 *  This routine will save the state of option processing to a file.  The name
 *  of that file can be specified with the argument to the @code{--save-opts}
 *  option, or by appending the @code{rcfile} attribute to the last
 *  @code{homerc} attribute.  If no @code{rcfile} attribute was specified, it
 *  will default to @code{.@i{programname}rc}.  If you wish to specify another
 *  file, you should invoke the @code{SET_OPT_SAVE_OPTS( @i{filename} )} macro.
 */
extern void optionSaveFile( tOptions* );


/* From: restore.c line 54
 *
 * optionSaveState - saves the option state to memory
 *
 * Arguments:
 *   pOpts        program options descriptor
 *
 *  This routine will allocate enough memory to save the current
 *  option processing state.  If this routine has been called before,
 *  that memory will be reused.  You may only save one copy of the
 *  option state.  This routine may be called before optionProcess(3AO).
 *  If you do call it before the first call to optionProcess, then
 *  you may also change the contents of argc/argv after you call
 *  optionRestore(3AO)
 */
extern void optionSaveState( tOptions* );


/* From: nested.c line 527
 *
 * optionUnloadNested - Deallocate the memory for a nested value
 *
 * Arguments:
 *   pOptVal      the hierarchical value
 *
 *  A nested value needs to be deallocated.  The pointer passed in should
 *  have been gotten from a call to @code{configFileLoad()} (See
 *  @pxref{libopts-configFileLoad}).
 */
extern void optionUnloadNested( const tOptionValue* );


/* From: version.c line 58
 *
 * optionVersion - return the compiled AutoOpts version number
 *
 * Returns: const char* - the version string in constant memory
 *
 *  Returns the full version string compiled into the library.
 *  The returned string cannot be modified.
 */
extern const char* optionVersion( void );


/* From: ../compat/pathfind.c line 24
 *
 * pathfind - fild a file in a list of directories
 *
 * Arguments:
 *   path         colon separated list of search directories
 *   file         the name of the file to look for
 *   mode         the mode bits that must be set to match
 *
 * Returns: char* - the path to the located file
 *
 * the pathfind function is available only if HAVE_PATHFIND is not defined
 *
 *  pathfind looks for a a file with name "FILE" and "MODE" access
 *  along colon delimited "PATH", and returns the full pathname as a
 *  string, or NULL if not found.  If "FILE" contains a slash, then
 *  it is treated as a relative or absolute path and "PATH" is ignored.
 *  
 *  @strong{NOTE}: this function is compiled into @file{libopts} only if
 *  it is not natively supplied.
 *  
 *  The "MODE" argument is a string of option letters chosen from the
 *  list below:
 *  @example
 *  Letter    Meaning
 *  r         readable
 *  w         writable
 *  x         executable
 *  f         normal file       (NOT IMPLEMENTED)
 *  b         block special     (NOT IMPLEMENTED)
 *  c         character special (NOT IMPLEMENTED)
 *  d         directory         (NOT IMPLEMENTED)
 *  p         FIFO (pipe)       (NOT IMPLEMENTED)
 *  u         set user ID bit   (NOT IMPLEMENTED)
 *  g         set group ID bit  (NOT IMPLEMENTED)
 *  k         sticky bit        (NOT IMPLEMENTED)
 *  s         size nonzero      (NOT IMPLEMENTED)
 *  @end example
 */
#ifndef HAVE_PATHFIND
extern char* pathfind( const char*, const char*, const char* );
#endif /* HAVE_PATHFIND */


/* From: streqvcmp.c line 233
 *
 * strequate - map a list of characters to the same value
 *
 * Arguments:
 *   ch_list      characters to equivalence
 *
 *  Each character in the input string get mapped to the first character
 *  in the string.
 *  This function name is mapped to option_strequate so as to not conflict
 *  with the POSIX name space.
 */
extern void strequate( const char* );


/* From: streqvcmp.c line 143
 *
 * streqvcmp - compare two strings with an equivalence mapping
 *
 * Arguments:
 *   str1         first string
 *   str2         second string
 *
 * Returns: int - the difference between two differing characters
 *
 *  Using a character mapping, two strings are compared for "equivalence".
 *  Each input character is mapped to a comparison character and the
 *  mapped-to characters are compared for the two NUL terminated input strings.
 *  This function name is mapped to option_streqvcmp so as to not conflict
 *  with the POSIX name space.
 */
extern int streqvcmp( const char*, const char* );


/* From: streqvcmp.c line 180
 *
 * streqvmap - Set the character mappings for the streqv functions
 *
 * Arguments:
 *   From         Input character
 *   To           Mapped-to character
 *   ct           compare length
 *
 *  Set the character mapping.  If the count (@code{ct}) is set to zero, then
 *  the map is cleared by setting all entries in the map to their index
 *  value.  Otherwise, the "@code{From}" character is mapped to the "@code{To}"
 *  character.  If @code{ct} is greater than 1, then @code{From} and @code{To}
 *  are incremented and the process repeated until @code{ct} entries have been
 *  set. For example,
 *  @example
 *  streqvmap( 'a', 'A', 26 );
 *  @end example
 *  @noindent
 *  will alter the mapping so that all English lower case letters
 *  will map to upper case.
 *  
 *  This function name is mapped to option_streqvmap so as to not conflict
 *  with the POSIX name space.
 */
extern void streqvmap( char, char, int );


/* From: streqvcmp.c line 102
 *
 * strneqvcmp - compare two strings with an equivalence mapping
 *
 * Arguments:
 *   str1         first string
 *   str2         second string
 *   ct           compare length
 *
 * Returns: int - the difference between two differing characters
 *
 *  Using a character mapping, two strings are compared for "equivalence".
 *  Each input character is mapped to a comparison character and the
 *  mapped-to characters are compared for the two NUL terminated input strings.
 *  The comparison is limited to @code{ct} bytes.
 *  This function name is mapped to option_strneqvcmp so as to not conflict
 *  with the POSIX name space.
 */
extern int strneqvcmp( const char*, const char*, int );


/* From: streqvcmp.c line 259
 *
 * strtransform - convert a string into its mapped-to value
 *
 * Arguments:
 *   dest         output string
 *   src          input string
 *
 *  Each character in the input string is mapped and the mapped-to
 *  character is put into the output.
 *  This function name is mapped to option_strtransform so as to not conflict
 *  with the POSIX name space.
 */
extern void strtransform( char*, const char* );

/*  AutoOpts PRIVATE FUNCTIONS:  */
tOptProc optionStackArg, optionUnstackArg, optionBooleanVal, optionNumericVal;

extern char* ao_string_cook( char*, int* );

extern unsigned int ao_string_cook_escape_char( const char*, char*, u_int );

extern void genshelloptUsage( tOptions*, int );

extern void optionBooleanVal( tOptions*, tOptDesc* );

extern char* optionEnumerationVal( tOptions*, tOptDesc*, const char**, unsigned int );

extern const char* optionKeywordName( tOptDesc*, unsigned int );

extern tOptionValue* optionLoadNested( const char*, const char*, size_t, tOptionLoadMode );

extern void optionLoadOpt( tOptions*, tOptDesc* );

extern ag_bool optionMakePath( char*, int, const char*, const char* );

extern void optionNestedVal( tOptions*, tOptDesc* );

extern void optionNumericVal( tOptions*, tOptDesc* );

extern void optionPagedUsage( tOptions*, tOptDesc* );

extern void optionParseShell( tOptions* );

extern void optionPrintVersion( tOptions*, tOptDesc* );

extern void optionPutShell( tOptions* );

extern void optionSetMembers( tOptions*, tOptDesc*, const char**, unsigned int );

extern void optionStackArg( tOptions*, tOptDesc* );

extern void optionUnstackArg( tOptions*, tOptDesc* );

extern void optionUsage( tOptions*, int );

extern void optionVersionStderr( tOptions*, tOptDesc* );

extern void* text_mmap( const char*, int, int, tmap_info_t* );

extern int text_munmap( tmap_info_t* );

CPLUSPLUS_CLOSER
#endif /* AUTOOPTS_OPTIONS_H_GUARD */
/*
 * Local Variables:
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * options.h ends here */
