AUTOMAKE_OPTIONS = foreign # ansi2knr

lib_LIBRARIES = libares.a
libares_a_SOURCES = arlib.c arplib.h
include_HEADERS = arlib.h

noinst_PROGRAMS = example
example_SOURCES = sample.c
example_LDADD = libares.a
man_MANS = arlib.3
EXTRA_DIST = $(man_MANS) UNSHAR.HDR
