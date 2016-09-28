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

typedef enum {none, capture, replay} intercept_mode;

intercept_mode intercept_get_mode(void);
void intercept_set_mode(intercept_mode);

void intercept_argparse(int *, char ***);
bool intercept_getaddrinfo(char *, sockaddr_u *);
void intercept_getconfig(const char *);
void intercept_get_systime(const char *, l_fp *);
long intercept_ntp_random(const char *);
void intercept_sendpkt(const char *,
		       sockaddr_u *, struct interface *, int, void *, int);
int intercept_pselect(int nfds, fd_set *readfds, const sigset_t *runMask);
ssize_t intercept_recvfrom(int, void *,
			   size_t, int, struct sockaddr *, socklen_t *);
ssize_t intercept_recvmsg(int sockfd, struct msghdr *msg, int flags);
SOCKET intercept_open_socket(sockaddr_u *, bool, bool, endpt *);
bool intercept_drift_read(const char *, double *);
void intercept_drift_write(char *, double);
int intercept_adjtime(const struct timeval *, struct timeval *);
#ifdef HAVE_KERNEL_PLL
int intercept_ntp_adjtime(struct timex *);
#endif
int intercept_set_tod(struct timespec *tvs);
extern bool intercept_leapsec_load_file(const char * fname, struct stat * sb,
					bool force, bool logall);
void intercept_getauthkeys(const char *);
bool intercept_replay(void);
void intercept_exit(const int);

/* end */
