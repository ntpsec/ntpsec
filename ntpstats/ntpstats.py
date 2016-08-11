"""
ntpstats.py - class for digesting and plotting NTP logfiles

Requires GNUPLOT and liberation fonts installed.

SPDX-License-Identifier: BSD-2-Clause

Python by ESR, concept and GNUPLOT code by Dan Drown.
"""
# RMS frequency jitter - Deviation from a root-mean-square linear approximation?
# Investigate.
from __future__ import print_function, division

import os, sys, time, glob, calendar, subprocess, socket

class NTPStats:
    "Gather statistics for a specified NTP site"
    @staticmethod
    def stampfields(line):
        "Extract first two fields, MJD and seconds past midnight."
        return (int(line.split()[0]), float(line.split()[1]))
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
        for stem in ("clockstats", "peerstats", "loopstats", "rawstats"):
            lines = []
            try:
                for logpart in glob.glob(os.path.join(statsdir, stem) + "*"):
                    if logpart.endswith("gz"):
                        line = gzip.open(logpart).readlines()
                    else:
                        lines += open(logpart).readlines()
            except IOError:
                pass
            # Sort by date
            lines.sort(key=NTPStats.stampfields)
            # Morph first field into Unix time with fractional seconds 
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
        "Get the latest timestamp in the files"
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
            # by NTP Classic or an NTPsec version before 0.4.  Nasty that we
            # have to emit a numeric driver type here.
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
    def local_clock_gnuplot(self):
        "Generate GNUPLOT code graphing local clock loop statistics"
        sitename = self.sitename
        plot_template = """\
set title "%(sitename)s: Local Clock Offsets"
set terminal png size 900,600
set xdata time
set timefmt "@s"
set grid
set xlabel "Time"
set format x "@d-@H:@M"
set xtic rotate by -45 scale 0
set ytics format "@1.2f us" nomirror textcolor rgb '#0060ad'
set y2tics format "@2.3f ppm" nomirror textcolor rgb '#dd181f'
set key bottom right box
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
set lmargin 12
set rmargin 12
plot \
 "-" using 1:($2*1000000) title "clock offset us" with linespoints ls 1, \
 "-" using 1:3 title "frequency offset ppm" with linespoints ls 2 axis x1y2
""" % locals()
        return plot_template.replace('@', '%') + self.dump("loopstats") + "e\n" + self.dump("loopstats")
    def loopstats_gnuplot(self, fld, title, legend):
        "Generate GNUPLOT code of a given loopstats field"
        sitename   = self.sitename
        ninetynine = self.percentile(fld, 95, self.loopstats) * 1000000
        ninetyfive = self.percentile(fld, 99, self.loopstats) * 1000000
        five       = self.percentile(fld,  5, self.loopstats) * 1000000
        one        = self.percentile(fld,  1, self.loopstats) * 1000000
        nf_m_f     = ninetyfive - five
        nn_m_o     = ninetynine - one
        plot_template = """\
set title "%(sitename)s: %(title)s"
set terminal png size 900,600
set xdata time
set timefmt "@s"
set grid
set xlabel "Time"
set format x "@d-@H:@M"
set xtic rotate by -45 scale 0
set ytics format "@1.2f us" nomirror
set key top right box
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
set label 1 gprintf("99@@ = %(ninetynine)s us",99) at graph 0.01,0.95 left front
set label 2 gprintf("95@@ = %(ninetyfive)s us",95) at graph 0.01,0.9 left front
set label 3 gprintf(" 5@@ = %(five)s us",5) at graph 0.01,0.85 left front
set label 4 gprintf(" 1@@ = %(one)s us",1) at graph 0.01,0.8 left front
set label 5 gprintf("95@@ - 5@@ = %(nf_m_f)s us",90) at graph 0.01,0.75 left front
set label 6 gprintf("99@@ - 1@@ = %(nn_m_o) us",98) at graph 0.01,0.7 left front
set lmargin 12
set rmargin 12
plot \
 "-" using 1:($%(fld)d*1000000) title "%(legend)s" with linespoints ls 1, \
 %(ninetynine)s title "99th percentile", \
 %(ninetyfive)s title "95th percentile", \
 %(five)s title "5th percentile", \
 %(one)s title "1st percentile"
""" % locals()
        return plot_template.replace('@', '%') + self.dump("loopstats")
    def local_clock_jitter_gnuplot(self):
        "Generate GNUPLOT code of local clock loop standard deviation"
        return self.loopstats_gnuplot(4, "RMS Time jitter", "Jitter")
    def local_clock_stability_gnuplot(self):
        "Generate GNUPLOT code graphing local clock stability"
        return self.loopstats_gnuplot(5, "RMS Frequency Jitter - ADEV aka wander", "Stability")
    def peerstats_gnuplot(self, peerlist, fld, title):
        "Plot a specified field from peerstats."
        peerdict = self.peersplit()
        if not peerlist:
            peerlist = list(peerdict.keys())
        peerlist.sort()	# For stability of output
        if len(peerlist) == 1:
            title += ": "+ peerlist[0]
        else:
            title += "s"
        plot_template = """\
set title "%s"
set terminal png size 900,600
set xdata time
set timefmt "@s"
set grid
set xlabel "Time"
set format x "@d-@H:@M"
set xtic rotate by -45 scale 0
set ylabel ""
set ytics format "@1.0f us" nomirror
set key top right box
set lmargin 12
set rmargin 12
plot \\
""" % title
	plot_template = plot_template.replace("@", "%")
        for key in peerlist:
            plot_template += "'-' using 1:($%d*1000000) title '%s' with line, \\\n" % (fld, self.ip_label(key))
        
        plot_template = plot_template[:-4] + "\n"
        for key in peerlist:
            if key in peerdict:
                plot_template += "\n".join(peerdict[key]) + "\ne\n"
            else:
                sys.stderr.write("No such peer as %s" % key)
                raise SystemExit(1)
        return plot_template[:-2]
    def peer_offsets_gnuplot(self, peerlist):
        return self.peerstats_gnuplot(peerlist, 4, "Peer clock offset")
    def peer_jitters_gnuplot(self, peerlist):
        return self.peerstats_gnuplot(peerlist, 7, "Peer clock jitter")
    def peer_rtt_gnuplot(self, host):
        "Plot offset with rtt bounds for specified host."
        entries = self.peersplit()[host]
        fifty = self.percentile(4,  50, entries) * 1000000
        host = self.ip_label(host)
        plot_template = """\
set title "offset of %(host)s"
set terminal png size 900,600
set xdata time
set timefmt "@s"
set grid
set xlabel "Time"
set format x "@d-@H:@M"
set xtic rotate by -45 scale 0
set ylabel ""
set ytics format "@1.0f us" nomirror
set key top right box
set lmargin 12
set rmargin 12
plot \
'-' using 1:($4*1000000) title 'offset' with line, \
'-' using 1:(($4+$5/2)*1000000) title 'offset+rtt/2' with line, \
'-' using 1:(($4-$5/2)*1000000) title 'offset-rtt/2' with line, \
%(fifty)s title '50th percentile'
""" % locals()
	plot_template = plot_template.replace("@", "%")
        data = "\n".join(entries)
        plot_template += data + "\ne\n"	+ data + "\ne\n" + data
        return plot_template

def isotime(s):
    "Convert timestamps in ISO8661 format to and from Unix time including optional fractional seconds."
    if isinstance(s, int):
        return time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(s))
    elif isinstance(s, float):
        date = int(s)
        msec = s - date
        date = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(s))
        return date + "." + repr(msec)[3:]
    elif isinstance(s, str) or isinstance(s, unicode)::
        if s[-1] == "Z":
            s = s[:-1]
        if "." in s:
            (date, msec) = s.split(".")
        else:
            date = s
            msec = "0"
        # Note: no leap-second correction!
        return time.mktime(time.strptime(date, "%Y-%m-%dT%H:%M:%S")) + float("0." + msec)
    else:
        raise TypeError

def gnuplot(template):
    "Run a specified GNUPLOT program."
    proc = subprocess.Popen("gnuplot",
                            shell=True, bufsize=4096, stdin=subprocess.PIPE)
    proc.stdin.write(template)
    proc.stdin.close()
    return proc.wait()

# end
