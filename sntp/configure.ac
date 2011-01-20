#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_include([m4/version.m4])
AC_PREREQ([2.61])
AC_INIT([sntp], [VERSION_NUMBER])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

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

sntp_configure_cache_version=20100916

# When the version of config.cache and configure do not
# match, NTP_CACHEVERSION will flush the cache.

NTP_CACHEVERSION([sntp], [$sntp_configure_cache_version])

AM_INIT_AUTOMAKE
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

dnl must come before AC_PROG_CC or similar
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CPP

AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

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

NTP_LIBNTP

# Checks for libraries.

AC_SEARCH_LIBS([inet_pton], [nsl])
AC_SEARCH_LIBS([openlog], [gen syslog])

# -lrt may only be needed by tests/
case "$host" in
 *-*-aix[[456]]*)
	# (prr) aix 4.1 doesn't have clock_settime, but in aix 4.3 it's a stub
	# (returning ENOSYS).  I didn't check 4.2.  If, in the future,
	# IBM pulls its thumbs out long enough to implement clock_settime,
	# this conditional will need to change.  Maybe use AC_TRY_RUN
	# instead to try to set the time to itself and check errno.
    ;;
 *)
    AC_SEARCH_LIBS([clock_gettime], [rt])
    AC_CHECK_FUNCS([clock_gettime clock_settime])
    ;;
esac

# Checks for header files.
AC_HEADER_STDC


AC_CHECK_HEADERS([netdb.h netinet/in.h stdlib.h string.h strings.h syslog.h])
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

NTP_OPENSSL
NTP_IPV6

AC_CACHE_CHECK(
    [for multicast IP support],
    [ntp_cv_multicast],
    [
	ntp_cv_multicast=no
	case "$host" in
	 i386-sequent-sysv4)
	    ;;
	 *)
	    AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
		    [[
			#ifdef HAVE_NETINET_IN_H
			#include <netinet/in.h>
			#endif
		    ]],
		    [[
			struct ip_mreq ipmr;
			ipmr.imr_interface.s_addr = 0;
		    ]]
		)],
		[ntp_cv_multicast=yes],
		[]
	    )
	esac
    ]
)
case "$ntp_cv_multicast" in
 yes)
    AC_DEFINE([MCAST], [1], [Does the target support multicast IP?])
    AC_CACHE_CHECK(
	[arg type needed for setsockopt() IP*_MULTICAST_LOOP],
	[ntp_cv_typeof_ip_multicast_loop],
	[
	    case "$host" in
	     *-*-netbsd*|*-*-*linux*)
		ntp_cv_typeof_ip_multicast_loop=u_int
		;;
	     *)
		ntp_cv_typeof_ip_multicast_loop=u_char
	    esac
	]
    )
    AC_DEFINE_UNQUOTED([TYPEOF_IP_MULTICAST_LOOP],
	$ntp_cv_typeof_ip_multicast_loop,
	[What type to use for setsockopt])
esac

# HMS: We don't need res_init, but since we may be using cached 
# values from ntpd we need to test the same way 
AC_SEARCH_LIBS([res_init], [resolv], , , [-lsocket -lnsl]) 
AC_SEARCH_LIBS([inet_ntop], [resolv], , , [-lsocket -lnsl])
AC_CHECK_FUNC([inet_ntop], [], 
    [AC_DEFINE([ISC_PLATFORM_NEEDNTOP], [1], [ISC: provide inet_ntop()])])
AC_CHECK_FUNC([inet_pton], [],
    [AC_DEFINE([ISC_PLATFORM_NEEDPTON], [1], [ISC: provide inet_pton()])])


###

# Hacks
# these need work if we're to move libntp under sntp
AC_DEFINE([HAVE_NO_NICE], 1, [sntp does not care about 'nice'])
AC_DEFINE([HAVE_TERMIOS], 1, [sntp does not care about TTY stuff])

# Checks for library functions.
AC_CHECK_FUNCS([socket vsnprintf vsprintf])

# HMS: if we don't find c++ we should not look for gtest.
AC_PROG_CXX
NTP_GOOGLETEST

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([include/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_OUTPUT
