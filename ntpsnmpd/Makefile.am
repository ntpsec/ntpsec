NULL=

bin_PROGRAMS=	$(NTPSNMPD_DB)
libexec_PROGRAMS=	$(NTPSNMPD_DL)
sbin_PROGRAMS=	$(NTPSNMPD_DS)

EXTRA_PROGRAMS=	ntpsnmpd
ntpsnmpd_SOURCES= netsnmp_daemonize.c ntpsnmpd.c ntpSnmpSubagentObject.c \
		ntpsnmpd-opts.c ntpsnmpd-opts.h ntpSnmpSubagentObject.h \
		ntp_snmp.h
# HMS: we probably want a version.o file here, too.
LDADD  = ../ntpq/libntpq.a ../libntp/libntp.a $(LDADD_LIBNTP)
LDADD += $(PTHREAD_LIBS) $(LCRYPTO) $(SNMP_LIBS) $(LIBOPTS_LDADD)

AM_CFLAGS = $(SNMP_CFLAGS) $(CFLAGS_NTP)

AM_CPPFLAGS  = -I$(top_srcdir)/ntpq
AM_CPPFLAGS += $(NTP_INCS)
AM_CPPFLAGS += $(LIBOPTS_CFLAGS)
AM_CPPFLAGS += $(SNMP_CPPFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)

EXTRA_DIST=			\
	ntpsnmpd-opts.def	\
	ntpsnmpd-opts.menu	\
	ntpsnmpd-opts.texi	\
	ntpsnmpd.1ntpsnmpdman	\
	ntpsnmpd.1ntpsnmpdmdoc	\
	ntpsnmpd.man.in		\
	ntpsnmpd.mdoc.in	\
	ntpv4-mib.mib		\
	$(NULL)

BUILT_SOURCES=	ntpsnmpd-opts.c ntpsnmpd-opts.h
CLEANFILES=
DISTCLEANFILES=	config.log $(man_MANS)
noinst_DATA=				\
	$(srcdir)/ntpsnmpd-opts.menu	\
	$(srcdir)/ntpsnmpd-opts.texi	\
	$(srcdir)/ntpsnmpd.man.in	\
	$(srcdir)/ntpsnmpd.mdoc.in	\
	$(NULL)

man1_MANS=
man8_MANS=
man_MANS=	ntpsnmpd.$(NTPSNMPD_MS)

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

$(srcdir)/ntpsnmpd-opts.h: $(srcdir)/ntpsnmpd-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/ntpsnmpd-opts.c: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) ntpsnmpd-opts.def

###

$(srcdir)/ntpsnmpd.1ntpsnmpdman: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpsnmpdman -Tagman-cmd.tpl ntpsnmpd-opts.def

$(srcdir)/ntpsnmpd.man.in: $(srcdir)/ntpsnmpd.1ntpsnmpdman $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntpsnmpd.1ntpsnmpdman > $(srcdir)/ntpsnmpd.man.in+
	mv $(srcdir)/ntpsnmpd.man.in+ $(srcdir)/ntpsnmpd.man.in

###

$(srcdir)/ntpsnmpd.1ntpsnmpdmdoc: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpsnmpdmdoc -Tagmdoc-cmd.tpl ntpsnmpd-opts.def

$(srcdir)/ntpsnmpd.mdoc.in: $(srcdir)/ntpsnmpd.1ntpsnmpdmdoc $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntpsnmpd.1ntpsnmpdmdoc > $(srcdir)/ntpsnmpd.mdoc.in+
	mv $(srcdir)/ntpsnmpd.mdoc.in+ $(srcdir)/ntpsnmpd.mdoc.in

###

ntpsnmpd.$(NTPSNMPD_MS): $(srcdir)/ntpsnmpd.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=ntpsnmpd.$(NTPSNMPD_MS)+:$(srcdir)/ntpsnmpd.$(MANTAGFMT).in
	mv ntpsnmpd.$(NTPSNMPD_MS)+ ntpsnmpd.$(NTPSNMPD_MS)

###

$(srcdir)/ntpsnmpd-opts.menu: $(srcdir)/ntpsnmpd-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/ntpsnmpd-opts.texi: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntpsnmpd-opts.def
	$(top_srcdir)/scripts/check--help $@

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/sntp/check-libntp.mf
include $(top_srcdir)/check-libopts.mf
include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
