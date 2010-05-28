check_PROGRAMS = tests
LDADD = @top_builddir@/libntp/libntp.a -lgtest_main -lpthread
tests_SOURCES = hextoint.cpp

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include \
	-I$(top_srcdir)/tests

TESTS = tests
