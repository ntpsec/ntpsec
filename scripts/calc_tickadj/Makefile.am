run_ag=		cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L ../../sntp/include -L ../../sntp/ag-tpl \
		--writable

EXTRA_DIST = 	\
	$(srcdir)/calc_tickadj.in \
	$(srcdir)/calc_tickadj 	\
	$(srcdir)/calc_tickadj.1calc_tickadjman	\
	$(srcdir)/calc_tickadj.1calc_tickadjmdoc	\
	$(srcdir)/calc_tickadj.texi	\
	$(srcdir)/calc_tickadj.html	\
	$(srcdir)/calc_tickadj.1		\
	$(srcdir)/calc_tickadj-opts.def	\
	$(srcdir)/calc_tickadj-opts	\
	$(srcdir)/invoke-calc_tickadj.texi	\
	$(srcdir)/invoke-calc_tickadj.menu

noinst_SCRIPTS = calc_tickadj

noinst_DATA = \
	$(srcdir)/calc_tickadj.in \
	$(srcdir)/calc_tickadj.1calc_tickadjman	\
	$(srcdir)/calc_tickadj.1calc_tickadjmdoc	\
	$(srcdir)/calc_tickadj.texi	\
	$(srcdir)/calc_tickadj.html	\
	$(srcdir)/calc_tickadj.1		\
	$(srcdir)/calc_tickadj-opts.def	\
	$(srcdir)/calc_tickadj-opts	\
	$(srcdir)/invoke-calc_tickadj.texi	\
	$(srcdir)/invoke-calc_tickadj.menu

calc_tickadj: $(srcdir)/calc_tickadj-opts

$(srcdir)/calc_tickadj-opts: $(srcdir)/calc_tickadj-opts.def $(srcdir)/../../sntp/ag-tpl/perlopt.tpl
	$(run_ag) calc_tickadj-opts.def

clean-local: extra-clean

extra-clean:
	rm calc_tickadj
	rm *.html *man *mdoc *.1 invoke-calc_tickadj.* calc_tickadj.*.in

### Nroff

$(srcdir)/calc_tickadj.1calc_tickadjman: $(srcdir)/calc_tickadj-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1calc_tickadjman -Tagman-cmd.tpl calc_tickadj-opts.def

$(srcdir)/calc_tickadj.man.in: $(srcdir)/calc_tickadj.1calc_tickadjman $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/calc_tickadj.1calc_tickadjman > $(srcdir)/calc_tickadj.man.in+
	mv $(srcdir)/calc_tickadj.man.in+ $(srcdir)/calc_tickadj.man.in

### Mdoc

$(srcdir)/calc_tickadj.1calc_tickadjmdoc: $(srcdir)/calc_tickadj-opts.def $(std_def_list)
	$(run_ag) -DMAN_SECTION=1calc_tickadjmdoc -Tagmdoc-cmd.tpl calc_tickadj-opts.def

$(srcdir)/calc_tickadj.mdoc.in: $(srcdir)/calc_tickadj.1calc_tickadjmdoc $(top_srcdir)/sntp/scripts/mansec2subst.sed
	sed -f $(top_srcdir)/sntp/scripts/mansec2subst.sed $(srcdir)/calc_tickadj.1calc_tickadjmdoc > $(srcdir)/calc_tickadj.mdoc.in+
	mv $(srcdir)/calc_tickadj.mdoc.in+ $(srcdir)/calc_tickadj.mdoc.in

### Manpage

$(srcdir)/calc_tickadj.1: $(srcdir)/calc_tickadj.$(MANTAGFMT).in $(top_builddir)/config.status
	$(top_builddir)/config.status --file=calc_tickadj.1+:$(srcdir)/calc_tickadj.$(MANTAGFMT).in
	mv calc_tickadj.1+ calc_tickadj.1

### Texinfo

$(srcdir)/invoke-calc_tickadj.menu: $(srcdir)/invoke-calc_tickadj.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi

$(srcdir)/invoke-calc_tickadj.texi: calc_tickadj $(srcdir)/calc_tickadj-opts.def $(std_def_list)
	$(run_ag) -Tagtexi-cmd.tpl -DLEVEL=section calc_tickadj-opts.def
	$(top_srcdir)/scripts/build/check--help $@

### HTML

$(srcdir)/calc_tickadj.html: $(srcdir)/invoke-calc_tickadj.menu $(srcdir)/invoke-calc_tickadj.texi $(srcdir)/calc_tickadj.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -I ../sntp -o calc_tickadj.html calc_tickadj.texi || true )
