NULL=

BUILT_SOURCES= check-libntp
CLEANFILES=

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	ntpdate
else
sbin_PROGRAMS=	ntpdate
endif

EXTRA_PROGRAMS=	ntptimeset

ntptimeset_SOURCES=	ntptimeset.c ntptime_config.c

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

# LDADD might need RESLIB and ADJLIB
LDADD = version.o ../libntp/libntp.a
ntpdate_LDADD  = $(LDADD) $(LDADD_LIBNTP) $(PTHREAD_LIBS) $(LIBOPTS_LDADD)
ntpdate_LDADD += $(LIBM) $(LCRYPTO)
DISTCLEANFILES=	.version version.c stamp-v
noinst_HEADERS=	ntpdate.h

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
include $(top_srcdir)/includes.mf
