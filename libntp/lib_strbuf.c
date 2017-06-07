/*
 * lib_strbuf - library string storage
 */
#include "config.h"

#include "isc_netaddr.h"

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"


/*
 * Storage declarations
 */
int		debug;
libbufstr	lib_stringbuf[LIB_NUMBUF];
int		lib_nextbuf;


/*
 * This stub is required to pacify the Mac OS X linker, which will
 * refuse to consider a module a candidate to be linked unless it
 * has an executable entry point called from somewhere else that
 * is linked.
 *
 * marking this __attribute__((const)) kills thelinker too.
 */
void
init_lib(void)
{
}
