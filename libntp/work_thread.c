/*
 * work_thread.c - threads implementation for blocking worker child.
 */
#include "config.h"
#include "ntp_workimpl.h"

#ifdef USE_WORK_THREAD

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>

#include "ntp_stdlib.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "timespecops.h"
#include "ntp_worker.h"

#define CHILD_EXIT_REQ	((blocking_pipe_header *)(intptr_t)-1)
#define CHILD_GONE_RESP	CHILD_EXIT_REQ
#define WORKITEMS_ALLOC_INC	64
#define RESPONSES_ALLOC_INC	16

#ifdef OVERRIDE_THREAD_MINSTACKSIZE
#define THREAD_MINSTACKSIZE OVERRIDE_THREAD_MINSTACKSIZE
#else
#define THREAD_MINSTACKSIZE	(64U * 1024)
#endif

#ifndef DEVOLATILE
#define DEVOLATILE(type, var) ((type)(uintptr_t)(volatile void *)(var))
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#undef sem_init
#define sem_init(s,p,c) semaphore_create(mach_task_self(),s,SYNC_POLICY_FIFO,c)
#undef sem_destroy
#define sem_destroy(s) semaphore_destroy(mach_task_self(),*s)
#undef sem_wait
#define sem_wait(s) semaphore_wait(*s)
#undef sem_post
#define sem_post(s) semaphore_signal(*s)
#undef sem_timedwait
static int sem_timedwait_osx(sem_ref sem, const struct timespec *abs_timeout)
{
	mach_timespec_t wait_time;
	wait_time.tv_sec = abs_timeout->tv_sec;
	wait_time.tv_nsec = abs_timeout->tv_nsec;
	return semaphore_timedwait(*sem, wait_time);
}
#define sem_timedwait sem_timedwait_osx
#endif

#define thread_exit(c)	pthread_exit((void*)(size_t)(c))

#ifdef USE_WORK_PIPE
addremove_io_fd_func		addremove_io_fd;
#else
addremove_io_semaphore_func	addremove_io_semaphore;
#endif

static	void	start_blocking_thread(blocking_child *);
static	void	start_blocking_thread_internal(blocking_child *);
static	void	prepare_child_sems(blocking_child *);
static	int	wait_for_sem(sem_ref, struct timespec *);
static	void	ensure_workitems_empty_slot(blocking_child *);
static	void	ensure_workresp_empty_slot(blocking_child *);
static	int	queue_req_pointer(blocking_child *, blocking_pipe_header *);
static	void	cleanup_after_child(blocking_child *);
void *		blocking_thread(void *);
static	void	block_thread_signals(sigset_t *);


void
exit_worker(
	int	exitcode
	)
{
	thread_exit(exitcode);	/* see #define thread_exit */
}

int
worker_sleep(
	blocking_child *	c,
	time_t			seconds
	)
{
	struct timespec	until;
	int		rc;

	if (0 != clock_gettime(CLOCK_REALTIME, &until)) {
		msyslog(LOG_ERR, "worker_sleep: clock_gettime() failed: %m");
		return -1;
	}
	until.tv_sec += seconds;
	do {
		rc = wait_for_sem(c->wake_scheduled_sleep, &until);
	} while (-1 == rc && EINTR == errno);
	if (0 == rc)
		return -1;
	if (-1 == rc && ETIMEDOUT == errno)
		return 0;
	msyslog(LOG_ERR, "worker_sleep: sem_timedwait: %m");
	return -1;
}


void
interrupt_worker_sleep(void)
{
	u_int			idx;
	blocking_child *	c;

	for (idx = 0; idx < blocking_children_alloc; idx++) {
		c = blocking_children[idx];
		if (NULL == c || NULL == c->wake_scheduled_sleep)
			continue;
		sem_post(c->wake_scheduled_sleep);
	}
}


static void
ensure_workitems_empty_slot(
	blocking_child *c
	)
{
	const size_t	each = sizeof(blocking_children[0]->workitems[0]);
	size_t		new_alloc;
	size_t		old_octets;
	size_t		new_octets;
	void *		nonvol_workitems;


	if (c->workitems != NULL &&
	    NULL == c->workitems[c->next_workitem])
		return;

	new_alloc = c->workitems_alloc + WORKITEMS_ALLOC_INC;
	old_octets = c->workitems_alloc * each;
	new_octets = new_alloc * each;
	nonvol_workitems = DEVOLATILE(void *, c->workitems);
	c->workitems = erealloc_zero(nonvol_workitems, new_octets,
				     old_octets);
	if (0 == c->next_workitem)
		c->next_workitem = c->workitems_alloc;
	c->workitems_alloc = new_alloc;
}


static void
ensure_workresp_empty_slot(
	blocking_child *c
	)
{
	const size_t	each = sizeof(blocking_children[0]->responses[0]);
	size_t		new_alloc;
	size_t		old_octets;
	size_t		new_octets;
	void *		nonvol_responses;

	if (c->responses != NULL &&
	    NULL == c->responses[c->next_response])
		return;

	new_alloc = c->responses_alloc + RESPONSES_ALLOC_INC;
	old_octets = c->responses_alloc * each;
	new_octets = new_alloc * each;
	nonvol_responses = DEVOLATILE(void *, c->responses);
	c->responses = erealloc_zero(nonvol_responses, new_octets,
				     old_octets);
	if (0 == c->next_response)
		c->next_response = c->responses_alloc;
	c->responses_alloc = new_alloc;
}


/*
 * queue_req_pointer() - append a work item or idle exit request to
 *			 blocking_workitems[].
 */
static int
queue_req_pointer(
	blocking_child	*	c,
	blocking_pipe_header *	hdr
	)
{
	c->workitems[c->next_workitem] = hdr;
	c->next_workitem = (1 + c->next_workitem) % c->workitems_alloc;

	/*
	 * We only want to signal the wakeup event if the child is
	 * blocking on it, which is indicated by setting the blocking
	 * event.  Wait with zero timeout to test.
	 */
	/* !!!! if (WAIT_OBJECT_0 == WaitForSingleObject(c->child_is_blocking, 0)) */
		sem_post(c->blocking_req_ready);

	return 0;
}


int
send_blocking_req_internal(
	blocking_child *	c,
	blocking_pipe_header *	hdr,
	void *			data
	)
{
	blocking_pipe_header *	threadcopy;
	size_t			payload_octets;

	REQUIRE(hdr != NULL);
	REQUIRE(data != NULL);
	DEBUG_REQUIRE(BLOCKING_REQ_MAGIC == hdr->magic_sig);

	if (hdr->octets <= sizeof(*hdr))
		return 1;	/* failure */
	payload_octets = hdr->octets - sizeof(*hdr);

	ensure_workitems_empty_slot(c);
	if (NULL == c->thread_ref) {
		ensure_workresp_empty_slot(c);
		start_blocking_thread(c);
	}

	threadcopy = emalloc(hdr->octets);
	memcpy(threadcopy, hdr, sizeof(*hdr));
	memcpy((char *)threadcopy + sizeof(*hdr), data, payload_octets);

	return queue_req_pointer(c, threadcopy);
}


blocking_pipe_header *
receive_blocking_req_internal(
	blocking_child *	c
	)
{
	blocking_pipe_header *	req;
	int			rc;

	/*
	 * Child blocks here when idle.  SysV semaphores maintain a
	 * count and release from sem_wait() only when it reaches 0.
	 */
	do {
		rc = wait_for_sem(c->blocking_req_ready, NULL);
	} while (-1 == rc && EINTR == errno);
	INSIST(0 == rc);

	req = c->workitems[c->next_workeritem];
	INSIST(NULL != req);
	c->workitems[c->next_workeritem] = NULL;
	c->next_workeritem = (1 + c->next_workeritem) %
				c->workitems_alloc;

	if (CHILD_EXIT_REQ == req) {	/* idled out */
		send_blocking_resp_internal(c, CHILD_GONE_RESP);
		req = NULL;
	}

	return req;
}


int
send_blocking_resp_internal(
	blocking_child *	c,
	blocking_pipe_header *	resp
	)
{
	ensure_workresp_empty_slot(c);

	c->responses[c->next_response] = resp;
	c->next_response = (1 + c->next_response) % c->responses_alloc;

#ifdef USE_WORK_PIPE
	IGNORE(write(c->resp_write_pipe, "", 1));
#else
	sem_post(c->blocking_response_ready);
#endif

	return 0;
}


#ifndef USE_WORK_PIPE
void
handle_blocking_resp_sem(
	void *	context
	)
{
	HANDLE			ready;
	blocking_child *	c;
	u_int			idx;

	ready = (HANDLE)context;
	c = NULL;
	for (idx = 0; idx < blocking_children_alloc; idx++) {
		c = blocking_children[idx];
		if (c != NULL && c->thread_ref != NULL &&
		    ready == c->blocking_response_ready)
			break;
	}
	if (idx < blocking_children_alloc)
		process_blocking_resp(c);
}
#endif	/* !USE_WORK_PIPE */


blocking_pipe_header *
receive_blocking_resp_internal(
	blocking_child *	c
	)
{
	blocking_pipe_header *	removed;
#ifdef USE_WORK_PIPE
	int			rc;
	char			scratch[32];

	do {
		rc = read(c->resp_read_pipe, scratch, sizeof(scratch));
	} while (-1 == rc && EINTR == errno);
#endif
	removed = c->responses[c->next_workresp];
	if (NULL != removed) {
		c->responses[c->next_workresp] = NULL;
		c->next_workresp = (1 + c->next_workresp) %
				   c->responses_alloc;
		DEBUG_ENSURE(CHILD_GONE_RESP == removed ||
			     BLOCKING_RESP_MAGIC == removed->magic_sig);
	}
	if (CHILD_GONE_RESP == removed) {
		cleanup_after_child(c);
		removed = NULL;
	}

	return removed;
}


static void
start_blocking_thread(
	blocking_child *	c
	)
{

	DEBUG_INSIST(!c->reusable);

	prepare_child_sems(c);
	start_blocking_thread_internal(c);
}


static void
start_blocking_thread_internal(
	blocking_child *	c
	)
{
	pthread_attr_t	thr_attr;
	int		rc;
	int		pipe_ends[2];	/* read then write */
	bool		is_pipe;
	int		flags;
	size_t		stacksize;
	sigset_t	saved_sig_mask;

	rc = pipe_socketpair(&pipe_ends[0], &is_pipe);
	if (0 != rc) {
		msyslog(LOG_ERR, "start_blocking_thread: pipe_socketpair() %m");
		exit(1);
	}
	c->resp_read_pipe = move_fd(pipe_ends[0]);
	c->resp_write_pipe = move_fd(pipe_ends[1]);
	c->ispipe = is_pipe;
	flags = fcntl(c->resp_read_pipe, F_GETFL, 0);
	if (-1 == flags) {
		msyslog(LOG_ERR, "start_blocking_thread: fcntl(F_GETFL) %m");
		exit(1);
	}
	rc = fcntl(c->resp_read_pipe, F_SETFL, O_NONBLOCK | flags);
	if (-1 == rc) {
		msyslog(LOG_ERR,
			"start_blocking_thread: fcntl(F_SETFL, O_NONBLOCK) %m");
		exit(1);
	}
	(*addremove_io_fd)(c->resp_read_pipe, c->ispipe, false);
	pthread_attr_init(&thr_attr);
	pthread_attr_setdetachstate(&thr_attr, PTHREAD_CREATE_DETACHED);
	rc = pthread_attr_getstacksize(&thr_attr, &stacksize);
	if (0 != rc) {
		errno = rc;
		msyslog(LOG_ERR,
			"start_blocking_thread: pthread_attr_getstacksize %m");
	} else if (stacksize < THREAD_MINSTACKSIZE) {
		rc = pthread_attr_setstacksize(&thr_attr,
					       THREAD_MINSTACKSIZE);
		if (0 != rc) {
			errno = rc;
			msyslog(LOG_ERR,
				"start_blocking_thread: pthread_attr_setstacksize(0x%lx -> 0x%lx) %m",
				(u_long)stacksize,
				(u_long)THREAD_MINSTACKSIZE);
		}
	}
	pthread_attr_setscope(&thr_attr, PTHREAD_SCOPE_SYSTEM);
	c->thread_ref = emalloc_zero(sizeof(*c->thread_ref));
	block_thread_signals(&saved_sig_mask);
	rc = pthread_create(c->thread_ref, &thr_attr,
			    &blocking_thread, c);
	if (0 != rc) {
	    if (EAGAIN == errno) {
	        msyslog(LOG_ERR, "EAGAIN from pthread_create(), probably out of (locked) memory");
	    } else {
	        msyslog(LOG_ERR, "pthread_create() blocking child: %m");
	    }
	    exit(1);
	}
	pthread_sigmask(SIG_SETMASK, &saved_sig_mask, NULL);
	pthread_attr_destroy(&thr_attr);
}


/*
 * block_thread_signals()
 *
 * Temporarily block signals used by ntpd main thread, so that signal
 * mask inherited by child threads leaves them blocked.  Returns prior
 * active signal mask via pmask, to be restored by the main thread
 * after pthread_create().
 */
void
block_thread_signals(
	sigset_t *	pmask
	)
{
	sigset_t	block;

	sigemptyset(&block);
	sigaddset(&block, SIGALRM);
	sigaddset(&block, MOREDEBUGSIG);
	sigaddset(&block, LESSDEBUGSIG);
	sigaddset(&block, SIGINT);
	sigaddset(&block, SIGQUIT);
	sigaddset(&block, SIGTERM);
	sigaddset(&block, SIGBUS);
	sigemptyset(pmask);
	pthread_sigmask(SIG_BLOCK, &block, pmask);
}


/*
 * prepare_child_sems()
 *
 * create sync events (semaphores)
 * child_is_blocking initially unset
 * blocking_req_ready initially unset
 *
 * Child waits for blocking_req_ready to be set after
 * setting child_is_blocking.  blocking_req_ready and
 * blocking_response_ready are auto-reset, so wake one
 * waiter and become unset (unsignalled) in one operation.
 */
static void
prepare_child_sems(
	blocking_child *c
	)
{
	size_t	octets;

	if (NULL == c->blocking_req_ready) {
		octets = sizeof(*c->blocking_req_ready);
		octets += sizeof(*c->wake_scheduled_sleep);
		/* !!!! octets += sizeof(*c->child_is_blocking); */
		c->blocking_req_ready = emalloc_zero(octets);;
		c->wake_scheduled_sleep = 1 + c->blocking_req_ready;
		/* !!!! c->child_is_blocking = 1 + c->wake_scheduled_sleep; */
	} else {
		sem_destroy(c->blocking_req_ready);
		sem_destroy(c->wake_scheduled_sleep);
		/* !!!! sem_destroy(c->child_is_blocking); */
	}
	sem_init(c->blocking_req_ready, false, 0);
	sem_init(c->wake_scheduled_sleep, false, 0);
	/* !!!! sem_init(c->child_is_blocking, false, 0); */
}


static int
wait_for_sem(
	sem_ref			sem,
	struct timespec *	timeout		/* wall-clock */
	)
	/* pthreads wait_for_sem() follows */
{
	int rc;

	if (NULL == timeout)
		rc = sem_wait(sem);
	else
#ifdef sem_timedwait
		rc = sem_timedwait(sem, timeout);
#else
		/* No sem_timedwait on NetBSD - thread stays around forever. */
		rc = sem_wait(sem);
#endif

	return rc;
}


/*
 * blocking_thread - thread functions have WINAPI calling convention
 */
void *
blocking_thread(
	void *	ThreadArg
	)
{
	blocking_child *c;

	c = ThreadArg;
	exit_worker(blocking_child_common(c));

	/* NOTREACHED */
	return 0;
}


/*
 * req_child_exit() runs in the parent.
 */
int
req_child_exit(
	blocking_child *c
	)
{
	return queue_req_pointer(c, CHILD_EXIT_REQ);
}


/*
 * cleanup_after_child() runs in parent.
 */
static void
cleanup_after_child(
	blocking_child *	c
	)
{
	u_int	idx;

	DEBUG_INSIST(!c->reusable);
	free(c->thread_ref);
	c->thread_ref = NULL;
	c->thread_id = 0;
#ifdef USE_WORK_PIPE
	DEBUG_INSIST(-1 != c->resp_read_pipe);
	DEBUG_INSIST(-1 != c->resp_write_pipe);
	(*addremove_io_fd)(c->resp_read_pipe, c->ispipe, true);
	close(c->resp_write_pipe);
	close(c->resp_read_pipe);
	c->resp_write_pipe = -1;
	c->resp_read_pipe = -1;
#else
	DEBUG_INSIST(NULL != c->blocking_response_ready);
	(*addremove_io_semaphore)(c->blocking_response_ready, true);
#endif
	for (idx = 0; idx < c->workitems_alloc; idx++)
		c->workitems[idx] = NULL;
	c->next_workitem = 0;
	c->next_workeritem = 0;
	for (idx = 0; idx < c->responses_alloc; idx++)
		c->responses[idx] = NULL;
	c->next_response = 0;
	c->next_workresp = 0;
	c->reusable = true;
}


#else	/* !USE_WORK_THREAD follows */
char work_thread_nonempty_compilation_unit;
#endif
