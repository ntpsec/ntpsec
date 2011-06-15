NULL=

man_MANS=	$(srcdir)/ntp-wait.1
run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../sntp/include -L ../sntp/ag-tpl --writable
std_def_list =							\
	$(top_srcdir)/sntp/include/debug-opt.def		\
	$(top_srcdir)/sntp/include/autogen-version.def 		\
	$(top_srcdir)/sntp/include/copyright.def 		\
	$(top_srcdir)/sntp/include/homerc.def 			\
	$(top_srcdir)/sntp/include/ntp.lic 			\
	$(top_srcdir)/sntp/include/version.def			\
	$(NULL)

if NTP_BINSUBDIR_IS_BIN
bin_SCRIPTS =	ntp-wait ntptrace
else
sbin_SCRIPTS =	ntp-wait ntptrace
endif

noinst_SCRIPTS =		\
	calc_tickadj		\
	checktime		\
	freq_adj		\
	html2man		\
	mkver			\
	ntpsweep		\
	ntpver			\
	plot_summary		\
	summary			\
	$(NULL)

noinst_DATA=				\
	$(srcdir)/ntp-wait.html		\
	$(srcdir)/ntp-wait-opts.texi	\
	$(srcdir)/ntp-wait-opts.menu	\
	$(NULL)

EXTRA_DIST =			\
	check--help		\
	checkChangeLog		\
	cvo.sh			\
	fixautomakedepsmagic	\
	genCommitLog		\
	genver			\
	hpadjtime.sh		\
	monitoring		\
	ntp-close		\
	ntp-groper		\
	ntp-restart		\
	ntp-status		\
	ntp-wait.1		\
	ntp-wait.html		\
	ntp-wait.texi		\
	ntp-wait-opts.def	\
	ntp-wait-opts.menu	\
	ntp-wait-opts.texi	\
	rc1			\
	rc2			\
	stats			\
	UpdatePoint		\
	VersionName		\
	$(NULL)

# For man pages, use agman-cmd.tpl
# For mdoc pages, use agmdoc-cmd.tpl
$(srcdir)/ntp-wait.1: $(srcdir)/ntp-wait-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1 -Tagmdoc-cmd.tpl ntp-wait-opts.def

$(srcdir)/ntp-wait-opts.menu: $(srcdir)/ntp-wait-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi

$(srcdir)/ntp-wait-opts.texi: $(srcdir)/ntp-wait-opts.def $(std_def_list)
	$(run_ag) -Taginfo.tpl -DLEVEL=section ntp-wait-opts.def
	-$(top_srcdir)/../scripts/check--help $@

$(srcdir)/ntp-wait.html: $(srcdir)/ntp-wait-opts.menu $(srcdir)/ntp-wait-opts.texi $(srcdir)/ntp-wait.texi $(top_srcdir)/sntp/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -o ntp-wait.html ntp-wait.texi || true )

