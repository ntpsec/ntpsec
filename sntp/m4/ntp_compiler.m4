dnl ######################################################################
dnl Common m4sh code for compiler stuff
AC_DEFUN([NTP_COMPILER], [
AC_REQUIRE([AC_PROG_CC_C89])

CFLAGS_NTP=
CPPFLAGS_NTP=
AC_SUBST([CFLAGS_NTP])
AC_SUBST([CPPFLAGS_NTP])

case "$ac_cv_prog_cc_c89" in
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
    AC_CACHE_CHECK(
	[if linker supports omitting unused code and data],
	[ntp_cv_gc_sections_works],
	[
	    origCFLAGS="$CFLAGS"
	    CFLAGS="$CFLAGS -Wl,--gc-sections"
	    AC_LINK_IFELSE(
		[AC_LANG_PROGRAM([[]], [[]])],
		[
		    if grep gc-sections conftest.err ; then
			ntp_cv_gc_sections_works=no
		    else
			ntp_cv_gc_sections_works=yes
		    fi
		],
		[ntp_cv_gc_sections_works=no]
	    )
	    CFLAGS="$origCFLAGS"
	    AS_UNSET([origCFLAGS])
	]
    )
    case "$ntp_cv_gc_sections_works" in
     yes)
	LDADD_LIBNTP="-Wl,--gc-sections"
	CFLAGS_NTP="$CFLAGS_NTP -ffunction-sections -fdata-sections"
	;;
     no)
	LDADD_LIBNTP=
	;;
    esac
    CFLAGS_NTP="$CFLAGS_NTP -Wall"
    CFLAGS_NTP="$CFLAGS_NTP -Wcast-align"
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

])dnl
dnl ======================================================================
