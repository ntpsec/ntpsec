NULL =

EXTRA_DIST =			\
	autogen-version.def	\
	copyright.def		\
	debug-opt.def		\
	homerc.def		\
	mansec2subst.sed	\
	ntp.lic			\
	version.def		\
	version.texi		\
	$(NULL)

noinst_HEADERS =

$(srcdir)/version.def: $(srcdir)/../../packageinfo.sh
	cd .. && $(MAKE) $(abs_top_srcdir)/include/version.def

$(srcdir)/version.texi: $(srcdir)/../../packageinfo.sh
	cd .. && $(MAKE) $(abs_top_srcdir)/include/version.texi
