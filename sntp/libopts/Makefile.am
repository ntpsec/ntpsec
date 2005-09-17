## LIBOPTS Makefile
MAINTAINERCLEANFILES  = Makefile.in
lib_LTLIBRARIES       = libopts.la
libopts_la_SOURCES    = libopts.c
libopts_la_CPPFLAGS   = -I$(top_srcdir)
libopts_la_LDFLAGS    = -version-info  26:0:1
EXTRA_DIST            = \
    COPYING.lgpl         COPYING.mbsd         MakeDefs.inc  \
    README               autoopts/options.h   autoopts/usage-txt.h  \
    autoopts.c           autoopts.h           boolean.c  \
    compat/pathfind.c    compat/compat.h      configfile.c  \
    cook.c               enumeration.c        environment.c  \
    genshell.c           genshell.h           libopts.m4  \
    load.c               makeshell.c          nested.c  \
    numeric.c            pgusage.c            proto.h  \
    putshell.c           restore.c            save.c  \
    sort.c               stack.c              streqvcmp.c  \
    text_mmap.c          tokenize.c           usage.c  \
    version.c
