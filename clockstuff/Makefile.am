AUTOMAKE_OPTIONS =
noinst_PROGRAMS = @PROPDELAY@ @CHUTEST@ @CLKTEST@
EXTRA_PROGRAMS = propdelay chutest clktest

BUILT_SOURCES =	clktest-opts.c clktest-opts.h
EXTRA_DIST = clktest-opts.def

clktest_SOURCES = clktest.c clktest-opts.c

INCLUDES =  -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include
INCLUDES += -I$(top_srcdir)/lib/isc/nothreads/include
INCLUDES += -I$(top_srcdir)/lib/isc/unix/include $(LIBOPTS_CFLAGS)

LDADD = ../libntp/libntp.a
propdelay_LDADD = $(LIBM) $(LDADD)

run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../include --writable
std_def_list=	$(top_srcdir)/include/debug-opt.def		\
		$(top_srcdir)/include/autogen-version.def 	\
		$(top_srcdir)/include/copyright.def 		\
		$(top_srcdir)/include/homerc.def 		\
		$(top_srcdir)/include/version.def

$(srcdir)/clktest-opts.h: $(srcdir)/clktest-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/clktest-opts.c: $(srcdir)/clktest-opts.def $(std_def_list)
	$(run_ag) clktest-opts.def

## chutest$(EXEEXT): ../libntp/libntp.a
## clktest$(EXEEXT): ../libntp/libntp.a

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

CLEANFILES =
include $(top_srcdir)/depsver.mf
