#ifndef NTP_IO_H
#define NTP_IO_H
/*
 * POSIX says use <fnct.h> to get O_* symbols and 
 * SEEK_SET symbol form <unistd.h>.
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <stdio.h>
#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#if !defined(SEEK_SET) && defined(L_SET)
# define SEEK_SET L_SET
#endif

#ifdef SYS_WINNT
# include <io.h>
# include "win32_io.h"
#endif

#include <isc/boolean.h>
#include <isc/netaddr.h>

#if HAVE_NETINET_IN_H && HAVE_NETINET_IP_H
#include <netinet/in.h>
# if HAVE_NETINET_IN_SYSTM_H
#  include <netinet/in_systm.h>
# endif
#include <netinet/ip.h>
#endif

/*
 * Define FNDELAY and FASYNC using O_NONBLOCK and O_ASYNC if we need
 * to (and can).  This is here initially for QNX, but may help for
 * others as well...
 */
#ifndef FNDELAY
# ifdef O_NONBLOCK
#  define FNDELAY O_NONBLOCK
# endif
#endif

#ifndef FASYNC
# ifdef O_ASYNC
#  define FASYNC O_ASYNC
# endif
#endif

isc_boolean_t get_broadcastclient_flag(void); /* Get the status of client broadcast */
extern  void  add_specific_interface (const char *);
extern  void  add_limit_address (const isc_netaddr_t *);
isc_boolean_t is_ip_address(const char *, isc_netaddr_t *);

#endif	/* NTP_IO_H */
