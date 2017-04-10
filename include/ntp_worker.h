/*
 * ntp_worker.h
 */

#ifndef GUARD_NTP_WORKER_H
#define GUARD_NTP_WORKER_H

#include "ntp_workimpl.h"

/* XXX: Why does OpenBSD need pthread.h here? */
#ifdef PLATFORM_OPENBSD
#include <pthread.h>
#endif

#ifdef USE_WORKER
# if defined(USE_WORK_THREAD) && defined(USE_WORK_PIPE)
#  ifdef HAVE_SEMAPHORE_H
#   include <semaphore.h>
#  endif
# endif
#include "ntp_stdlib.h"

typedef enum blocking_work_req_tag {
	BLOCKING_GETNAMEINFO,
	BLOCKING_GETADDRINFO,
} blocking_work_req;

typedef void (*blocking_work_callback)(blocking_work_req, void *, size_t, void *);

typedef enum blocking_magic_sig_e {
	BLOCKING_REQ_MAGIC  = 0x510c7ecf,
	BLOCKING_RESP_MAGIC = 0x510c7e54,
} blocking_magic_sig;

/*
 * The same header is used for both requests to and responses from
 * the child.  In the child, done_func and context are opaque.
 */
typedef struct blocking_pipe_header_tag {
	size_t			octets;
	blocking_magic_sig	magic_sig;
	blocking_work_req	rtype;
	u_int			child_idx;
	blocking_work_callback	done_func;
	void *			context;
} blocking_pipe_header;

# ifdef USE_WORK_THREAD
#  ifdef USE_WORK_PIPE
typedef pthread_t *	thr_ref;
#    ifdef __MACH__
#include <mach/task.h>
#include <mach/semaphore.h>
typedef semaphore_t* sem_ref;
#    else /* !__MACH__ */
typedef sem_t *		sem_ref;
#    endif /* !__MACH__ */
#  else
typedef HANDLE		thr_ref;
typedef HANDLE		sem_ref;
#  endif
# endif

#ifdef USE_WORK_THREAD
typedef struct blocking_child_tag {
/*
 * blocking workitems and blocking_responses are dynamically-sized
 * one-dimensional arrays of pointers to blocking worker requests and
 * responses.
 */
	int			reusable;
	thr_ref			thread_ref;
	u_int			thread_id;
	blocking_pipe_header * volatile * volatile 
				workitems;
	volatile size_t		workitems_alloc;
	size_t			next_workitem;	 /* parent */
	size_t			next_workeritem; /* child */
	blocking_pipe_header * volatile * volatile 
				responses;
	volatile size_t		responses_alloc;
	size_t			next_response;	/* child */
	size_t			next_workresp;	/* parent */
	/* event handles / sem_t pointers */
	/* sem_ref		child_is_blocking; */
	sem_ref			blocking_req_ready;
	sem_ref			wake_scheduled_sleep;
#ifdef USE_WORK_PIPE
	int			resp_read_pipe;	/* parent */
	int			resp_write_pipe;/* child */
	int			ispipe;
	void *			resp_read_ctx;	/* child */
#else
	sem_ref			blocking_response_ready;
#endif
} blocking_child;

#endif	/* USE_WORK_THREAD */

extern	blocking_child **	blocking_children;
extern	size_t			blocking_children_alloc;
extern	int			worker_per_query;	/* boolean */
extern	int			intres_req_pending;

extern	u_int	available_blocking_child_slot(void);
extern	int	queue_blocking_request(blocking_work_req, void *,
				       size_t, blocking_work_callback,
				       void *);
extern	int	queue_blocking_response(blocking_child *, 
					blocking_pipe_header *, size_t,
					const blocking_pipe_header *);
extern	void	process_blocking_resp(blocking_child *);
extern	int	send_blocking_req_internal(blocking_child *,
					   blocking_pipe_header *,
					   void *);
extern	int	send_blocking_resp_internal(blocking_child *,
					    blocking_pipe_header *);
extern	blocking_pipe_header *
		receive_blocking_req_internal(blocking_child *);
extern	blocking_pipe_header *
		receive_blocking_resp_internal(blocking_child *);
extern	int	blocking_child_common(blocking_child *);
extern	void	exit_worker(int)
			__attribute__ ((__noreturn__));
extern	int	worker_sleep(blocking_child *, time_t);
extern	void	worker_idle_timer_fired(void);
extern	void	interrupt_worker_sleep(void);
extern	int	req_child_exit(blocking_child *);
extern	int	pipe_socketpair(int fds[2], bool *is_pipe);
extern	void	kill_asyncio	(int);

# ifdef USE_WORK_PIPE
typedef	void	(*addremove_io_fd_func)(int, int, int);
extern	addremove_io_fd_func		addremove_io_fd;
# else
extern	void	handle_blocking_resp_sem(void *);
typedef	void	(*addremove_io_semaphore_func)(sem_ref, int);
extern	addremove_io_semaphore_func	addremove_io_semaphore;
# endif

#endif	/* USE_WORKER */

#endif	/* GUARD_NTP_WORKER_H */
