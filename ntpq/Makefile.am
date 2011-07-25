NULL=

EXTRA_PROGRAMS=	ntpq
bin_PROGRAMS=	$(NTPQ_DB)
libexec_PROGRAMS=	$(NTPQ_DL)
sbin_PROGRAMS=	$(NTPQ_DS)

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
DISTCLEANFILES=	.version version.c config.log $(man_MANS)
ETAGS_ARGS=	Makefile.am
EXTRA_DIST=			\
	ntpq-opts.def		\
	ntpq-opts.menu		\
	ntpq-opts.texi		\
	ntpq.1ntpqman		\
	ntpq.1ntpqmdoc		\
	ntpq.man.in		\
	ntpq.mdoc.in		\
	$(NULL)

BUILT_SOURCES=	ntpq-opts.c ntpq-opts.h
noinst_DATA=				\
	$(srcdir)/ntpq-opts.menu	\
	$(srcdir)/ntpq-opts.texi	\
	$(srcdir)/ntpq.man.in		\
	$(srcdir)/ntpq.mdoc.in		\
	$(NULL)

man1_MANS=
man8_MANS=
man_MANS=	ntpq.$(NTPQ_MS)

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

###

$(srcdir)/ntpq.1ntpqman: $(srcdir)/ntpq-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpqman -Tagman-cmd.tpl ntpq-opts.def

$(srcdir)/ntpq.man.in: $(srcdir)/ntpq.1ntpqman $(top_srcdir)/sntp/include/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/include/mansec2subst.sed $(srcdir)/ntpq.1ntpqman > $(srcdir)/ntpq.man.in+
	mv $(srcdir)/ntpq.man.in+ $(srcdir)/ntpq.man.in

###

$(srcdir)/ntpq.1ntpqmdoc: $(srcdir)/ntpq-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpqmdoc -Tagmdoc-cmd.tpl ntpq-opts.def

$(srcdir)/ntpq.mdoc.in: $(srcdir)/ntpq.1ntpqmdoc $(top_srcdir)/sntp/include/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/include/mansec2subst.sed $(srcdir)/ntpq.1ntpqmdoc > $(srcdir)/ntpq.mdoc.in+
	mv $(srcdir)/ntpq.mdoc.in+ $(srcdir)/ntpq.mdoc.in

###

ntpq.$(NTPQ_MS): $(srcdir)/ntpq.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=ntpq.$(NTPQ_MS)+:$(srcdir)/ntpq.$(MANTAGFMT).in
	mv ntpq.$(NTPQ_MS)+ ntpq.$(NTPQ_MS)

###

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
