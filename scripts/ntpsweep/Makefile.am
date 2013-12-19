NULL=
run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)" PERL5LIB="$(abs_top_srcdir)/scripts/lib"	\
		autogen -L ../../sntp/include -L ../../sntp/ag-tpl \
		--writable

noinst_SCRIPTS = ntpsweep

EXTRA_DIST =					\
	$(srcdir)/ntpsweep.in			\
	$(srcdir)/ntpsweep.1ntpsweepman		\
	$(srcdir)/ntpsweep.1ntpsweepmdoc	\
	$(srcdir)/ntpsweep.texi			\
	$(srcdir)/ntpsweep.html			\
	$(srcdir)/ntpsweep-opts.def		\
	$(srcdir)/ntpsweep-opts			\
	$(srcdir)/invoke-ntpsweep.texi		\
	$(srcdir)/invoke-ntpsweep.menu		\
	$(NULL)

html_DATA = $(srcdir)/ntpsweep.html

noinst_DATA =					\
	$(srcdir)/ntpsweep.in			\
	$(srcdir)/ntpsweep.1ntpsweepman		\
	$(srcdir)/ntpsweep.1ntpsweepmdoc	\
	$(srcdir)/ntpsweep.texi			\
	$(srcdir)/ntpsweep.html			\
	$(srcdir)/ntpsweep-opts.def		\
	$(srcdir)/ntpsweep-opts			\
	$(srcdir)/invoke-ntpsweep.texi		\
	$(srcdir)/invoke-ntpsweep.menu		\
	$(NULL)

ntpsweep: $(srcdir)/ntpsweep-opts

$(srcdir)/ntpsweep-opts: $(srcdir)/ntpsweep-opts.def ../../sntp/ag-tpl/perlopt.tpl
	$(run_ag) ntpsweep-opts.def

clean-local: extra-clean

extra-clean:
	rm ntpsweep
	rm *.html *man *mdoc *.1 invoke-ntpsweep.* ntpsweep.man.in

### Nroff

$(srcdir)/ntpsweep.1ntpsweepman: $(srcdir)/ntpsweep-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpsweepman -Tagman-cmd.tpl ntpsweep-opts.def

$(srcdir)/ntpsweep.man.in: $(srcdir)/ntpsweep.1ntpsweepman $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntpsweep.1ntpsweepman > $(srcdir)/ntpsweep.man.in+
	mv $(srcdir)/ntpsweep.man.in+ $(srcdir)/ntpsweep.man.in

### Mdoc

$(srcdir)/ntpsweep.1ntpsweepmdoc: $(srcdir)/ntpsweep-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1ntpsweepmdoc -Tagmdoc-cmd.tpl ntpsweep-opts.def

$(srcdir)/ntpsweep.mdoc.in: $(srcdir)/ntpsweep.1ntpsweepmdoc $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/ntpsweep.1ntpsweepmdoc > $(srcdir)/ntpsweep.mdoc.in+
	mv $(srcdir)/ntpsweep.mdoc.in+ $(srcdir)/ntpsweep.mdoc.in

### Manpage

ntpsweep.1: $(srcdir)/ntpsweep.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=ntpsweep.1+:$(srcdir)/ntpsweep.$(MANTAGFMT).in
	mv ntpsweep.1+ ntpsweep.1

### Texinfo

$(srcdir)/invoke-ntpsweep.menu: $(srcdir)/invoke-ntpsweep.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi

$(srcdir)/invoke-ntpsweep.texi: ntpsweep $(srcdir)/ntpsweep-opts.def $(std_def_list)
	$(run_ag) -Tagtexi-cmd.tpl -DLEVEL=section ntpsweep-opts.def
	$(top_srcdir)/scripts/build/check--help $@

### HTML

$(srcdir)/ntpsweep.html: $(srcdir)/invoke-ntpsweep.menu $(srcdir)/invoke-ntpsweep.texi $(srcdir)/ntpsweep.texi $(top_srcdir)/sntp/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -I ../sntp -o ntpsweep.html ntpsweep.texi || true )
