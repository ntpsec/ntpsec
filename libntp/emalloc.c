/*
 * emalloc - return new memory obtained from the system.  Belch if none.
 */
#include "ntp_types.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#if !defined(_MSC_VER) || !defined(_DEBUG)

void *
emalloc(
	u_int size
	)
{
	void *mem = malloc(size);

	if (!mem) {
		msyslog(LOG_ERR, "Exiting: No more memory!");
		exit(1);
	}
	return mem;
}

#else /* below is _MSC_VER && _DEBUG */

/*
 * When using the debug MS CRT malloc, preserve the original caller's
 * line and file via the emalloc macro.
 */
void *
debug_emalloc(
	u_int size,
	char *filename,
	int line
	)
{
	void *mem = _malloc_dbg(size, _NORMAL_BLOCK, filename, line);

	if (!mem) {
		msyslog(LOG_ERR, "Exiting: No more memory!");
		exit(1);
	}
	return mem;
}

#endif /* _MSC_VER && _DEBUG */
