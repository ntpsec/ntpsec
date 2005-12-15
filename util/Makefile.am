#AUTOMAKE_OPTIONS = ../ansi2knr no-dependencies
AUTOMAKE_OPTIONS=	ansi2knr

bin_PROGRAMS=	@MAKE_NTPTIME@ @MAKE_TICKADJ@ @MAKE_TIMETRIM@ \
		ntp-keygen
EXTRA_PROGRAMS=	audio-pcm byteorder hist jitter kern longsize \
	ntptime pps-api precision sht testrs6000 tg tickadj timetrim

AM_CPPFLAGS=	-I$(top_srcdir)/include $(LIBOPTS_CFLAGS)
# LDADD might need RESLIB and ADJLIB
LDADD=		../libntp/libntp.a
ntp_keygen_SOURCES = ntp-keygen.c ntp-keygen-opts.c ntp-keygen-opts.h
ntp_keygen_LDADD=	@LCRYPTO@ $(LIBOPTS_LDADD) ../libntp/libntp.a

ETAGS_ARGS=	Makefile.am
#EXTRA_DIST=	README TAGS
EXTRA_DIST=	ntp-keygen-opts.def ntp-keygen.1 ntp-keygen-opts.texi ntp-keygen-opts.menu
BUILT_SOURCES=	ntp-keygen-opts.c ntp-keygen-opts.h ntp-keygen.1	\
		ntp-keygen-opts.texi ntp-keygen-opts.menu
man_MANS=	ntp-keygen.1

$(srcdir)/ntp-keygen-opts.h: ntp-keygen-opts.c
$(srcdir)/ntp-keygen-opts.c: ntp-keygen-opts.def
	cd $(srcdir) && autogen ntp-keygen-opts.def

$(srcdir)/ntp-keygen.1: ntp-keygen-opts.def
	cd $(srcdir) && autogen -Tagman1.tpl -bntp-keygen ntp-keygen-opts.def

$(srcdir)/ntp-keygen-opts.texi $(srcdir)/ntp-keygen-opts.menu: ntp-keygen-opts.def
	cd $(srcdir) && autogen -L ${prefix}/share/autogen \
		-Taginfo.tpl -DLEVEL=section ntp-keygen-opts.def


jitter_SOURCES=	jitter.c jitter.h
jitter_LDADD=

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

kern.o: kern.c
	$(COMPILE) -DHAVE_TIMEX_H -c kern.c
