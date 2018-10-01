#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""\
ntpwait - Wait for ntpd to stabilize the system clock.
USAGE: ntpwait [-n tries] [-s sleeptime] [-v] [-h]

    -n, --tries=num              Number of times to check ntpd
    -s, --sleep=num              How long to sleep between tries
    -v, --verbose                Be verbose
    -h, --help                   Issue help
    -V, --version                Output version information and exit

Options are specified by doubled hyphens and their name or by a single
hyphen and the flag character.

A spurious 'not running' message can result from queries being disabled.
"""
# SPDX-License-Identifier: BSD-2-Clause

# Because we've actually seen this fail on a ^C during import of ntp.packet.
import signal
import sys
signal.signal(signal.SIGINT, lambda signal, frame: sys.exit(2))

import getopt
import re
import time
import socket

try:
    import ntp.magic
    import ntp.packet
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntpwait: can't find Python NTP library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


class Unbuffered(object):
    def __init__(self, stream):
        self.stream = stream

    def write(self, data):
        self.stream.write(data)
        self.stream.flush()

    def __getattr__(self, attr):
        return getattr(self.stream, attr)


if __name__ == "__main__":
    try:
        (options, arguments) = getopt.getopt(sys.argv[1:], "hn:s:vV", [
            "tries=", "sleep=", "verbose", "help", "version"
        ])
    except getopt.GetoptError as err:
        sys.stderr.write(str(err) + "\n")
        raise SystemExit(2)
    tries = 100
    sleep = 6
    verbose = 0
    for (switch, val) in options:
        if switch in ("-n", "--tries"):
            errmsg = "Error: -n parameter '%s' not a number\n"
            tries = ntp.util.safeargcast(val, int, errmsg, __doc__)
        elif switch in ("-s", "--sleep"):
            errmsg = "Error: -s parameter '%s' not a number\n"
            sleep = ntp.util.safeargcast(val, int, errmsg, __doc__)
        elif switch in ("-v", "--verbose"):
            verbose += 1
        elif switch in ("-h", "--help"):
            sys.stdout.write(__doc__)
            raise SystemExit(0)
        elif switch in ("-V", "--version"):
            print("ntpwait %s" % ntp.util.stdversion())
            raise SystemExit(0)

    # Autoflush stdout
    sys.stdout = Unbuffered(sys.stdout)

    basetime = ntp.util.monoclock()
    if verbose:
        sys.stdout.write("Waiting for ntpd to synchronize...  ")

    for i in range(1, tries):
        session = ntp.packet.ControlSession()
        # session.debug = 4
        if not session.openhost("localhost"):
            if verbose:
                sys.stdout.write("\bntpd is not running!\n")
            continue

        msg = None
        try:
            msg = session.doquery(2)     # Request system variables
        except ntp.packet.ControlException as e:
            sys.stderr.write("localhost: timed out, nothing received\n")
            sys.stderr.write(e.message)
        except socket.error:
            if verbose:
                sys.stdout.write("\b" + "*+:."[i % 4])
            time.sleep(sleep)
            continue

        if verbose >= 2:
            sys.stderr.write(repr(session.response) + "\n")

        if msg and msg.startswith("***"):
            if verbose:
                sys.stdout.write("\b" + msg + "\n")
            sys.exit(1)

        m = re.search(r"leap=([^,]*),", repr(session.response))
        if m:
            leap = int(m.group(1))
        else:
            sys.stdout.write("\bLeap status not available\n")
            sys.exit(1)

        if leap == ntp.magic.LEAP_NOTINSYNC:
            if verbose:
                sys.stdout.write("\b" + "*+:."[i % 4])
            if i < tries:
                time.sleep(sleep)
            continue

        if leap in (ntp.magic.LEAP_NOWARNING, ntp.magic.LEAP_ADDSECOND,
                    ntp.magic.LEAP_DELSECOND):
            # We could check "sync" here to make sure we like the source...
            if verbose:
                sys.stdout.write("\bOK! (%.1f seconds)\n" %
                                 (ntp.util.monoclock() - basetime))
            sys.exit(0)

        sys.stdout.write("\bUnexpected 'leap' status <%s>\n" % leap)
        sys.exit(1)

    if verbose:
        sys.stdout.write("\bNo!\nntpd did not synchronize.\n")
    sys.exit(1)

# end
