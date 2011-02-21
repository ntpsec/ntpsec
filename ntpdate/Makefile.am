NULL=
AUTOMAKE_OPTIONS=

BUILT_SOURCES= check-libntp
CLEANFILES=

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	ntpdate
else
sbin_PROGRAMS=	ntpdate
endif

EXTRA_PROGRAMS=	ntptimeset

ntptimeset_SOURCES=	ntptimeset.c ntptime_config.c

INCLUDES  = -I$(top_srcdir)/include
INCLUDES += -I$(top_srcdir)/lib/isc/include
INCLUDES += -I$(top_srcdir)/lib/isc/@LIBISC_PTHREADS_NOTHREADS@/include
INCLUDES += -I$(top_srcdir)/lib/isc/unix/include

AM_CFLAGS = @CFLAGS_NTP@
AM_CPPFLAGS = @CPPFLAGS_NTP@

# LDADD might need RESLIB and ADJLIB
LDADD = version.o ../libntp/libntp.a
ntpdate_LDADD  = $(LDADD) @LDADD_LIBNTP@ @PTHREAD_LIBS@ $(LIBOPTS_LDADD)
ntpdate_LDADD += $(LIBM) @LCRYPTO@
DISTCLEANFILES=	.version version.c stamp-v
noinst_HEADERS=	ntpdate.h
ETAGS_ARGS=	Makefile.am

$(PROGRAMS): $(LDADD)

check-libntp: FRC
	cd ../libntp && $(MAKE)

FRC:

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

$(top_srcdir)/sntp/version:
	cd $(top_srcdir)/sntp && $(MAKE) version

version.o: $(ntpdate_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/sntp/version
	env CSET=`cat $(top_srcdir)/sntp/version` $(top_builddir)/scripts/mkver ntpdate
	$(COMPILE) -c version.c

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/depsver.mf
