## adjtimed Makefile.am

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS = @MAKE_ADJTIMED@
else
sbin_PROGRAMS =	@MAKE_ADJTIMED@
endif

BUILT_SOURCES =
CLEANFILES =
EXTRA_PROGRAMS = adjtimed

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

LDADD = ../libntp/libntp.a $(LDADD_LIBNTP) $(PTHREAD_LIBS)

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
