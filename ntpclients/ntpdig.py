#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ntpdig - simple SNTP client

"""
# SPDX-License-Identifier: BSD-2-clause
# This code runs identically under Python 2 and Python 3. Keep it that way!
from __future__ import print_function, division
import getopt
import math
import select
import socket
import sys
import time

try:
    import ntp.magic
    import ntp.packet
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntpdig: can't find Python NTP library -- check PYTHONPATH.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)
# This code is somewhat stripped down from the legacy C version.  It
# does however have one additional major feature; it can filter
# out falsetickers from multiple samples, like the ntpdate of old,
# rather than just taking the first reply it gets.
#
# Listening to broadcast addresses is not implemented because that is
# impossible to secure. KOD recording is also not implemented, as it
# can too easily be spammed.  Thus, the options -b and -K are not
# implemented.
#
# There are no version 3 NTP servers left, so the -o version for setting
# NTP version has been omitted.
#
# Because ntpdig doesn't use symmetric-peer mode (it never did, and NTPsec has
# abolished that mode because it was a security hazard), there's no need to
# set the packet source port, so -r/--usereservedport has been dropped.
# If this option ever needs to be reinstated, the magic is described here:
# http://stackoverflow.com/questions/2694212/socket-set-source-port-number
# and would be s.bind(('', 123)) right after the socket creation.
#
# The -w/--wait and -W/--nowait options only made sense with asynchronous
# DNS.  Asynchronous DNS was absurd overkill for this application, we are
# not looking up 20,000 hosts here.  It has not been implemented, so neither
# have these options.
#
# Finally, logging to syslog by default was a design error, violating
# Unix principles, that has been fixed. To get this behavior when
# running in a script, redirect standard error to logger(1).
#
# The one new option in this version is -p, borrowed from ntpdate.


def read_append(s, packets, packet, sockaddr):
    d, a = s.recvfrom(1024)
    if debug >= 2:
        ntp.packet.dump_hex_printable(d)
    if credentials:
        if not ntp.packet.Authenticator.have_mac(d):
            if debug:
                log("no MAC on reply from %s" % packet.hostname)
        if not credentials.verify_mac(d):
            packet.trusted = False
            log("MAC verification on reply from %s failed"
                % sockaddr[0])
        elif debug:
            log("MAC verification on reply from %s succeeded"
                % sockaddr[0])
    pkt = ntp.packet.SyncPacket(d)
    pkt.hostname = server
    pkt.resolved = sockaddr[0]
    packets.append(pkt)
    return packets


def queryhost(server, concurrent, timeout=5, port=123):
    "Query IP addresses associated with a specified host."
    try:
        iptuples = socket.getaddrinfo(server, port,
                                      af, socket.SOCK_DGRAM,
                                      socket.IPPROTO_UDP)
    except socket.gaierror as e:
        log("lookup of %s failed, errno %d = %s" % (server, e[0], e[1]))
        return []
    sockets = []
    packets = []
    request = ntp.packet.SyncPacket()
    request.transmit_timestamp = ntp.packet.SyncPacket.posix_to_ntp(
        time.time())
    packet = request.flatten()
    needgap = (len(iptuples) > 1) and (gap > 0)
    firstloop = True
    for (family, socktype, proto, canonname, sockaddr) in iptuples:
        if needgap and not firstloop:
            time.sleep(gap)
        if firstloop:
            firstloop = False
        if debug:
            log("querying %s (%s)" % (sockaddr[0], server))
        s = socket.socket(family, socktype)
        if keyid and keytype and passwd:
            if debug:
                log("authenticating with %s key %d" % (keytype, keyid))
            mac = ntp.packet.Authenticator.compute_mac(packet,
                                                       keyid, keytype, passwd)
            if mac is None:
                log("MAC generation failed while querying %s" % server)
                raise SystemExit(1)
            else:
                packet += mac
        try:
            s.sendto(packet, sockaddr)
        except socket.error as e:
            log("socket error on transmission: %s" % e)
            raise SystemExit(1)
        if debug >= 2:
            log("Sent to %s:" % (sockaddr[0],))
            ntp.packet.dump_hex_printable(packet)
        if concurrent:
            sockets.append(s)
        else:
            r, _, _ = select.select([s], [], [], timeout)
            if not r:
                return []
            read_append(s, packets, packet, sockaddr)
        while sockets:
            r, _, _ = select.select(sockets, [], [], timeout)
            if not r:
                return packets
            for s in sockets:
                read_append(s, packets, packet, sockaddr)
                sockets.remove(s)
    return packets


def clock_select(packets):
    "Select the pick-of-the-litter clock from the samples we've got."
    # This is a slightly simplified version of the filter ntpdate used
    NTP_INFIN = 15          # max stratum, infinity a la Bellman-Ford

    # This first chunk of code is supposed to go through all
    # servers we know about to find the servers that
    # are most likely to succeed. We run through the list
    # doing the sanity checks and trying to insert anyone who
    # looks okay. We are at all times aware that we should
    # only keep samples from the top two strata.
    #
    filtered = []
    for response in packets:
        def drop(msg):
            log("%s: Response dropped: %s" % (response.hostname, msg))
        if response.stratum > NTP_INFIN:
            drop("stratum too high")
            continue
        if response.version() < ntp.magic.NTP_OLDVERSION:
            drop("response version %d is too old" % response.version())
            continue
        if response.mode() != ntp.magic.MODE_SERVER:
            drop("unexpected response mode %d" % response.mode())
            continue
        if response.version() > ntp.magic.NTP_VERSION:
            drop("response version %d is too new" % response.version())
            continue
        if response.stratum == 0:
            # FIXME: Do some kind of semi-useful diagnostic dump here
            drop("stratum 0, probable KOD packet")
            continue
        if response.leap() == "unsync":
            drop("leap not in sync")
            continue
        if not response.trusted:
            drop("request was authenticated but server is untrusted")
            continue
        # Bypass this test if we ever support broadcast-client mode again
        if response.origin_timestamp == 0:
            drop("unexpected response timestamp")
            continue
        filtered.append(response)

    if len(filtered) <= 1:
        return filtered

    # Sort by stratum and other figures of merit
    filtered.sort(key=lambda s: (s.stratum, s.synchd(), s.root_delay))

    # Return the best
    return filtered[:1]


def report(packet, json):
    "Report on the SNTP packet selected for display, and its adjustment."
    say = sys.stdout.write

    packet.posixize()

    if time.daylight:
        tmoffset = time.altzone // 60  # In minutes
    else:
        tmoffset = time.timezone // 60  # In minutes

    # The server's idea of the time
    t = time.localtime(int(packet.transmit_timestamp))
    ms = int(packet.transmit_timestamp * 1000000) % 1000000

    # Number of decimal digits of precision indicated by the precision field
    digits = min(6, -int(math.log10(2**packet.precision)))

    date = time.strftime("%Y-%m-%d", t)
    tod = time.strftime("%T", t) + (".%-*d" % (digits, ms)).rstrip()
    sgn = ("%+d" % tmoffset)[0]
    tz = "%s%02d%02d" % (sgn, abs(tmoffset) // 60, tmoffset % 60)

    if json:
        say('{"time":"%sT%s%s","offset":%f,"precision":%f,"host":"%s",'
            'ip:"%s","stratum":%s,"leap":"%s","adjusted":%s}\n'
            % (date, tod, tz,
               packet.adjust(), packet.synchd(),
               packet.hostname, packet.resolved or packet.hostname,
               packet.stratum, packet.leap(),
               "true" if adjusted else "false"))
    else:
        say("%s %s (%s) %+f +/- %f %s"
            % (date, tod, tz,
               packet.adjust(), packet.synchd(),
               packet.hostname))
        if packet.resolved and packet.resolved != packet.hostname:
            say(" " + packet.resolved)
        say(" s%d %s\n" % (packet.stratum, packet.leap()))


usage = """
USAGE:  ntpdig [-<flag> [<val>] | --<name>[{=| }<val>]]...
                [ hostname-or-IP ...]
  Flg Arg Option-Name     Description
   -4 no  ipv4           Force IPv4 DNS name resolution
                                - prohibits the option 'ipv6'
   -6 no  ipv6           Force IPv6 DNS name resolution
                                - prohibits the option 'ipv4'
   -a Num authentication  Enable authentication with the numbered key
   -c yes concurrent      Hosts to be queried concurrently
   -d no  debug           Increase debug verbosity
   -D yes set-debug-level Set debug verbosity
   -g yes gap             Set gap between requests in miliseconds
   -j no  json            Use JSON output format
   -l Str logfile         Log to specified logfile
                                 - prohibits the option 'syslog'
   -p yes samples         Number of samples to take (default 1)
   -S no  step            Set (step) the time with clock_settime()
                                 - prohibits the option 'step'
   -s no  slew            Set (slew) the time with adjtime()
                                 - prohibits the option 'slew'
   -t Num timeout         Request timeout in seconds (default 5)
   -k Str keyfile         Specify a keyfile. ntpdig will look in this file
                          for the key specified with -a
   -V no version          Output version information and exit
   -h no  help            Display extended usage information and exit
"""


if __name__ == '__main__':
    try:
        try:
            (options, arguments) = getopt.getopt(
                sys.argv[1:],
                "46a:c:dD:g:hjk:l:M:o:p:r:Sst:wWV",
                ["ipv4", "ipv6",
                 "authentication=",
                 "concurrent=",
                 "gap=", "help", "json",
                 "keyfile=", "logfile=",
                 "replay=",
                 "samples=", "steplimit=",
                 "step", "slew",
                 "timeout=",
                 "debug", "set-debug-level=",
                 "version"])
        except getopt.GetoptError as e:
            print(e)
            raise SystemExit(1)
        progname = sys.argv[0]

        logfp = sys.stderr
        log = lambda m: logfp.write("ntpdig: %s\n" % m)

        af = socket.AF_UNSPEC
        authkey = None
        concurrent_hosts = []
        debug = 0
        gap = .05
        json = False
        keyfile = None
        steplimit = 0       # Default is intentionally zero
        samples = 1
        step = False
        slew = False
        timeout = 5
        replay = None
        try:
            for (switch, val) in options:
                if switch in ("-4", "--ipv4"):
                    af = socket.AF_INET
                elif switch in ("-6", "--ipv6"):
                    af = socket.AF_INET6
                elif switch in ("-a", "--authentication"):
                    errmsg = "Error: -a parameter '%s' not a number\n"
                    authkey = ntp.util.safeargcast(val, int, errmsg, usage)
                elif switch in ("-c", "--concurrent"):
                    concurrent_hosts.append(val)
                elif switch in ("-d", "--debug"):
                    debug += 1
                elif switch in ("-D", "--set-debug-level"):
                    errmsg = "Error: -D parameter '%s' not a number\n"
                    debug = ntp.util.safeargcast(val, int, errmsg, usage)
                elif switch in ("-g", "--gap"):
                    errmsg = "Error: -g parameter '%s' not a number\n"
                    gap = ntp.util.safeargcast(val, int, errmsg, usage)
                elif switch in ("-j", "--json"):
                    json = True
                elif switch in ("-k", "--keyfile"):
                    keyfile = val
                elif switch in ("-l", "--logfile"):
                    try:
                        logfp = open(val, "w")
                    except OSError:
                        sys.stderr.write("logfile open of %s failed.\n" % val)
                        raise SystemExit(1)
                elif switch in ("-M", "--steplimit"):
                    errmsg = "Error: -M parameter '%s' not a number\n"
                    steplimit = ntp.util.safeargcast(val, int, errmsg, usage)
                elif switch in ("-p", "--samples"):
                    errmsg = "Error: -p parameter '%s' not a number\n"
                    samples = ntp.util.safeargcast(val, int, errmsg, usage)
                    if samples < 1:  # If <1 it won't run at all
                        samples = 1
                elif switch in ('-r', "--replay"):
                    replay = val
                elif switch in ("-S", "--step"):
                    step = True
                elif switch in ("-s", "--slew"):
                    slew = True
                elif switch in ("-t", "--timeout"):
                    errmsg = "Error: -t parameter '%s' not a number\n"
                    timeout = ntp.util.safeargcast(val, int, errmsg, usage)
                elif switch in ("-h", "--help"):
                    print(usage)
                    raise SystemExit(0)
                elif switch in ("-V", "--version"):
                    print("ntpdig %s" % ntp.util.stdversion())
                    raise SystemExit(0)
                else:
                    sys.stderr.write(
                        "Unknown command line switch or missing argument.\n")
                    sys.stderr.write(usage)
                    raise SystemExit(1)
        except ValueError:
            sys.stderr.write("Invalid argument.\n")
            sys.stderr.write(usage)
            raise SystemExit(1)

        gap /= 1000  # convert to milliseconds

        credentials = keyid = keytype = passwd = None
        try:
            credentials = ntp.packet.Authenticator(keyfile)
        except (OSError, IOError):
            sys.stderr.write("ntpdig: %s nonexistent or unreadable\n" %
                             keyfile)
            raise SystemExit(1)
        if credentials:
            try:
                (keyid, keytype, passwd) = credentials.control(authkey)
            except ValueError:
                # There are no trusted keys.  Barf.
                log("cannot get authentication key")
                raise SystemExit(1)

        if not credentials and authkey and keyfile is None:
            sys.stderr.write("-a option requires -k.\n")
            raise SystemExit(1)

        if not arguments:
            arguments = ["localhost"]

        if replay:
            (pkt, dst) = replay.split(":")
            packet = ntp.packet.SyncPacket(pkt.decode("hex"))
            packet.received = ntp.packet.SyncPacket.posix_to_ntp(float(dst))
            returned = [packet]
        else:
            returned = []
            needgap = (samples > 1) and (gap > 0)
            firstloop = True
            for s in range(samples):
                if needgap and not firstloop:
                    time.sleep(gap)
                if firstloop is True:
                    firstloop = False
                for server in concurrent_hosts:
                    try:
                        returned += queryhost(server=server,
                                              concurrent=True,
                                              timeout=timeout)
                    except ntp.packet.SyncException as e:
                        log(str(e))
                        continue
                for server in arguments:
                    try:
                        returned += queryhost(server=server,
                                              concurrent=False,
                                              timeout=timeout)
                    except ntp.packet.SyncException as e:
                        log(str(e))
                        continue

            returned = clock_select(returned)

        if returned:
            pkt = returned[0]
            if debug:
                # print(repr(pkt))
                def hexstamp(n):
                    return "%08x.%08x" % (n >> 32, n & 0x00000000ffffffff)
                print("org t1: %s rec t2: %s"
                      % (hexstamp(pkt.t1()), hexstamp(pkt.t2())))
                print("xmt t3: %s dst t4: %s"
                      % (hexstamp(pkt.t3()), hexstamp(pkt.t4())))
            pkt.posixize()
            if debug:
                print("org t1: %f rec t2: %f" % (pkt.t1(), pkt.t2()))
                print("xmt t3: %f dst t4: %f" % (pkt.t3(), pkt.t4()))
                print("rec-org t21: %f  xmt-dst t34: %f"
                      % (pkt.t2() - pkt.t1(), pkt.t3() - pkt.t4()))
            offset = pkt.adjust()
            adjusted = (step and
                        (not slew or (slew and (abs(offset) > steplimit))))
            report(pkt, json)
            # If we can step but we cannot slew, then step.
            # If we can step or slew and |offset| > steplimit, then step.
            rc = True
            ntp.ntpc.setprogname("ntpdig")
            if adjusted:
                rc = ntp.ntpc.step_systime(offset)
            elif slew:
                rc = ntp.ntpc.adj_systime(offset)
            if rc:
                raise SystemExit(0)
            else:
                raise SystemExit(1)
        else:
            log("no eligible servers")
            raise SystemExit(1)
    except KeyboardInterrupt:
        print("")

# end
