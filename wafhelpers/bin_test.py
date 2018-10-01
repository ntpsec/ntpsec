from __future__ import print_function
import sys
from os.path import exists
from waflib.Utils import subprocess
from waflib.Logs import pprint

# Need the build form of util.py to get the version string
sys.path.insert(0, "build/main/pylib/")
import util

verStr = util.stdversion()

cmd_map = {
    ("main/ntpd/ntpd", "--version"): "ntpd %s\n" % verStr,
    ("main/ntpclients/ntpdig", "--version"): "ntpdig %s\n" % verStr,
    ("main/ntpclients/ntpkeygen", "--version"): "ntpkeygen %s\n" % verStr,
    ("main/ntpclients/ntpq", "--version"): "ntpq %s\n" % verStr,
    ("main/ntpclients/ntpmon", "--version"): "ntpmon %s\n" % verStr,
    ("main/ntpclients/ntpleapfetch", "--version"): "ntpleapfetch %s\n" % verStr,
    ("main/ntpclients/ntplogtemp", "--version"): "ntplogtemp %s\n" % verStr,
    ("main/ntpclients/ntpsnmpd", "--version"): "ntpsnmpd %s\n" % verStr,
    ("main/ntpclients/ntpsweep", "--version"): "ntpsweep %s\n" % verStr,
    ("main/ntpclients/ntptrace", "--version"): "ntptrace %s\n" % verStr,
    ("main/ntpclients/ntpviz", "--version"): "ntpviz %s\n" % verStr,
    ("main/ntpclients/ntpwait", "--version"): "ntpwait %s\n" % verStr,
    ("main/ntpfrob/ntpfrob", "-V"): "ntpfrob %s\n" % verStr,
    ("main/ntptime/ntptime", "-V"): "ntptime %s\n" % verStr,
}


# XXX: Needs to run in a thread with a timeout.
def run(cmd, reg):
    check = False

    if cmd[1] is None:
        cmd = [cmd[0]]

    print("running: ", " ".join(cmd), end="")

    if not exists("build/%s" % cmd[0]):
        pprint("YELLOW", " SKIPPING (does not exist)")
        return False

    p = subprocess.Popen(cmd, stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE, env=None, cwd="build")

    stdout, stderr = p.communicate()

    if (stdout == reg) or (stderr == reg):
        check = True

    if check:
        pprint("GREEN", "  OK")
        return True
    else:
        pprint("RED", "  FAILED")
        return False


def cmd_bin_test(ctx, config):
    fails = 0

    for cmd in sorted(cmd_map):
        if not run(cmd, cmd_map[cmd]):
            fails += 1

    if 1 == fails:
        ctx.fatal("1 binary test failed!")
    elif 1 < fails:
        ctx.fatal("%d binary tests failed!" % fails)

# cmd_bin_test(None, None)
