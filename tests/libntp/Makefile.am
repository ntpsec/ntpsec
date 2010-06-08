check_PROGRAMS = tests
LDADD = @top_builddir@/libntp/libntp.a @LCRYPTO@ -lgtest_main -lpthread
tests_SOURCES = libntptest.cpp		\
		a_md5encrypt.cpp	\
		authkeys.cpp		\
		hextoint.cpp

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include \
	-I$(top_srcdir)/tests

TESTS = 

if !NTP_CROSSCOMPILE
TESTS += tests
endif
