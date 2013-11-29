man_MANS=	ntptrace.$(NTPTRACE_MS)

bin_SCRIPTS =	$(NTPTRACE_DB)
libexec_SCRIPTS =	$(NTPTRACE_DL)
sbin_SCRIPTS =	$(NTPTRACE_DS)

run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../../sntp/include -L ../../sntp/ag-tpl \
		--writable

EXTRA_SCRIPTS = ntptrace

EXTRA_DIST = 	\
	$(srcdir)/ntptrace.in \
	$(srcdir)/ntptrace.1ntptraceman	\
	$(srcdir)/ntptrace.1ntptracemdoc	\
	$(srcdir)/ntptrace.texi	\
	$(srcdir)/ntptrace.html	\
	$(srcdir)/ntptrace.1		\
	$(srcdir)/ntptrace-opts.def	\
	$(srcdir)/ntptrace-opts	\
	$(srcdir)/invoke-ntptrace.texi	\
	$(srcdir)/invoke-ntptrace.menu

html_DATA = $(srcdir)/ntptrace.html

noinst_DATA = \
	$(srcdir)/ntptrace.in \
	$(srcdir)/ntptrace.1ntptraceman	\
	$(srcdir)/ntptrace.1ntptracemdoc	\
	$(srcdir)/ntptrace.texi	\
	$(srcdir)/ntptrace.html	\
	$(srcdir)/ntptrace.1		\
	$(srcdir)/ntptrace-opts.def	\
	$(srcdir)/ntptrace-opts	\
	$(srcdir)/invoke-ntptrace.texi	\
	$(srcdir)/invoke-ntptrace.menu

ntptrace: $(srcdir)/ntptrace-opts

$(srcdir)/ntptrace-opts: $(srcdir)/ntptrace-opts.def ../../sntp/ag-tpl/perlopt.tpl
	$(run_ag) ntptrace-opts.def

clean-local: extra-clean

extra-clean:
	rm ntptrace
	rm *.html *man *mdoc *.1 invoke-ntptrace.* ntptrace.man.in

### Nroff

$(srcdir)/ntptrace.1ntptraceman: $(srcdir)/ntptrace-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntptraceman -Tagman-cmd.tpl ntptrace-opts.def

$(srcdir)/ntptrace.man.in: $(srcdir)/ntptrace.1ntptraceman $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntptrace.1ntptraceman > $(srcdir)/ntptrace.man.in+
	mv $(srcdir)/ntptrace.man.in+ $(srcdir)/ntptrace.man.in

### Mdoc

$(srcdir)/ntptrace.1ntptracemdoc: $(srcdir)/ntptrace-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntptracemdoc -Tagmdoc-cmd.tpl ntptrace-opts.def

$(srcdir)/ntptrace.mdoc.in: $(srcdir)/ntptrace.1ntptracemdoc $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntptrace.1ntptracemdoc > $(srcdir)/ntptrace.mdoc.in+
	mv $(srcdir)/ntptrace.mdoc.in+ $(srcdir)/ntptrace.mdoc.in

### Manpage

$(srcdir)/ntptrace.1: $(srcdir)/ntptrace.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=ntptrace.1+:$(srcdir)/ntptrace.$(MANTAGFMT).in
	mv ntptrace.1+ ntptrace.1

### Texinfo

$(srcdir)/invoke-ntptrace.menu: $(srcdir)/invoke-ntptrace.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi

$(srcdir)/invoke-ntptrace.texi: ntptrace $(srcdir)/ntptrace-opts.def $(std_def_list)
	$(run_ag) -Tagtexi-cmd.tpl -DLEVEL=section ntptrace-opts.def
	$(top_srcdir)/scripts/build/check--help $@

### HTML

$(srcdir)/ntptrace.html: $(srcdir)/invoke-ntptrace.menu $(srcdir)/invoke-ntptrace.texi $(srcdir)/ntptrace.texi $(top_srcdir)/sntp/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -I ../sntp -o ntptrace.html ntptrace.texi || true )
