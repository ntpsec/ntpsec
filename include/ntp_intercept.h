/*
 * ntp_intercept.h - intercept/replay support for environment calls
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_SYS_TIMEX_H
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>
#endif

/* Macro for declaring function with printf-like arguments. */
# if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define PRINTF_FUNC(format_index, arg_index) \
    __attribute__((__format__(__printf__, format_index, arg_index)))
# else
#define PRINTF_FUNC(format_index, arg_indx)
#endif

typedef enum {none, capture, replay} intercept_mode;

intercept_mode intercept_get_mode(void);
void intercept_set_mode(intercept_mode);

PRINTF_FUNC(1, 2) void intercept_log(const char *, ...);
void intercept_argparse(int *, char ***);
void intercept_getconfig(const char *);
void intercept_get_systime(const char *, l_fp *);
long intercept_ntp_random(const char *);
void intercept_alarm(void);
void intercept_sendpkt(const char *,
		       sockaddr_u *, struct interface *, int, struct pkt *, int);
bool intercept_drift_read(const char *, double *);
void intercept_drift_write(char *, double);
#ifdef HAVE_KERNEL_PLL
int intercept_kernel_pll_adjtime(struct timex *);
#endif
extern bool intercept_leapsec_load_file(const char * fname, struct stat * sb,
					bool force, bool logall);

/* end */
