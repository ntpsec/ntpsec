#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_include([version.m4])
AC_INIT(sntp, VERSION_NUMBER)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
dnl the 'build' machine is where we run configure and compile
dnl the 'host' machine is where the resulting stuff runs.
AC_DEFINE_UNQUOTED(STR_SYSTEM, "$host", [canonical system (cpu-vendor-os) of where we should run])
AC_CONFIG_HEADER([config.h])
dnl AC_ARG_PROGRAM
AC_PREREQ(2.53)

dnl AC_PREREQ(2.61)
dnl AC_CONFIG_SRCDIR([main.c])

# Checks for programs.
AC_PROG_CC

# NTP has (so far) been relying on leading-edge autogen.
# Therefore, by default:
# - use the version we ship with
# - do not install it
# - build a static copy (AC_DISABLE_SHARED - done earlier)
case "${enable_local_libopts+set}" in
 set) ;;
 *) enable_local_libopts=yes ;;
esac
case "${enable_libopts_install+set}" in
 set) ;;
 *) enable_libopts_install=no ;;
esac
LIBOPTS_CHECK(libopts)

AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netdb.h netinet/in.h stdlib.h string.h strings.h sys/socket.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

AC_C_INLINE
AC_C_CHAR_UNSIGNED		dnl CROSS_COMPILE?
AC_CHECK_SIZEOF(signed char)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)

AC_CHECK_TYPES([s_char])
case "$ac_cv_c_char_unsigned$ac_cv_sizeof_signed_char$ac_cv_type_s_char" in
 *yes)
    # We have a typedef for s_char.  Might as well believe it...
    ;;
 no0no)
    # We have signed chars, can't say 'signed char', no s_char typedef.
    AC_DEFINE(NEED_S_CHAR_TYPEDEF, 1, [Do we need an s_char typedef?])
    ;;
 no1no)
    # We have signed chars, can say 'signed char', no s_char typedef.
    AC_DEFINE(NEED_S_CHAR_TYPEDEF)
    ;;
 yes0no)
    # We have unsigned chars, can't say 'signed char', no s_char typedef.
    AC_MSG_ERROR(No way to specify a signed character!)
    ;;
 yes1no)
    # We have unsigned chars, can say 'signed char', no s_char typedef.
    AC_DEFINE(NEED_S_CHAR_TYPEDEF)
    ;;
esac
AC_TYPE_UID_T

# Hacks
AC_DEFINE(HAVE_NO_NICE, 1, [sntp does not care about 'nice'])
AC_DEFINE(HAVE_TERMIOS, 1, [sntp does not care about TTY stuff])
 


# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
