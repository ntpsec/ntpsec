noinst_PROGRAMS = @PROPDELAY@ @CHUTEST@
EXTRA_PROGRAMS = propdelay chutest

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

LDADD = ../libntp/libntp.a $(LDADD_LIBNTP) $(PTHREAD_LIBS)
propdelay_LDADD = $(LIBM) $(LDADD)

BUILT_SOURCES =
CLEANFILES =

include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
