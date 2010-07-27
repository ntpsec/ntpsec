check_PROGRAMS = tests
LDADD = @GTEST_LDFLAGS@ @GTEST_LIBS@ $(LIBOPTS_LDADD) @top_builddir@/libntp/libntp.a
AM_CXXFLAGS = @GTEST_CXXFLAGS@
AM_CPPFLAGS = @GTEST_CPPFLAGS@

sntp_src = $(top_srcdir)/sntp

sntp_SOURCES_USED =	$(sntp_src)/kod_management.c	\
			$(sntp_src)/log.c		\
			$(sntp_src)/sntp-opts.c

tests_SOURCES = ../main.cpp		\
		sntptest.cpp		\
		kodDatabase.cpp		\
		$(sntp_SOURCES_USED)


INCLUDES = $(LIBOPTS_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include \
	-I$(top_srcdir)/tests \
	-I$(top_srcdir)/sntp

TESTS = 

if !NTP_CROSSCOMPILE
TESTS += tests
endif
