NULL=
AUTOMAKE_OPTIONS =
BUILT_SOURCES = info_trimble.c
noinst_LIBRARIES = @MAKE_LIBPARSE@ @MAKE_LIBPARSE_KERNEL@
EXTRA_LIBRARIES = libparse.a libparse_kernel.a
EXTRA_PROGRAMS = parsestreams parsesolaris
noinst_PROGRAMS = @MAKE_PARSEKMODULE@
CLEANFILES = 

K_CFLAGS = -DPARSESTREAM -DNTP_NEED_BOPS

# info_trimble.c was mistakenly created in the build directory
# previously.  Without special steps, building an updated tree from
# outside the source directory (a VPATH build) would break trying to
# update info_trimble.c in the build dir using the default SCCS get
# action.
# As a transitional measure, info_trimble.c is listed in
# libparse_a_SOURCES prefixed by "$(srcdir)/".
# This prevents attempting to update or use an old leftover
# info_trimble.c in the build directory, when building outside
# the source directory.  At the same time, the rule to generate
# info_trimble.c (now in the source directory) has been updated
# to delete any leftover info_trimble.c in the build directory.  To
# ensure that fires in all build trees sharing a common source tree,
# $(srcdir)/info_trimble.c now depends on Makefile, and has been added
# temporarily to BUILT_SOURCES.  This causes builds of updated trees to
# get rid of the troublesome leftover info_trimble.c files in the build
# directories.
# After a month or so, this comment block should be removed, the
# reference to info_trimble.c in libparse_a_SOURCES simplified to
# filename only again, info_trimble.c be removed from BUILT_SOURCES,
# and the commented-out $(srcdir)/info_trimble.c rule should replace
# the info_trimble.c rule.

libparse_a_SOURCES = parse.c \
		     parse_conf.c \
		     clk_meinberg.c \
		     clk_schmid.c \
		     clk_rawdcf.c \
		     clk_trimtsip.c \
		     clk_dcf7000.c \
		     clk_trimtaip.c \
		     clk_rcc8000.c \
		     clk_hopf6021.c \
		     clk_computime.c \
		     clk_wharton.c \
		     clk_varitext.c \
		     data_mbg.c \
		     $(srcdir)/info_trimble.c \
		     trim_info.c \
		     binio.c \
		     ieee754io.c \
		     mfp_mul.c \
		     gpstolfp.c \
		     $(NULL)

libparse_kernel_a_SOURCES = 
libparse_kernel_a_LIBADD =	\
	kparse.o		\
	kparse_conf.o		\
	kclk_computime.o	\
	kclk_dcf7000.o		\
	kclk_hopf6021.o		\
	kclk_meinberg.o		\
	kclk_rawdcf.o		\
	kclk_rcc8000.o		\
	kclk_schmid.o		\
	kclk_trimtaip.o		\
	kclk_trimtsip.o		\
	kclk_varitext.o		\
	kclk_wharton.o		\
	kbinio.o		\
	kieee754io.o		\
	kmfp_mul.o		\
	kgpstolfp.o		\
	$(NULL)


INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib/isc/include \
	-I$(top_srcdir)/lib/isc/nothreads/include \
	-I$(top_srcdir)/lib/isc/unix/include -I$(top_srcdir)/kernel

ETAGS_ARGS = Makefile.am

EXTRA_DIST = parsesolaris.c parsestreams.c mkinfo_scmd.sed mkinfo_rcmd.sed info_trimble.c

info_trimble.c: Makefile $(top_srcdir)/include/trimble.h $(srcdir)/Makefile.am $(srcdir)/mkinfo_scmd.sed $(srcdir)/mkinfo_rcmd.sed
	[ "$(srcdir)" != "." ] && rm -f info_trimble.c	# rid ourselves of leftover from old way
	sed -n -f $(srcdir)/mkinfo_scmd.sed $(top_srcdir)/include/trimble.h  > info_trimble.new
	sed -n -f $(srcdir)/mkinfo_rcmd.sed $(top_srcdir)/include/trimble.h >> info_trimble.new
	cmp info_trimble.new $(srcdir)/info_trimble.c || mv -f info_trimble.new $(srcdir)/info_trimble.c
	rm -f info_trimble.new
	
# the workaround form above fires too much, such as during
# "make install", due to the incorrect info_trimble.c target
# rather than $(srcdir)/info_trimble.c.  That's why it verifies
# the file contents have changed before modifying it.
# post-workaround version:
# $(srcdir)/info_trimble.c: $(top_srcdir)/include/trimble.h $(srcdir)/Makefile.am $(srcdir)/mkinfo_scmd.sed $(srcdir)/mkinfo_rcmd.sed
#	sed -n -f $(srcdir)/mkinfo_scmd.sed $(top_srcdir)/include/trimble.h  > info_trimble.new
#	sed -n -f $(srcdir)/mkinfo_rcmd.sed $(top_srcdir)/include/trimble.h >> info_trimble.new
#	mv -f info_trimble.new $@

kieee754io.o: $(srcdir)/ieee754io.c
	 $(COMPILE) $(K_CFLAGS) -c $(srcdir)/ieee754io.c -o $@

kmfp_mul.o: $(srcdir)/mfp_mul.c
	 $(COMPILE) $(K_CFLAGS) -c $(srcdir)/mfp_mul.c -o $@

kgpstolfp.o: $(srcdir)/gpstolfp.c
	 $(COMPILE) $(K_CFLAGS) -c $(srcdir)/gpstolfp.c -o $@

kbinio.o: $(srcdir)/binio.c
	 $(COMPILE) $(K_CFLAGS) -c $(srcdir)/binio.c -o $@

kclk_computime.o: $(srcdir)/clk_computime.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_computime.c -o $@

kclk_dcf7000.o: $(srcdir)/clk_dcf7000.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_dcf7000.c -o $@

kclk_hopf6021.o: $(srcdir)/clk_hopf6021.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_hopf6021.c -o $@

kclk_meinberg.o: $(srcdir)/clk_meinberg.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_meinberg.c -o $@

kclk_rawdcf.o: $(srcdir)/clk_rawdcf.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_rawdcf.c -o $@

kclk_rcc8000.o: $(srcdir)/clk_rcc8000.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_rcc8000.c -o $@

kclk_schmid.o: $(srcdir)/clk_schmid.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_schmid.c -o $@

kclk_trimtaip.o: $(srcdir)/clk_trimtaip.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_trimtaip.c -o $@

kclk_trimtsip.o: $(srcdir)/clk_trimtsip.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_trimtsip.c -o $@

kclk_varitext.o: $(srcdir)/clk_varitext.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_varitext.c -o $@

kclk_wharton.o: $(srcdir)/clk_wharton.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/clk_wharton.c -o $@

kparse.o: $(srcdir)/parse.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/parse.c -o $@

kparse_conf.o: $(srcdir)/parse_conf.c
	$(COMPILE) $(K_CFLAGS) -c $(srcdir)/parse_conf.c -o $@

parsestreams.loadable_module.o: $(parsestreams_OBJECTS) libparse_kernel.a ../libntp/libntp.a
	$(LD) -r -o $@ $(parsestreams_OBJECTS) libparse_kernel.a ../libntp/libntp.a

parse: $(parsesolaris_OBJECTS) libparse_kernel.a ../libntp/libntp.a
	$(LD) -r -o $@ $(parsesolaris_OBJECTS) libparse_kernel.a ../libntp/libntp.a

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

parsesolaris.o: sys/systm.h

sys/systm.h:
	-mkdir sys
	sed -e '/ffs(.*)/d' < /usr/include/sys/systm.h > sys/systm.h

include $(top_srcdir)/depsver.mf
