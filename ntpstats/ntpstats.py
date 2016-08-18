"""
ntpstats.py - class for digesting and plotting NTP logfiles

Requires GNUPLOT and liberation fonts installed.

SPDX-License-Identifier: BSD-2-Clause
"""
from __future__ import print_function, division

import os, sys, time, glob, calendar, subprocess, socket

class NTPStats:
    "Gather statistics for a specified NTP site"
    @staticmethod
    def stampfields(line):
        "Extract first two fields, MJD and seconds past midnight."
        line = line.strip()
        try:
            return (int(line.split()[0]), float(line.split()[1]))
        except:
            # unparseable  time 0 and it will be stripped later
            return (0, 0)
    @staticmethod
    def unixtime(line):
        "Log timestamp (MJD & seconds past midnight) to Unix time"
        (mjd, second) = NTPStats.stampfields(line)
        return 24*60*60*mjd+second-3506716800; # warning: 32 bit overflows
    @staticmethod
    def unixize(line):
        "Replace MJD+second with Unix time."
        return str(NTPStats.unixtime(line)) + " " + " ".join(line.split()[2:])
    @staticmethod
    def timestamp(line):
        "get Unix time from converted line."
        return float(line.split()[0])
    def __init__(self, sitename, statsdir):
        "Grab content of all logfiles, sorted by timestamp."
        self.sitename = sitename
        for stem in ("clockstats", "peerstats", "loopstats", "rawstats", "cputemp"):
            lines = []
            try:
                for logpart in glob.glob(os.path.join(statsdir, stem) + "*"):
                    if logpart.endswith("gz"):
                        line = gzip.open(logpart).readlines()
                    else:
                        lines += open(logpart).readlines()
            except IOError:
                pass
            # Filter out blank lines (we don't know where these come from)
            lines = [line.strip(' \0\r\n\t')  \
                     for line in lines if line.strip(' \0\r\n\t')]
            # Sort by date
            lines.sort(key=NTPStats.stampfields)
            # Morph first field into Unix time with fractional seconds 
            if stem != "cputemp":
                lines = [NTPStats.unixize(line) for line in lines]
            setattr(self, stem, lines)
    def clip(self, start, end):
        "Select a range of entries"
        for stem in ("clockstats", "peerstats", "loopstats", "rawstats"):
            lines = getattr(self, stem)
            lines = [line for line in lines
                     if float(line.split()[0]) >= start and float(line.split()[0]) <= end]
            setattr(self, stem, lines)
    def rangemax(self):
        "Get the latest timestamp in the files"
        m = sys.float_info.min
        for field in  ("clockstats", "peerstats", "loopstats", "rawstats"):
            row = getattr(self, field)
            if row:
                ts = NTPStats.timestamp(row[-1])
                if ts > m:
                    m = ts
        return m
    def rangemin(self):
        "Get the earliest timestamp in the files"
        m = sys.float_info.max
        for field in  ("clockstats", "peerstats", "loopstats", "rawstats"):
            row = getattr(self, field)
            if row:
                ts = NTPStats.timestamp(row[0])
                if ts < m:
                    m = ts
        return m
    def percentile(self, n, percentile, entries):
        "Return given percentiles of a given row in a given set of entries."
        # Row is decremented so we match GNUPLOT's 1-origin indexing.
        values = [float(line.split()[n-1]) for line in entries]
        values.sort()
        return values[int(len(values) * (percentile/100))]
    def peersplit(self):
        "Return a dictionary mapping peerstats IPs to entry subsets."
        peermap = {}
        for line in self.peerstats:
            ip = line.split()[1]
            if ip not in peermap:
                peermap[ip] = []
            peermap[ip].append(line)
        return peermap
    def dump(self, row):
        "dump a stored list of logfile entries"
        return "\n".join(getattr(self, row)) + "\n"
    def ip_label(self, key):
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
                (_, _, t, u) = key.split(".")
                return "REFCLOCK(ttype=%s,unit=%s)" % (t, u) 
            # Ordinary IP address - replace with primary hostname.
            # Punt if the lookup fails.
            try:
                (hostname, _, _) = socket.gethostbyaddr(key)
                return hostname
            except socket.herror:
                pass
        return key	# Someday, be smarter than this.

def iso_to_posix(s):
    "Accept timestamps in ISO8661 format or numeric POSIX time."
    if s.isdigit(s):
        return int(s)
    else:
        return time.mktime(time.strptime(date, "%Y-%m-%dT%H:%M:%S"))

def posix_to_iso(t):
    "ISO8601 string from Unix time."
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(t))

# end
