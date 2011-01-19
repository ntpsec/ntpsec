/*
 * work_thread.c - threads implementation for blocking worker child.
 */
#include <config.h>
#include "ntp_workimpl.h"

#ifdef WORK_THREAD

#include <stdio.h>
#include <ctype.h>

#include "ntp_stdlib.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "ntp_unixtime.h"
#include "ntp_worker.h"

#define CHILD_EXIT_REQ	((blocking_pipe_header *)(intptr_t)-1)
#define WORKITEMS_ALLOC_INC	16
#define RESPONSES_ALLOC_INC	4

/*
 * blocking workitems and blocking_responses are dynamically-sized
 * one-dimensional arrays of pointers to blocking worker requests and
 * responses.
 */
blocking_pipe_header **blocking_workitems;
size_t blocking_workitems_alloc;
size_t next_workitem;
blocking_pipe_header **blocking_responses;
size_t blocking_responses_alloc;
size_t next_response;
HANDLE blocking_child_thread;	/* non-NULL when active */
/* event handles (semaphore references) */
HANDLE child_is_blocking;
HANDLE wake_blocking_child;
HANDLE blocking_response_ready;
HANDLE wake_scheduled_sleep;

static void	start_blocking_thread(void);
unsigned WINAPI	blocking_thread(void *);
static void	ensure_workitems_empty_slot(void);
static void	ensure_workresp_empty_slot(void);
static int	queue_req_pointer(blocking_pipe_header *);


void
exit_worker(
	int	exitcode
	)
{
	_endthreadex(exitcode);
}


int
worker_sleep(
	time_t	seconds
	)
{
	DWORD rc;

	DEBUG_REQUIRE(seconds * 1000 < MAXDWORD);
	rc = WaitForSingleObject(wake_scheduled_sleep, (DWORD)seconds * 1000);
	DEBUG_INSIST(WAIT_FAILED != rc);
	return (WAIT_TIMEOUT == rc)
		   ? 0
		   : -1;
}


void
interrupt_worker_sleep(void)
{
	SetEvent(wake_scheduled_sleep);
}


static void
ensure_workitems_empty_slot(void)
{
	const size_t each = sizeof(blocking_workitems[0]);
	size_t new_alloc;
	size_t old_octets;
	size_t new_octets;

	if (blocking_workitems != NULL &&
	    NULL == blocking_workitems[next_workitem])
		return;

	new_alloc = blocking_workitems_alloc + WORKITEMS_ALLOC_INC;
	old_octets = blocking_workitems_alloc * each;
	new_octets = new_alloc * each;
	blocking_workitems = erealloc_zero(blocking_workitems,
					   new_octets,
					   old_octets);
	if (0 == next_workitem)
		next_workitem = blocking_workitems_alloc;
	blocking_workitems_alloc = new_alloc;
}


static void
ensure_workresp_empty_slot(void)
{
	const size_t each = sizeof(blocking_responses[0]);
	size_t new_alloc;
	size_t old_octets;
	size_t new_octets;

	if (blocking_responses != NULL &&
	    NULL == blocking_responses[next_response])
		return;

	new_alloc = blocking_responses_alloc + RESPONSES_ALLOC_INC;
	old_octets = blocking_responses_alloc * each;
	new_octets = new_alloc * each;
	blocking_responses = erealloc_zero(blocking_responses,
					   new_octets,
					   old_octets);
	if (0 == next_response)
		next_response = blocking_responses_alloc;
	blocking_responses_alloc = new_alloc;
}


/*
 * queue_req_pointer() - append a work item or idle exit request to
 *			 blocking_workitems[].
 */
static int
queue_req_pointer(
	blocking_pipe_header *	hdr
	)
{
	blocking_workitems[next_workitem] = hdr;
	next_workitem = (1 + next_workitem) % blocking_workitems_alloc;

	/*
	 * We only want to signal the wakeup event if the child is
	 * blocking on it, which is indicated by setting the blocking
	 * event.  Wait with zero timeout to test.
	 */
	if (WAIT_OBJECT_0 == WaitForSingleObject(child_is_blocking, 0))
		SetEvent(wake_blocking_child);

	return 0;
}


int
send_blocking_req_internal(
	blocking_pipe_header *	hdr,
	void *			data
	)
{
	blocking_pipe_header *	threadcopy;

	DEBUG_REQUIRE(hdr != NULL);
	DEBUG_REQUIRE(data != NULL);
	DEBUG_REQUIRE(BLOCKING_REQ_MAGIC == hdr->magic_sig);

	ensure_workitems_empty_slot();
	if (NULL == blocking_child_thread) {
		ensure_workresp_empty_slot();
		start_blocking_thread();
	}

	threadcopy = emalloc(hdr->octets);
	memcpy(threadcopy, hdr, sizeof(*hdr));
	memcpy((char *)threadcopy + sizeof(*hdr),
	       data, hdr->octets - sizeof(*hdr));

	return queue_req_pointer(threadcopy);
}


blocking_pipe_header *
receive_blocking_req_internal(
	void
	)
{
	static size_t next_workeritem;
	blocking_pipe_header *req;
	int once;

	once = 1;

	/* we block here when idle */
	if (NULL == blocking_workitems[next_workeritem]) {
		SetEvent(child_is_blocking);
		while (NULL == blocking_workitems[next_workeritem])
			if (WAIT_OBJECT_0 != 
			    WaitForSingleObject(wake_blocking_child, INFINITE)) {
				DPRINTF(1, ("fatal receive_blocking_req_internal wait code\n"));
				exit(-1);
			}
		ResetEvent(child_is_blocking);
	}

	req = blocking_workitems[next_workeritem];
	blocking_workitems[next_workitem] = NULL;
	next_workeritem = (1 + next_workeritem) %
			  blocking_workitems_alloc;

	if (CHILD_EXIT_REQ == req)	/* idled out */
		req = NULL;

	return req;
}


int
send_blocking_resp_internal(
	blocking_pipe_header *resp
	)
{
	ensure_workresp_empty_slot();

	blocking_responses[next_response] = resp;
	next_response = (1 + next_response) % blocking_responses_alloc;

	SetEvent(blocking_response_ready);

	return 0;
}


blocking_pipe_header *
receive_blocking_resp_internal(
	void
	)
{
	static size_t next_workresp;
	blocking_pipe_header *	retval;

	retval = blocking_responses[next_workresp];

	if (NULL != retval) {
		blocking_responses[next_workresp] = NULL;
		next_workresp = (1 + next_workresp) %
				blocking_responses_alloc;
		DEBUG_ENSURE(BLOCKING_RESP_MAGIC == retval->magic_sig);
	}

	return retval;
}


static void
start_blocking_thread(
	void
	)
{
	unsigned blocking_thread_id;

	/*
	 * create sync events (semaphores)
	 * child_is_blocking initially unset
	 * wake_blocking_child initially unset
	 *
	 * Child waits for wake_blocking_child to be set after
	 * setting child_is_blocking.  wake_blocking_child and
	 * blocking_response_ready are auto-reset, so wake one
	 * waiter and become unset (unsignalled) in one operation.
	 * blocking_response_ready is created by the IO
	 * completion port initialization code and used before
	 * any blocking requests are made.
	 */
	if (NULL == child_is_blocking) {
		/* manual reset using ResetEvent() */
		child_is_blocking = CreateEvent(NULL, TRUE, FALSE, NULL);
		/* auto reset - release from wait resets it */
		wake_blocking_child = CreateEvent(NULL, FALSE, FALSE, NULL);
		wake_scheduled_sleep = CreateEvent(NULL, FALSE, FALSE, NULL);
		/*
		 * blocking_response_ready event is created in
		 * init_io_completion_port(), so it can be waited on
		 * before any blocking worker is started.
		 */
	} else {
		ResetEvent(child_is_blocking);
		ResetEvent(wake_scheduled_sleep);
		ResetEvent(wake_blocking_child);
		ResetEvent(blocking_response_ready);
	}

	blocking_child_thread =
		(HANDLE)_beginthreadex(
			NULL, 
			0, 
			blocking_thread, 
			NULL, 
			CREATE_SUSPENDED, 
			&blocking_thread_id);

	if (NULL == blocking_child_thread) {
		DPRINTF(1, ("fatal can not start blocking thread\n"));
		exit(-1);
	}
	/* remember the thread priority is only within the process class */
	if (!SetThreadPriority(blocking_child_thread,
			       THREAD_PRIORITY_BELOW_NORMAL)) {
		DPRINTF(1, ("Error setting blocking thread priority\n"));
	}

	ResumeThread(blocking_child_thread);

	/* wait for the child to set child_is_blocking the first time */
	while (WAIT_OBJECT_0 != WaitForSingleObject(child_is_blocking, INFINITE))
		/* null stmt */;
}

/*
 * blocking_thread - thread functions have WINAPI calling convention
 */
unsigned WINAPI
blocking_thread(
	void *UnusedThreadArg
	)
{
	UNUSED_ARG(UnusedThreadArg);

	return blocking_child_common();
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
	DEBUG_REQUIRE(0 == intres_req_pending);

	worker_idle_timer = 0;
	if (NULL != blocking_child_thread) {
		queue_req_pointer(CHILD_EXIT_REQ);
		blocking_child_thread = NULL;
	}
}


#else	/* !WORK_THREAD follows */
char work_thread_nonempty_compilation_unit;
#endif
