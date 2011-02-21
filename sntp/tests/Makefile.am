NULL =

check_PROGRAMS = 
if BUILD_SNTP
check_PROGRAMS += tests
endif

sntp_objs = ..

sntp_SOURCES_USED =			\
	$(sntp_objs)/crypto.o		\
	$(sntp_objs)/kod_management.o	\
	$(sntp_objs)/log.o		\
	$(sntp_objs)/main.o		\
	$(sntp_objs)/networking.o	\
	$(sntp_objs)/sntp-opts.o	\
	$(sntp_objs)/utilities.o	\
	$(sntp_objs)/version.o		\
	$(NULL)

base_SOURCES =				\
	$(srcdir)/../tests_main.cpp	\
	sntptest.cpp			\
	$(NULL)

tests_SOURCES =			\
	$(base_SOURCES)		\
	crypto.cpp		\
	keyFile.cpp		\
	kodDatabase.cpp		\
	kodFile.cpp		\
	networking.cpp		\
	packetHandling.cpp	\
	packetProcessing.cpp	\
	utilities.cpp		\
	$(NULL)

noinst_HEADERS =		\
	sntptest.h		\
	fileHandlingTest.h	\
	$(NULL)

dist_check_SCRIPTS =	test-driver

EXTRA_DIST =	data

CLEANFILES =				\
	data/kod-output-multiple	\
	data/kod-output-single		\
	data/debug-output-pkt		\
	data/debug-output-lfp-dec	\
	data/kod-output-blank		\
	data/debug-output-lfp-bin	\
	data/debug-output-lfp-hex	\
	$(NULL)

LDADD =						\
	$(LIBOPTS_LDADD)			\
	@LDADD_LIBEVENT@			\
	@top_builddir@/../libntp/libntp.a	\
	@LDADD_LIBNTP@				\
	@LCRYPTO@				\
	@GTEST_LDFLAGS@				\
	@GTEST_LIBS@				\
	$(sntp_SOURCES_USED)			\
	$(NULL)

AM_CFLAGS   = @CFLAGS_NTP@
AM_CXXFLAGS = @GTEST_CXXFLAGS@
AM_CPPFLAGS = @CPPFLAGS_LIBEVENT@ @GTEST_CPPFLAGS@ @CPPFLAGS_NTP@

INCLUDES  = $(LIBOPTS_CFLAGS)
INCLUDES += -I$(top_srcdir)/../include
INCLUDES += -I$(top_srcdir)/../lib/isc/include
INCLUDES += -I$(top_srcdir)/../lib/isc/@LIBISC_PTHREADS_NOTHREADS@/include
INCLUDES += -I$(top_srcdir)/../lib/isc/unix/include
INCLUDES += -I$(top_srcdir)/../tests
INCLUDES += -I$(top_srcdir)

TESTS = 

TEST_INPUT_DIR = $(abs_srcdir)/data
TEST_OUTPUT_DIR = $(abs_builddir)/data

TESTS_ENVIRONMENT = $(top_srcdir)/tests/test-driver $(TEST_INPUT_DIR) $(TEST_OUTPUT_DIR)

if !NTP_CROSSCOMPILE
TESTS += $(check_PROGRAMS)
endif
