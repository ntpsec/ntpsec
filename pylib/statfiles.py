# -*- coding: utf-8 -*-

"""
statfiles.py - class for digesting and plotting NTP logfiles

Requires gnuplot and liberation fonts installed.

"""
# SPDX-License-Identifier: BSD-2-Clause
from __future__ import print_function, division

import calendar
import glob
import gzip
import os
import socket
import sys
import time


class NTPStats:
    "Gather statistics for a specified NTP site"
    SecondsInDay = 24*60*60
    DefaultPeriod = 7*24*60*60  # default 7 days, 604800 secs
    peermap = {}    # cached result of peersplit()
    period = None
    starttime = None
    endtime = None
    sitename = ''

    @staticmethod
    def unixize(lines, starttime, endtime):
        """Extract first two fields, MJD and seconds past midnight.
        convert timestamp (MJD & seconds past midnight) to Unix time
        Replace MJD+second with Unix time."""
        # HOT LOOP!  Do not change w/o profiling before and after
        lines1 = []
        for line in lines:
            try:
                split = line.split()
                mjd = int(split[0])
                second = float(split[1])
            except ValueError:
                # unparseable, skip this line
                continue

            # warning: 32 bit overflows
            time = NTPStats.SecondsInDay * mjd + second - 3506716800
            if starttime <= time <= endtime:
                # time as integer number milli seconds
                split[0] = int(time * 1000)
                # time as string
                split[1] = str(time)
                lines1.append(split)
        return lines1

    @staticmethod
    def timestamp(line):
        "get Unix time from converted line."
        return float(line.split()[0])

    @staticmethod
    def percentiles(percents, values):
        """Return given percentiles of a given row in a given set of entries.
        assuming values are already split and sorted"""
        ret = {}
        length = len(values)
        if 1 >= length:
            # uh, oh...
            if 1 == length:
                # just one data value, set all to that one value
                value = values[0]
            else:
                # no data, set all to zero
                value = 0
            for perc in percents:
                ret["p" + str(perc)] = value
        else:
            for perc in percents:
                if perc == 100:
                    ret["p100"] = values[length - 1]
                else:
                    ret["p" + str(perc)] = values[int(length * (perc/100))]
        return ret

    @staticmethod
    def ip_label(key):
        "Produce appropriate label for an IP address."
        # If it's a new-style NTPsep clock label, pass it through,
        # Otherwise we expect it to be an IP address and the next guard fires
        if key[0].isdigit():
            # TO BE REMOVED SOMEDAY
            # Clock address - only possible if we're looking at a logfile made
            # by NTP Classic or an NTPsec version configured with
            # --enable-classic-mode.  Nasty that we have to emit a numeric
            # driver type here.
            if key.startswith("127.127."):
                (_, _, clock_type, unit) = key.split(".")
                return "REFCLOCK(type=%s,unit=%s)" % (clock_type, unit)
            # Ordinary IP address - replace with primary hostname.
            # Punt if the lookup fails.
            try:
                (hostname, _, _) = socket.gethostbyaddr(key)
                return hostname
            except socket.herror:
                pass
        return key      # Someday, be smarter than this.

    def __init__(self, statsdir, sitename=None,
                 period=None, starttime=None, endtime=None):
        "Grab content of logfiles, sorted by timestamp."
        if period is None:
            period = NTPStats.DefaultPeriod
        self.period = period

        # Default to one week before the latest date
        if endtime is None and starttime is None:
            endtime = int(time.time())
            starttime = endtime - period
        elif starttime is None and endtime is not None:
            starttime = endtime - period
        elif starttime is not None and endtime is None:
            endtime = starttime + period
        self.starttime = starttime
        self.endtime = endtime

        self.sitename = sitename or os.path.basename(statsdir)
        if 'ntpstats' == self.sitename:
            # boring, use hostname
            self.sitename = socket.getfqdn()

        if not os.path.isdir(statsdir):  # pragma: no cover
            sys.stderr.write("ntpviz: ERROR: %s is not a directory\n"
                             % statsdir)
            raise SystemExit(1)

        self.clockstats = []
        self.peerstats = []
        self.loopstats = []
        self.rawstats = []
        self.temps = []
        self.gpsd = []

        for stem in ("clockstats", "peerstats", "loopstats",
                     "rawstats", "temps", "gpsd"):
            lines = self.__load_stem(statsdir, stem)
            processed = self.__process_stem(stem, lines)
            setattr(self, stem, processed)

    def __load_stem(self, statsdir, stem):
        lines = []
        try:
            pattern = os.path.join(statsdir, stem)
            if stem != "temps" and stem != "gpsd":
                pattern += "."
            for logpart in glob.glob(pattern + "*"):
                # skip files older than starttime
                if self.starttime > os.path.getmtime(logpart):
                    continue
                if logpart.endswith("gz"):
                    lines += gzip.open(logpart, 'rt').readlines()
                else:
                    lines += open(logpart, 'r').readlines()
        except IOError:  # pragma: no cover
            sys.stderr.write("ntpviz: WARNING: could not read %s\n"
                             % logpart)

        return lines

    def __process_stem(self, stem, lines):
        lines1 = []
        if stem == "temps" or stem == "gpsd":
            # temps and gpsd are already in UNIX time
            for line in lines:
                split = line.split()
                if 3 > len(split):
                    # skip short lines
                    continue

                try:
                    time_float = float(split[0])
                except ValueError:
                    # ignore comment lines, lines with no time
                    continue

                if self.starttime <= time_float <= self.endtime:
                    # prefix with int milli sec.
                    split.insert(0, int(time_float * 1000))
                    lines1.append(split)
        else:
            # Morph first fields into Unix time with fractional seconds
            # ut into nice dictionary of dictionary rows
            lines1 = NTPStats.unixize(lines, self.starttime, self.endtime)

        # Sort by datestamp
        # by default, a tuple sort()s on the 1st item, which is a nice
        # integer of milli seconds.  This is faster than using
        # cmp= or key=
        lines1.sort()
        return lines1

    def peersplit(self):
        """Return a dictionary mapping peerstats IPs to entry subsets.
        This is very expensive, so cache the result"""
        if len(self.peermap):
            return self.peermap

        for row in self.peerstats:
            try:
                ip_address = row[2]     # peerstats field 2, refclock id
                if ip_address not in self.peermap:
                    self.peermap[ip_address] = []
                self.peermap[ip_address].append(row)
            except IndexError:  # pragma: no cover
                # ignore corrupted rows
                pass
        return self.peermap

    def gpssplit(self):
        "Return a dictionary mapping gps sources to entry subsets."
        gpsmap = {}
        for row in self.gpsd:
            try:
                source = row[2]
                if source not in gpsmap:
                    gpsmap[source] = []
                gpsmap[source].append(row)
            except IndexError:  # pragma: no cover
                # ignore corrupted rows
                pass
        return gpsmap

    def tempssplit(self):
        "Return a dictionary mapping temperature sources to entry subsets."
        tempsmap = {}
        for row in self.temps:
            try:
                source = row[2]
                if source not in tempsmap:
                    tempsmap[source] = []
                tempsmap[source].append(row)
            except IndexError:  # pragma: no cover
                # ignore corrupted rows
                pass
        return tempsmap


def iso_to_posix(time_string):
    "Accept timestamps in ISO 8661 format or numeric POSIX time. UTC only."
    if str(time_string).isdigit():
        return int(time_string)
    time_struct = time.strptime(time_string, "%Y-%m-%dT%H:%M:%S")
    # don't use time.mktime() as that is local tz
    return calendar.timegm(time_struct)


def posix_to_iso(unix_time):
    "ISO 8601 string in UTC from Unix time."
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(unix_time))

# end
