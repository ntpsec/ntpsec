/*
 * work_fork.c - fork implementation for blocking worker child.
 */
#include <config.h>
#include "ntp_workimpl.h"

#ifdef WORK_FORK
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "iosignal.h"
#include "ntp_stdlib.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "ntp_unixtime.h"
#include "ntp_worker.h"

/* 
 * pipe descriptors for both directions
 * 0 indicates invalid/unopened, not descriptor 0.
 */
static	int	parent_req_write_pipe;
	int	parent_resp_read_pipe;
static	int	child_req_read_pipe;
static	int	child_resp_write_pipe;
static	int	blocking_worker_pid;
static	volatile int worker_sighup_received;

static void		fork_blocking_child(void);
static RETSIGTYPE	worker_sighup(int);
static void		send_worker_home_atexit(void);


/*
 * exit_worker()
 *
 * On some systems _exit() is preferred to exit() for forked children.
 * For example, http://netbsd.gw.com/cgi-bin/man-cgi?fork++NetBSD-5.0
 * recommends _exit() to avoid double-flushing C runtime stream buffers
 * and also to avoid calling the parent's atexit() routines in the
 * child.  On those systems WORKER_CHILD_EXIT is _exit.  Since _exit
 * bypasses CRT cleanup, fflush() files we know might have output
 * buffered.
 */
void
exit_worker(
	int	exitcode
	)
{
	if (syslog_file != NULL)
		fflush(syslog_file);
	fflush(stdout);
	fflush(stderr);
	WORKER_CHILD_EXIT (exitcode);	/* space before ( required */
}


static RETSIGTYPE
worker_sighup(
	int sig
	)
{
	if (SIGHUP == sig)
		worker_sighup_received = 1;
}


int
worker_sleep(
	time_t	seconds
	)
{
	u_int sleep_remain;

	sleep_remain = (u_int)seconds;
	do {
		if (!worker_sighup_received)
			sleep_remain = sleep(sleep_remain);
		if (worker_sighup_received) {
			DPRINTF(1, ("worker SIGHUP with %us left to sleep",
				    sleep_remain));
			worker_sighup_received = 0;
			return -1;
		}
	} while (sleep_remain);

	return 0;
}


void
interrupt_worker_sleep(void)
{
	if (blocking_worker_pid != 0 &&
	    -1 == kill(blocking_worker_pid, SIGHUP))
		msyslog(LOG_ERR,
			"Unable to signal HUP to wake child pid %d: %m",
			blocking_worker_pid);
}


static void
send_worker_home_atexit(void)
{
	if (!parent_req_write_pipe)
		return;

	close(parent_req_write_pipe);
	parent_req_write_pipe = 0;
	close(parent_resp_read_pipe);
	parent_resp_read_pipe = 0;
	interrupt_worker_sleep();
	blocking_worker_pid = 0;
}


int
send_blocking_req_internal(
	blocking_pipe_header *	hdr,
	void *			data
	)
{
	int octets;
	int rc;

	NTP_REQUIRE(hdr != NULL);
	NTP_REQUIRE(data != NULL);
	NTP_REQUIRE(BLOCKING_REQ_MAGIC == hdr->magic_sig);

	if (!parent_req_write_pipe) {
		fork_blocking_child();
		NTP_INSIST(parent_req_write_pipe);
	}

	octets = sizeof(*hdr);
	rc = write(parent_req_write_pipe, hdr, octets);

	if (rc == octets) {
		octets = hdr->octets - sizeof(*hdr);
		rc = write(parent_req_write_pipe, data,
			   octets);

		if (rc == octets)
			return 0;
	}

	if (rc < 0)
		msyslog(LOG_ERR, "send_blocking_req_internal: pipe write: %m");
	else
		msyslog(LOG_ERR, "send_blocking_req_internal: short write %d of %d\n", rc, octets);

	exit(1);	/* otherwise would be return -1 */
}


blocking_pipe_header *
receive_blocking_req_internal(
	void
	)
{
	blocking_pipe_header hdr;
	blocking_pipe_header *req;
	int rc;

	NTP_REQUIRE(child_req_read_pipe);

	req = NULL;

	do {
		rc = read(child_req_read_pipe, &hdr, sizeof(hdr));
	} while (rc < 0 && EINTR == errno);

	if (rc < 0)
		msyslog(LOG_ERR,
			"receive_blocking_req_internal: pipe read %m\n");
	else if (0 == rc)
		DPRINTF(1, ("parent closed request pipe\n"));
	else if (rc != sizeof(hdr))
		msyslog(LOG_ERR,
			"receive_blocking_req_internal: short header read %d of %lu\n",
			rc, (u_long)sizeof(hdr));
	else {
		NTP_INSIST(sizeof(hdr) < hdr.octets && hdr.octets < 4 * 1024);
		req = emalloc(hdr.octets);
		memcpy(req, &hdr, sizeof(*req));

		rc = read(child_req_read_pipe,
			  (char *)req + sizeof(*req),
			  hdr.octets - sizeof(hdr));

		if (rc < 0)
			msyslog(LOG_ERR,
				"receive_blocking_req_internal: pipe data read %m\n");
		else if (rc != hdr.octets - sizeof(hdr))
			msyslog(LOG_ERR,
				"receive_blocking_req_internal: short read %d of %lu\n",
				rc, (u_long)(hdr.octets - sizeof(hdr)));
		else if (BLOCKING_REQ_MAGIC != req->magic_sig)
			msyslog(LOG_ERR,
				"receive_blocking_req_internal: packet header mismatch (0x%x)",
				req->magic_sig);
		else
			return req;
	}

	if (req)
		free(req);

	return NULL;
}


int
send_blocking_resp_internal(
	blocking_pipe_header *resp
	)
{
	int octets;
	int rc;

	NTP_REQUIRE(child_resp_write_pipe);

	octets = resp->octets;
	rc = write(child_resp_write_pipe, resp, octets);
	free(resp);

	if (octets == rc)
		return 0;

	if (rc < 0)
		DPRINTF(1, ("send_blocking_resp_internal: pipe write %m\n"));
	else
		DPRINTF(1, ("send_blocking_resp_internal: short write %d of %d\n", rc, octets));

	return -1;
}


blocking_pipe_header *
receive_blocking_resp_internal(
	void
	)
{
	blocking_pipe_header hdr;
	blocking_pipe_header *resp;
	int rc;

	NTP_REQUIRE(parent_resp_read_pipe);

	resp = NULL;

	rc = read(parent_resp_read_pipe, &hdr, sizeof(hdr));

	if (rc < 0)
		DPRINTF(1, ("receive_blocking_resp_internal: pipe read %m\n"));
	else if (rc != sizeof(hdr))
		DPRINTF(1, ("receive_blocking_resp_internal: short header read %d of %lu\n",
			    rc, (u_long)sizeof(hdr)));
	else if (BLOCKING_RESP_MAGIC != hdr.magic_sig)
		DPRINTF(1, ("receive_blocking_resp_internal: header mismatch (0x%x)\n",
			    hdr.magic_sig));
	else {
		NTP_INSIST(sizeof(hdr) < hdr.octets && hdr.octets < 16 * 1024);
		resp = emalloc(hdr.octets);
		memcpy(resp, &hdr, sizeof(*resp));

		rc = read(parent_resp_read_pipe,
			  (char *)resp + sizeof(*resp),
			  hdr.octets - sizeof(hdr));

		if (rc < 0)
			DPRINTF(1, ("receive_blocking_resp_internal: pipe data read %m\n"));
		else if (rc < hdr.octets - sizeof(hdr))
			DPRINTF(1, ("receive_blocking_resp_internal: short read %d of %lu\n",
				    rc, (u_long)(hdr.octets - sizeof(hdr))));
		else
			return resp;
	}

	if (resp)
		free(resp);

	return NULL;
}


#if defined(HAVE_DROPROOT) && defined(WORK_FORK)
void
fork_deferred_worker(void)
{
	NTP_REQUIRE(droproot);
	NTP_REQUIRE(root_dropped);

	if (parent_req_write_pipe && !blocking_worker_pid)
		fork_blocking_child();
}
#endif


static void
fork_blocking_child(
	void
	)
{
	static int atexit_installed;
	static int blocking_pipes[4];
	int childpid;
	int keep_fd;
	int fd;

	/*
	 * parent and child communicate via a pair of pipes.
	 * 
	 * 0 child read request
	 * 1 parent write request
	 * 2 parent read response
	 * 3 child write response
	 */
	if (!parent_req_write_pipe) {
		if (pipe(&blocking_pipes[0]) < 0 ||
		    pipe(&blocking_pipes[2]) < 0) {
			msyslog(LOG_ERR, "unable to create worker pipes: %m");
			exit(1);
		}

		/*
		 * Move the descriptors the parent will keep open out of the
		 * low descriptors preferred by C runtime buffered FILE *.
		 */
		parent_req_write_pipe = move_fd(blocking_pipes[1]);
		parent_resp_read_pipe = move_fd(blocking_pipes[2]);
		/* zero is reserved to mean invalid/not open */
		NTP_INSIST(parent_req_write_pipe != 0);
		NTP_INSIST(parent_resp_read_pipe != 0);
		/*
		 * wake any worker child on orderly shutdown of the
		 * daemon so that it can notice the broken pipes and
		 * go away promptly.
		 */
		if (!atexit_installed) {
			atexit(&send_worker_home_atexit);
			atexit_installed = 1;
		}
	}

#ifdef HAVE_DROPROOT
	/* defer the fork until after root is dropped */
	if (droproot && !root_dropped)
		return;
#endif
	if (syslog_file != NULL)
		fflush(syslog_file);
	fflush(stdout);
	fflush(stderr);

	signal_no_reset(SIGCHLD, SIG_IGN);

	childpid = fork();
	if (-1 == childpid) {
		msyslog(LOG_ERR, "unable to fork worker: %m");
		exit(1);
	}

	if (childpid) {
		/* this is the parent */
		NLOG(NLOG_SYSEVENT)
			msyslog(LOG_INFO,
				"forked worker child (pid %d)",
				childpid);
		blocking_worker_pid = childpid;

		/* close the child's pipe descriptors. */
		close(blocking_pipes[0]);
		close(blocking_pipes[3]);

		/* wire into I/O loop */
		update_resp_pipe_fd(parent_resp_read_pipe, 0);

		return;		/* parent returns */
	}

	/*
	 * In the child, close all files except stdin, stdout, stderr,
	 * and the two child ends of the pipes.
	 */
	child_req_read_pipe = blocking_pipes[0];
	child_resp_write_pipe = blocking_pipes[3];

	kill_asyncio(0);
	closelog();
	if (syslog_file != NULL) {
		fclose(syslog_file);
		syslog_file = NULL;
		syslogit = 1;
	}
	keep_fd = max(child_req_read_pipe, child_resp_write_pipe);
	for (fd = 3; fd < keep_fd; fd++)
		if (fd != child_req_read_pipe && 
		    fd != child_resp_write_pipe)
			close(fd);
	close_all_beyond(keep_fd);
	/*
	 * We get signals from refclock serial I/O on NetBSD in the
	 * worker if we do not reset SIGIO's handler to the default.
	 * It is not conditionalized for NetBSD alone because on
	 * systems where it is not needed, it is harmless, and that
	 * allows us to handle unknown others with NetBSD behavior.
	 * [Bug 1386]
	 */
#if defined(USE_SIGIO)
	signal_no_reset(SIGIO, SIG_DFL);
#elif defined(USE_SIGPOLL)
	signal_no_reset(SIGPOLL, SIG_DFL);
#endif
	signal_no_reset(SIGHUP, worker_sighup);
	init_logging("ntpd_intres", 0);
	setup_logfile(0);

	/*
	 * And now back to the portable code
	 */
	exit_worker(blocking_child_common());
}

/*
 * worker_idle_timer_fired()
 *
 * The parent starts this timer when the last pending response has been
 * received from the child, making it idle, and clears the timer when a
 * request is dispatched to the child.  Once the timer expires, the
 * child is sent packing.
 *
 * This is called when worker_idle_timer is nonzero and less than or
 * equal to current_time, and is responsible for resetting
 * worker_idle_timer.
 *
 * Note there are implementations in both work_fork.c and work_thread.c
 * that should remain in sync.
 */
void
worker_idle_timer_fired(void)
{
	NTP_REQUIRE(0 == intres_req_pending);

	worker_idle_timer = 0;
	blocking_worker_pid = 0;
	close(parent_req_write_pipe);
	parent_req_write_pipe = 0;
	update_resp_pipe_fd(parent_resp_read_pipe, 1);
	close(parent_resp_read_pipe);
	parent_resp_read_pipe = 0;
}


#else	/* !WORK_FORK follows */
char work_fork_nonempty_compilation_unit;
#endif
