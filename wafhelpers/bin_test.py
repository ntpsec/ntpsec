"""Run a suite of tests on the listed binaries."""
from __future__ import print_function
import os
import os.path
import sys
import waflib.Context
import waflib.Logs
import waflib.Utils
sys.path.insert(0, "%s/main/tests/pylib" % waflib.Context.out_dir)
import ntp.util

version = ntp.util.stdversion()
Popen = waflib.Utils.subprocess.Popen

cmd_smap = {
    ("main/ntpd/ntpd", "--version"): "ntpd %s\n" % version,
}
cmd_map = {
    ("main/ntpclients/ntpleapfetch", "--version"): "ntpleapfetch %s\n"
                                                   % version,
    ("main/ntpfrob/ntpfrob", "-V"): "ntpfrob %s\n" % version,
    ("main/ntptime/ntptime", "-V"): "ntptime %s\n" % version
}
cmd_map_python = {
    ("main/ntpclients/ntpdig", "--version"): "ntpdig %s\n" % version,
    ("main/ntpclients/ntpkeygen", "--version"): "ntpkeygen %s\n" % version,
    ("main/ntpclients/ntpq", "--version"): "ntpq %s\n" % version,
    ("main/ntpclients/ntpsnmpd", "--version"): "ntpsnmpd %s\n" % version,
    ("main/ntpclients/ntpsweep", "--version"): "ntpsweep %s\n" % version,
    ("main/ntpclients/ntptrace", "--version"): "ntptrace %s\n" % version,
    ("main/ntpclients/ntpwait", "--version"): "ntpwait %s\n" % version
}
# Need argparse
cmd_map_python_argparse = {
    ("main/ntpclients/ntplogtemp", "--version"): "ntplogtemp %s\n" % version,
    ("main/ntpclients/ntpviz", "--version"): "ntpviz %s\n" % version,
}
# Need python curses
cmd_map_python_curses = {
    ("main/ntpclients/ntpmon", "--version"): "ntpmon %s\n" % version,
}

test_logs = []


def addLog(color, text):
    """Add to the log."""
    test_logs.append((color, text))


def bin_test_summary(_):
    """Print out the log."""
    for i in test_logs:
        waflib.Logs.pprint(i[0], i[1])


def run(cmd, reg, pythonic, apath=None, environ=None):
    """Run an individual non-python test."""
    check = False
    odir = apath if apath else waflib.Context.out_dir

    environ = {} if environ is None else environ
    if apath:
        cmd = [os.sep.join(['.'] + cmd[0].split(os.sep)[-1:])] + list(cmd[1:])
    prefix = "running: " + " ".join(cmd)
    if not os.path.exists("%s/%s" % (odir, cmd[0])):
        addLog("YELLOW", '"%s" SKIPPING (does not exist)' % "%s/%s" % (odir, cmd[0]))
        return None
    if not apath:
        if pythonic:
            cmd = [sys.executable] + list(cmd)
            prefix = "running: " + " ".join(cmd)

    p = Popen(cmd, universal_newlines=True,
              env=environ, cwd=odir,
              stdin=waflib.Utils.subprocess.PIPE,
              stdout=waflib.Utils.subprocess.PIPE,
              stderr=waflib.Utils.subprocess.PIPE)

    stdout, stderr = p.communicate()

    if reg in (stdout, stderr):
        check = True

    if check:
        addLog("GREEN", prefix + "  OK")
        return True
    addLog("RED", prefix + "  FAILED")
    addLog("PINK", "Expected: " + reg)
    if stdout:
        addLog("PINK", "Got (stdout): " + stdout)
    if stderr:
        addLog("PINK", "Got (stderr): " + stderr)
    return False


def cmd_bin_test(ctx):
    """Run a suite of binary tests."""
    fails = 0
    skips = 0
    env = {}

    destdir = os.path.abspath(os.environ.get('DESTDIR', '/'))
    path = spath = None

    if ctx.cmd == 'install':
        pypath = destdir + ctx.env.PYTHONARCHDIR  # + os.sep + 'ntp'
        env = {'PYTHONPATH': pypath} if destdir != '/' else {}
        path = destdir + os.sep if destdir != '/' else '/'
        spath = path + ctx.env.SBINDIR[1:] + os.sep
        path = path + ctx.env.BINDIR[1:] + os.sep
        addLog('BLUE', 'Following relative to: %s' % spath)

    if ctx.env['PYTHON_ARGPARSE']:
        cmd_map_python.update(cmd_map_python_argparse)

    if ctx.env['PYTHON_CURSES']:
        cmd_map_python.update(cmd_map_python_curses)

    for cmd in sorted(cmd_smap):
        status = run(cmd, cmd_smap[cmd], False, spath, env)
        fails = fails + 1 if status is False else fails
        skips = skips + 1 if status is None else skips

    if ctx.cmd == 'install':
        addLog('BLUE', 'Following relative to: %s' % path)

    for cmd in sorted(cmd_map):
        status = run(cmd, cmd_map[cmd], False, path, env)
        fails = fails + 1 if status is False else fails
        skips = skips + 1 if status is None else skips

    for cmd in sorted(cmd_map_python):
        status = run(cmd, cmd_map_python[cmd], True, path, env)
        fails = fails + 1 if status is False else fails
        skips = skips + 1 if status is None else skips

    if skips > 0 or fails > 0:
        bin_test_summary(ctx)
    if skips == 1:
        waflib.Logs.warn("1 binary test skipped!")
    elif skips > 1:
        waflib.Logs.warn("%d binary tests skipped!" % skips)

    if fails >= 7:
        pass
    if fails == 1:
        ctx.fatal("1 binary test failed!")
    elif fails > 1:
        ctx.fatal("%d binary tests failed!" % fails)
