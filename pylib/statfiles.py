"""
statfiles.py - class for digesting and plotting NTP logfiles

Requires gnuplot and liberation fonts installed.

"""
#SPDX-License-Identifier: BSD-2-Clause
from __future__ import print_function, division

import calendar
import glob, gzip, os, socket, sys, time

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
        "Extract first two fields, MJD and seconds past midnight."
        "convert timestamp (MJD & seconds past midnight) to Unix time"
        "Replace MJD+second with Unix time."
        # HOT LOOP!  Do not change w/o profiling before and after
        lines1 = []
        for line in lines:
            try:
                split = line.split()
                mjd = int(split[0])
                second = float(split[1])
            except:
                # unparseable, skip this line
                continue

            # warning: 32 bit overflows
            time = NTPStats.SecondsInDay * mjd + second - 3506716800
            if starttime  <= time <= endtime:
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

        if not os.path.isdir(statsdir):
            sys.stderr.write("ntpviz: ERROR: %s is not a directory\n" \
                 % statsdir)
            raise SystemExit(1)

        for stem in ("clockstats", "peerstats", "loopstats", "rawstats", \
                 "temps", "gpsd"):
            lines = []
            try:
                for logpart in glob.glob(os.path.join(statsdir, stem) + "*"):
                    # skip files older than starttime
                    if starttime > os.path.getmtime(logpart):
                        continue
                    if logpart.endswith("gz"):
                        lines += gzip.open(logpart, 'rb').readlines()
                    else:
                        lines += open(logpart, 'rb').readlines()
            except IOError:
                sys.stderr.write("ntpviz: WARNING: could not read %s\n" \
                     % logpart)
                pass

            lines1 = []
            if stem == "temps" or stem == "gpsd":
                # temps and gpsd are already in UNIX time
                for line in lines:
                    split = line.split()
                    try:
                        t = float(split[0])
                    except:
                        # ignore comment lines, lines with no time
                        continue

                    if starttime <= t <= endtime:
                        # prefix with int milli sec.
                        split.insert(0, int(t * 1000))
                        lines1.append( split)
            else:
                # Morph first fields into Unix time with fractional seconds
                # ut into nice dictionary of dictionary rows
                lines1 = NTPStats.unixize(lines, starttime, endtime)

            # Sort by datestamp
            # by default, a tuple sort()s on the 1st item, which is a nice
            # integer of milli seconds.  This is faster than using
            # cmp= or key=
            lines1.sort()
            setattr(self, stem, lines1)

    def percentiles(self, percents, values):
        "Return given percentiles of a given row in a given set of entries."
        "assuming values are already split and sorted"
        ret = {}
        length = len(values)
        for perc in percents:
            if perc == 100:
                ret["p100"] = values[length - 1]
            else:
                ret[ "p" + str(perc)] = values[int(length * (perc/100))]
        return ret

    def peersplit(self):
        "Return a dictionary mapping peerstats IPs to entry subsets."
        "This is very expensive, so cache the result"
        if len( self.peermap):
            return self.peermap

        for row in self.peerstats:
            try:
                ip = row[2]     # peerstats field 2, refclock id
                if ip not in self.peermap:
                    self.peermap[ip] = []
                self.peermap[ip].append(row)
            except IndexError:
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
            except IndexError:
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
            except IndexError:
                # ignore corrupted rows
                pass
        return tempsmap

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
    "Accept timestamps in ISO 8661 format or numeric POSIX time. UTC only."
    if str(s).isdigit():
        return int(s)
    else:
        t = time.strptime(s, "%Y-%m-%dT%H:%M:%S")
        # don't use time.mktime() as that is local tz
        return calendar.timegm(t)

def posix_to_iso(t):
    "ISO 8601 string in UTC from Unix time."
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(t))

# end
