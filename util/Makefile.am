#AUTOMAKE_OPTIONS = ../ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ansi2knr
#bin_PROGRAMS = @MAKE_NTPTIME@ @MAKE_TICKADJ@ @MAKE_TIMETRIM@ ntp-genkeys
bin_PROGRAMS = @MAKE_NTPTIME@ @MAKE_TICKADJ@ @MAKE_TIMETRIM@ genkeys
EXTRA_PROGRAMS = audio-pcm byteorder hist jitter kern longsize ntptime \
precision sht testrs6000 tickadj timetrim

#ntp_genkeys_LDADD = ../ntpd/ntp_config$U.o ../libntp/libntp.a
#ntp_genkeys_DEPENDENCIES = ../ntpd/ntp_config$U.o ../libntp/libntp.a

#genkeys_LDADD = ../ntpd/ntp_config$U.o ../libntp/libntp.a
#genkeys_DEPENDENCIES = ../ntpd/ntp_config$U.o ../libntp/libntp.a

INCLUDES = -I$(top_srcdir)/include
# LDADD might need RESLIB and ADJLIB
LDADD =	../libntp/libntp.a
#EXTRA_DIST = README TAGS
EXTRA_DIST = 
ETAGS_ARGS = Makefile.am

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

kern.o: kern.c
	$(COMPILE) -DHAVE_TIMEX_H -c kern.c
