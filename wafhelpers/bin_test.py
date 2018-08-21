from __future__ import print_function
import re
from os.path import exists
from waflib.Utils import subprocess
from waflib.Logs import pprint

cmd_map = {
    ("main/ntpd/ntpd", "-invalid"): br'.*must be run as root, not uid.*',
    ("main/ntpclients/ntpdig", "time.apple.com"): br'.*time.apple.com.*',
    ("main/ntpfrob/ntpfrob", "-h"): br'usage:',
    ("main/ntpfrob/ntpfrob", "-b 100000"):
        br"Bumping clock by 100000 microseconds",
    ("main/ntpclients/ntpkeygen", None):
        br'Generating new ',
    ("main/ntpclients/ntpq", "-p"): br'.*remote.*jitter.*',
    ("main/ntptime/ntptime", None):
        br'ntp_gettime\(\) returns code 0 \(OK\)',
    ("main/attic/sht", "2:r"): br'reader',

    # Perl library
    #       ("main/ntpclients/ntptrace", ""): br'',
    #       ("main/ntpclients/ntpwait", ""): br'',
    #       ("main/ntpclients/ntpsweep", ""): br'',
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

    regex = re.compile(reg)

    if regex.match(stdout) or regex.match(stderr):
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
