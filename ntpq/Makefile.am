AUTOMAKE_OPTIONS=	../util/ansi2knr

bin_PROGRAMS=	ntpq
AM_CPPFLAGS=	-I$(top_srcdir)/include $(LIBOPTS_CFLAGS)

# LDADD might need RESLIB and ADJLIB
ntpq_LDADD=	version.o @READLINE_LIBS@ $(LIBOPTS_LDADD) ../libntp/libntp.a
DISTCLEANFILES=	.version version.c
noinst_HEADERS=	ntpq.h
ETAGS_ARGS=	Makefile.am
EXTRA_DIST=	ntpq-opts.def ntpq.1 ntpq-opts.texi ntpq-opts.menu
BUILT_SOURCES=	ntpq-opts.c ntpq-opts.h ntpq.1 ntpq-opts.texi ntpq-opts.menu
man_MANS=	ntpq.1

ntpq_SOURCES=	ntpq.c ntpq-subs.c ntpq-opts.c ntpq-opts.h

ntpq-opts.h: ntpq-opts.c
ntpq-opts.c: ntpq-opts.def
	cd $(srcdir) && autogen ntpq-opts.def

ntpq.1: ntpq-opts.def
	cd $(srcdir) && autogen -Tagman1.tpl -bntpq ntpq-opts.def

ntpq.texi ntpq.menu: ntpq-opts.def
	cd $(srcdir) && autogen -L ${prefix}/share/autogen \
		-Taginfo.tpl -DLEVEL=section ntpq-opts.def

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

version.o: $(ntpq_OBJECTS) ../libntp/libntp.a Makefile $(top_srcdir)/version
	env CSET=`cat $(top_srcdir)/version` $(top_builddir)/scripts/mkver ntpq
	$(COMPILE) -c version.c
