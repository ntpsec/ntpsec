from __future__ import print_function
import re
from os.path import exists
from waflib.Utils import subprocess
from waflib.Logs import pprint

cmd_map = {
        ("main/ntpd/ntpd",              "-invalid"):        br'.*must be run as root, not uid.*',
        ("main/ntpclients/ntpdig",      "time.apple.com"):  br'.*time.apple.com.*',
        ("main/ntpfrob/ntpfrob",        "-h"):              br'.*illegal option.*',
        ("main/ntpfrob/ntpfrob",        "-b 100000"):       br".*Bumping clock by 100000 microseconds.*",
        ("main/ntpclients/ntpkeygen",   "-M"):              br'.*Generating new md5 file and link.*',
        ("main/ntpclients/ntpq",        "-p"):              br'.*remote.*jitter.*',
        ("main/ntptime/ntptime",        None):              br'.*ntp_gettime\(\) returns code 0 \(OK\).*',
        ("main/attic/sht",               "2:r"):             br'.*reader.*',

# XXX: Need to figure out how to test this.
#       ("main/attic/hist",                        ""):                                  br'',

# Perl library
#       ("main/ntpclients/ntptrace",        ""):                                  br'',
#       ("main/ntpclients/ntpwait",                  ""):                                  br'',
#       ("main/ntpclients/ntpsweep",   ""):                      br'',
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

        p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=None, cwd="build")

        stdout, stderr = p.communicate()

        regex = re.compile(reg)

        if regex.match(stdout) or regex.match(stderr):
                check = True

        if check:
                pprint("GREEN", "  OK")
                return False
        else:
                pprint("RED", "  FAILED")
                return True


def cmd_bin_test(ctx, config):
        fail = True

        for cmd in sorted(cmd_map):
                fail = run(cmd, cmd_map[cmd])

        if fail:
                pprint("RED", "Tests failed!")
#               ctx.fatal("Failed")

#cmd_bin_test(None, None)
