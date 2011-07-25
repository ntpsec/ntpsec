dnl SNTP subpackage configure.ac			-*- Autoconf -*-
dnl
m4_include([m4/version.m4])
AC_PREREQ([2.61])
AC_INIT(
    [sntp],
    [VERSION_NUMBER],
    [http://bugs.ntp.org./],
    [],
    [http://www.ntp.org./]dnl
)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([libevent/build-aux])

# Bump sntp_configure_cache_version for each change to configure.ac or
# .m4 files which invalidates cached values from previous configure 
# runs.
#
# If the change affects cache variables used only by the main NTP
# configure.ac, then only its version number should be bumped, while
# the subdir configure.ac version numbers should be unchanged.  The
# same is true for a test/variable that is used only by one subdir
# being changed incompatibly; only that subdir's cache version needs
# bumping.
#
# If a change affects variables shared by all NTP configure scripts,
# please bump the version numbers of each.  If you are not sure, the
# safe choice is to bump all on any cache-invalidating change.
#
# In order to avoid the risk of version stamp collision between -stable
# and -dev branches, do not simply increment the version, instead use
# the date YYYYMMDD optionally with -HHMM if there is more than one
# bump in a day.

sntp_configure_cache_version=20110219

# When the version of config.cache and configure do not
# match, NTP_CACHEVERSION will flush the cache.

NTP_CACHEVERSION([sntp], [$sntp_configure_cache_version])

AM_INIT_AUTOMAKE([1.10 foreign -Wall -Wno-gnu])
dnl AM_SILENT_RULES req. automake 1.11.  [yes] defaults V=0
m4_ifdef(
    [AM_SILENT_RULES],
    [AM_SILENT_RULES([yes])]
)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
dnl the 'build' machine is where we run configure and compile
dnl the 'host' machine is where the resulting stuff runs.
AC_DEFINE_UNQUOTED([STR_SYSTEM], "$host", [canonical system (cpu-vendor-os) of where we should run])
AC_CONFIG_HEADER([config.h])
dnl AC_ARG_PROGRAM

NTP_PROG_CC

dnl NTP_LOCINFO
AC_SUBST([NTPDATE_MS])

NTP_LIBNTP

AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

NTP_WITHSNTP

# We need to know:
# - where to install sntp {bin,sbin}
# - what manual section the sntp page belongs in {1,1m,1M,8}
# - which style of macros should be used {man.mdoc}
#
# When configure is invoked from the main NTP directory, we will have:
# - SNTP_DB=sntp (if we are installing sntp in bindir, empty otherwise)
# - SNTP_DL=sntp (if we are installing sntp in libexecdir, empty otherwise)
# - SNTP_DS=sntp (if we are installing sntp in sbindir, empty otherwise)
# - SNTP_MS={1,1m,1M,8}
# - MANTAGFMT={man,mdoc}

AC_MSG_CHECKING([for bin subdirectory])
AC_ARG_WITH(
    [binsubdir],
    [AS_HELP_STRING(
	[--with-binsubdir],
	[bin ={bin,libexec,sbin}]
    )],
    [use_binsubdir="$withval"],
    [use_binsubdir="bin"]
)
case "$use_binsubdir" in
 bin)
    SNTP_DB=$SNTP
    SNTP_DL=
    SNTP_DS=
    ;;
 libexec)
    SNTP_DB=
    SNTP_DL=$SNTP
    SNTP_DS=
    ;;
 sbin)
    SNTP_DB=
    SNTP_DL=
    SNTP_DS=$SNTP
    ;;
 *)
    AC_MSG_ERROR([<$use_binsubdir> is illegal - must be "bin" "libexec" or "sbin"])
    ;;
esac
AC_MSG_RESULT([$use_binsubdir])

AC_SUBST([SNTP_DB])
AC_SUBST([SNTP_DL])
AC_SUBST([SNTP_DS])

AC_MSG_CHECKING([for manual page installation section])
AC_ARG_WITH(
    [mansec],
    [AS_HELP_STRING(
	[--with-mansec],
	[1 ={1,1m,1M,8}]
    )],
    [use_mansec="$withval"],
    [use_mansec="1"]
)
case "$use_mansec" in
 1)
    ;;
 1m)
    ;;
 1M)
    ;;
 8)
    ;;
 *)
    AC_MSG_ERROR([<$use_mansec> is illegal - must be "1" or "1m" or "1M" or "8"])
    ;;
esac
SNTP_MS=$use_mansec
AC_MSG_RESULT([$use_mansec])

AC_SUBST([SNTP_MS])

AC_MSG_CHECKING([for manual page formatting macros])
AC_ARG_WITH(
    [mantagfmt],
    [AS_HELP_STRING(
	[--with-mantagfmt],
	[man ={man,mdoc}]
    )],
    [use_mantagfmt="$withval"],
    [use_mantagfmt="man"]
)
case "$use_mantagfmt" in
 man)
    ;;
 mdoc)
    ;;
 *)
    AC_MSG_ERROR([<$use_mantagfmt> is illegal - must be "man" or "mdoc"])
    ;;
esac
MANTAGFMT=$use_mantagfmt
AC_MSG_RESULT([$use_mantagfmt])

AC_SUBST([MANTAGFMT])

###

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
LIBOPTS_CHECK

AM_COND_IF(
    [BUILD_SNTP],
    [NTP_LIBEVENT_CHECK],
    [NTP_LIBEVENT_CHECK_NOBUILD]
)

# Checks for libraries.

dnl NTP_LIBNTP checks for inet_XtoY
dnl AC_SEARCH_LIBS([inet_pton], [nsl])
AC_SEARCH_LIBS([openlog], [gen syslog])

# Checks for header files.
AC_CHECK_HEADERS([netdb.h string.h strings.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

NTP_OPENSSL
NTP_IPV6

###

# Hacks
# these need work if we're to move libntp under sntp
AC_DEFINE([HAVE_NO_NICE], 1, [sntp does not care about 'nice'])
AC_DEFINE([HAVE_TERMIOS], 1, [sntp does not care about TTY stuff])

# Checks for library functions.
AC_CHECK_FUNCS([socket])

# HMS: if we don't find c++ we should not look for gtest.
AC_PROG_CXX
NTP_GOOGLETEST

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([include/Makefile])
AC_CONFIG_FILES([tests/Makefile])

AC_OUTPUT
