/* 
 * netof - return the net address part of an ip address in a sockaddr_storage structure
 *         (zero out host part)
 */
#include <stdio.h>

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp.h"

struct sockaddr_storage*
netof(
        struct sockaddr_storage* hostaddr
	)
{
	register u_int32 netnum;
        struct sockaddr_storage *netaddr = malloc(sizeof(struct sockaddr_storage));

        memcpy(netaddr, hostaddr, sizeof(struct sockaddr_storage));

        if(netaddr->ss_family == AF_INET) {
                netnum = ((struct sockaddr_in*)netaddr)->sin_addr.s_addr;
	if(IN_CLASSC(netnum))
	    netnum &= IN_CLASSC_NET;
	else if (IN_CLASSB(netnum))
	    netnum &= IN_CLASSB_NET;
	else			/* treat all other like class A */
	    netnum &= IN_CLASSA_NET;
                ((struct sockaddr_in*)netaddr)->sin_addr.s_addr = netnum;
         }
         else if(netaddr->ss_family == AF_INET6) {
		/* Here we put 0 at the local link address so we get net address */
		  memset(&((struct sockaddr_in6*)netaddr)->sin6_addr.s6_addr[8], 0, 8*sizeof(u_char));
         }

         return netaddr;
}
