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

/*
 * Macro to get a pointer to the next buffer
 */
char *lib_getbuf(void)
{
    static libbufstr	lib_stringbuf[LIB_NUMBUF];
    static int		lib_nextbuf;
    char *bufp;

    ZERO(lib_stringbuf[lib_nextbuf]);
    (bufp) = &lib_stringbuf[lib_nextbuf++][0];
    lib_nextbuf %= (int)COUNTOF(lib_stringbuf);
    return bufp;
}
