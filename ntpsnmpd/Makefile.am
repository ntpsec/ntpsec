AUTOMAKE_OPTIONS= 

sbin_PROGRAMS=	ntpsnmpd
ntpsnmpd_SOURCES= ntpsnmpd.c ntpSnmpSubagentObject.c
# HMS: we probably want a version.o file here, too.
LDADD=		../ntpq/libntpq.a ../libntp/libntp.a @SNMP_LIBS@
AM_CPPFLAGS=	-I$(top_srcdir)/ntpq -I$(top_srcdir)/include `net-snmp-config --cflags`

#CFLAGS=-I. -I../ntp-dev/ntpq -I../ntp-dev/include -I../net-snmp/include `net-snmp-config --cflags`  -g
#BUILDLIBS=`net-snmp-config --libs` 
#BUILDAGENTLIBS=`net-snmp-config --agent-libs`  
#BUILDNTPLIBS=-L ../ntp-dev/libntp -lntp -L../ntp-dev/ntpq -lntpq 

#ntpsnmpd: $(OBJS)
#	$(CC)  -o ntpsnmpd  $(OBJS) $(BUILDNTPLIBS) $(BUILDAGENTLIBS) 
#
#ntpSnmpPluginObject.so: ntpSnmpPluginObject.c Makefile
#	$(CC) $(CFLAGS) $(DLFLAGS) -c -o ntpSnmpPluginObject.o ntpSnmpPluginObject.c
#	$(CC) $(CFLAGS) $(DLFLAGS) -o ntpSnmpPluginObject.so ntpSnmpPluginObject.o
