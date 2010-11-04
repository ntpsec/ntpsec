NULL =

check_PROGRAMS = tests

sntp_objs = ..

sntp_SOURCES_USED =	$(sntp_objs)/crypto.o		\
			$(sntp_objs)/kod_management.o	\
			$(sntp_objs)/log.o		\
			$(sntp_objs)/main.o		\
			$(sntp_objs)/networking.o	\
			$(sntp_objs)/sntp-opts.o	\
			$(sntp_objs)/utilities.o	\
			$(NULL)

base_SOURCES =		$(srcdir)/../tests_main.cpp	\
			sntptest.cpp			\
			$(NULL)

tests_SOURCES = $(base_SOURCES)		\
		crypto.cpp		\
		keyFile.cpp		\
		kodDatabase.cpp		\
		kodFile.cpp		\
		logFile.cpp		\
		networking.cpp		\
		packetHandling.cpp	\
		packetProcessing.cpp	\
		utilities.cpp		\
		$(NULL)

noinst_HEADERS =	sntptest.h		\
			fileHandlingTest.h	\
			$(NULL)

EXTRA_DIST =	data			\
		test-driver		\
		$(NULL)

CLEANFILES =	data/log-output-debug		\
		data/kod-output-multiple	\
		data/kod-output-single		\
		data/debug-output-pkt		\
		data/debug-output-lfp-dec	\
		data/kod-output-blank		\
		data/log-output-log		\
		data/debug-output-lfp-bin	\
		data/debug-output-lfp-hex	\
		$(NULL)

LDADD = 	@GTEST_LDFLAGS@			\
		@GTEST_LIBS@			\
		$(LIBOPTS_LDADD)		\
		@top_builddir@/../libntp/libntp.a	\
		@LCRYPTO@			\
		$(sntp_SOURCES_USED)

AM_CXXFLAGS = @GTEST_CXXFLAGS@
AM_CPPFLAGS = @GTEST_CPPFLAGS@

INCLUDES  = $(LIBOPTS_CFLAGS)
INCLUDES += -I$(top_srcdir)/../include
INCLUDES += -I$(top_srcdir)/../lib/isc/include
INCLUDES += -I$(top_srcdir)/../lib/isc/nothreads/include
INCLUDES += -I$(top_srcdir)/../lib/isc/unix/include
INCLUDES += -I$(top_srcdir)/../tests
INCLUDES += -I$(top_srcdir)

TESTS = 

TEST_INPUT_DIR = $(abs_srcdir)/data
TEST_OUTPUT_DIR = $(abs_builddir)/data

TESTS_ENVIRONMENT = $(top_srcdir)/tests/test-driver $(TEST_INPUT_DIR) $(TEST_OUTPUT_DIR)

if !SNTP_CROSSCOMPILE
TESTS += $(check_PROGRAMS)
endif
