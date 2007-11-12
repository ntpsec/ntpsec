AUTOMAKE_OPTIONS= 

bindir=	${exec_prefix}/${BINSUBDIR}

bin_PROGRAMS=	ntpd @MAKE_NTPDSIM@

noinst_LIBRARIES=	libntpd.a

AM_CPPFLAGS= -I$(top_srcdir)/include -I../include $(LIBOPTS_CFLAGS)
# LDADD might need RESLIB and ADJLIB.
LDADD=	version.o libntpd.a @LIBPARSE@
AM_YFLAGS=	-d -t

if VPATH_HACK
VPHACK=	vphack
else
VPHACK=
endif

vphack:
	test -e ntp_parser.c || ln -s $(srcdir)/ntp_parser.c .
	test -e ntp_parser.h || ln -s $(srcdir)/ntp_parser.h .

BUILT_SOURCES=	${VPHACK} ntp_parser.c ntp_parser.h	\
		ntpd-opts.c ntpd-opts.h ntpd.1 ntpd-opts.texi ntpd-opts.menu

man_MANS=	ntpd.1

# What about ntpdsim.1?

# ntpd may need:
# log10                               refclock_wwv.o
# sqrt                                ntp_control.o
# floor                               refclock_wwv.o
# which are (usually) provided by -lm.
ntpd_LDADD = $(LDADD) -lm @LCRYPTO@ @LSCF@ $(LIBOPTS_LDADD) ../libntp/libntp.a
ntpdsim_LDADD = $(LDADD) -lm @LCRYPTO@ @LSCF@ $(LIBOPTS_LDADD) ../libntp/libntpsim.a
ntpdsim_CFLAGS = $(CFLAGS) -DSIM
check_y2k_LDADD = $(LDADD) ../libntp/libntp.a
DISTCLEANFILES = .version version.c
EXTRA_DIST = ntpd-opts.def ntpdbase-opts.def \
	refclock_msfees.c	\
	refclock_trak.c		\
	$(BUILT_SOURCES)
ETAGS_ARGS = Makefile.am
###							Y2Kfixes
check_PROGRAMS = @MAKE_CHECK_Y2K@
EXTRA_PROGRAMS = check_y2k ntpdsim
run_ag=		cd $(srcdir) && autogen -L ../include --writable
std_def_list=	$(top_srcdir)/include/debug-opt.def		\
		$(top_srcdir)/include/autogen-version.def 	\
		$(top_srcdir)/include/copyright.def 		\
		$(top_srcdir)/include/version.def

check-local: @MAKE_CHECK_Y2K@
	test -z "@MAKE_CHECK_Y2K@" || ./@MAKE_CHECK_Y2K@

# SIM: cmd_args.c ntp_config.c ntp_io.c ntpd.c + ntpsim.c (include/ntpsim.h)
# ntp_resolver.c is presently unused...
ntpd_SOURCES = cmd_args.c ntp_config.c ntp_io.c ntp_parser.y	\
	ntp_data_structures.c ntp_scanner.c ntp_scanner.h ntpd.c \
	ntpd-opts.c ntpd-opts.h
ntpdsim_SOURCES = $(ntpd_SOURCES) ntpsim.c
libntpd_a_SOURCES = jupiter.h ntp_control.c \
	ntp_crypto.c ntp_filegen.c \
	ntp_intres.c ntp_loopfilter.c ntp_monitor.c ntp_peer.c \
	ntp_proto.c ntp_refclock.c ntp_request.c \
	ntp_restrict.c ntp_timer.c ntp_util.c \
	ppsapi_timepps.h \
	refclock_acts.c refclock_arbiter.c refclock_arc.c refclock_as2201.c \
	refclock_atom.c refclock_bancomm.c refclock_chronolog.c \
	refclock_chu.c refclock_conf.c refclock_datum.c refclock_dumbclock.c \
	refclock_fg.c refclock_gpsvme.c refclock_heath.c refclock_hopfser.c \
	refclock_hopfpci.c refclock_hpgps.c refclock_irig.c refclock_jjy.c \
	refclock_jupiter.c refclock_leitch.c refclock_local.c \
	refclock_mx4200.c refclock_neoclock4x.c \
	refclock_nmea.c refclock_oncore.c refclock_palisade.c \
	refclock_palisade.h refclock_parse.c \
	refclock_pcf.c refclock_pst.c refclock_ripencc.c refclock_shm.c \
	refclock_tpro.c refclock_true.c refclock_tt560.c \
	refclock_ulink.c refclock_wwv.c refclock_wwvb.c \
	refclock_zyfer.c

$(srcdir)/ntpd-opts.h: $(srcdir)/ntpd-opts.c
$(srcdir)/ntpd-opts.c: $(srcdir)/ntpd-opts.def $(srcdir)/ntpdbase-opts.def $(std_def_list)
	$(run_ag) ntpd-opts.def

$(srcdir)/ntpd.1: $(srcdir)/ntpd-opts.def $(srcdir)/ntpdbase-opts.def $(std_def_list)
	$(run_ag) -Tagman1.tpl -bntpd ntpd-opts.def

$(srcdir)/ntpd-opts.texi $(srcdir)/ntpd-opts.menu: $(srcdir)/ntpd-opts.def $(srcdir)/ntpdbase-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntpd-opts.def

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE) libntp.a

../libparse/libparse.a:
	cd ../libparse && $(MAKE)

$(top_srcdir)/version :
	cd $(top_srcdir) && $(MAKE) version

version.o: $(ntpd_OBJECTS) ../libntp/libntp.a @LIBPARSE@ Makefile $(top_srcdir)/version
	env CSET=`cat $(top_srcdir)/version` $(top_builddir)/scripts/mkver ntpd
	$(COMPILE) -c version.c

include ../bincheck.mf
