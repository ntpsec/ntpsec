#
# libevent sources are in libevent/
# we need to install the built  stuff in builddir/sntp/_il/ (or something)
# make sure 'distcheck' still works...

NULL =

EXTRA_DIST =				\
	$(NULL)

DISTCLEANFILES=			\
	../_il			\
	_b			\
	libevent-check		\
	libevent-configure	\
	libevent-make		\
	local-install-libevent	\
	$(NULL)

BUILT_SOURCES=				\
	local-install-libevent		\
	$(NULL)

local-install-libevent: libevent-check
	( cd _b					\
	&& $(MAKE) install			\
	) && touch $@

libevent-check: libevent-make
	( cd _b					\
	&& $(MAKE) check			\
	) && touch $@

libevent-make: libevent-configure
	( cd _b					\
	&& $(MAKE)				\
	) && touch $@

libevent-configure: Makefile.am _b
	( cd _b					\
	&& ../$(srcdir)/../libevent/configure	\
		--prefix=$(abs_top_builddir)/_il \
		--disable-shared		\
		$(NULL)				\
	) && touch $@

_b:
	test -d _b || mkdir _b

clean-local:
	-( cd _b && make clean )

FRC:
	@: do-nothing action to prevent default SCCS get
	@: FRC "force" depends on nothing and is not a file, so is
	@: always out-of-date causing targets which depend on it to
	@: similarly always be outdated causing their rules to fire
	@: each time they or a dependent is built.

# include $(top_srcdir)/bincheck.mf
# include $(top_srcdir)/depsver.mf
