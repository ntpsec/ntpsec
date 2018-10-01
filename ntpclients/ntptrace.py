#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ntptrace - trace peers of an NTP server

Usage: ntptrace [-n | --numeric] [-m number | --max-hosts=number]
                [-r hostname | --host=hostname] [--help | --more-help]
                [-V | --version]
                hostname

See the manual page for details.
"""
# SPDX-License-Identifier: BSD-2-Clause

from __future__ import print_function

import getopt
import re
import subprocess
import sys

try:
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntptrace: can't find Python NTP library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


def get_info(host):
    info = ntp_read_vars(0, [], host)
    if info is None or 'stratum' not in info:
        return

    info['offset'] = round(float(info['offset']) / 1000, 6)
    info['syncdistance'] = \
        (float(info['rootdisp']) + (float(info['rootdelay']) / 2)) / 1000

    return info


def get_next_host(peer, host):
    info = ntp_read_vars(peer, ["srcadr"], host)
    if info is None:
        return
    return info['srcadr']


def ntp_read_vars(peer, vars, host):
    obsolete = {'phase': 'offset',
                'rootdispersion': 'rootdisp'}

    if not len(vars):
        do_all = True
    else:
        do_all = False
    outvars = {}.fromkeys(vars)

    if do_all:
        outvars['status_line'] = {}

    cmd = ["ntpq", "-n", "-c", "rv %s %s" % (peer, ",".join(vars))]
    if host is not None:
        cmd.append(host)

    try:
        # sadly subprocess.check_output() is not in Python 2.6
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT)
        out = proc.communicate()[0]
        output = out.decode('utf-8').splitlines()
    except subprocess.CalledProcessError as e:
        print("Could not start ntpq: %s" % e.output, file=sys.stderr)
        raise SystemExit(1)
    except OSError as e:
        print("Could not start ntpq: %s" % e.strerror, file=sys.stderr)
        raise SystemExit(1)

    for line in output:
        if re.search(r'Connection refused', line):
            return

        match = re.search(r'^asso?c?id=0 status=(\S{4}) (\S+), (\S+),', line,
                          flags=re.IGNORECASE)
        if match:
            outvars['status_line']['status'] = match.group(1)
            outvars['status_line']['leap'] = match.group(2)
            outvars['status_line']['sync'] = match.group(3)

        iterator = re.finditer(r'(\w+)=([^,]+),?\s?', line)
        for match in iterator:
            key = match.group(1)
            val = match.group(2)
            val = re.sub(r'^"([^"]+)"$', r'\1', val)
            if key in obsolete:
                key = obsolete[key]
            if do_all or key in outvars:
                outvars[key] = val

    return outvars


usage = r"""ntptrace - trace peers of an NTP server
USAGE: ntptrace [-<flag> [<val>] | --<name>[{=| }<val>]]... [host]

    -n, --numeric                Print IP addresses instead of hostnames
    -m, --max-hosts=num          Maximum number of peers to trace
    -r, --host=str               Single remote host
    -?, --help                   Display usage information and exit
        --more-help              Pass the extended usage text through a pager
    -V, --version                Output version information and exit

Options are specified by doubled hyphens and their name or by a single
hyphen and the flag character.""" + "\n"

try:
    (options, arguments) = getopt.getopt(
        sys.argv[1:], "m:nr:?V",
        ["help", "host=", "max-hosts=", "more-help", "numeric", "version"])
except getopt.GetoptError as err:
    sys.stderr.write(str(err) + "\n")
    raise SystemExit(1)

numeric = False
maxhosts = 99
host = '127.0.0.1'

for (switch, val) in options:
    if switch == "-m" or switch == "--max-hosts":
        errmsg = "Error: -m parameter '%s' not a number\n"
        maxhosts = ntp.util.safeargcast(val, int, errmsg, usage)
    elif switch == "-n" or switch == "--numeric":
        numeric = True
    elif switch == "-r" or switch == "--host":
        host = val
    elif switch == "-?" or switch == "--help" or switch == "--more-help":
        print(usage, file=sys.stderr)
        raise SystemExit(0)
    elif switch == "-V" or switch == "--version":
        print("ntptrace %s" % ntp.util.stdversion())
        raise SystemExit(0)

if len(arguments):
    host = arguments[0]

hostcount = 0

while True:
    hostcount += 1

    info = get_info(host)

    if info is None:
        break

    if not numeric:
        host = ntp.util.canonicalize_dns(host)

    print("%s: stratum %d, offset %f, synch distance %f" %
          (host, int(info['stratum']), info['offset'], info['syncdistance']),
          end='')
    if int(info['stratum']) == 1:
        print(", refid '%s'" % info['refid'], end='')
    print()

    if (int(info['stratum']) == 0 or int(info['stratum']) == 1 or
            int(info['stratum']) == 16):
        break

    if re.search(r'^127\.127\.\d{1,3}\.\d{1,3}$', info['refid']):
        break

    if hostcount == maxhosts:
        break

    next_host = get_next_host(info['peer'], host)

    if next_host is None:
        break
    if re.search(r'^127\.127\.\d{1,3}\.\d{1,3}$', next_host):
        break

    host = next_host
