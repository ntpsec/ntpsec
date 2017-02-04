import sys
from waflib.Logs import pprint


def check_seccomp(ctx):

    if not ctx.options.enable_seccomp:
        return
    if ctx.env.DEST_OS != "linux":
        return

    ctx.check_cc(header_name="seccomp.h", mandatory=False)
    ctx.check_cc(lib="seccomp", comment="seccomp library", mandatory=False)

    if ctx.get_define("HAVE_SECCOMP_H") and ctx.env.LIB_SECCOMP:
        ctx.define("HAVE_SECCOMP", 1)
    else:
        pprint("RED", "Warning libseccomp and headers")
        pprint("RED", "Fedora needs libseccomp-devel")
        pprint("RED", "Debian needs libseccomp-dev")
