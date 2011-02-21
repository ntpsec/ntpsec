NULL=
AUTOMAKE_OPTIONS= 

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS=	@MAKE_NTPSNMPD@
else
sbin_PROGRAMS=	@MAKE_NTPSNMPD@
endif

EXTRA_PROGRAMS=	ntpsnmpd
ntpsnmpd_SOURCES= netsnmp_daemonize.c ntpsnmpd.c ntpSnmpSubagentObject.c \
		ntpsnmpd-opts.c ntpsnmpd-opts.h ntpSnmpSubagentObject.h \
		ntp_snmp.h
# HMS: we probably want a version.o file here, too.
LDADD  = ../ntpq/libntpq.a ../libntp/libntp.a @LDADD_LIBNTP@ @PTHREAD_LIBS@ @LCRYPTO@
LDADD += @SNMP_LIBS@ $(LIBOPTS_LDADD)

INCLUDES  = -I$(top_srcdir)/ntpq
INCLUDES += -I$(top_srcdir)/include
INCLUDES += -I$(top_srcdir)/lib/isc/include
INCLUDES += -I$(top_srcdir)/lib/isc/@LIBISC_PTHREADS_NOTHREADS@/include
INCLUDES += -I$(top_srcdir)/lib/isc/unix/include
INCLUDES += $(LIBOPTS_CFLAGS)
INCLUDES += @SNMP_CPPFLAGS@

AM_CFLAGS =	@SNMP_CFLAGS@ @CFLAGS_NTP@
AM_CPPFLAGS =	@CPPFLAGS_NTP@
EXTRA_DIST=	ntpsnmpd-opts.def ntpsnmpd.1 ntpsnmpd-opts.texi	\
		ntpsnmpd-opts.menu ntpv4-mib.mib
BUILT_SOURCES=	check-libntp ntpsnmpd-opts.c ntpsnmpd-opts.h
CLEANFILES=
noinst_DATA=	$(srcdir)/ntpsnmpd-opts.texi $(srcdir)/ntpsnmpd-opts.menu
man_MANS=	$(srcdir)/ntpsnmpd.1
run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../sntp/include --writable
std_def_list =							\
	$(top_srcdir)/sntp/include/debug-opt.def		\
	$(top_srcdir)/sntp/include/autogen-version.def 		\
	$(top_srcdir)/sntp/include/copyright.def 		\
	$(top_srcdir)/sntp/include/homerc.def 			\
	$(top_srcdir)/sntp/include/version.def			\
	$(NULL)

$(srcdir)/ntpsnmpd-opts.h: $(srcdir)/ntpsnmpd-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/ntpsnmpd-opts.c: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) ntpsnmpd-opts.def

$(srcdir)/ntpsnmpd.1: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) -Tagman1.tpl -bntpsnmpd ntpsnmpd-opts.def

$(srcdir)/ntpsnmpd-opts.menu: $(srcdir)/ntpsnmpd-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/ntpsnmpd-opts.texi: $(srcdir)/ntpsnmpd-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntpsnmpd-opts.def
	$(top_srcdir)/scripts/check--help $@

check-libntp: FRC
	cd ../libntp && $(MAKE)

FRC:

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/depsver.mf
