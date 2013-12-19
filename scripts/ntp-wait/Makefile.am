NULL=

bin_SCRIPTS =	$(NTP_WAIT_DB)
libexec_SCRIPTS =	$(NTP_WAIT_DL)
sbin_SCRIPTS =	$(NTP_WAIT_DS)

man_MANS=	ntp-wait.$(NTP_WAIT_MS)

run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)" PERL5LIB="$(abs_top_srcdir)/scripts/lib"	\
		autogen -L ../../sntp/include -L ../../sntp/ag-tpl \
		--writable

EXTRA_SCRIPTS = ntp-wait

EXTRA_DIST =					\
	$(srcdir)/ntp-wait.in			\
	$(srcdir)/ntp-wait.1ntp-waitman		\
	$(srcdir)/ntp-wait.1ntp-waitmdoc	\
	$(srcdir)/ntp-wait.texi			\
	$(srcdir)/ntp-wait.html			\
	$(srcdir)/ntp-wait-opts.def		\
	$(srcdir)/ntp-wait-opts			\
	$(srcdir)/invoke-ntp-wait.texi		\
	$(srcdir)/invoke-ntp-wait.menu		\
	$(NULL)

html_DATA = $(srcdir)/ntp-wait.html

noinst_DATA =					\
	$(srcdir)/ntp-wait.in 			\
	$(srcdir)/ntp-wait.1ntp-waitman		\
	$(srcdir)/ntp-wait.1ntp-waitmdoc	\
	$(srcdir)/ntp-wait.texi			\
	$(srcdir)/ntp-wait.html			\
	$(srcdir)/ntp-wait-opts.def		\
	$(srcdir)/ntp-wait-opts			\
	$(srcdir)/invoke-ntp-wait.texi		\
	$(srcdir)/invoke-ntp-wait.menu		\
	$(NULL)

ntp-wait: $(srcdir)/ntp-wait-opts

$(srcdir)/ntp-wait-opts: $(srcdir)/ntp-wait-opts.def ../../sntp/ag-tpl/perlopt.tpl
	$(run_ag) ntp-wait-opts.def

clean-local: extra-clean

extra-clean:
	rm ntp-wait
	rm *.html *man *mdoc *.1 invoke-ntp-wait.* ntp-wait.man.in

### Nroff

$(srcdir)/ntp-wait.1ntp-waitman: $(srcdir)/ntp-wait-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntp-waitman -Tagman-cmd.tpl ntp-wait-opts.def

$(srcdir)/ntp-wait.man.in: $(srcdir)/ntp-wait.1ntp-waitman $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntp-wait.1ntp-waitman > $(srcdir)/ntp-wait.man.in+
	mv $(srcdir)/ntp-wait.man.in+ $(srcdir)/ntp-wait.man.in

### Mdoc

$(srcdir)/ntp-wait.1ntp-waitmdoc: $(srcdir)/ntp-wait-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntp-waitmdoc -Tagmdoc-cmd.tpl ntp-wait-opts.def

$(srcdir)/ntp-wait.mdoc.in: $(srcdir)/ntp-wait.1ntp-waitmdoc $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntp-wait.1ntp-waitmdoc > $(srcdir)/ntp-wait.mdoc.in+
	mv $(srcdir)/ntp-wait.mdoc.in+ $(srcdir)/ntp-wait.mdoc.in

### Manpage

ntp-wait.$(NTP_WAIT_MS): $(srcdir)/ntp-wait.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=ntp-wait.$(NTP_WAIT_MS)+:$(srcdir)/ntp-wait.$(MANTAGFMT).in
	mv ntp-wait.$(NTP_WAIT_MS)+ ntp-wait.$(NTP_WAIT_MS)

### Texinfo

$(srcdir)/invoke-ntp-wait.menu: $(srcdir)/invoke-ntp-wait.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi

$(srcdir)/invoke-ntp-wait.texi: ntp-wait $(srcdir)/ntp-wait-opts.def $(std_def_list)
	$(run_ag) -Tagtexi-cmd.tpl -DLEVEL=section ntp-wait-opts.def
	$(top_srcdir)/scripts/build/check--help $@

### HTML

$(srcdir)/ntp-wait.html: $(srcdir)/invoke-ntp-wait.menu $(srcdir)/invoke-ntp-wait.texi $(srcdir)/ntp-wait.texi $(top_srcdir)/sntp/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -I ../sntp -o ntp-wait.html ntp-wait.texi || true )
