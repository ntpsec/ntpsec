#AUTOMAKE_OPTIONS = ../util/ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ../util/ansi2knr
noinst_LIBRARIES = @MAKE_LIBRSAREF@
EXTRA_LIBRARIES = librsaref.a
CLEANFILES = $(EXTRA_LIBRARIES)

# NOTES:
# don't use RSAREF's global.h - we use ours.
# We already have a copy of des.h

librsaref_a_SOURCES = \
	desc.c \
	digit.c \
	digit.h \
	md2.h \
	md2c.c \
	md5.h \
	md5c.c \
	nn.c \
	nn.h \
	prime.c \
	prime.h \
	r_dh.c \
	r_encode.c \
	r_enhanc.c \
	r_keygen.c \
	r_random.c \
	r_random.h \
	r_stdlib.c \
	rsa.c \
	rsa.h \
	rsaref.h

BUILT_SOURCES = $(librsaref_a_SOURCES)
INCLUDES = -I$(top_srcdir)/include
ETAGS_ARGS = Makefile.am

#EXTRA_DIST = 

$(srcdir)/des.h: $(srcdir)/../rsaref2/source/des.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/desc.c: $(srcdir)/../rsaref2/source/desc.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/digit.c: $(srcdir)/../rsaref2/source/digit.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/digit.h: $(srcdir)/../rsaref2/source/digit.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/global.h: $(srcdir)/../rsaref2/source/global.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/md2.h: $(srcdir)/../rsaref2/source/md2.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/md2c.c: $(srcdir)/../rsaref2/source/md2c.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/md5.h: $(srcdir)/../rsaref2/source/md5.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/md5c.c: $(srcdir)/../rsaref2/source/md5c.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/nn.c: $(srcdir)/../rsaref2/source/nn.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/nn.h: $(srcdir)/../rsaref2/source/nn.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/prime.c: $(srcdir)/../rsaref2/source/prime.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/prime.h: $(srcdir)/../rsaref2/source/prime.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_dh.c: $(srcdir)/../rsaref2/source/r_dh.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_encode.c: $(srcdir)/../rsaref2/source/r_encode.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_enhanc.c: $(srcdir)/../rsaref2/source/r_enhanc.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_keygen.c: $(srcdir)/../rsaref2/source/r_keygen.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_random.c: $(srcdir)/../rsaref2/source/r_random.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_random.h: $(srcdir)/../rsaref2/source/r_random.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/r_stdlib.c: $(srcdir)/../rsaref2/source/r_stdlib.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/rsa.c: $(srcdir)/../rsaref2/source/rsa.c
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/rsa.h: $(srcdir)/../rsaref2/source/rsa.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

$(srcdir)/rsaref.h: $(srcdir)/../rsaref2/source/rsaref.h
	cmp -s $@ $< 2>/dev/null || cp $< $@

