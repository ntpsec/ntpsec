dnl ######################################################################
dnl rlimit capabilities checks
AC_DEFUN([NTP_RLIMIT_ITEMS], [

AC_CACHE_CHECK(
    [for RLIMIT_MEMLOCK],
    [ntp_cv_rlimit_memlock],
    [AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
	    [[
		#include <sys/types.h>
		#include <sys/time.h>
		#include <sys/resource.h>
	    ]],
	    [[
		getrlimit(RLIMIT_MEMLOCK, 0);
	    ]]
	)],
	[ntp_cv_rlimit_memlock=yes],
	[ntp_cv_rlimit_memlock=no]
    )]
)
case "$ntp_cv_rlimit_memlock" in
 yes)
    AC_SUBST([HAVE_RLIMIT_MEMLOCK])
    HAVE_RLIMIT_MEMLOCK=" memlock 32"
esac

AC_CACHE_CHECK(
    [for RLIMIT_STACK],
    [ntp_cv_rlimit_stack],
    [AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
	    [[
		#include <sys/types.h>
		#include <sys/time.h>
		#include <sys/resource.h>
	    ]],
	    [[
		getrlimit(RLIMIT_STACK, 0);
	    ]]
	)],
	[ntp_cv_rlimit_stack=yes],
	[ntp_cv_rlimit_stack=no]
    )]
)
case "$ntp_cv_rlimit_stack" in
 yes)
    AC_SUBST([HAVE_RLIMIT_STACK])
    HAVE_RLIMIT_STACK=" stacksize 50"
esac

])dnl
dnl ======================================================================
