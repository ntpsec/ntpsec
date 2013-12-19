NULL=
SUBDIRS=		\
	build		\
	calc_tickadj	\
	ntp-wait	\
	ntpsweep	\
	ntptrace	\
	lib		\
	$(NULL)

man1_MANS=
man8_MANS=

DISTCLEANFILES=	config.log $(man_MANS)

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

noinst_SCRIPTS =		\
	ntpver			\
	plot_summary		\
	summary			\
	$(NULL)

install-data-local: install-html

EXTRA_DIST =			\
	deprecated		\
	monitoring		\
	rc			\
	stats			\
	$(NULL)

OPTTPL = ../sntp/ag-tpl/perlopt.tpl

##

plot_summary: plot_summary-opts

plot_summary-opts: plot_summary-opts.def $(OPTTPL)
	$(run_ag) plot_summary-opts.def 

##

summary: summary-opts

summary-opts: summary-opts.def $(OPTTPL)
	$(run_ag) summary-opts.def


