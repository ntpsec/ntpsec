dnl ######################################################################
dnl Common m4sh code for libntp clients
AC_DEFUN([NTP_LIBNTP], [

# Expose a cross-compilation indicator to makefiles
AM_CONDITIONAL([NTP_CROSSCOMPILE], [test $build != $host])

CFLAGS_NTP=
CPPFLAGS_NTP=
AC_SUBST([CFLAGS_NTP])
AC_SUBST([CPPFLAGS_NTP])

# AC_PROG_CC_STDC has two functions.  It attempts to find a compiler
# capable of C99, or failing that, for C89.  CC is set afterward with
# the selected invocation, such as "gcc --std=gnu99".  Also, the
# ac_cv_prog_cc_stdc variable is no if the compiler selected for CC
# does not accept C89.

AC_PROG_CC_STDC

case "$ac_cv_prog_cc_stdc" in
 no)
    AC_MSG_WARN([ANSI C89/ISO C90 is the minimum to compile NTP]
		[ version 4.2.5 and higher.])
    ;;
esac

AC_CACHE_CHECK(
    [if $CC can handle @%:@warning],
    [ntp_cv_cpp_warning],
    [AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[]], [[#warning foo]])],
	[ntp_cv_cpp_warning=yes],
	[ntp_cv_cpp_warning=no]
    )]
)
case "$ntp_cv_cpp_warning" in
 no)
    AC_DEFINE([NO_OPTION_NAME_WARNINGS], [1],
	[Should we avoid @%:@warning on option name collisions?])
esac

case "$GCC" in
 yes)
    SAVED_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -Wstrict-overflow"
    AC_CACHE_CHECK(
	[if $CC can handle -Wstrict-overflow], 
	[ntp_cv_gcc_Wstrict_overflow], 
	[AC_COMPILE_IFELSE(
	    [AC_LANG_PROGRAM([[]], [[]])],
	    [ntp_cv_gcc_Wstrict_overflow=yes],
	    [ntp_cv_gcc_Wstrict_overflow=no]
	)	]
    )
    #
    # $ntp_cv_gcc_Wstrict_overflow is tested later to add the 
    # flag to CFLAGS.
    #
    CFLAGS="$SAVED_CFLAGS -Winit-self"
    AC_CACHE_CHECK(
	[if $CC can handle -Winit-self], 
	[ntp_cv_gcc_Winit_self],
	[
	    AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([[]], [[]])],
		[ntp_cv_gcc_Winit_self=yes],
		[ntp_cv_gcc_Winit_self=no]
	    )
	]
    )
    CFLAGS="$SAVED_CFLAGS"
    AS_UNSET([SAVED_CFLAGS])
    #
    # $ntp_cv_gcc_Winit_self is tested later to add the 
    # flag to CFLAGS_NTP.
    #
esac

case "$GCC" in
 yes)
    CFLAGS_NTP="$CFLAGS_NTP -Wall"
    # CFLAGS_NTP="$CFLAGS_NTP -Wcast-align"
    CFLAGS_NTP="$CFLAGS_NTP -Wcast-qual"
    # CFLAGS_NTP="$CFLAGS_NTP -Wconversion"
    # CFLAGS_NTP="$CFLAGS_NTP -Werror"
    # CFLAGS_NTP="$CFLAGS_NTP -Wextra"
    # CFLAGS_NTP="$CFLAGS_NTP -Wfloat-equal"
    CFLAGS_NTP="$CFLAGS_NTP -Wmissing-prototypes"
    CFLAGS_NTP="$CFLAGS_NTP -Wpointer-arith"
    CFLAGS_NTP="$CFLAGS_NTP -Wshadow"
    # CFLAGS_NTP="$CFLAGS_NTP -Wtraditional"
    # CFLAGS_NTP="$CFLAGS_NTP -Wwrite-strings"
    case "$ntp_cv_gcc_Winit_self" in
     yes)
	CFLAGS_NTP="$CFLAGS_NTP -Winit-self"
    esac
    case "$ntp_cv_gcc_Wstrict_overflow" in
     yes)
	CFLAGS_NTP="$CFLAGS_NTP -Wstrict-overflow"
    esac
    # -W[no-]strict-prototypes might be added by NTP_OPENSSL
esac

NTP_OS_CFLAGS
NTP_LIB_M
NTP_DIR_SEP

AC_MSG_CHECKING([for bin subdirectory])
AC_ARG_WITH(
    [binsubdir],
    [AS_HELP_STRING(
	[--with-binsubdir],
	[bin ={bin,sbin}]
    )],
    [use_binsubdir="$withval"],
    [use_binsubdir="bin"]
)
case "$use_binsubdir" in
 bin)
    ;;
 sbin)
    ;;
 *)
    AC_MSG_ERROR([<$use_binsubdir> is illegal - must be "bin" or "sbin"])
    ;;
esac
AC_MSG_RESULT([$use_binsubdir])

BINSUBDIR=$use_binsubdir
AC_SUBST([BINSUBDIR])
AM_CONDITIONAL([NTP_BINSUBDIR_IS_BIN], [test "bin" = "$BINSUBDIR"])

ac_busted_vpath_in_make=no
case "$build" in
 *-*-irix6.1*)	# 64 bit only
    # busted vpath?
    ;;
 *-*-irix6*)	# 6.2 (and later?)
    ac_busted_vpath_in_make=yes
    ;;
 *-*-solaris2.5.1)
    ac_busted_vpath_in_make=yes
    ;;
 *-*-unicosmp*)
    ac_busted_vpath_in_make=yes
    ;;
esac

case "$ac_busted_vpath_in_make$srcdir" in
 yes.|no*)
    ;;
 *) case "`${MAKE-make} -v -f /dev/null 2>/dev/null | grep 'GNU Make'`" in
     '')
	AC_MSG_ERROR([building outside of the main directory requires GNU make])
    esac
    ;;
esac

dnl  HP-UX 11.31 on HPPA has a net/if.h that can't be compiled with gcc4
dnl  due to an incomplete type (a union) mpinfou used in an array.  gcc3
dnl  compiles it without complaint.  The mpinfou union is defined later
dnl  in the resulting preprocessed source than the spu_info array in
dnl  /usr/include/machine/sys/getppdp.h:
dnl	extern union mpinfou spu_info[];
dnl  triggering the error.  Our strategy is on HP-UX only, test compile
dnl  net/if.h.  If that fails, try adding a duplicate definition of
dnl  mpinfou, and if that helps add it to confdefs.h (used for further
dnl  configure tests) and config.h.
#
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([net/if.h], [], [], [
    #ifdef HAVE_SYS_SOCKET_H
    # include <sys/socket.h>
    #endif
])
case "$host" in
 *-hp-hpux*)
    AC_CACHE_CHECK(
	[if net/if.h requires mpinfou predeclaration],
	[ntp_cv_predecl_mpinfou],
	[
	    np_cv_predecl_mpinfou=no
	    case "$ac_cv_header_net_if_h" in
	     no)
		AC_COMPILE_IFELSE(
		    [AC_LANG_PROGRAM(
			[[
			    typedef union mpinfou {
				    struct pdk_mpinfo *pdkptr;
				    struct mpinfo *pikptr;
			    } mpinfou_t;
			    #ifdef HAVE_SYS_SOCKET_H
			    # include <sys/socket.h>
			    #endif
			    #include <net/if.h>
			]],
			[[
			]]
		    )],
		    [
			ntp_cv_predecl_mpinfou=yes
			ac_cv_header_net_if_h=yes
		    ]
		)
	    esac
	]
    )
    case "$ntp_cv_predecl_mpinfou" in
     yes)
	cat >>confdefs.h <<_ACEOF
#ifndef MPINFOU_PREDECLARED
# define MPINFOU_PREDECLARED
typedef union mpinfou {
	struct pdk_mpinfo *pdkptr;
	struct mpinfo *pikptr;
} mpinfou_t;
#endif
_ACEOF
	AH_BOTTOM([
#ifndef MPINFOU_PREDECLARED
# define MPINFOU_PREDECLARED
typedef union mpinfou {
	struct pdk_mpinfo *pdkptr;
	struct mpinfo *pikptr;
} mpinfou_t;
#endif
])
    esac
esac

# [Bug 1628] On Solaris, we need -lxnet -lsocket.  Generalize this to
# avoid keying on the OS name:  If we find socket functions in
# libsocket, next try for them in libxnet without libsocket, if found,
# list both.  If we simply tested libxnet first, we would find the
# functions there and never add libsocket.  See also [Bug 660]
# http://bugs.ntp.org/show_bug.cgi?id=660#c9
saved_LIBS=$LIBS
AC_SEARCH_LIBS([setsockopt], [socket])
case "$ac_cv_search_setsockopt" in
 -lsocket)
    LIBS="$saved_LIBS"
    AC_SEARCH_LIBS([getsockopt], [xnet])
    case "$ac_cv_search_getsockopt" in
     -lxnet)
	LIBS="-lxnet -lsocket $saved_LIBS"
	;;
     *) LIBS="-lsocket $saved_LIBS"
	;;
    esac
    ;;
esac
AS_UNSET([saved_LIBS])

AC_C_INLINE

case "$ac_cv_c_inline" in
 '')
    ;;
 *)
    AC_DEFINE([HAVE_INLINE], [1], [inline keyword or macro available])
    AC_SUBST([HAVE_INLINE])
esac

AC_HEADER_TIME
AC_CHECK_SIZEOF([time_t])
AC_C_CHAR_UNSIGNED		dnl CROSS_COMPILE?
AC_CHECK_SIZEOF([signed char])
AC_CHECK_TYPES([s_char, long long])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

case "$ac_cv_type_long_long" in
 no)
    ;;
 *)
    AC_CHECK_SIZEOF([long long])
    ;;
esac

case "$ac_cv_c_char_unsigned$ac_cv_sizeof_signed_char$ac_cv_type_s_char" in
 *yes)
    # We have a typedef for s_char.  Might as well believe it...
    ;;
 no0no)
    # We have signed chars, can't say 'signed char', no s_char typedef.
    AC_DEFINE([NEED_S_CHAR_TYPEDEF], [1],
	[Do we need an s_char typedef?])
    ;;
 no1no)
    # We have signed chars, can say 'signed char', no s_char typedef.
    AC_DEFINE([NEED_S_CHAR_TYPEDEF], [1],
	[Do we need an s_char typedef?])
    ;;
 yes0no)
    # We have unsigned chars, can't say 'signed char', no s_char typedef.
    AC_MSG_ERROR([No way to specify a signed character!])
    ;;
 yes1no)
    # We have unsigned chars, can say 'signed char', no s_char typedef.
    AC_DEFINE([NEED_S_CHAR_TYPEDEF], [1],
	[Do we need an s_char typedef?])
    ;;
esac

AC_TYPE_UID_T
AC_FUNC_STRERROR_R

dnl preset withsntp=no in env to change default to --without-sntp
AC_MSG_CHECKING([if sntp will be built])
AC_ARG_WITH(
    [sntp],
    [AS_HELP_STRING(
	[--without-sntp],
	[- disable building sntp and sntp/tests]
    )],
    [],
    [with_sntp="${withsntp=yes}"]
)
case "$with_sntp" in
 no)
    SNTP=
    ;;
 *)
    SNTP=sntp
    ;;
esac
AC_SUBST([SNTP])
AM_CONDITIONAL([BUILD_SNTP], [test -n "$SNTP"])
AC_MSG_RESULT([$with_sntp])

])dnl
dnl ======================================================================
