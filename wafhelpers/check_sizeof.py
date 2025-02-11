# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from waflib.Configure import conf
from waflib import Errors

SIZE_FRAG = """
%s
#include <stdio.h>
int main(void) {
    printf("%%lu", (unsigned long)sizeof(%s));
    return 0;
}
"""


def check_sizeof_host(ctx, header, sizeof, mandatory=True):
    sizeof_ns = sizeof.replace(" ", "_")
    name = "NTP_SIZEOF_%s" % sizeof_ns.upper()

    header_snippet = ""
    if header:
        ctx.start_msg("Checking sizeof %s (%s)" % (sizeof, header))
        header_snippet = "#include <%s>" % header
    else:
        ctx.start_msg("Checking sizeof %s" % (sizeof))

    ctx.check_cc(
        fragment=SIZE_FRAG % (header_snippet, sizeof),
        define_name=name,
        execute=True,
        define_ret=True,
        quote=False,
        mandatory=mandatory,
        comment="Size of %s from <%s>" % (sizeof, header)
    )
    ctx.end_msg(ctx.get_define(name))


# Cross compile check.  Much slower so we do not run it all the time.

SIZE_FRAG_CROSS = """
%s
#include <sys/stat.h>
int main(void) {
  static int test_array [1 - 2 * !(((long int) (sizeof (%s))) <= %d)];
  test_array [0] = 0;
  return test_array[0];
}
"""


def check_sizeof_cross(ctx, header, sizeof, mandatory=True):
    sizeof_ns = sizeof.replace(" ", "_")
    name = "NTP_SIZEOF_%s" % sizeof_ns.upper()

    header_snippet = ""
    if header:
        ctx.start_msg("Checking sizeof %s (%s)" % (sizeof, header))
        header_snippet = "#include <%s>" % header
    else:
        ctx.start_msg("Checking sizeof %s" % (sizeof))

    for size in range(2, 99):

        try:
            ctx.check_cc(
                fragment=SIZE_FRAG_CROSS % (header_snippet, sizeof, size),
                features="c",
                execute=False,
                mandatory=mandatory,
            )
            ctx.define(name, size, comment="Size of %s from <%s>"
                       % (sizeof, header))
            ctx.end_msg(ctx.get_define(name))
            return
        except Errors.ConfigurationError:
            pass

    raise     # never reached.


@conf
def check_sizeof(*kwargs):
    if kwargs[0].env.ENABLE_CROSS:
        check_sizeof_cross(*kwargs)
    else:
        check_sizeof_host(*kwargs)


# timex slots are documented as long
# #if (__TIMESIZE == 64 && __WORDSIZE == 32)
#    they turn into long long
# This fails to build in the normal case.
# So we set NTP_TIMEX_LONG_LONG to 0
SIZE_FRAG_TIMEX = """
#include <sys/time.h>    /* for NetBSD */
#include <sys/timex.h>
#include <stdio.h>
int main(void) {
  struct timex dummy;
  long long *foo = &dummy.jitter;
  *foo = 1;  /* supress unused warning */
  if (*foo) printf("1");
  return 0;
}
"""

def check_timex(ctx):
    name = "NTP_TIMEX_LONG_LONG"
    ctx.start_msg("Checking for long long in struct timex" )
    ctx.check_cc(
        cflags="-Werror",
        fragment=SIZE_FRAG_TIMEX,
        define_name=name,
        execute=not ctx.env.ENABLE_CROSS,
        define_ret=True,
        quote=False,
        mandatory=False,
        comment="struct timex has long long"
    )
    ctx.end_msg(ctx.get_define(name))

