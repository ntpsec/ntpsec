NULL=

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	ntpdc
else
sbin_PROGRAMS=	ntpdc
endif

EXTRA_PROGRAMS=	ntpdc-layout
EXTRA_DATA=	check-layout
BUILT_SOURCES=	@MAKE_CHECK_LAYOUT@ ntpdc-opts.c ntpdc-opts.h

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

# LDADD might need RESLIB and ADJLIB
ntpdc_LDADD  = version.o $(LIBOPTS_LDADD) ../libntp/libntp.a
ntpdc_LDADD += $(LDADD_LIBNTP) $(PTHREAD_LIBS) $(EDITLINE_LIBS)
ntpdc_LDADD += $(LCRYPTO)
# ntpdc-layout doesn't need any additional libraries at all
ntpdc_layout_LDADD=

DISTCLEANFILES=	.version version.c
CLEANFILES=	check-layout layout.here nl.c ntpdc-layout
noinst_HEADERS=	ntpdc.h
ETAGS_ARGS= 	Makefile.am
EXTRA_DIST=			\
	nl_in.c			\
	nl.pl			\
	layout.std		\
	ntpdc-opts.def		\
	ntpdc.1			\
	ntpdc.html		\
	ntpdc.texi		\
	ntpdc-opts.texi		\
	ntpdc-opts.menu		\
	$(NULL)
man_MANS=	$(srcdir)/ntpdc.1

## HMS: Real Soon Now...
##info_TEXINFOS=	ntpdc.texi
##ntpdc_TEXINFOS=	ntpdc-opts.texi

noinst_DATA=				\
	$(srcdir)/ntpdc.html		\
	$(srcdir)/ntpdc-opts.texi	\
	$(srcdir)/ntpdc-opts.menu	\
	$(NULL)

run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../sntp/include -L ../sntp/ag-tpl --writable

std_def_list =							\
	$(top_srcdir)/sntp/include/debug-opt.def		\
	$(top_srcdir)/sntp/include/autogen-version.def 		\
	$(top_srcdir)/sntp/include/copyright.def 		\
	$(top_srcdir)/sntp/include/homerc.def 			\
	$(top_srcdir)/sntp/include/ntp.lic			\
	$(top_srcdir)/sntp/include/version.def			\
	$(NULL)

$(srcdir)/ntpdc-opts.h: $(srcdir)/ntpdc-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/ntpdc-opts.c: $(srcdir)/ntpdc-opts.def $(std_def_list)
	$(run_ag) ntpdc-opts.def

# For man pages, use agman-cmd.tpl
# For mdoc pages, use agmdoc-cmd.tpl
#$(srcdir)/sntp.1: $(srcdir)/sntp-opts.def $(srcdir)/include/version.def $(srcdir)/include/copyright.def
#	$(run_ag) -DMAN_SECTION=1 -Tagmdoc-cmd.tpl sntp-opts.def
###
$(srcdir)/ntpdc.1: $(srcdir)/ntpdc-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1 -Tagmdoc-cmd.tpl ntpdc-opts.def

$(srcdir)/ntpdc-opts.menu: $(srcdir)/ntpdc-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/ntpdc-opts.texi: $(srcdir)/ntpdc-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntpdc-opts.def
	$(top_srcdir)/scripts/check--help $@

$(srcdir)/ntpdc.html: $(srcdir)/ntpdc-opts.menu $(srcdir)/ntpdc-opts.texi $(srcdir)/ntpdc.texi $(top_srcdir)/sntp/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -I ../sntp -o ntpdc.html ntpdc.texi || true )

ntpdc_SOURCES = ntpdc.c ntpdc_ops.c ntpdc-opts.c ntpdc-opts.h

ntpdc-layout.o: nl.c

layout.here: ntpdc-layout
	./ntpdc-layout > $@

check-layout: ntpdc-layout $(srcdir)/layout.std layout.here
	cmp $(srcdir)/layout.std layout.here && echo stamp > $@

$(PROGRAMS): version.o

$(top_srcdir)/sntp/scm-rev:
	cd $(top_srcdir)/sntp && $(MAKE) check-scm-rev

version.c: $(ntpdc_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/sntp/scm-rev
	env CSET=`cat $(top_srcdir)/sntp/scm-rev` $(top_builddir)/scripts/mkver ntpdc
	
version.o: version.c
	env CCACHE_DISABLE=1 $(COMPILE) -c version.c -o version.o

# I ran nl_in.c (attached, to be installed into ntpdc) through
# $(CC) -E nl_in.c | nl.pl > nl.c
nl.c: nl_in.c nl.pl Makefile.am ../config.h ../include/ntp_request.h ntpdc.h
	$(CC) -E $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(srcdir)/nl_in.c | ./nl.pl > nl.c

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/check-libopts.mf
include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
