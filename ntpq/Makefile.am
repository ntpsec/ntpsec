#AUTOMAKE_OPTIONS = ../util/ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ../util/ansi2knr
bin_PROGRAMS = ntpq
INCLUDES = -I$(top_srcdir)/include
# LDADD might need RESLIB and ADJLIB
LDADD =	version.o ../libntp/libntp.a
DISTCLEANFILES = .version
noinst_HEADERS = ntpq.h
#EXTRA_DIST = ntpq.mak
ETAGS_ARGS = Makefile.am

ntpq_SOURCES = ntpq.c ntpq_ops.c

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

version.o: $(ntpq_OBJECTS) ../libntp/libntp.a Makefile
	$(top_srcdir)/scripts/mkversion ntpq @VERSION@
	$(COMPILE) -c version.c
