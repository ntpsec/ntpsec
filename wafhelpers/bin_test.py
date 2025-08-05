# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""Run a suite of tests on the listed binaries.

| command | DESTDIR | value | tests |
| === | === | === | === |
| bin_test, build, check | n/a | False as TEST_BUILD | build/main |
| install | no | True as TEST_INSTALL | $PREFIX/bin, $PREFIX/sbin |
| install | yes | None as TEST_NO | nothing |
| * | n/a | None as TEST_NO | nothing |
"""
from __future__ import print_function
import os
import os.path
import sys
import waflib.Context
import waflib.Logs
import waflib.Utils


NTPCLIENTS = "main/ntpclients"
NTPD = "main/ntpd"
NTPFROB = "main/ntpfrob"
NTPTIME = "main/ntptime"
POPEN = waflib.Utils.subprocess.Popen
TEST_BUILD = 0
TEST_NO = 1
TEST_INSTALL = 2

test_logs = []


def addLog(color, text):
    """Add to the log."""
    test_logs.append((color, text))


def bin_test_summary(_ctx):
    """Print out the log."""
    for i in test_logs:
        waflib.Logs.pprint(i[0], i[1])


def run(cmd, expected):
    """Run an individual test."""
    prefix = "running: " + " ".join(cmd)

    if not os.path.exists(cmd[0]):
        addLog("YELLOW", prefix + " SKIPPING (does not exist)")
        return None

    p = POPEN(cmd, env={"PATH": os.environ["PATH"]},
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


def do_we_test(ctx):
    """Evaluate if we should test and how."""
    if ctx.cmd in ["build", "check", "bin_test"]:
        return TEST_BUILD
    if ctx.cmd == "install":
        if bool(waflib.Options.options.destdir):
            waflib.Logs.pprint(
                "YELLOW", "bin_test disabled in DESTDIR installs."
            )
            return TEST_NO
        return TEST_INSTALL
    if "uninstall" == ctx.cmd:
        return TEST_NO
    # Probably add more cases here...
    waflib.Logs.pprint(
        "RED", "Please report that bin_test is broken for `./waf %s`!"
        % ctx.cmd
    )
    sys.exit(1)


def etl_cases(ctx, version, params):
    """Convert cmd_list_* to run() arguments."""
    ret = []
    installed = do_we_test(ctx)
    if installed is TEST_NO:
        return []
    for dest_dir, bld_dir, name, flag in params:
        if installed != TEST_INSTALL:
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
    rets = []

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
        (BIN, NTPCLIENTS, "ntplogtemp", "--version"),
        (BIN, NTPCLIENTS, "ntpviz", "--version"),
    ]
    cmd_list_python_gps = [  # Needs gps
        (BIN, NTPCLIENTS, "ntploggps", "--version"),
    ]
    cmd_list_python_curses = [  # Needs python curses
        (BIN, NTPCLIENTS, "ntpmon", "--version"),
    ]

    with open(str(ctx.bldnode) + "/VERSION.bld") as fp:
        version = fp.read().strip()

    if ctx.env['PYTHON_GPS']:
        cmd_list_python += cmd_list_python_gps

    if ctx.env['PYTHON_CURSES']:
        cmd_list_python += cmd_list_python_curses

    for cmd in etl_cases(ctx, version, cmd_list):
        rets.append(run(cmd[0], cmd[1]))
    if 'none' != ctx.env['ntpc']:
        for cmd in etl_cases(ctx, version, cmd_list_python):
            rets.append(run(cmd[0], cmd[1]))

    def test_s(number):
        """Return values for string formatting."""
        return number, '' if 1 == number else 's'

    skips = sum(int(None is i) for i in rets)
    fails = sum(int(False is i) for i in rets)
    if skips:
        waflib.Logs.pprint('YELLOW', "%d binary test%s skipped!"
                           % test_s(skips))
    if fails:
        # on failure print number of failed tests and the summary
        waflib.Logs.pprint(
            "RED", "%d binary test%s failed!" % test_s(fails)
        )
        bin_test_summary(ctx)
        sys.exit(1)
