# Makefile.am, by Harlan Stenn, from:
# Makefile for sntp
# N.M. Maclaren, October 2000.

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS= -I libopts/m4

AM_CPPFLAGS=	$(LIBOPTS_CFLAGS)
LDADD=	$(LIBOPTS_LDADD)

NULL=

EXTRA_DIST=	bincheck.mf

bindir=	${exec_prefix}/${BINSUBDIR}
bin_PROGRAMS=	sntp
run_ag=         cd $(srcdir) && autogen -L $(top_srcdir)/include

SUBDIRS=
if NEED_LIBOPTS
SUBDIRS+= libopts
endif
SUBDIRS+= .

sntp_SOURCES = \
	header.h \
	internet.c \
	internet.h \
	kludges.h \
	main.c \
	sntp-opts.c \
	sntp-opts.h \
	socket.c \
	timing.c \
	unix.c \
	$(NULL)

dist_man_MANS=	sntp.1

EXTRA_DIST=	sntp-opts.def sntp.1 sntp-opts.texi sntp-opts.menu
BUILT_SOURCES=	sntp-opts.c sntp-opts.h sntp.1 sntp-opts.texi sntp-opts.menu
man_MANS=	sntp.1

$(srcdir)/sntp-opts.h: sntp-opts.c
$(srcdir)/sntp-opts.c: sntp-opts.def
	$(run_ag) sntp-opts.def

$(srcdir)/sntp.1: sntp-opts.def
	$(run_ag) -Tagman1.tpl -bsntp sntp-opts.def

$(srcdir)/sntp-opts.texi $(srcdir)/sntp-opts.menu: sntp-opts.def
	$(run_ag) -Taginfo.tpl -DLEVEL=section sntp-opts.def

include bincheck.mf
