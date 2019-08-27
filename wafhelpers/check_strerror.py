# check to see if strerror_r has type char*

STRERROR_FRAG = """
#include <string.h>
int main(void) {
  char buf [100];
  const char *foo = strerror_r(6, buf, sizeof(buf));
  return foo == NULL;
}
"""


def check_strerror(ctx):
    old_CFLAGS = ctx.env.CFLAGS
    ctx.env.CFLAGS = ["-Werror"] + ctx.env.CFLAGS
    ctx.check_cc(
        fragment=STRERROR_FRAG,
        define_name="STRERROR_CHAR",
        features="c",
        msg="Checking if strerror_r returns char*",
        mandatory=False,
        comment="Whether strerror_r returns char*"
    )
    ctx.env.CFLAGS = old_CFLAGS
