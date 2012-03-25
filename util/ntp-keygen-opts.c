/*  
 *  EDIT THIS FILE WITH CAUTION  (ntp-keygen-opts.c)
 *  
 *  It has been AutoGen-ed  March 25, 2012 at 05:07:05 AM by AutoGen 5.14
 *  From the definitions    ntp-keygen-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 36:1:11 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the ntp-keygen author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The ntp-keygen program is copyrighted and licensed
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
#include "ntp-keygen-opts.h"
#include <errno.h>
#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;

/* TRANSLATORS: choose the translation for option names wisely because you
                cannot ever change your mind. */
#define zCopyright      (ntp_keygen_opt_strs+0)
#define zLicenseDescrip (ntp_keygen_opt_strs+326)

extern tUsageProc optionUsage;
/*
 *  global included definitions
 */#include <stdlib.h>

#ifdef __windows
  extern int atoi(const char*);
#else
# include <stdlib.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

/*
 *  ntp-keygen option static const strings
 */
static char const ntp_keygen_opt_strs[2320] =
/*     0 */ "ntp-keygen (ntp) 4.2.7p269\n"
            "Copyright (C) 1970-2012 The University of Delaware, all rights reserved.\n"
            "This is free software. It is licensed for use, modification and\n"
            "redistribution under the terms of the NTP License, copies of which\n"
            "can be seen at:\n"
            "  <http://ntp.org/license>\n"
            "  <http://opensource.org/licenses/ntp-license.php>\n\0"
/*   326 */ "Permission to use, copy, modify, and distribute this software and its\n"
            "documentation for any purpose with or without fee is hereby granted,\n"
            "provided that the above copyright notice appears in all copies and that\n"
            "both the copyright notice and this permission notice appear in supporting\n"
            "documentation, and that the name The University of Delaware not be used in\n"
            "advertising or publicity pertaining to distribution of the software\n"
            "without specific, written prior permission. The University of Delaware\n"
            "makes no representations about the suitability this software for any\n"
            "purpose. It is provided \"as is\" without express or implied warranty.\n\0"
/*   964 */ "certificate scheme\0"
/*   983 */ "CERTIFICATE\0"
/*   995 */ "certificate\0"
/*  1007 */ "privatekey cipher\0"
/*  1025 */ "CIPHER\0"
/*  1032 */ "cipher\0"
/*  1039 */ "Increase debug verbosity level\0"
/*  1070 */ "DEBUG_LEVEL\0"
/*  1082 */ "debug-level\0"
/*  1094 */ "Set the debug verbosity level\0"
/*  1124 */ "SET_DEBUG_LEVEL\0"
/*  1140 */ "set-debug-level\0"
/*  1156 */ "Write IFF or GQ identity keys\0"
/*  1186 */ "ID_KEY\0"
/*  1193 */ "id-key\0"
/*  1200 */ "Generate GQ parameters and keys\0"
/*  1232 */ "GQ_PARAMS\0"
/*  1242 */ "gq-params\0"
/*  1252 */ "generate RSA host key\0"
/*  1274 */ "HOST_KEY\0"
/*  1283 */ "host-key\0"
/*  1292 */ "generate IFF parameters\0"
/*  1316 */ "IFFKEY\0"
/*  1323 */ "iffkey\0"
/*  1330 */ "set Autokey group name\0"
/*  1353 */ "IDENT\0"
/*  1359 */ "ident\0"
/*  1365 */ "set certificate lifetime\0"
/*  1390 */ "LIFETIME\0"
/*  1399 */ "lifetime\0"
/*  1408 */ "generate MD5 keys\0"
/*  1426 */ "MD5KEY\0"
/*  1433 */ "md5key\0"
/*  1440 */ "modulus\0"
/*  1448 */ "MODULUS\0"
/*  1456 */ "generate PC private certificate\0"
/*  1488 */ "PVT_CERT\0"
/*  1497 */ "pvt-cert\0"
/*  1506 */ "output private password\0"
/*  1530 */ "PVT_PASSWD\0"
/*  1541 */ "pvt-passwd\0"
/*  1552 */ "input private password\0"
/*  1575 */ "GET_PVT_PASSWD\0"
/*  1590 */ "get-pvt-passwd\0"
/*  1605 */ "generate sign key (RSA or DSA)\0"
/*  1636 */ "SIGN_KEY\0"
/*  1645 */ "sign-key\0"
/*  1654 */ "set host and optionally group name\0"
/*  1689 */ "SUBJECT_NAME\0"
/*  1702 */ "subject-name\0"
/*  1715 */ "trusted certificate (TC scheme)\0"
/*  1747 */ "TRUSTED_CERT\0"
/*  1760 */ "trusted-cert\0"
/*  1773 */ "generate <num> MV parameters\0"
/*  1802 */ "MV_PARAMS\0"
/*  1812 */ "mv-params\0"
/*  1822 */ "update <num> MV keys\0"
/*  1843 */ "MV_KEYS\0"
/*  1851 */ "mv-keys\0"
/*  1859 */ "Display extended usage information and exit\0"
/*  1903 */ "help\0"
/*  1908 */ "Extended usage information passed thru pager\0"
/*  1953 */ "more-help\0"
/*  1963 */ "Output version information and exit\0"
/*  1999 */ "version\0"
/*  2007 */ "Save the option state to a config file\0"
/*  2046 */ "save-opts\0"
/*  2056 */ "Load options from a config file\0"
/*  2088 */ "LOAD_OPTS\0"
/*  2098 */ "no-load-opts\0"
/*  2111 */ "no\0"
/*  2114 */ "NTP_KEYGEN\0"
/*  2125 */ "ntp-keygen (ntp) - Create a NTP host key - Ver. 4.2.7p269\n"
            "USAGE:  %s [ -<flag> [<val>] | --<name>[{=| }<val>] ]...\n\0"
/*  2241 */ ".ntprc\0"
/*  2248 */ "$HOME\0"
/*  2254 */ ".\0"
/*  2256 */ "http://bugs.ntp.org, bugs@ntp.org\0"
/*  2290 */ "\n\n\0"
/*  2293 */ "ntp-keygen (ntp) 4.2.7p269";

/*
 *  certificate option description:
 */
#ifdef AUTOKEY
#define CERTIFICATE_DESC      (ntp_keygen_opt_strs+964)
#define CERTIFICATE_NAME      (ntp_keygen_opt_strs+983)
#define CERTIFICATE_name      (ntp_keygen_opt_strs+995)
#define CERTIFICATE_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable certificate */
#define CERTIFICATE_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define CERTIFICATE_NAME      NULL
#define CERTIFICATE_DESC      NULL
#define CERTIFICATE_name      NULL
#endif  /* AUTOKEY */

/*
 *  cipher option description:
 */
#ifdef AUTOKEY
#define CIPHER_DESC      (ntp_keygen_opt_strs+1007)
#define CIPHER_NAME      (ntp_keygen_opt_strs+1025)
#define CIPHER_name      (ntp_keygen_opt_strs+1032)
#define CIPHER_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable cipher */
#define CIPHER_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define CIPHER_NAME      NULL
#define CIPHER_DESC      NULL
#define CIPHER_name      NULL
#endif  /* AUTOKEY */

/*
 *  debug-level option description:
 */
#define DEBUG_LEVEL_DESC      (ntp_keygen_opt_strs+1039)
#define DEBUG_LEVEL_NAME      (ntp_keygen_opt_strs+1070)
#define DEBUG_LEVEL_name      (ntp_keygen_opt_strs+1082)
#define DEBUG_LEVEL_FLAGS     (OPTST_DISABLED)

/*
 *  set-debug-level option description:
 */
#define SET_DEBUG_LEVEL_DESC      (ntp_keygen_opt_strs+1094)
#define SET_DEBUG_LEVEL_NAME      (ntp_keygen_opt_strs+1124)
#define SET_DEBUG_LEVEL_name      (ntp_keygen_opt_strs+1140)
#define SET_DEBUG_LEVEL_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  id-key option description:
 */
#ifdef AUTOKEY
#define ID_KEY_DESC      (ntp_keygen_opt_strs+1156)
#define ID_KEY_NAME      (ntp_keygen_opt_strs+1186)
#define ID_KEY_name      (ntp_keygen_opt_strs+1193)
#define ID_KEY_FLAGS     (OPTST_DISABLED)

#else   /* disable id-key */
#define ID_KEY_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define ID_KEY_NAME      NULL
#define ID_KEY_DESC      NULL
#define ID_KEY_name      NULL
#endif  /* AUTOKEY */

/*
 *  gq-params option description:
 */
#ifdef AUTOKEY
#define GQ_PARAMS_DESC      (ntp_keygen_opt_strs+1200)
#define GQ_PARAMS_NAME      (ntp_keygen_opt_strs+1232)
#define GQ_PARAMS_name      (ntp_keygen_opt_strs+1242)
#define GQ_PARAMS_FLAGS     (OPTST_DISABLED)

#else   /* disable gq-params */
#define GQ_PARAMS_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define GQ_PARAMS_NAME      NULL
#define GQ_PARAMS_DESC      NULL
#define GQ_PARAMS_name      NULL
#endif  /* AUTOKEY */

/*
 *  host-key option description:
 */
#ifdef AUTOKEY
#define HOST_KEY_DESC      (ntp_keygen_opt_strs+1252)
#define HOST_KEY_NAME      (ntp_keygen_opt_strs+1274)
#define HOST_KEY_name      (ntp_keygen_opt_strs+1283)
#define HOST_KEY_FLAGS     (OPTST_DISABLED)

#else   /* disable host-key */
#define HOST_KEY_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define HOST_KEY_NAME      NULL
#define HOST_KEY_DESC      NULL
#define HOST_KEY_name      NULL
#endif  /* AUTOKEY */

/*
 *  iffkey option description:
 */
#ifdef AUTOKEY
#define IFFKEY_DESC      (ntp_keygen_opt_strs+1292)
#define IFFKEY_NAME      (ntp_keygen_opt_strs+1316)
#define IFFKEY_name      (ntp_keygen_opt_strs+1323)
#define IFFKEY_FLAGS     (OPTST_DISABLED)

#else   /* disable iffkey */
#define IFFKEY_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define IFFKEY_NAME      NULL
#define IFFKEY_DESC      NULL
#define IFFKEY_name      NULL
#endif  /* AUTOKEY */

/*
 *  ident option description:
 */
#ifdef AUTOKEY
#define IDENT_DESC      (ntp_keygen_opt_strs+1330)
#define IDENT_NAME      (ntp_keygen_opt_strs+1353)
#define IDENT_name      (ntp_keygen_opt_strs+1359)
#define IDENT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable ident */
#define IDENT_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define IDENT_NAME      NULL
#define IDENT_DESC      NULL
#define IDENT_name      NULL
#endif  /* AUTOKEY */

/*
 *  lifetime option description:
 */
#ifdef AUTOKEY
#define LIFETIME_DESC      (ntp_keygen_opt_strs+1365)
#define LIFETIME_NAME      (ntp_keygen_opt_strs+1390)
#define LIFETIME_name      (ntp_keygen_opt_strs+1399)
#define LIFETIME_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

#else   /* disable lifetime */
#define LIFETIME_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define LIFETIME_NAME      NULL
#define LIFETIME_DESC      NULL
#define LIFETIME_name      NULL
#endif  /* AUTOKEY */

/*
 *  md5key option description:
 */
#define MD5KEY_DESC      (ntp_keygen_opt_strs+1408)
#define MD5KEY_NAME      (ntp_keygen_opt_strs+1426)
#define MD5KEY_name      (ntp_keygen_opt_strs+1433)
#define MD5KEY_FLAGS     (OPTST_DISABLED)

/*
 *  modulus option description:
 */
#ifdef AUTOKEY
#define MODULUS_DESC      (ntp_keygen_opt_strs+1440)
#define MODULUS_NAME      (ntp_keygen_opt_strs+1448)
#define MODULUS_name      (ntp_keygen_opt_strs+1440)
#define MODULUS_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

#else   /* disable modulus */
#define MODULUS_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define MODULUS_NAME      NULL
#define MODULUS_DESC      NULL
#define MODULUS_name      NULL
#endif  /* AUTOKEY */

/*
 *  pvt-cert option description:
 */
#ifdef AUTOKEY
#define PVT_CERT_DESC      (ntp_keygen_opt_strs+1456)
#define PVT_CERT_NAME      (ntp_keygen_opt_strs+1488)
#define PVT_CERT_name      (ntp_keygen_opt_strs+1497)
#define PVT_CERT_FLAGS     (OPTST_DISABLED)

#else   /* disable pvt-cert */
#define PVT_CERT_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define PVT_CERT_NAME      NULL
#define PVT_CERT_DESC      NULL
#define PVT_CERT_name      NULL
#endif  /* AUTOKEY */

/*
 *  pvt-passwd option description:
 */
#ifdef AUTOKEY
#define PVT_PASSWD_DESC      (ntp_keygen_opt_strs+1506)
#define PVT_PASSWD_NAME      (ntp_keygen_opt_strs+1530)
#define PVT_PASSWD_name      (ntp_keygen_opt_strs+1541)
#define PVT_PASSWD_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable pvt-passwd */
#define PVT_PASSWD_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define PVT_PASSWD_NAME      NULL
#define PVT_PASSWD_DESC      NULL
#define PVT_PASSWD_name      NULL
#endif  /* AUTOKEY */

/*
 *  get-pvt-passwd option description:
 */
#ifdef AUTOKEY
#define GET_PVT_PASSWD_DESC      (ntp_keygen_opt_strs+1552)
#define GET_PVT_PASSWD_NAME      (ntp_keygen_opt_strs+1575)
#define GET_PVT_PASSWD_name      (ntp_keygen_opt_strs+1590)
#define GET_PVT_PASSWD_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable get-pvt-passwd */
#define GET_PVT_PASSWD_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define GET_PVT_PASSWD_NAME      NULL
#define GET_PVT_PASSWD_DESC      NULL
#define GET_PVT_PASSWD_name      NULL
#endif  /* AUTOKEY */

/*
 *  sign-key option description:
 */
#ifdef AUTOKEY
#define SIGN_KEY_DESC      (ntp_keygen_opt_strs+1605)
#define SIGN_KEY_NAME      (ntp_keygen_opt_strs+1636)
#define SIGN_KEY_name      (ntp_keygen_opt_strs+1645)
#define SIGN_KEY_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable sign-key */
#define SIGN_KEY_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define SIGN_KEY_NAME      NULL
#define SIGN_KEY_DESC      NULL
#define SIGN_KEY_name      NULL
#endif  /* AUTOKEY */

/*
 *  subject-name option description:
 */
#ifdef AUTOKEY
#define SUBJECT_NAME_DESC      (ntp_keygen_opt_strs+1654)
#define SUBJECT_NAME_NAME      (ntp_keygen_opt_strs+1689)
#define SUBJECT_NAME_name      (ntp_keygen_opt_strs+1702)
#define SUBJECT_NAME_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

#else   /* disable subject-name */
#define SUBJECT_NAME_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define SUBJECT_NAME_NAME      NULL
#define SUBJECT_NAME_DESC      NULL
#define SUBJECT_NAME_name      NULL
#endif  /* AUTOKEY */

/*
 *  trusted-cert option description:
 */
#ifdef AUTOKEY
#define TRUSTED_CERT_DESC      (ntp_keygen_opt_strs+1715)
#define TRUSTED_CERT_NAME      (ntp_keygen_opt_strs+1747)
#define TRUSTED_CERT_name      (ntp_keygen_opt_strs+1760)
#define TRUSTED_CERT_FLAGS     (OPTST_DISABLED)

#else   /* disable trusted-cert */
#define TRUSTED_CERT_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define TRUSTED_CERT_NAME      NULL
#define TRUSTED_CERT_DESC      NULL
#define TRUSTED_CERT_name      NULL
#endif  /* AUTOKEY */

/*
 *  mv-params option description:
 */
#ifdef AUTOKEY
#define MV_PARAMS_DESC      (ntp_keygen_opt_strs+1773)
#define MV_PARAMS_NAME      (ntp_keygen_opt_strs+1802)
#define MV_PARAMS_name      (ntp_keygen_opt_strs+1812)
#define MV_PARAMS_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

#else   /* disable mv-params */
#define MV_PARAMS_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define MV_PARAMS_NAME      NULL
#define MV_PARAMS_DESC      NULL
#define MV_PARAMS_name      NULL
#endif  /* AUTOKEY */

/*
 *  mv-keys option description:
 */
#ifdef AUTOKEY
#define MV_KEYS_DESC      (ntp_keygen_opt_strs+1822)
#define MV_KEYS_NAME      (ntp_keygen_opt_strs+1843)
#define MV_KEYS_name      (ntp_keygen_opt_strs+1851)
#define MV_KEYS_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

#else   /* disable mv-keys */
#define MV_KEYS_FLAGS     (OPTST_OMITTED | OPTST_NO_INIT)
#define MV_KEYS_NAME      NULL
#define MV_KEYS_DESC      NULL
#define MV_KEYS_name      NULL
#endif  /* AUTOKEY */

/*
 *  Help/More_Help/Version option descriptions:
 */
#define HELP_DESC       (ntp_keygen_opt_strs+1859)
#define HELP_name       (ntp_keygen_opt_strs+1903)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (ntp_keygen_opt_strs+1908)
#define MORE_HELP_name  (ntp_keygen_opt_strs+1953)
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
#define VER_DESC        (ntp_keygen_opt_strs+1963)
#define VER_name        (ntp_keygen_opt_strs+1999)
#define SAVE_OPTS_DESC  (ntp_keygen_opt_strs+2007)
#define SAVE_OPTS_name  (ntp_keygen_opt_strs+2046)
#define LOAD_OPTS_DESC     (ntp_keygen_opt_strs+2056)
#define LOAD_OPTS_NAME     (ntp_keygen_opt_strs+2088)
#define NO_LOAD_OPTS_name  (ntp_keygen_opt_strs+2098)
#define LOAD_OPTS_pfx      (ntp_keygen_opt_strs+2111)
#define LOAD_OPTS_name     (NO_LOAD_OPTS_name + 3)
/*
 *  Declare option callback procedures
 */
#ifdef AUTOKEY
  static tOptProc doOptModulus;
#else /* not AUTOKEY */
# define doOptModulus NULL
#endif /* def/not AUTOKEY */
#if defined(TEST_NTP_KEYGEN_OPTS)
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


#else /* NOT defined TEST_NTP_KEYGEN_OPTS */
/*
 *  When not under test, there are different procs to use
 */
extern tOptProc
    ntpOptionPrintVersion, optionBooleanVal,      optionNestedVal,
    optionNumericVal,      optionPagedUsage,      optionResetOpt,
    optionStackArg,        optionTimeDate,        optionTimeVal,
    optionUnstackArg,      optionVendorOption,    optionVersionStderr;
static tOptProc
    doOptSet_Debug_Level, doUsageOpt;

/*
 *  #define map the "normal" callout procs
 */
#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level

#define SET_DEBUG_LEVEL_OPT_PROC doOptSet_Debug_Level
#endif /* defined(TEST_NTP_KEYGEN_OPTS) */
#define VER_PROC        ntpOptionPrintVersion

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Define the Ntp_Keygen Option Descriptions.
 */
static tOptDesc optDesc[OPTION_CT] = {
  {  /* entry idx, value */ 0, VALUE_OPT_CERTIFICATE,
     /* equiv idx, value */ 0, VALUE_OPT_CERTIFICATE,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ CERTIFICATE_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --certificate */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ CERTIFICATE_DESC, CERTIFICATE_NAME, CERTIFICATE_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_CIPHER,
     /* equiv idx, value */ 1, VALUE_OPT_CIPHER,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ CIPHER_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --cipher */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ CIPHER_DESC, CIPHER_NAME, CIPHER_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_DEBUG_LEVEL,
     /* equiv idx, value */ 2, VALUE_OPT_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ DEBUG_LEVEL_DESC, DEBUG_LEVEL_NAME, DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equiv idx, value */ 3, VALUE_OPT_SET_DEBUG_LEVEL,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, NOLIMIT, 0,
     /* opt state flags  */ SET_DEBUG_LEVEL_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --set-debug-level */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ SET_DEBUG_LEVEL_OPT_PROC,
     /* desc, NAME, name */ SET_DEBUG_LEVEL_DESC, SET_DEBUG_LEVEL_NAME, SET_DEBUG_LEVEL_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_ID_KEY,
     /* equiv idx, value */ 4, VALUE_OPT_ID_KEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ ID_KEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --id-key */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ ID_KEY_DESC, ID_KEY_NAME, ID_KEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 5, VALUE_OPT_GQ_PARAMS,
     /* equiv idx, value */ 5, VALUE_OPT_GQ_PARAMS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ GQ_PARAMS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --gq-params */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ GQ_PARAMS_DESC, GQ_PARAMS_NAME, GQ_PARAMS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 6, VALUE_OPT_HOST_KEY,
     /* equiv idx, value */ 6, VALUE_OPT_HOST_KEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ HOST_KEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --host-key */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ HOST_KEY_DESC, HOST_KEY_NAME, HOST_KEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 7, VALUE_OPT_IFFKEY,
     /* equiv idx, value */ 7, VALUE_OPT_IFFKEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IFFKEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --iffkey */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ IFFKEY_DESC, IFFKEY_NAME, IFFKEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 8, VALUE_OPT_IDENT,
     /* equiv idx, value */ 8, VALUE_OPT_IDENT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ IDENT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --ident */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ IDENT_DESC, IDENT_NAME, IDENT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 9, VALUE_OPT_LIFETIME,
     /* equiv idx, value */ 9, VALUE_OPT_LIFETIME,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ LIFETIME_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --lifetime */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ LIFETIME_DESC, LIFETIME_NAME, LIFETIME_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 10, VALUE_OPT_MD5KEY,
     /* equiv idx, value */ 10, VALUE_OPT_MD5KEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MD5KEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --md5key */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ MD5KEY_DESC, MD5KEY_NAME, MD5KEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 11, VALUE_OPT_MODULUS,
     /* equiv idx, value */ 11, VALUE_OPT_MODULUS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MODULUS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --modulus */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptModulus,
     /* desc, NAME, name */ MODULUS_DESC, MODULUS_NAME, MODULUS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 12, VALUE_OPT_PVT_CERT,
     /* equiv idx, value */ 12, VALUE_OPT_PVT_CERT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PVT_CERT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --pvt-cert */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PVT_CERT_DESC, PVT_CERT_NAME, PVT_CERT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 13, VALUE_OPT_PVT_PASSWD,
     /* equiv idx, value */ 13, VALUE_OPT_PVT_PASSWD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ PVT_PASSWD_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --pvt-passwd */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ PVT_PASSWD_DESC, PVT_PASSWD_NAME, PVT_PASSWD_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 14, VALUE_OPT_GET_PVT_PASSWD,
     /* equiv idx, value */ 14, VALUE_OPT_GET_PVT_PASSWD,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ GET_PVT_PASSWD_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --get-pvt-passwd */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ GET_PVT_PASSWD_DESC, GET_PVT_PASSWD_NAME, GET_PVT_PASSWD_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 15, VALUE_OPT_SIGN_KEY,
     /* equiv idx, value */ 15, VALUE_OPT_SIGN_KEY,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SIGN_KEY_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --sign-key */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SIGN_KEY_DESC, SIGN_KEY_NAME, SIGN_KEY_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 16, VALUE_OPT_SUBJECT_NAME,
     /* equiv idx, value */ 16, VALUE_OPT_SUBJECT_NAME,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ SUBJECT_NAME_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --subject-name */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ SUBJECT_NAME_DESC, SUBJECT_NAME_NAME, SUBJECT_NAME_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 17, VALUE_OPT_TRUSTED_CERT,
     /* equiv idx, value */ 17, VALUE_OPT_TRUSTED_CERT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ TRUSTED_CERT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --trusted-cert */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ TRUSTED_CERT_DESC, TRUSTED_CERT_NAME, TRUSTED_CERT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 18, VALUE_OPT_MV_PARAMS,
     /* equiv idx, value */ 18, VALUE_OPT_MV_PARAMS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MV_PARAMS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --mv-params */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ MV_PARAMS_DESC, MV_PARAMS_NAME, MV_PARAMS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 19, VALUE_OPT_MV_KEYS,
     /* equiv idx, value */ 19, VALUE_OPT_MV_KEYS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MV_KEYS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --mv-keys */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ optionNumericVal,
     /* desc, NAME, name */ MV_KEYS_DESC, MV_KEYS_NAME, MV_KEYS_name,
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
 *  Define the Ntp_Keygen Option Environment
 */
#define zPROGNAME       (ntp_keygen_opt_strs+2114)
#define zUsageTitle     (ntp_keygen_opt_strs+2125)
#define zRcName         (ntp_keygen_opt_strs+2241)
static char const * const apzHomeList[3] = {
    ntp_keygen_opt_strs+2248,
    ntp_keygen_opt_strs+2254,
    NULL };
#define zBugsAddr       (ntp_keygen_opt_strs+2256)
#define zExplain        (ntp_keygen_opt_strs+2290)
#define zDetail         (NULL)
#define zFullVersion    (ntp_keygen_opt_strs+2293)
/* extracted from optcode.tlib near line 315 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */


#define ntp_keygen_full_usage (NULL)

#define ntp_keygen_short_usage (NULL)

/*
 *  Create the static procedure(s) declared above.
 */
static void
doUsageOpt(tOptions * pOptions, tOptDesc * pOptDesc)
{
    (void)pOptions;
    USAGE(NTP_KEYGEN_EXIT_SUCCESS);
}

#if ! defined(TEST_NTP_KEYGEN_OPTS)

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
#endif /* defined(TEST_NTP_KEYGEN_OPTS) */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   For the modulus option, when AUTOKEY is #define-d.
 */
#ifdef AUTOKEY
static void
doOptModulus(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static struct {long rmin, rmax;} const rng[1] = {
        { 256, 2048 } };
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
#endif /* defined AUTOKEY */
/* extracted from optmain.tlib near line 128 */

#if defined(TEST_NTP_KEYGEN_OPTS) /* TEST MAIN PROCEDURE: */

extern void optionPutShell(tOptions*);

int
main(int argc, char ** argv)
{
    int res = NTP_KEYGEN_EXIT_SUCCESS;
    (void)optionProcess(&ntp_keygenOptions, argc, argv);
    optionPutShell(&ntp_keygenOptions);
    res = ferror(stdout);
    if (res != 0)
        fputs("output error writing to stdout\n", stderr);
    return res;
}
#endif  /* defined TEST_NTP_KEYGEN_OPTS */
/* extracted from optmain.tlib near line 1148 */

#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

#ifndef  WITH_PACKAGER
# define ntp_keygen_packager_info NULL
#else
static char const ntp_keygen_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport ntp_keygen bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif

tOptions ntp_keygenOptions = {
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
    25 /* full option count */, 20 /* user option count */,
    ntp_keygen_full_usage, ntp_keygen_short_usage,
    NULL, NULL,
    PKGDATADIR, ntp_keygen_packager_info
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
        exit(NTP_KEYGEN_EXIT_FAILURE);
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
    tOptions * const pOpt = &ntp_keygenOptions;

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
        ntp_keygenOptions.fOptSet |= OPTPROC_NXLAT_OPT_CFG | OPTPROC_NXLAT_OPT;
    }
}

#endif /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
/* ntp-keygen-opts.c ends here */
