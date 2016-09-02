"""
statfiles.py - class for digesting and plotting NTP logfiles

Requires GNUPLOT and liberation fonts installed.

SPDX-License-Identifier: BSD-2-Clause
"""
from __future__ import print_function, division

import os, sys, time, glob, calendar, subprocess, socket, gzip, datetime

class NTPStats:
    "Gather statistics for a specified NTP site"
    SecondsInWeek = 24*60*60
    DefaultPeriod = 7*24*60*60
    @staticmethod
    def unixize(line, starttime, endtime):
        "Extract first two fields, MJD and seconds past midnight."
        "convert timestamp (MJD & seconds past midnight) to Unix time"
        "Replace MJD+second with Unix time."
        try:
            split = line.split(None, 2)
            mjd = int(split[0])
            second = float(split[1])
        except:
            # unparseable  time 0 and it will be stripped later
            return None
        # 86400 = 24 * 60 * 60
        time = 86400*mjd + second - 3506716800; # warning: 32 bit overflows
        if time < starttime or time > endtime:
            return None
        return str(time) + " " + split[2]
    @staticmethod
    def timestamp(line):
        "get Unix time from converted line."
        return float(line.split()[0])
    def __init__(self, statsdir, sitename=None, 
                 period=None, starttime=None, endtime=None):
        "Grab content of logfiles, sorted by timestamp."
        if period is None:
            period = NTPStats.DefaultPeriod
        self.period = period

        # Default to one week before the latest date
        if endtime is None and starttime == None:
            endtime = int(time.time())
            starttime = endtime - period
        elif starttime is None and endtime is not None:
            starttime = endtime - period
        elif starttime is not None and endtime is None:
            endtime = starttime + period
        self.starttime = starttime
        self.endtime = endtime

        self.sitename = sitename or os.path.basename(statsdir)
        if not os.path.isdir(statsdir):
            sys.stderr.write("ntpviz: ERROR: %s is not a directory\n" \
                 % statsdir)
            raise SystemExit(1)

        for stem in ("clockstats", "peerstats", "loopstats", "rawstats", \
                 "temps"):
            lines = []
            try:
                for logpart in glob.glob(os.path.join(statsdir, stem) + "*"):
                    # skip files older than starttime
                    if starttime > os.path.getmtime(logpart):
                        continue;
                    if logpart.endswith("gz"):
                        lines += gzip.open(logpart).readlines()
                    else:
                        lines += open(logpart).readlines()
            except IOError:
                sys.stderr.write("ntpviz: WARNING: could not read %s\n" \
                     % logpart)
                pass

            lines1 = []
            if stem == "temps":
                # temps is already in UNIX time
                for line in lines:
                    line = line.strip(' \0\r\n\t')
                    if line != None:
                        split = line.split(None, 2)
                        if int(split[0]) >= starttime and \
                           int(split[0]) <= endtime:
                            lines1.append( line)
            else:
                # Morph first field into Unix time with fractional seconds
                for line in lines:
                    line = line.strip(' \0\r\n\t')
                    line = NTPStats.unixize(line,starttime, endtime)
                    if line != None:
                        lines1.append( line)

            # Sort by datestamp
            lines1.sort(key=lambda line: line.split()[0])
            setattr(self, stem, lines1)
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
        "If you call this twice on the same data set you should use"
        "percentiles() instead"
        # Row is decremented so we match GNUPLOT's 1-origin indexing.
        values = [float(line.split()[n-1]) for line in entries]
        values.sort()
        return values[int(len(values) * (percentile/100))]
    def percentiles(self, percentile, values):
        "Return given percentiles of a given row in a given set of entries."
        "assuming values are already spilit and sorted"
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
    def tempssplit(self):
        "Return a dictionary mapping temperature sources to entry subsets."
        tempsmap = {}
        for line in self.temps:
            source = line.split()[1]
            if source not in tempsmap:
                tempsmap[source] = []
            tempsmap[source].append(line)
        return tempsmap
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
                return "REFCLOCK(type=%s,unit=%s)" % (t, u)
            # Ordinary IP address - replace with primary hostname.
            # Punt if the lookup fails.
            try:
                (hostname, _, _) = socket.gethostbyaddr(key)
                return hostname
            except socket.herror:
                pass
        return key      # Someday, be smarter than this.

def iso_to_posix(s):
    "Accept timestamps in ISO8661 format or numeric POSIX time."
    if s.isdigit():
        return int(s)
    else:
        return time.mktime(time.strptime(s, "%Y-%m-%dT%H:%M:%S"))

def posix_to_iso(t):
    "ISO8601 string from Unix time."
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(t))

# end
