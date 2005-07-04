# This configure.ac file is for use when "libopts" is an independent project.
#
AC_INIT([libopts],[25.1.0],[autogen-users@lists.sf.net])
AC_CONFIG_SRCDIR(libopts/autoopts.c)
AC_CONFIG_AUX_DIR(m4)
AC_CONFIG_MACRO_DIR(m4)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([gnu check-news 1.5 dist-bzip2])
AC_LIBTOOL_WIN32_DLL    m4_define(AC_PROVIDE_AC_LIBTOOL_WIN32_DLL)
AC_PROG_LIBTOOL
ifdef([AC_REVISION],AC_REVISION(: 4.11 $),)dnl
AC_SUBST(AO_CURRENT)
AC_SUBST(AO_REVISION)
AC_SUBST(AO_AGE)
AC_DEFINE_UNQUOTED(AO_CURRENT,25,
                   [Define this to the autoopts current interface number])
AC_DEFINE_UNQUOTED(AO_REVISION,1,
                   [Define this to the autoopts interface revision number])
AC_DEFINE_UNQUOTED(AO_AGE,0,
                   [Define this to the autoopts interface age number])

# ----------------------------------------------------------------------
# If 'configure' is invoked (in)directly via 'make', ensure that it
# encounters no 'make' conflicts.  Ignore error if shell does not have
# unset, but at least set these to empty values.
# ----------------------------------------------------------------------
MFLAGS=
MAKEFLAGS=
MAKELEVEL=
unset MFLAGS MAKEFLAGS MAKELEVEL 2>/dev/null
AC_PROG_CC
AC_PROG_CC_STDC
AC_EXEEXT
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_C_LONG_DOUBLE
AC_CHECK_TYPES([u_int, long long, uintmax_t])
AC_CHECK_TYPES([uintptr_t], ,
               [AC_DEFINE(uintptr_t, unsigned long,
                          [Alternate uintptr_t for systems without it.])])
AC_CHECK_SIZEOF(char*, 4)
AC_CHECK_SIZEOF(int,   4)
AC_CHECK_SIZEOF(long,  4)
AC_CHECK_FUNCS(strchr setjmp sigsetjmp strsignal strlcpy snprintf realpath 	               dlopen)
INVOKE_LIBOPTS_MACROS
AC_CONFIG_FILES([Makefile libopts/Makefile])
AM_CONFIG_HEADER(config.h:config-h.in)
AC_OUTPUT
