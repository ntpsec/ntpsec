#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ntpsweep - print various information about given NTP servers
USAGE: ntpsweep [-<flag> [<val>] | --<name>[{=| }<val>]]... [hostfile]

    -h, --host=str             Host to execute actions on
    -l, --host-list=str        Comma-delimited list of Hosts to
                                   execute actions on
    -p, --peers                Recursively list all peers a host syncs to
    -m, --maxlevel=num         Traverse peers up to this level
                                   (4 is a reasonable number)
    -s, --strip=str            Strip this string from hostnames
    -V, --version              Output version information and exit

Options are specified by doubled hyphens and their name or by a single
hyphen and the flag character.
"""
# SPDX-License-Identifier: BSD-2-clause
#
# Python translation by ESR of a Perl script written long ago by
# Hans Lambermont <ntpsweep@lambermont.dyndns.org>
#
# It is unclear how useful this will be under modern conditions (e.g. most
# hosts refuse to be queried, and requests to them will just time out).

from __future__ import print_function

import os
import sys
import getopt

try:
    import ntp.packet
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntpsweep: can't find Python NTP library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


def ntp_peers(host):
    """Return: a list of peer IP addrs for a specified host,
            empty list if query failed.
    """
    try:
        with os.popen("ntpq -npw " + host) as rp:
            hostlines = rp.readlines()[2:]      # Drop display header
            # Strip tally mark from first field
            return [ln.split()[0][1:] for
                    ln in hostlines if ln[0] in " x.-+#*o"]
    except OSError:
        return []


def scan_host(host, level):
    stratum = 0
    offset = 0
    daemonversion = ""
    system = ""
    processor = ""
    known_host = False

    if host in known_host_info:
        known_host = True
    else:
        session = ntp.packet.ControlSession()
        session.openhost(host)
        sysvars = session.readvar()

        # got answers ? If so, go on.
        if isinstance(sysvars, dict):
            stratum = sysvars['stratum']
            offset = sysvars['offset']
            daemonversion = sysvars['version']
            system = sysvars['system']
            processor = sysvars['processor']

            # Shorten daemon_version string.
            # daemonversion =~ s/(|Mon|Tue|Wed|Thu|Fri|Sat|Sun).*$//
            daemonversion = daemonversion.replace("version=", "")
            daemonversion = daemonversion.replace("ntpd ", "")
            daemonversion = daemonversion.replace("(", "").replace(")", "")
            daemonversion = daemonversion.replace("beta", "b")
            daemonversion = daemonversion.replace("multicast", "mc")

            # Shorten system string. Note, the assumptions here
            # are very old, reflecting ancient big-iron Unixes
            system = system.replace("UNIX/", "")
            system = system.replace("RELEASE", "r")
            system = system.replace("CURRENT", "c")

            # Shorten processor string
            processor = processor.replace("unknown", "")

        # got answers ? If so, go on.
        if daemonversion and recurse:
            # Consider doing something more intelligent on failure
            # than simply returning an empty list.  Though it might
            # be the right thing to do under modern conditions in
            # which most hosts will refuse to be queried.
            known_host_peers[host] = ntp_peers(host)

        # Collect info on host
        if stratum:
            known_host_info[host] = "%2d %9.3f %-11s %-12s %s"  \
                                    % (stratum, offset, daemonversion[:11],
                                       system[:12], processor[0:9])
        else:
            # Stratum level 0 is considered invalid
            known_host_info[host] = " ?"

    if stratum or known_host:   # Valid or known host
        printhost = (' ' * level) + (ntp.util.canonicalize_dns(host) or host)
        # Shorten host string
        if strip:
            printhost = printhost.replace(strip, "")
        # append number of peers in brackets if requested and valid
        if (recurse and (known_host_info[host] != " ?") and
           (host in known_host_peers)):
            printhost += " (%d)" % len(known_host_peers[host])
        # Finally print complete host line
        print("%-32s %s" % (printhost[:32], known_host_info[host]))
        if recurse and (maxlevel == 0 or level < maxlevel):
            trace.append(host)
            # Loop through peers
            for peer in known_host_peers[host]:
                if peer in trace:
                    # we've detected a loop!
                    printhost = (' ' * (level + 1)) + "= " + peer
                    # Shorten host string
                    if strip:
                        printhost = printhost.replace(strip, "")
                    print("%-32s" % printhost[:32])
                else:
                    # FIXME: Ugh! Magic-address assumption.
                    # Needed to deal with peers running legacy NTP.
                    # Might cause problems in the future.  First
                    # part of the guard is an attempt to skip
                    # NTPsec-style clock IDs.
                    if peer[0].isdigit() and not peer.startswith("127"):
                        scan_host(peer, level + 1)
    else:   # We did not get answers from this host
        printhost = (' ' * level) + (ntp.util.canonicalize_dns(host) or host)
        if strip:
            printhost = printhost.replace(strip, "")
        print("%-32s  ?" % printhost[:32])


if __name__ == '__main__':
    try:
        (options, arguments) = getopt.getopt(
            sys.argv[1:], "h:l:m:ps:?V",
            ["host=", "host-list=", "maxlevel=", "peers", "strip=", "version"])
    except getopt.GetoptError as err:
        sys.stderr.write(str(err) + "\n")
        raise SystemExit(1)
    hostlist = []
    maxlevel = 1
    recurse = False
    strip = ""
    for (switch, val) in options:
        if switch == "-h" or switch == "--host":
            hostlist = [val]
        elif switch == "-l" or switch == "--host-list":
            hostlist = val.split(",")
        elif switch == "-m" or switch == "--maxlevel":
            errmsg = "Error: -m parameter '%s' not a number\n"
            maxlevel = ntp.util.safeargcast(val, int, errmsg, __doc__)
        elif switch == "-p" or switch == "--peers":
            recurse = True
        elif switch == "-s" or switch == "--strip":
            strip = val
        elif switch == "-?" or switch == "--help":
            print(__doc__, file=sys.stderr)
            raise SystemExit(0)
        elif switch == "-V" or switch == "--version":
            print("ntpsweep %s" % ntp.util.stdversion())
            raise SystemExit(0)

    try:
        if arguments:
            hostlist += [ln.strip() for ln in open(arguments[0]).readlines()]
    except IOError:
        sys.stderr.write("Host file not found.\n")
        raise SystemExit(1)

    if not hostlist:
        hostlist = ["localhost"]

    # Print header
    print("""\
Host                             st offset(s) version     system       processor
--------------------------------+--+---------+-----------+------------+---------\
""")

    known_host_info = {}
    known_host_peers = {}
    trace = []
    for host in hostlist:
        try:
            scan_host(host, 0)
        except ntp.packet.ControlException as e:
            sys.stderr.write(e.message + "\n")
            continue
    sys.exit(0)

# end
