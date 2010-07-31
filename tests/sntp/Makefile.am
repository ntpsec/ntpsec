check_PROGRAMS = tests

sntp_src = $(top_builddir)/sntp

sntp_SOURCES_USED =	$(sntp_src)/crypto.o		\
			$(sntp_src)/kod_management.o	\
			$(sntp_src)/log.o		\
			$(sntp_src)/networking.o	\
			$(sntp_src)/sntp-opts.o		\
			$(sntp_src)/utilities.o

base_SOURCES =		../main.cpp	\
			sntptest.cpp

tests_SOURCES = $(base_SOURCES)		\
		keyFile.cpp		\
		kodDatabase.cpp		\
		kodFile.cpp		\
		networking.cpp		\
		utilities.cpp

LDADD = 	@GTEST_LDFLAGS@			\
		@GTEST_LIBS@			\
		$(LIBOPTS_LDADD)		\
		@top_builddir@/libntp/libntp.a	\
		@LCRYPTO@			\
		$(sntp_SOURCES_USED)

AM_CXXFLAGS = @GTEST_CXXFLAGS@
AM_CPPFLAGS = @GTEST_CPPFLAGS@

INCLUDES = $(LIBOPTS_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include \
	-I$(top_srcdir)/tests \
	-I$(top_srcdir)/sntp

TESTS = 

TESTS_ENVIRONMENT = $(top_srcdir)/tests/sntp/test-driver $(abs_srcdir)/data $(abs_builddir)/data

if !NTP_CROSSCOMPILE
TESTS += $(check_PROGRAMS)
endif
