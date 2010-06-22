check_PROGRAMS = tests
LDADD = @LCRYPTO@ @GTEST_LDFLAGS@ @GTEST_LIBS@ @top_builddir@/libntp/libntp.a
AM_CXXFLAGS = @GTEST_CXXFLAGS@
AM_CPPFLAGS = @GTEST_CPPFLAGS@
tests_SOURCES = ../main.cpp		\
		libntptest.cpp		\
		a_md5encrypt.cpp	\
		atoint.cpp		\
		atouint.cpp		\
		authkeys.cpp		\
		caljulian.cpp		\
		decodenetnum.cpp	\
		hextoint.cpp		\
		hextolfp.cpp		\
		humandate.cpp		\
		inttoa.cpp		\
		lfptostr.cpp		\
		modetoa.cpp		\
		netof.cpp		\
		numtoa.cpp		\
		numtohost.cpp		\
		octtoint.cpp		\
		prettydate.cpp		\
		refnumtoa.cpp		\
		sfptostr.cpp		\
		socktoa.cpp		\
		ssl_init.cpp		\
		strtolfp.cpp		\
		tsftomsu.cpp		\
		uglydate.cpp		\
		uinttoa.cpp		\
		ymd2yd.cpp


INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include \
	-I$(top_srcdir)/tests

TESTS = 

if !NTP_CROSSCOMPILE
TESTS += tests
endif
