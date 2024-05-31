# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""Run a suite of tests on the listed binaries."""
from __future__ import print_function
import os
import os.path
import waflib.Context
import waflib.Logs
import waflib.Utils


NTPCLIENTS = "main/ntpclients"
NTPD = "main/ntpd"
NTPFROB = "main/ntpfrob"
NTPTIME = "main/ntptime"
POPEN = waflib.Utils.subprocess.Popen

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

    if not os.path.exists(cmd[0]):
        addLog("YELLOW", prefix + " SKIPPING (does not exist)")
        return None

    if python:
        cmd = python + list(cmd)
    p = POPEN(cmd, env={'PYTHONPATH': '%s/main/tests/pylib' %
                        waflib.Context.out_dir},
              universal_newlines=True,
              stdin=waflib.Utils.subprocess.PIPE,
              stdout=waflib.Utils.subprocess.PIPE,
              stderr=waflib.Utils.subprocess.PIPE,
              cwd=waflib.Context.out_dir)

    stdout, stderr = p.communicate()

    if expected in (stdout.strip(), stderr.strip()):
        addLog("GREEN", prefix + "  OK")
        return True
    addLog("RED", prefix + "  FAILED")
    addLog("PINK", "Expected: " + expected)
    if stdout:
        addLog("PINK", "Got (stdout): " + stdout)
    if stderr:
        addLog("PINK", "Got (stderr): " + stderr)
    return False


def etl_cases(installed, version, params):
    """Convert cmd_list_* to run() arguments."""
    ret = []
    for dest_dir, bld_dir, name, flag in params:
        if not installed:
            prefix = waflib.Context.out_dir + os.sep + bld_dir
        else:
            prefix = waflib.Options.options.destdir + dest_dir
        target = prefix + os.sep + name
        ret.append([[target, flag], name + " " + version])
    return sorted(ret)


def cmd_bin_test(ctx):
    """Run a suite of binary tests."""
    BIN = ctx.env.BINDIR
    SBIN = ctx.env.SBINDIR
    fails = 0

    cmd_list = [
        (BIN, NTPCLIENTS, "ntpleapfetch", "--version"),
        (SBIN, NTPD, "ntpd", "--version"),
        (BIN, NTPFROB, "ntpfrob", "-V"),
        (BIN, NTPTIME, "ntptime", "-V"),
    ]
    cmd_list_python = [
        (BIN, NTPCLIENTS, "ntpdig", "--version"),
        (BIN, NTPCLIENTS, "ntpkeygen", "--version"),
        (BIN, NTPCLIENTS, "ntpq", "--version"),
        (BIN, NTPCLIENTS, "ntpsnmpd", "--version"),
        (BIN, NTPCLIENTS, "ntpsweep", "--version"),
        (BIN, NTPCLIENTS, "ntptrace", "--version"),
        (BIN, NTPCLIENTS, "ntpwait", "--version"),
    ]
    cmd_list_python_argparse = [  # Need argparse
        (BIN, NTPCLIENTS, "ntplogtemp", "--version"),
        (BIN, NTPCLIENTS, "ntpviz", "--version"),
    ]
    cmd_list_python_curses = [  # Need python curses
        (BIN, NTPCLIENTS, "ntpmon", "--version"),
    ]

    with open(str(ctx.bldnode) + "/VERSION.bld") as fp:
        version = fp.read().strip()

    if ctx.env['PYTHON_ARGPARSE']:
        cmd_list_python += cmd_list_python_argparse

    if ctx.env['PYTHON_CURSES']:
        cmd_list_python += cmd_list_python_curses

    INSTALL = False
    if 'install' == ctx.cmd:
        cmd_list += cmd_list_python
        cmd_list_python = []
        INSTALL = True

    for cmd in etl_cases(INSTALL, version, cmd_list):
        if not run(cmd[0], cmd[1]):
            fails += 1

    if 1 == fails:
        bin_test_summary(ctx)
        ctx.fatal("1 binary test failed!")
    elif 1 < fails:
        bin_test_summary(ctx)
        ctx.fatal("%d binary tests failed!" % fails)
