from waflib.Errors import ConfigurationError

"""
10  check for: int i = PTHREAD_CREATE_JOINABLE;
8   pthread_detach doesn't exist
7   PTHREAD_CREATE_UNDETACHED exists
6   pthread_attr_init renamed from pthread_attr_create
5   PTHREAD_MUTEX_INITIALIZER was introduced
4   PTHREAD_MUTEX_INITIALIZER does not exist
"""

# From openldap.m4
#
# tests:
# pthread_detach() was dropped in Draft 8, it is present
#     in every other version
# PTHREAD_CREATE_UNDETACHED is only in Draft 7, it was called
#     PTHREAD_CREATE_JOINABLE after that
# pthread_attr_create was renamed to pthread_attr_init in Draft 6.
#     Draft 6-10 has _init, Draft 4-5 has _create.
# pthread_attr_default was dropped in Draft 6, only 4 and 5 have it
# PTHREAD_MUTEX_INITIALIZER was introduced in Draft 5. It's not
#     interesting to us because we don't try to statically
#     initialize mutexes. 5-10 has it.



def posix_thread_version_10(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>
int main() {
  return PTHREAD_CREATE_JOINABLE;
}
""",
		msg         = "Checking for POSIX thread version 10",
		mandatory   = True
	)


def posix_thread_version_8(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>

int main() {
  pthread_t handle;
  pthread_detach(handle);
}
""",
		msg         = "Checking for POSIX thread version 8",
		mandatory   = False
	)


def posix_thread_version_7(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>
#ifdef PTHREAD_CREATE_UNDETACHED
#error
#endif
int main() {}
""",
		msg         = "Checking for POSIX thread version 7",
		mandatory   = False
	)


def posix_thread_version_6a(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>

int main() {
  pthread_attr_t    attr;
  pthread_attr_init(&attr);
}
""",
		msg         = "Checking for POSIX thread version 6 (test 1)",
		mandatory   = True
	)


def posix_thread_version_6b(ctx):
	ctx.check_cc(
		fragment    = """
#if !defined(pthread_attr_default)
#error
#endif
int main() {}
""",
		msg         = "Checking for POSIX thread version 6 (test 2)",
		mandatory   = True
	)


def posix_thread_version_5(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>
#ifndef      PTHREAD_MUTEX_INITIALIZER
#error
#endif


#if !defined(pthread_attr_default)
#error
#endif
int main() {}
""",
		msg         = "Checking for POSIX thread version 5",
		mandatory   = True
	)


def posix_thread_version_4(ctx):
	ctx.check_cc(
		fragment    = """
#include <pthread.h>
#ifdef      PTHREAD_MUTEX_INITIALIZER
#error
#endif

#if defined(pthread_attr_default)
#error
#endif
int main() {}
""",
		msg         = "Checking for POSIX thread version 4",
		mandatory   = True
	)


def check_posix_thread_version(ctx):
	try:
		posix_thread_version_10(ctx)
	except ConfigurationError:
		pass
	else:
		ctx.env.POSIX_THREAD_VERISON = 10
		return


	try:
		posix_thread_version_8(ctx)
	except ConfigurationError:
		ctx.env.POSIX_THREAD_VERISON = 8
		return

	try:
		posix_thread_version_7(ctx)
	except ConfigurationError:
		ctx.env.POSIX_THREAD_VERISON = 7
		return


	try:
		posix_thread_version_6a(ctx)
		posix_thread_version_6b(ctx)
	except ConfigurationError:
		ctx.env.POSIX_THREAD_VERISON = 6
		return

	try:
		posix_thread_version_5(ctx)
	except ConfigurationError:
		ctx.env.POSIX_THREAD_VERISON = 5
		return


	try:
		posix_thread_version_4(ctx)
	except ConfigurationError:
		ctx.env.POSIX_THREAD_VERISON = 4
		return
	else:
		ctx.fatal("There is no supported POSIX threads implementation on this system.")
