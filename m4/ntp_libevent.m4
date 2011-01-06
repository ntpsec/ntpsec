dnl NTP_LIBEVENT_CHECK([MINVERSION, [DIR [, NOBUILD]]])	     -*- Autoconf -*-
dnl
dnl Look for libevent, which must be at least MINVERSION.
dnl DIR is the location of our "bundled" copy of libevent.
dnl If NOBUILD is provided as the 3rd argument, do all of the above,
dnl but DO NOT invoke DIR/configure if we are going to use our bundled
dnl version.  This may be the case for nested packages.
dnl
dnl provide --enable-local-libevent .
dnl
dnl Examples:
dnl
dnl    NTP_LIBEVENT_CHECK([2.0.9], [sntp/libevent], NOBUILD)
dnl    NTP_LIBEVENT_CHECK
dnl

dnl
dnl NOTE WELL: right now, if we build the local libevent, the 'install'
dnl target *will fire*
dnl

AC_DEFUN([NTP_LIBEVENT_CHECK], [
_ntp_pkg_min_version=m4_default([$1], [0.9.0])
_ntp_libevent_min_version=m4_default([$1], [2.0.9])
_ntp_libevent_path=m4_default([$2], [libevent])
_ntp_libevent_nobuild=m4_default([$3],[])
_ntp_libevent_found=no

NEED_LIBEVENT_DIR=

AC_PATH_TOOL([PKG_CONFIG], [pkg-config])
# Make sure pkg-config is recent enough
case "$PKG_CONFIG" in
 /*)
    AC_MSG_CHECKING([pkg-config is at least version $_ntp_pkg_min_version])
    if $PKG_CONFIG --atleast-pkgconfig-version $_ntp_pkg_min_version; then
	AC_MSG_RESULT([yes])
    else
	AC_MSG_RESULT([no])
	PKG_CONFIG=""
    fi
    ;;
esac

AC_ARG_ENABLE([local-libevent],
    AC_HELP_STRING([-enable-local-libevent],
	[Force using the supplied libevent tearoff code]),[
    use_local_libevent=$enableval])

case "$use_local_libevent" in
 yes)
    ;;
 *) # If we have (a good enough) pkg-config, see if it can find libevent
    case "$PKG_CONFIG" in
     /*)
	if $PKG_CONFIG --atleast-version=$_ntp_libevent_min_version libevent
	then
	    AC_MSG_NOTICE([Using the installed libevent])
	    LIBEVENT_CPPFLAGS=`$PKG_CONFIG --cflags libevent`
	    LIBEVENT_LDADD=`$PKG_CONFIG --libs libevent`
	    _ntp_libevent_found=yes
	fi
	;;
    esac
    ;;
esac

case "$_ntp_libevent_found" in
 no)
    AC_MSG_NOTICE([Using supplied libevent tearoff])
    LIBEVENT_CPPFLAGS='-I$(top_builddir)'/_il/include
    LIBEVENT_LDADD='$(top_builddir)'/_il/lib/libevent.la
    NEED_LIBEVENT_DIR=true
    ;;
esac

AM_CONDITIONAL([NEED_LIBEVENT], [test -n "${NEED_LIBEVENT_DIR}"])
AC_SUBST(LIBEVENT_CPPFLAGS)
AC_SUBST(LIBEVENT_LDADD)
])
