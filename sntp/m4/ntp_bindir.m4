dnl ######################################################################
dnl Installation binary directory.  This may only apply to ntpd, not sntp
AC_DEFUN([NTP_BINDIR], [

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

])dnl
dnl ======================================================================
