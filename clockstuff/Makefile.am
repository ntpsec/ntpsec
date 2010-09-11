#AUTOMAKE_OPTIONS = ../ansi2knr no-dependencies
AUTOMAKE_OPTIONS = 
noinst_PROGRAMS = @PROPDELAY@ @CHUTEST@ @CLKTEST@
EXTRA_PROGRAMS = propdelay chutest clktest

INCLUDES = -I$(top_srcdir)/include
propdelay_LDADD = $(LIBM) ../libntp/libntp.a
chutest_LDADD = ../libntp/libntp.a
clktest_LDADD = ../libntp/libntp.a
ETAGS_ARGS = Makefile.am
#EXTRA_DIST = TAGS
BUILT_SOURCES =
CLEANFILES =

# clktest-opts.def wants ../include/copyright.def ../include/homerc.def

chutest$(EXEEXT): ../libntp/libntp.a

clktest$(EXEEXT): ../libntp/libntp.a

include $(top_srcdir)/depsver.mf
