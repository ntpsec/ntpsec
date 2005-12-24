#auTOMAKE_OPTIONS = util/ansi2knr foreign dist-tarZ no-dependencies
AUTOMAKE_OPTIONS = util/ansi2knr foreign 1.5
ACLOCAL_AMFLAGS= -I m4

SUBDIRS=
if NEED_LIBOPTS
SUBDIRS+= sntp/libopts
endif
SUBDIRS+= \
	scripts \
	include \
	ElectricFence	\
	@ARLIB_DIR@ \
	libntp	\
	libparse	\
	ntpd	\
	ntpdate	\
	ntpdc	\
	ntpq	\
	parseutil	\
	adjtimed	\
	clockstuff	\
	kernel	\
	@MAKE_SNTP@	\
	util

DIST_SUBDIRS= \
	scripts \
	include \
	ElectricFence	\
	arlib \
	libntp	\
	libparse	\
	ntpd	\
	ntpdate	\
	ntpdc	\
	ntpq	\
	parseutil	\
	adjtimed	\
	clockstuff	\
	kernel	\
	sntp	\
	sntp/libopts	\
	util
DISTCHECK_CONFIGURE_FLAGS=	--with-arlib --with-sntp

EXTRA_DIST = \
	COPYRIGHT \
	ChangeLog \
	ChangeLog-4.1.0 \
	NEWS \
	NOTES.y2kfixes \
	README.bk \
	README.hackers \
	README.patches \
	README.refclocks \
	README.versions \
	TODO \
	WHERE-TO-START \
	build \
	config.guess \
	config.h.in \
	config.sub \
	dot.emacs \
	excludes \
	flock-build \
	install-sh \
	readme.y2kfixes \
	results.y2kfixes \
	\
	conf \
	html \
	libisc \
	ports \
	\
	version

DISTCLEANFILES = .gcc-warning

#ETAGS_ARGS = $(srcdir)/Makefile.am $(srcdir)/configure.ac
ETAGS_ARGS = Makefile.am configure.ac

# HMS: make ports be the last directory...
# DIST_HOOK_DIRS = conf html scripts ports

# HMS: Keep .gcc-warning first, as that way it gets printed first.
BUILT_SOURCES = .gcc-warning

dist-hook:
	@find $(distdir) -type d -name CVS -print | xargs rm -rf
	@find $(distdir) -type d -name SCCS -print | xargs rm -rf
	@chmod u+w $(distdir)/ports/winnt
	@for i in `find $(distdir)/ports/winnt -type f -name '*.ds*' -print`; \
	   do chmod u+w $$i ; unix2dos $$i $$i; done

.gcc-warning:
	@echo "Compiling with GCC now generates lots of new warnings."
	@echo " "
	@echo "Don't be concerned. They're just warnings."
	@echo " "
	@echo "Don't send bug reports about the warnings, either."
	@echo " "
	@echo "Feel free to send patches that fix these warnings, though."
	@echo " "
	@sleep 1
	@touch .gcc-warning

# HMS: The following seems to be a work-in-progress...

CVO=`$(srcdir)/config.guess`

.buildcvo:
	echo "$(CVO)" > .buildcvo

.checkcvo: .buildcvo FRC.checkcvo
	@if [ "`cat .buildcvo`" != "$(CVO)" ];then	\
		echo "This directory was configured for `cat .buildcvo`"; \
		echo "but this machine is a $(CVO)";	\
		exit 1;	\
	fi

BHOST=`(hostname || uname -n)`

.buildhost:
	echo "$(BHOST)" > .buildhost

.checkhost: .buildhost FRC.checkhost
	@if [ "`cat .buildhost`" != "$(BHOST)" ];then	\
		echo "Built on `cat .buildhost` but this is $(BHOST)"; \
		echo " "; \
	fi

FRC.distwarn FRC.checkcvo FRC.checkhost FRC.version:

# HMS: what was I trying to do with this?
#dot.emacs: FRC.distwarn
