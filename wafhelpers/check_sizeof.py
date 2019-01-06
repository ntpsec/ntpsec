from waflib.Configure import conf
from waflib import Errors

SIZE_FRAG = """
%s
#include <stdio.h>
int main(void) {
    printf("%%lu", sizeof(%s));
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

    for size in range(2, 13):

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
