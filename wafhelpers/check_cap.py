import sys
from waflib.Logs import pprint


def check_cap(ctx):

    if ctx.options.disable_droproot:
        return
    if ctx.env.DEST_OS != "linux":
        return

    ctx.check_cc(header_name="sys/prctl.h", mandatory=False)
    ctx.check_cc(header_name="sys/capability.h", mandatory=False)
    ctx.check_cc(lib="cap", comment="Capability library", mandatory=False)

    if ((ctx.get_define("HAVE_SYS_CAPABILITY_H")
         and ctx.get_define("HAVE_SYS_PRCTL_H") and ctx.env.LIB_CAP)):
        ctx.define("HAVE_LINUX_CAPABILITY", 1)
    else:
        pprint("RED", "Warning libcap and headers not installed")
        pprint("RED", "Fedora needs libcap-devel")
        pprint("RED", "Debian needs libcap-dev")
