# Makefile.am for JMK's SNTP, by Harlan Stenn

NULL =

## LIBOPTS_CHECK_NOBUILD works with Automake 1.10 now
AUTOMAKE_OPTIONS = foreign 1.10
ACLOCAL_AMFLAGS = -I m4 -I libopts/m4

CFLAGS = @CFLAGS_NTP@
AM_CPPFLAGS = @CPPFLAGS_NTP@

INCLUDES  = $(LIBOPTS_CFLAGS)
INCLUDES += -I$(top_srcdir)/../include
INCLUDES += -I$(top_srcdir)/../lib/isc/include
INCLUDES += -I$(top_srcdir)/../lib/isc/nothreads/include
INCLUDES += -I$(top_srcdir)/../lib/isc/unix/include

LDADD  = $(LIBOPTS_LDADD)
LDADD += $(LIBM)
LDADD += ../libntp/libntp.a
LDADD += @LCRYPTO@

run_ag =	cd $(srcdir) &&	env PATH="$(abs_builddir):$(PATH)"	\
		autogen -L include --writable

EXTRA_PROGRAMS = sntp

if NTP_BINSUBDIR_IS_BIN
bin_PROGRAMS =	@SNTP@
else
sbin_PROGRAMS =	@SNTP@
endif

SUBDIRS = include

if NEED_LIBOPTS
SUBDIRS += libopts
endif

if GTEST_AVAILABLE
SUBDIRS += tests
endif

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
	main.h		\
	networking.c	\
	networking.h	\
	sntp.c		\
	sntp-opts.c	\
	sntp-opts.h	\
	utilities.c	\
	utilities.h	\
	$(NULL)

EXTRA_DIST =				\
	bincheck.mf			\
	$(srcdir)/COPYRIGHT		\
	deps-ver			\
	depsver.mf			\
	sntp-opts.def			\
	sntp-opts.menu			\
	sntp-opts.texi			\
	sntp.1				\
	sntp.html			\
	sntp.texi			\
	tests_main.h			\
	$(srcdir)/version		\
	$(srcdir)/m4/version.m4		\
	$(NULL)

BUILT_SOURCES =				\
	check-libntp			\
	$(srcdir)/COPYRIGHT		\
	libtool				\
	$(srcdir)/sntp-opts.c		\
	$(srcdir)/sntp-opts.h		\
	$(srcdir)/version		\
	$(srcdir)/include/version.def	\
	$(srcdir)/m4/version.m4		\
	$(srcdir)/include/version.texi	\
	$(NULL)

CLEANFILES =				\
	built-sources-only		\
	check-COPYRIGHT-submake		\
	$(NULL)

man_MANS=	$(srcdir)/sntp.1

# HMS: Real Soon Now...
#info_TEXINFOS=	sntp.texi
#sntp_TEXINFOS=	sntp-opts.texi

noinst_DATA=				\
	$(srcdir)/sntp.html		\
	$(srcdir)/sntp-opts.texi	\
	$(srcdir)/sntp-opts.menu	\
	$(NULL)

FRC FRC.version:
	@: do-nothing action to prevent default SCCS get
	@: FRC "force" depends on nothing and is not a file, so is
	@: always out-of-date causing targets which depend on it to
	@: similarly always be outdated causing their rules to fire
	@: each time they or a dependent is built.

built-sources-only: $(BUILT_SOURCES)
	@umask 0 && touch $@

# HMS: The next bit is still suboptimal.  If bk is present but this NTP
# repo is not a bk repo, we'll get an error message from the prs command.
# Unfortunately, I haven't found the necessary magic to redirect this error
# output to /dev/null under ancient/unique shells like the one Ultrix uses.
# We'll also get an error if srcdir or version is unwritable.
$(srcdir)/version: FRC.version
	-test -r $(srcdir)/../SCCS/s.ChangeSet &&				\
	    (bk version) >/dev/null 2>&1 &&			\
	    cd $(srcdir)/.. &&					\
	    x=`bk -R prs -hr+ -nd:I: ChangeSet` &&		\
	    y=`cat sntp/version 2>/dev/null` || true &&		\
	    case "$$x" in ''|$$y) ;; *) echo $$x > sntp/version ;; esac

$(srcdir)/m4/version.m4: $(srcdir)/../packageinfo.sh
	TEMPDIR=`pwd` && export TEMPDIR && cd $(srcdir) && \
	../scripts/genver m4/version.m4

$(srcdir)/include/version.def: $(srcdir)/../packageinfo.sh
	TEMPDIR=`pwd` && export TEMPDIR && cd $(srcdir) && \
	../scripts/genver include/version.def

$(srcdir)/include/version.texi: $(srcdir)/../packageinfo.sh
	TEMPDIR=`pwd` && export TEMPDIR && cd $(srcdir) && \
	../scripts/genver include/version.texi

$(srcdir)/../COPYRIGHT:
	cd .. && $(MAKE) COPYRIGHT-please

check-COPYRIGHT-submake: $(srcdir)/../COPYRIGHT
	@cmp -s $(srcdir)/../COPYRIGHT $(srcdir)/COPYRIGHT	\
	 || { cp $(srcdir)/../COPYRIGHT $(srcdir)		\
	      && echo 'updated sntp/COPYRIGHT installed' ;}
	@echo 'submake avoided if this file is newer than parent COPYRIGHT' > $@

check-COPYRIGHT: FRC
	[ ! -r $(srcdir)/../COPYRIGHT ]					\
	|| [ check-COPYRIGHT-submake -nt $(srcdir)/../COPYRIGHT ]	\
	|| $(MAKE) check-COPYRIGHT-submake

$(srcdir)/COPYRIGHT: check-COPYRIGHT
	@: do-nothing action to prevent any default

$(srcdir)/sntp-opts.h: $(srcdir)/sntp-opts.c
	@: do-nothing action to avoid default SCCS get, .h built with .c
	
$(srcdir)/sntp-opts.c: $(srcdir)/sntp-opts.def $(srcdir)/include/version.def $(srcdir)/include/copyright.def
	$(run_ag) sntp-opts.def

$(srcdir)/sntp.1: $(srcdir)/sntp-opts.def $(srcdir)/include/version.def $(srcdir)/include/copyright.def
	$(run_ag) -Tagman1.tpl -bsntp sntp-opts.def

$(srcdir)/sntp-opts.menu: $(srcdir)/sntp-opts.texi
	@: do-nothing action to avoid default SCCS get, .menu built with .texi
	
$(srcdir)/sntp-opts.texi: $(srcdir)/sntp-opts.def $(srcdir)/include/version.def $(srcdir)/include/copyright.def
	$(run_ag) -Taginfo.tpl -DLEVEL=section sntp-opts.def
	$(top_srcdir)/../scripts/check--help $@

$(srcdir)/sntp.html: $(srcdir)/sntp-opts.menu $(srcdir)/sntp-opts.texi $(srcdir)/sntp.texi $(srcdir)/include/version.texi
	cd $(srcdir) && ( makeinfo --force --html --no-split -o sntp.html sntp.texi || true )

check-libntp: FRC
	cd ../libntp && $(MAKE)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

libtool: $(LIBTOOL_DEPS)
	./config.status --recheck

include $(top_srcdir)/bincheck.mf
include $(top_srcdir)/depsver.mf
