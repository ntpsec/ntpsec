AUTOMAKE_OPTIONS=	

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	@MAKE_ADJTIMED@
else
sbin_PROGRAMS=	@MAKE_ADJTIMED@
endif

BUILT_SOURCES= check-libntp
CLEANFILES=
EXTRA_PROGRAMS=	adjtimed

INCLUDES  = -I$(top_srcdir)/include
INCLUDES += -I$(top_srcdir)/lib/isc/include
INCLUDES += -I$(top_srcdir)/lib/isc/@LIBISC_PTHREADS_NOTHREADS@/include
INCLUDES += -I$(top_srcdir)/lib/isc/unix/include
INCLUDES += $(LIBOPTS_CFLAGS)

AM_CFLAGS = @CFLAGS_NTP@
AM_CPPFLAGS = @CPPFLAGS_NTP@

LDADD=		../libntp/libntp.a @LDADD_LIBNTP@ @PTHREAD_LIBS@
ETAGS_ARGS=	Makefile.am

check-libntp: FRC
	cd ../libntp && $(MAKE)

FRC:

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/depsver.mf
