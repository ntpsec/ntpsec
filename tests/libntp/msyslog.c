#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(msyslog);

TEST_SETUP(msyslog) {}

TEST_TEAR_DOWN(msyslog) {}


#include <stdio.h>
#include <string.h>
#include <errno.h>

static int     msnprintf(char *, size_t, const char *, ...) __sysloglike(3, 4);

static int
msnprintf(
	char *		buf,
	size_t		bufsiz,
	const char *	fmt,
	...
	)
{
	va_list	ap;
	int	rc;

	va_start(ap, fmt);
	rc = mvsnprintf(buf, bufsiz, fmt, ap);
	va_end(ap);

	return rc;
}

TEST(msyslog, msnprintfNullTarget)
{
	int	exp_cnt;
	int	act_cnt;

	exp_cnt = snprintf(NULL, 0, "%d", 123);
	errno = ENOENT;
	act_cnt = msnprintf(NULL, 0, "%d", 123);
	TEST_ASSERT_EQUAL(exp_cnt, act_cnt);
}


/* gcc 4.6 added support for push/pop
 * gcc 4.4 added support for -Wformat-contains-nul
 * Put this test last since we can't undo turning off some warnings. */
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wformat"
#else /* GCC */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
# pragma GCC diagnostic push
#endif
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
# pragma GCC diagnostic ignored "-Wformat-contains-nul"
#endif
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wformat"
#endif
TEST(msyslog, msnprintfHangingPercent)
{
	char exp_buf[64];
	char act_buf[64];
	int	exp_cnt;
	int	act_cnt;

	ZERO(exp_buf);
	ZERO(act_buf);
/* warning: format string contains '\0' within the string body [-Wformat] */
	exp_cnt = snprintf(exp_buf, sizeof(exp_buf), "percent then nul term then non-nul %\0oops!");
	act_cnt = msnprintf(act_buf, sizeof(act_buf), "percent then nul term then non-nul %\0oops!");

	TEST_ASSERT_EQUAL(exp_cnt, act_cnt);
	TEST_ASSERT_EQUAL_STRING(exp_buf, act_buf);
	TEST_ASSERT_EQUAL_STRING("", act_buf + 1 + strlen(act_buf));
}
#ifdef __clang__
#  pragma clang diagnostic pop
#else
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
# pragma GCC diagnostic pop
#endif
#endif

TEST_GROUP_RUNNER(msyslog) {
	RUN_TEST_CASE(msyslog, msnprintfHangingPercent)
	RUN_TEST_CASE(msyslog, msnprintfNullTarget)
}
