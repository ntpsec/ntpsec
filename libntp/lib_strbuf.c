/*
 * lib_strbuf - library string storage
 */
#include <pthread.h>

#include "config.h"

#include "isc_netaddr.h"

#include "ntp_assert.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"

/*
 * Storage declarations
 */
static pthread_mutex_t cookie_lock = PTHREAD_MUTEX_INITIALIZER;

static pthread_t me;

void getbuf_init(void) {
	me = pthread_self();
}

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
 *
 * HGM: But I'm not willing to ship a lurking time bomb,
 * so I fixed the non-main threads (NTS, DNS) not to call
 * lib_getbuf and added a trap to make sure I really fixed them all.
 */


char *lib_getbuf(void)
{
	static libbufstr	lib_stringbuf[LIB_NUMBUF];
	static int		lib_nextbuf;
	char *bufp;

// FIXME - need this until python tests can call getbuf_init
	static bool init_done = false;
	if (!init_done) {
		getbuf_init();
		init_done = true;
	}

	if (pthread_self() != me) {
		msyslog(LOG_ERR, "ERR: lib_getbuf() called from non-main thread.");
#ifndef BACKTRACE_DISABLED
//        backtrace_log();
#endif
//        exit(1);
	}

	pthread_mutex_lock(&cookie_lock);
	ZERO(lib_stringbuf[lib_nextbuf]);
	(bufp) = &lib_stringbuf[lib_nextbuf++][0];
	lib_nextbuf %= (int)COUNTOF(lib_stringbuf);
	pthread_mutex_unlock(&cookie_lock);
	return bufp;
}
