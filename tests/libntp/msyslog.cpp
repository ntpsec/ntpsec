#include "libntptest.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include <errno.h>
// format_errmsg() is normally private to msyslog.c
void	format_errmsg	(char *, size_t, const char *, int);
};

class msyslogTest : public libntptest {
};

// msnprintf()
TEST_F(msyslogTest, msnprintf)
{
#define FMT_PREFIX "msyslog.cpp ENOENT: "
	char exp_buf[512];
	char act_buf[512];

	snprintf(exp_buf, sizeof(exp_buf), FMT_PREFIX "%s",
		 strerror(ENOENT));
	errno = ENOENT;
	msnprintf(act_buf, sizeof(act_buf), FMT_PREFIX "%m");
	EXPECT_STREQ(exp_buf, act_buf);
}

TEST_F(msyslogTest, msnprintfLiteralPercentm)
{
	char exp_buf[32];
	char act_buf[32];

	snprintf(exp_buf, sizeof(exp_buf), "%%m");
	errno = ENOENT;
	msnprintf(act_buf, sizeof(act_buf), "%%m");
	EXPECT_STREQ(exp_buf, act_buf);
}

TEST_F(msyslogTest, msnprintfBackslashLiteralPercentm)
{
	char exp_buf[32];
	char act_buf[32];

	snprintf(exp_buf, sizeof(exp_buf), "\%%m");
	errno = ENOENT;
	msnprintf(act_buf, sizeof(act_buf), "\%%m");
	EXPECT_STREQ(exp_buf, act_buf);
}

TEST_F(msyslogTest, msnprintfBackslashPercent)
{
	char exp_buf[32];
	char act_buf[32];

	snprintf(exp_buf, sizeof(exp_buf), "\%s", strerror(ENOENT));
	errno = ENOENT;
	msnprintf(act_buf, sizeof(act_buf), "\%m");
	EXPECT_STREQ(exp_buf, act_buf);
}

TEST_F(msyslogTest, msnprintfHangingPercent)
{
	static char fmt[] = "percent then nul term then non-nul %\0oops!";
	char exp_buf[64];
	char act_buf[64];

	memset(exp_buf, 0, sizeof(exp_buf));
	memset(act_buf, 0, sizeof(act_buf));
	snprintf(exp_buf, sizeof(exp_buf), fmt);
	msnprintf(act_buf, sizeof(act_buf), fmt);
	EXPECT_STREQ(exp_buf, act_buf);
	EXPECT_STREQ("", act_buf + 1 + strlen(act_buf));
}

TEST_F(msyslogTest, format_errmsgHangingPercent)
{
	static char fmt[] = "percent then nul term then non-nul %\0oops!";
	char act_buf[64];

	memset(act_buf, 0, sizeof(act_buf));
	format_errmsg(act_buf, sizeof(act_buf), fmt, ENOENT);
	EXPECT_STREQ(fmt, act_buf);
	EXPECT_STREQ("", act_buf + 1 + strlen(act_buf));
}

