## LIBOPTS Makefile
MAINTAINERCLEANFILES  = Makefile.in
lib_LTLIBRARIES       = libopts.la
libopts_la_SOURCES    = libopts.c
libopts_la_CPPFLAGS   = -I$(top_srcdir)
libopts_la_LDFLAGS    = -version-info  25:1:0
EXTRA_DIST = \
autoopts.c    enumeration.c  load.c	  proto.h     stack.c	   version.c \
autoopts.h    environment.c  makeshell.c  putshell.c  streqvcmp.c \
boolean.c     genshell.c     nested.c	  restore.c   text_mmap.c \
configfile.c  genshell.h     numeric.c	  save.c      tokenize.c \
cook.c	            pgusage.c	  sort.c      usage.c
