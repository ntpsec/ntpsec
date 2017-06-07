/*
 * lib_strbuf - library string storage
 */
#include "config.h"

#include "isc_netaddr.h"
#include "ntp_stdlib.h"

/*
 * Storage declarations
 */
bool		ipv4_works;
bool		ipv6_works;

/*
 * initialization routine.  Might be needed if the code is ROMized.
 */
void
init_network(void)
{
	ipv4_works = isc_net_probeipv4_bool();
	ipv6_works = isc_net_probeipv6_bool();
}
