NULL=

AUTOMAKE_OPTIONS = 
BUILT_SOURCES =
CLEANFILES =
noinst_PROGRAMS = @TESTDCF@ @DCFD@
EXTRA_PROGRAMS = testdcf dcfd

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
		-I$(top_srcdir)/lib/isc/nothreads/include \
		-I$(top_srcdir)/lib/isc/unix/include

AM_CFLAGS = @CFLAGS_NTP@
AM_CPPFLAGS = @CPPFLAGS_NTP@

ETAGS_ARGS = Makefile.am
DISTCLEANFILES = $(EXTRA_PROGRAMS)

check-local: @DCFD@
	case "$(noinst_PROGRAMS)" in	\
	 *dcfd*) ./dcfd -Y ;;		\
	esac

include $(top_srcdir)/depsver.mf
