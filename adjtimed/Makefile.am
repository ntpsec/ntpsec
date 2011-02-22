## adjtimed Makefile.am

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	@MAKE_ADJTIMED@
else
sbin_PROGRAMS=	@MAKE_ADJTIMED@
endif

BUILT_SOURCES= check-libntp
CLEANFILES=
EXTRA_PROGRAMS=	adjtimed

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

LDADD = ../libntp/libntp.a $(LDADD_LIBNTP) $(PTHREAD_LIBS)

check-libntp: FRC
	cd ../libntp && $(MAKE)

FRC:

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
