/*
 * lib_strbuf - library string storage
 */
#include <pthread.h>

#include "config.h"

#include "isc_netaddr.h"

#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"

/*
 * Storage declarations
 */
static pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * Function to get a pointer to the next buffer.  Needs to be thread-safe because
 * it's used in callers that need to be thread-safe, notably msyslog.  For the 
 * same reason, we don't try to log a lock-acquisition failure.
 *
 * ESR: Yes, this is ugly and kludgy. I'm not getting rid of of it
 * because I have an eye forward on translation to a garbage-collected
 * language, at which point something with this behavior will be
 * better than all the contortions we'd have to go through to get rid
 * of it in C.
 */
char *lib_getbuf(void)
{
    static libbufstr	lib_stringbuf[LIB_NUMBUF];
    static int		lib_nextbuf;
    char *bufp;

    pthread_mutex_lock(&cookie_lock);
    ZERO(lib_stringbuf[lib_nextbuf]);
    (bufp) = &lib_stringbuf[lib_nextbuf++][0];
    lib_nextbuf %= (int)COUNTOF(lib_stringbuf);
    pthread_mutex_unlock(&cookie_lock);
    return bufp;
}
