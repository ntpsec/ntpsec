# Makefile.am for JMK's SNTP, by Harlan Stenn

NULL=

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS= -I ../m4 -I libopts/m4

AM_CPPFLAGS=	$(LIBOPTS_CFLAGS) 				\
		-I$(top_srcdir)/../include			\
		-I$(top_srcdir)/../lib/isc/include		\
		-I$(top_srcdir)/../lib/isc/nothreads/include	\
		-I$(top_srcdir)/../lib/isc/unix/include		\
		$(NULL)
LDADD=	$(LIBOPTS_LDADD) -lm ../libntp/libntp.a

run_ag=         cd $(srcdir) && autogen -L ../include --writable

bindir=	${exec_prefix}/${BINSUBDIR}
bin_PROGRAMS=	sntp

SUBDIRS=
if NEED_LIBOPTS
SUBDIRS+= libopts
endif
SUBDIRS+= .

sntp_SOURCES =		\
	crypto.c	\
	crypto.h	\
	data_formats.h	\
	header.h	\
	kod_management.c \
	kod_management.h \
	log.c		\
	log.h		\
	main.c		\
	netutils.h	\
	networking.c	\
	networking.h	\
	sntp-opts.c	\
	sntp-opts.h	\
	utilities.c	\
	utilities.h	\
	$(NULL)

# it's in EXTRA_DIST
#dist_man_MANS=	sntp.1

EXTRA_DIST=	bincheck.mf	\
		COPYRIGHT	\
		sntp-opts.def sntp.1 sntp-opts.texi sntp-opts.menu	\
		$(NULL)
OLD_EXTRA_DIST=		\
		autogen-version.def version.def version.m4
BUILT_SOURCES=	check-autogen-version.def check-version.def check-version.m4 \
		sntp-opts.c sntp-opts.h sntp.1 sntp-opts.texi sntp-opts.menu
man_MANS=	sntp.1

FRC:
check-autogen-version.def: FRC
foo1:
	@cd $(srcdir)							\
	&& test -r ../include/autogen-version.def			\
	&& ( if cmp -s ../include/autogen-version.def autogen-version.def; \
	   then : ;							\
	   else cp ../include/autogen-version.def autogen-version.def;	 \
		echo "Installing new sntp/autogen-version.def file";	\
	   fi )

check-version.def: FRC
foo2:
	@cd $(srcdir)					\
	&& test -r ../include/version.def		\
	&& ( if cmp -s ../include/version.def version.def;	\
	   then : ;						\
	   else cp ../include/version.def version.def;		\
		echo "Installing new sntp/version.def file";	\
	   fi )

check-version.m4: FRC
foo3:
	@cd $(srcdir)					\
	&& test -r ../version.m4			\
	&& ( if cmp -s ../version.m4 version.m4;	\
	   then : ;					\
	   else cp ../version.m4 version.m4;		\
		echo "Installing new sntp/version.m4 file";	\
	   fi )

$(srcdir)/sntp-opts.h: $(srcdir)/sntp-opts.c
$(srcdir)/sntp-opts.c: $(srcdir)/sntp-opts.def $(srcdir)/../include/version.def
	$(run_ag) sntp-opts.def

$(srcdir)/sntp.1: $(srcdir)/sntp-opts.def $(srcdir)/../include/version.def
	$(run_ag) -Tagman1.tpl -bsntp sntp-opts.def

$(srcdir)/sntp-opts.texi $(srcdir)/sntp-opts.menu: $(srcdir)/sntp-opts.def $(srcdir)/../include/version.def
	$(run_ag) -Taginfo.tpl -DLEVEL=section sntp-opts.def

include bincheck.mf
