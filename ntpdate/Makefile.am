#AUTOMAKE_OPTIONS = ../util/ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ../util/ansi2knr
bin_PROGRAMS = ntpdate
INCLUDES = -I$(top_srcdir)/include
# LDADD might need RESLIB and ADJLIB
LDADD =	version.o ../libntp/libntp.a
DISTCLEANFILES = .version stamp-v
noinst_HEADERS = ntpdate.h
#EXTRA_DIST = ntpdate.mak
ETAGS_ARGS = Makefile.am

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

version.o: $(ntpdate_OBJECTS) ../libntp/libntp.a Makefile
	$(top_srcdir)/scripts/mkversion ntpdate @VERSION@
	$(COMPILE) -c version.c
