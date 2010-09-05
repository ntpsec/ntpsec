dnl ######################################################################
dnl What do we need for math libraries?
AC_DEFUN([NTP_LIB_M], [
LIBM=
case "$host" in
 *-*-darwin*)
    ;;
 *)
    AC_CHECK_LIB(m, cos, LIBM="-lm")
    ;;
esac
AC_SUBST([LIBM])
])
dnl ======================================================================
