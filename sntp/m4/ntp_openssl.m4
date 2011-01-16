dnl ######################################################################
dnl OpenSSL support shared by top-level and sntp/configure.ac
AC_DEFUN([NTP_OPENSSL], [

LCRYPTO=
AC_SUBST([LCRYPTO])

AC_ARG_WITH(
    [rpath],
    [AS_HELP_STRING(
	[--without-rpath],
	[s Disable auto-added -R linker paths]
    )],
    [ans=$withval],
    [ans=x]
)
case "$ans" in
 no)
    need_dash_r=
    ;;
 yes)
    need_dash_r=1
    ;;
esac
# HMS: Why isn't this $build?
# Well, that depends on if we need this for the build toolchain or
# for info in the host executable...
# I still have no idea which way this should go, but nobody has complained.
case "$host" in
 *-*-netbsd*)
    case "$need_dash_r" in
     no) ;;
     *)  need_dash_r=1
	 ;;
    esac
    ;;
 *-*-solaris*)
    case "$need_dash_r" in
     no) ;;
     *)  need_dash_r=1
	 ;;
    esac
    ;;
esac

AC_SUBST([OPENSSL])
AC_SUBST([OPENSSL_INC])
AC_SUBST([OPENSSL_LIB])

AC_PATH_PROG([PKG_CONFIG], [pkg-config])

AC_MSG_CHECKING([for openssl library directory])
AC_ARG_WITH(
    [openssl-libdir],
    [AS_HELP_STRING(
	[--with-openssl-libdir], 
	[+ =/something/reasonable]
    )],
    [ans=$withval],
    [
	case "$build" in
	 $host) ans=yes ;;
	 *)     ans=no ;;
	esac
    ]
)
case "$ans" in
 no) 
    ;;
 yes)
    case "$PKG_CONFIG" in
     '')
	;;
     *)
	pkgans=`$PKG_CONFIG --libs-only-L openssl 2>/dev/null`
	pkgans=`echo $pkgans | sed -e 's/^-L//' | tr -d '\n' | tr -d ' '`
	if test -f "${pkgans}/pkgconfig/openssl.pc" ; then
	    ans="$pkgans"
	fi
	;;
    esac
    ;;
 *) # Look where they said
    ;;
esac
case "$ans" in
 yes)
    # Look in:
    ans="/usr/lib /usr/lib/openssl /usr/sfw/lib"
    ans="$ans /usr/local/lib /usr/local/ssl/lib /lib"
esac
case "$ans" in
 no)
    ;;
 *) # Look for libcrypto.a and libssl.a:
    for i in $ans no
    do
	case "$host" in
	 *-*-darwin*)
	    test -f $i/libcrypto.dylib -a -f $i/libssl.dylib && break
	    ;;
	 *)
	    test -f $i/libcrypto.so -a -f $i/libssl.so && break
	    test -f $i/libcrypto.a -a -f $i/libssl.a && break
	    ;;
	esac
    done
    case "$i" in
     no)
	echo ""
	echo "did not find libcrypto and libssl in any of $ans"
	ans=no
	OPENSSL_LIB=
	;;
     *) ans=$i
	OPENSSL_LIB=$ans
	;;
    esac
    ;;
esac
AC_MSG_RESULT([$ans])

AC_MSG_CHECKING([for openssl include directory])
AC_ARG_WITH(
    [openssl-incdir],
    [AS_HELP_STRING(
	[--with-openssl-incdir],
	[+ =/something/reasonable]
    )],
    [ans=$withval],
    [
	case "$build" in
	 $host) ans=yes ;;
	 *)     ans=no ;;
	esac
    ]
)
case "$ans" in
 no)
    ;;
 yes) # look in:
    case "$PKG_CONFIG" in
     '')
	;;
     *)
	pkgans=`$PKG_CONFIG --cflags-only-I openssl 2>/dev/null`
	pkgans=`echo $pkgans | sed -e 's/^-I//' | tr -d '\n' | tr -d ' '`
	if test -f "${pkgans}/openssl/evp.h" ; then
	    ans="$pkgans"
	fi
	;;
    esac
    ;;
 *) # Look where they said
    ;;
esac
case "$ans" in
 yes)
    # Look in:
    ans="/usr/include /usr/sfw/include /usr/local/include"
    ans="$ans /usr/local/ssl/include"
esac
case "$ans" in
 no)
    ;;
 *) # look for openssl/evp.h:
    for i in $ans no
    do
	test -f $i/openssl/evp.h && break
    done
    case "$i" in
     no)
	echo ""
	echo "did not find openssl/evp.h in any of $ans"
	ans=no
	OPENSSL_INC=
	;;
     *) ans=$i
	OPENSSL_INC=$ans
	;;
    esac
    ;;
esac
AS_UNSET([pkgans])
AC_MSG_RESULT([$ans])

AC_MSG_CHECKING([if we will use crypto])
AC_ARG_WITH(
    [crypto],
    [AS_HELP_STRING(
	[--with-crypto],
	[+ =openssl]
    )],
    [ans=$withval],
    [ans=yes]
)
case "$ans" in
 no)
    ;;
 yes|openssl)
    if test -z "$OPENSSL_LIB" -o -z "$OPENSSL_INC"
    then
	ans=no
    else
	ans=yes
    fi
esac
ntp_openssl=$ans
AC_MSG_RESULT([$ans])

case "$ntp_openssl" in
 yes)
    # We have OpenSSL inc/lib - use them.
    case "$OPENSSL_INC" in
     /usr/include)
	;;
     *)	
	CPPFLAGS_NTP="$CPPFLAGS_NTP -I$OPENSSL_INC"
	;;
    esac
    case "$OPENSSL_LIB" in
     /usr/lib)
	;;
     *)	
	LDFLAGS="$LDFLAGS -L$OPENSSL_LIB"
	case "$need_dash_r" in
	 1)
	    LDFLAGS="$LDFLAGS -R$OPENSSL_LIB"
	esac
	;;
    esac
    LCRYPTO="-lcrypto"
    AC_DEFINE([OPENSSL], [1], [Use OpenSSL?])
esac

NTPO_SAVED_CPPFLAGS="$CPPFLAGS"
NTPO_SAVED_LIBS="$LIBS"

#
# check for linking with -lcrypto failure, and try -lz -lcrypto.
# Helps m68k-atari-mint
#
case "$ntp_openssl" in
 yes)
    CPPFLAGS="$CPPFLAGS $CPPFLAGS_NTP"
    LIBS="$NTPO_SAVED_LIBS $LCRYPTO"
    AC_CACHE_CHECK(
	[if linking with $LCRYPTO alone works],
	[ntp_cv_bare_lcrypto],
	[AC_LINK_IFELSE(
	    [AC_LANG_PROGRAM(
		[[
		    #include "openssl/err.h"
		    #include "openssl/evp.h"
		]],
		[[
		    ERR_load_crypto_strings();
		    OpenSSL_add_all_algorithms();
		]]
	    )],
	    [ntp_cv_bare_lcrypto=yes],
	    [ntp_cv_bare_lcrypto=no]
	)]
    )
    case "$ntp_cv_bare_lcrypto" in
     no)
	LIBS="$NTPO_SAVED_LIBS $LCRYPTO -lz"
	AC_CACHE_CHECK(
	    [if linking with $LCRYPTO -lz works],
	    [ntp_cv_lcrypto_lz],
	    [AC_LINK_IFELSE(
		[AC_LANG_PROGRAM(
		    [[
			#include "openssl/err.h"
			#include "openssl/evp.h"
		    ]],
		    [[
			ERR_load_crypto_strings();
			OpenSSL_add_all_algorithms();
		    ]]
		)],
		[ntp_cv_lcrypto_lz=yes],
		[ntp_cv_lcrypto_lz=no]
	    )]
	)
	case "$ntp_cv_lcrypto_lz" in
	 yes)
	     LCRYPTO="$LCRYPTO -lz"
	esac
    esac
esac

#
# Older OpenSSL headers have a number of callback prototypes inside
# other function prototypes which trigger copious warnings with gcc's
# -Wstrict-prototypes, which is included in -Wall.
#
# An example:
#
# int i2d_RSA_NET(const RSA *a, unsigned char **pp, 
#		  int (*cb)(), int sgckey);
#		  ^^^^^^^^^^^
#
# 
#
openssl_triggers_warnings=unknown
SAVED_CFLAGS="$CFLAGS"

case "$GCC$ntp_openssl" in
 yesyes)
    CFLAGS="$CFLAGS -Werror"
    AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
	    [[
	    ]],
	    [[
		/* see if -Werror breaks gcc */
	    ]]
	)],
	[gcc_handles_Werror=yes],
	[gcc_handles_Werror=no]
    )
    case "$gcc_handles_Werror" in
     no)
	# if this gcc doesn't do -Werror go ahead and use
	# -Wstrict-prototypes.
	openssl_triggers_warnings=yes
	;;
     yes)
	CFLAGS="$CFLAGS -Wstrict-prototypes"
	AC_COMPILE_IFELSE(
	    [AC_LANG_PROGRAM(
		[[
		    #include "openssl/asn1_mac.h"
		    #include "openssl/bn.h"
		    #include "openssl/err.h"
		    #include "openssl/evp.h"
		    #include "openssl/pem.h"
		    #include "openssl/rand.h"
		    #include "openssl/x509v3.h"
		]],
		[[
		    /* empty body */
		]]
	    )],
	    [openssl_triggers_warnings=no],
	    [openssl_triggers_warnings=yes]
	)
    esac
    case "$openssl_triggers_warnings" in
     yes)
	CFLAGS_NTP="$CFLAGS_NTP -Wno-strict-prototypes"
	;;
     *)
	CFLAGS_NTP="$CFLAGS_NTP -Wstrict-prototypes"
    esac
    ;;
 yesno)
    # gcc without OpenSSL
    CFLAGS_NTP="$CFLAGS_NTP -Wstrict-prototypes"
esac

CFLAGS="$SAVED_CFLAGS"
CPPFLAGS="$NTPO_SAVED_CPPFLAGS"
LIBS="$NTPO_SAVED_LIBS"
AS_UNSET([SAVED_CFLAGS])
AS_UNSET([NTPO_SAVED_CPPFLAGS])
AS_UNSET([NTPO_SAVED_LIBS])
])
dnl ======================================================================
