# What we really want to do here is test to see if the following program
# compiles *and exits with 9 status*.  Because we don't know how to check
# the return status we must settle for a simpler test.
'''
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int call_vsnprintf(char *dst, size_t sz, const char *fmt,...)
{
        va_list ap;
        int             rc;

        va_start(ap, fmt);
        rc = vsnprintf(dst, sz, fmt, ap);
        va_end(ap);

        return rc;
}

int main(void)
{
        char    sbuf[512];
        char    pbuf[512];
        int             slen;

        strcpy(sbuf, strerror(ENOENT));
        errno = ENOENT;
        slen = call_vsnprintf(pbuf, sizeof(pbuf), "%m",
                          "wrong");
        return strcmp(sbuf, pbuf);
}
'''


def check_vsprintfm(ctx):
    "Check for %m expanding to strerror(error) in glibc style."
    ctx.check_cc(
        fragment='''
#include <features.h>
int main(void)
{
        #ifndef __GLIBC__
        # error __GLIBC__ is not defined
        #endif
}
''',
        define_name="VSNPRINTF_PERCENT_M",
        msg="Checking for %m expansion in vsnprintf(3)",
        mandatory=False,
        comment="%m expanding to strerror(error) in glibc style")
