"""Run a suite of tests on the listed binaries."""
from __future__ import print_function
import os
import os.path
import sys
import waflib.Context
import waflib.Logs
import waflib.Utils

Popen = waflib.Utils.subprocess.Popen

cmd_map = {
    ("main/ntpclients/ntpleapfetch", "--version"): "ntpleapfetch %s\n",
    ("main/ntpd/ntpd", "--version"): "ntpd %s\n",
    ("main/ntpfrob/ntpfrob", "-V"): "ntpfrob %s\n",
    ("main/ntptime/ntptime", "-V"): "ntptime %s\n"
}
cmd_map_python = {
    ("main/ntpclients/ntpdig", "--version"): "ntpdig %s\n",
    ("main/ntpclients/ntpkeygen", "--version"): "ntpkeygen %s\n",
    ("main/ntpclients/ntpq", "--version"): "ntpq %s\n",
    ("main/ntpclients/ntpsnmpd", "--version"): "ntpsnmpd %s\n",
    ("main/ntpclients/ntpsweep", "--version"): "ntpsweep %s\n",
    ("main/ntpclients/ntptrace", "--version"): "ntptrace %s\n",
    ("main/ntpclients/ntpwait", "--version"): "ntpwait %s\n"
}
# Need argparse
cmd_map_python_argparse = {
    ("main/ntpclients/ntplogtemp", "--version"): "ntplogtemp %s\n",
    ("main/ntpclients/ntpviz", "--version"): "ntpviz %s\n",
}
# Need python curses
cmd_map_python_curses = {
    ("main/ntpclients/ntpmon", "--version"): "ntpmon %s\n",
}

test_logs = []


def addLog(color, text):
    """Add to the log."""
    test_logs.append((color, text))


def bin_test_summary(ctx):
    """Print out the log."""
    for i in test_logs:
        waflib.Logs.pprint(i[0], i[1])


def run(cmd, expected, python=None):
    """Run an individual test."""

    prefix = "running: " + " ".join(cmd)

    if not os.path.exists("%s/%s" % (waflib.Context.out_dir, cmd[0])):
        addLog("YELLOW", prefix + " SKIPPING (does not exist)")
        return False

    if python:
        cmd = python + list(cmd)
    p = Popen(cmd, env={'PYTHONPATH': '%s/main/tests/pylib' %
                        waflib.Context.out_dir},
              universal_newlines=True,
              stdin=waflib.Utils.subprocess.PIPE,
              stdout=waflib.Utils.subprocess.PIPE,
              stderr=waflib.Utils.subprocess.PIPE,
              cwd=waflib.Context.out_dir)

    stdout, stderr = p.communicate()

    if expected in (stdout, stderr):
        addLog("GREEN", prefix + "  OK")
        return True
    addLog("RED", prefix + "  FAILED")
    addLog("PINK", "Expected: " + expected)
    if stdout:
        addLog("PINK", "Got (stdout): " + stdout)
    if stderr:
        addLog("PINK", "Got (stderr): " + stderr)
    return False


def cmd_bin_test(ctx):
    """Run a suite of binary tests."""
    fails = 0

    cmd = ctx.env['PYTHON'] + ['-c',
        'from __future__ import print_function;'
        'import ntp.util;'
        'print(ntp.util.stdversion())']
    p = waflib.Utils.subprocess.Popen(cmd, env={'PYTHONPATH': '%s/main/tests/pylib' %
                                                 waflib.Context.out_dir},
                         universal_newlines=True,
                         stdin=waflib.Utils.subprocess.PIPE,
                         stdout=waflib.Utils.subprocess.PIPE,
                         stderr=waflib.Utils.subprocess.PIPE, cwd=waflib.Context.out_dir)
    version = p.communicate()[0].rstrip()

    if ctx.env['PYTHON_ARGPARSE']:
        cmd_map_python.update(cmd_map_python_argparse)

    if ctx.env['PYTHON_CURSES']:
        cmd_map_python.update(cmd_map_python_curses)

    for cmd in sorted(cmd_map):
        if not run(cmd, cmd_map[cmd] % version):
            fails += 1

    for cmd in sorted(cmd_map_python):
        if not run(cmd, cmd_map_python[cmd] % version, ctx.env['PYTHON']):
            fails += 1

    if 1 == fails:
        bin_test_summary(ctx)
        ctx.fatal("1 binary test failed!")
    elif 1 < fails:
        bin_test_summary(ctx)
        ctx.fatal("%d binary tests failed!" % fails)
