#AUTOMAKE_OPTIONS = ../util/ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ../util/ansi2knr
bin_PROGRAMS = ntpdc
EXTRA_PROGRAMS = ntpdc-layout
INCLUDES = -I$(top_srcdir)/include
# LDADD might need RESLIB and ADJLIB
LDADD =	version.o ../libntp/libntp.a @READLINE_LIBS@
# ntpdc-layout doesn't need any additional libraries at all
ntpdc_layout_LDADD =
DISTCLEANFILES = .version version.c
noinst_HEADERS = ntpdc.h
EXTRA_DIST = nl_in.c nl.pl
ETAGS_ARGS = Makefile.am

ntpdc_SOURCES = ntpdc.c ntpdc_ops.c

ntpdc-layout.o: nl.c

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

version.o: $(ntpdc_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/version
	env CSET=`cat $(top_srcdir)/version` $(top_builddir)/scripts/mkver ntpdc
	$(COMPILE) -c version.c

# I ran nl_in.c (attached, to be installed into ntpdc) through
# $(CC) -E nl_in.c | nl.pl > nl.c
nl.c: nl_in.c nl.pl
	$(CC) -E $(DEFAULT_INCLUDES) $(INCLUDES) $(srcdir)/nl_in.c | nl.pl > nl.c
