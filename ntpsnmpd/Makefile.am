AUTOMAKE_OPTIONS= 

sbin_PROGRAMS=	ntpsnmpd
ntpsnmpd_SOURCES= ntpsnmpd.c ntpSnmpSubagentObject.c
noinst_HEADERS=	ntpSnmpSubagentObject.h
# HMS: we probably want a version.o file here, too.
LDADD=		../ntpq/libntpq.a ../libntp/libntp.a @SNMP_LIBS@
AM_CPPFLAGS=	-I$(top_srcdir)/ntpq -I$(top_srcdir)/include @SNMP_CPPFLAGS@
AM_CFLAGS=	@SNMP_CFLAGS@
