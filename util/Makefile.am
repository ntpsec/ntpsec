NULL=

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	@MAKE_NTPTIME@ @MAKE_TICKADJ@ @MAKE_TIMETRIM@ \
		ntp-keygen
else
sbin_PROGRAMS=	@MAKE_NTPTIME@ @MAKE_TICKADJ@ @MAKE_TIMETRIM@ \
		ntp-keygen
endif

EXTRA_PROGRAMS=	audio-pcm byteorder hist jitter kern longsize \
	ntptime pps-api precision sht testrs6000 tg tg2 tickadj timetrim

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

# LDADD might need RESLIB and ADJLIB
LDADD=		../libntp/libntp.a $(LDADD_LIBNTP) $(PTHREAD_LIBS)
tg2_LDADD=	$(LIBM)
ntp_keygen_LDADD  = version.o $(LIBOPTS_LDADD) ../libntp/libntp.a
ntp_keygen_LDADD += $(LDADD_LIBNTP) $(PTHREAD_LIBS) $(LCRYPTO)
ntp_keygen_SOURCES = ntp-keygen.c ntp-keygen-opts.c ntp-keygen-opts.h

EXTRA_DIST=	ntp-keygen-opts.def ntp-keygen.1 ntp-keygen-opts.texi ntp-keygen-opts.menu
BUILT_SOURCES=	ntp-keygen-opts.c ntp-keygen-opts.h
CLEANFILES=
DISTCLEANFILES=	.version version.c
noinst_DATA=	$(srcdir)/ntp-keygen-opts.texi $(srcdir)/ntp-keygen-opts.menu
man_MANS=	$(srcdir)/ntp-keygen.1
run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../sntp/include --writable
std_def_list =							\
	$(top_srcdir)/sntp/include/debug-opt.def		\
	$(top_srcdir)/sntp/include/autogen-version.def 		\
	$(top_srcdir)/sntp/include/copyright.def 		\
	$(top_srcdir)/sntp/include/homerc.def 			\
	$(top_srcdir)/sntp/include/version.def			\
	$(NULL)

$(srcdir)/ntp-keygen-opts.h: $(srcdir)/ntp-keygen-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/ntp-keygen-opts.c: $(srcdir)/ntp-keygen-opts.def $(std_def_list)
	$(run_ag) ntp-keygen-opts.def

$(srcdir)/ntp-keygen.1: $(srcdir)/ntp-keygen-opts.def $(std_def_list)
	$(run_ag) -Tagman1.tpl -bntp-keygen ntp-keygen-opts.def

$(srcdir)/ntp-keygen-opts.menu: $(srcdir)/ntp-keygen-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/ntp-keygen-opts.texi: $(srcdir)/ntp-keygen-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntp-keygen-opts.def
	$(top_srcdir)/scripts/check--help $@


jitter_SOURCES=	jitter.c jitter.h
jitter_LDADD=

kern.o: kern.c
	$(COMPILE) -DHAVE_TIMEX_H -c kern.c

$(PROGRAMS): version.o

$(top_srcdir)/sntp/scm-rev:
	cd $(top_srcdir)/sntp && $(MAKE) check-scm-rev

version.c: $(ntp_keygen_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/sntp/scm-rev
	env CSET=`cat $(top_srcdir)/sntp/scm-rev` $(top_builddir)/scripts/mkver ntp-keygen
	
version.o: version.c
	env CCACHE_DISABLE=1 $(COMPILE) -c version.c -o version.o

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/check-libopts.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
