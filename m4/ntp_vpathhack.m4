dnl ######################################################################
dnl Are we using FreeBSD's make?
AC_DEFUN([NTP_VPATH_HACK], [
AC_CACHE_CHECK([to see if we need a VPATH hack], ac_cv_vpath_hack,
[
  case "$ac_cv_vpath_hack" in
   '')
    ac_cv_vpath_hack="no"
    case "$srcdir::$host_os" in
     ??*::freebsd*)
	case "`${MAKE-make} -v -f /dev/null 2>/dev/null | grep 'GNU Make version '`" in
	 '')
	    ac_cv_vpath_hack="yes"
	    ;;
	esac
	;;
    esac
    ;;
  esac
])
AM_CONDITIONAL([VPATH_HACK], [test x$ac_cv_vpath_hack = xyes])
])
dnl ======================================================================
