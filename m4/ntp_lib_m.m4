dnl ######################################################################
dnl What do we need for math libraries?
AC_DEFUN([NTP_LIB_M], [
LIBM=
case "$host" in
 *-*-darwin*)
    ;;
 *)
    AC_SEARCH_LIBS(cos, m, LIBM="-lm")
    ;;
esac
AC_SUBST([LIBM])
])
dnl ======================================================================
