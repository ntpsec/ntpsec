NULL=

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	ntpq
else
sbin_PROGRAMS=	ntpq
endif

AM_CFLAGS = $(CFLAGS_NTP)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

# LDADD might need RESLIB and ADJLIB
ntpq_LDADD  = version.o $(LIBOPTS_LDADD) $(LIBM) ../libntp/libntp.a
ntpq_LDADD += $(LDADD_LIBNTP) $(PTHREAD_LIBS) $(EDITLINE_LIBS)
ntpq_LDADD += $(LCRYPTO)
noinst_HEADERS=	ntpq.h
noinst_LIBRARIES=	libntpq.a
libntpq_a_CFLAGS=	-DNO_MAIN_ALLOWED -DBUILD_AS_LIB
CLEANFILES=
DISTCLEANFILES=	.version version.c
ETAGS_ARGS=	Makefile.am
EXTRA_DIST=	ntpq-opts.def ntpq.1 ntpq-opts.texi ntpq-opts.menu
BUILT_SOURCES=	ntpq-opts.c ntpq-opts.h
man_MANS=	$(srcdir)/ntpq.1
noinst_DATA=	$(srcdir)/ntpq-opts.texi $(srcdir)/ntpq-opts.menu
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
ntpq_SOURCES=	ntpq.c ntpq-subs.c ntpq-opts.c ntpq-opts.h

libntpq_a_SOURCES =	libntpq.c libntpq.h libntpq_subs.c 

$(srcdir)/ntpq-opts.h: $(srcdir)/ntpq-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/ntpq-opts.c: $(srcdir)/ntpq-opts.def $(std_def_list)
	$(run_ag) ntpq-opts.def

# For man pages, use agman-cmd.tpl
# For mdoc pages, use agmdoc-cmd.tpl
#$(srcdir)/sntp.1: $(srcdir)/sntp-opts.def $(srcdir)/include/version.def $(srcdir)/include/copyright.def
#	$(run_ag) -DMAN_SECTION=1 -Tagmdoc-cmd.tpl sntp-opts.def
###
$(srcdir)/ntpq.1: $(srcdir)/ntpq-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1 -Tagmdoc-cmd.tpl ntpq-opts.def

$(srcdir)/ntpq-opts.menu: $(srcdir)/ntpq-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/ntpq-opts.texi: $(srcdir)/ntpq-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntpq-opts.def
	$(top_srcdir)/scripts/check--help $@

$(PROGRAMS): version.o

$(top_srcdir)/sntp/scm-rev:
	cd $(top_srcdir)/sntp && $(MAKE) check-scm-rev

version.c: $(ntpq_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/sntp/scm-rev
	env CSET=`cat $(top_srcdir)/sntp/scm-rev` $(top_builddir)/scripts/mkver ntpq
	
version.o: version.c
	env CCACHE_DISABLE=1 $(COMPILE) -c version.c -o version.o

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/check-libopts.mf
include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
