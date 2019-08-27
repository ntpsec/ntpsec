# check to see if strerror_r has type char*

# streror_r() has 2 APIs.
# our environment doesn't make a clean choice.

# There is code for mystrerror() in the bottom of msyslog.c
# This code tried to setup a #define for STRERROR_CHAR
# if strerror_r() returns char* rather than int

# Unfortunately, this test code compiles in the other case.
# It generates a warning on the type conversion from char* to int,
# but that's only a warning, so it "works".

# This uses -Werror which may not be portable.

# Another possibility is to run the test code,
# and have it check for 0/NULL which the int mode should return.


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
