"""Run a suite of tests on the listed binaries."""
from __future__ import print_function
import os
import os.path
import sys
import waflib.Context
import waflib.Logs
import waflib.Utils
sys.path.insert(0, "%s/main/tests/pylib" % waflib.Context.out_dir)
import ntp.poly
import ntp.util

verStr = ntp.util.stdversion()

cmd_map = {
    ("main/ntpclients/ntpleapfetch", "--version"): "ntpleapfetch %s\n"
                                                   % verStr,
    ("main/ntpd/ntpd", "--version"): "ntpd %s\n" % verStr,
    ("main/ntpfrob/ntpfrob", "-V"): "ntpfrob %s\n" % verStr,
    ("main/ntptime/ntptime", "-V"): "ntptime %s\n" % verStr
}
cmd_map2 = {
    ("main/ntpclients/ntpdig", "--version"): "ntpdig %s\n" % verStr,
    ("main/ntpclients/ntpkeygen", "--version"): "ntpkeygen %s\n" % verStr,
    ("main/ntpclients/ntpq", "--version"): "ntpq %s\n" % verStr,
    ("main/ntpclients/ntplogtemp", "--version"): "ntplogtemp %s\n" % verStr,
    ("main/ntpclients/ntpsnmpd", "--version"): "ntpsnmpd %s\n" % verStr,
    ("main/ntpclients/ntpsweep", "--version"): "ntpsweep %s\n" % verStr,
    ("main/ntpclients/ntptrace", "--version"): "ntptrace %s\n" % verStr,
    ("main/ntpclients/ntpviz", "--version"): "ntpviz %s\n" % verStr,
    ("main/ntpclients/ntpwait", "--version"): "ntpwait %s\n" % verStr
}
cmd_map3 = {    # Need curses
    ("main/ntpclients/ntpmon", "--version"): "ntpmon %s\n" % verStr,
}

test_logs = []

def addLog(color, text):
    test_logs.append((color, text))

def bin_test_summary(ctx):
    for i in test_logs:
        waflib.Logs.pprint(i[0], i[1])

def run(cmd, reg, pythonic):
    """Run an individual non-python test."""
    check = False

    breg = ntp.poly.polybytes(reg)

    prefix = "running: " + " ".join(cmd)

    if not os.path.exists("%s/%s" % (waflib.Context.out_dir, cmd[0])):
        addLog("YELLOW", prefix + " SKIPPING (does not exist)")
        return False

    if pythonic:
        cmd = [sys.executable] + list(cmd)
    p = waflib.Utils.subprocess.Popen(cmd, env={'PYTHONPATH': '%s/main/tests/pylib' %
                                                 waflib.Context.out_dir},
                         stdin=waflib.Utils.subprocess.PIPE,
                         stdout=waflib.Utils.subprocess.PIPE,
                         stderr=waflib.Utils.subprocess.PIPE, cwd=waflib.Context.out_dir)

    stdout, stderr = p.communicate()

    if (stdout == breg) or (stderr == breg):
        check = True

    if check:
        addLog("GREEN", prefix + "  OK")
        return True
    addLog("RED", prefix + "  FAILED")
    addLog("PINK", "Expected: " + breg)
    if stdout:
        addLog("PINK", "Got (stdout): " + ntp.poly.polystr(stdout))
    if stderr:
        addLog("PINK", "Got (stderr): " + ntp.poly.polystr(stderr))
    return False


def cmd_bin_test(ctx, config):
    """Run a suite of binary tests."""
    fails = 0

    if ctx.env['PYTHON_CURSES']:
      for cmd in cmd_map3:
        cmd_map2[cmd] = cmd_map3[cmd]

    for cmd in sorted(cmd_map):
        if not run(cmd, cmd_map[cmd], False):
            fails += 1

    for cmd in sorted(cmd_map2):
        if not run(cmd, cmd_map2[cmd], True):
            fails += 1

    if 1 == fails:
        bin_test_summary(ctx)
        ctx.fatal("1 binary test failed!")
    elif 1 < fails:
        bin_test_summary(ctx)
        ctx.fatal("%d binary tests failed!" % fails)
