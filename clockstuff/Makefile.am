noinst_PROGRAMS = @PROPDELAY@ @CHUTEST@ @CLKTEST@
EXTRA_PROGRAMS = propdelay chutest clktest

BUILT_SOURCES =	check-libntp clktest-opts.c clktest-opts.h
EXTRA_DIST = clktest-opts.def

clktest_SOURCES = clktest.c clktest-opts.c clktest-opts.h

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

LDADD = ../libntp/libntp.a $(LDADD_LIBNTP) $(PTHREAD_LIBS)
propdelay_LDADD = $(LIBM) $(LDADD)

run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../sntp/include --writable
std_def_list =							\
	$(top_srcdir)/sntp/include/debug-opt.def		\
	$(top_srcdir)/sntp/include/autogen-version.def 		\
	$(top_srcdir)/sntp/include/copyright.def 		\
	$(top_srcdir)/sntp/include/homerc.def 			\
	$(top_srcdir)/sntp/include/version.def			\
	$(NULL)

$(srcdir)/clktest-opts.h: $(srcdir)/clktest-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/clktest-opts.c: $(srcdir)/clktest-opts.def $(std_def_list)
	$(run_ag) clktest-opts.def

## chutest$(EXEEXT): ../libntp/libntp.a
## clktest$(EXEEXT): ../libntp/libntp.a

check-libntp: FRC
	cd ../libntp && $(MAKE)

FRC:

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

CLEANFILES =

include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
