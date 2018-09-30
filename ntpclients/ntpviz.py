#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""\
ntpviz - visualizer for NTP log files

ntpviz [-d LOGDIR] [-g] [-n name] [-p DAYS]
         [-s starttime] [-e endtime]
         [-o OUTDIR]
         [-c | --clip]
         [-w SIZE | --width SIZE]
         [--all-peer-jitters |
          --all-peer-offsets |
          --local-error |
          --local-freq-temps |
          --local-gps |
          --local-jitter |
          --local-offset |
          --local-offset-histogram |
          --local-offset-multiplot |
          --local-stability |
          --local-temps |
          --peer-jitters=hosts |
          --peer-offsets=hosts |
         ]
         [-D DLVL | --debug DLVL]
         [-N | --nice]
         [-V | --version]
         [@OPTIONFILE]

See the manual page for details.

Python by ESR, concept and gnuplot code by Dan Drown.
"""
# SPDX-License-Identifier: BSD-2-Clause
from __future__ import print_function, division

import atexit
import binascii
import collections
import csv
import datetime
import math
import re
import os
import socket
import sys
import subprocess
import tempfile
try:
    import argparse
except ImportError:
    sys.stderr.write("""
ntpviz: can't find the Python argparse module
        If your Python version is < 2.7, then manual installation is needed:
        # pip install argparse
""")
    sys.exit(1)

if sys.version_info[0] == 2:
    import codecs

    # force UTF-8 strings, otherwise some systems crash on micro.
    reload(sys)           # why?
    sys.setdefaultencoding('utf8')

    def open(file, mode='r', buffering=-1, encoding=None, errors=None):
        "Redefine open()"
        return(codecs.open(filename=file, mode=mode, encoding=encoding,
               errors=errors, buffering=buffering))

# believe it or not, Python has no way to make a simple constant!
MS_PER_S = 1e3          # milliseconds per second
NS_PER_S = 1e9          # nanoseconds per second
US_PER_S = 1e6          # microseconds per second
S_PER_MS = 1.0e-3       # seconds per millisecond
S_PER_NS = 1.0e-9       # seconds per nanosecond
S_PER_US = 1.0e-6       # seconds per microsecond

# table to translate refclock names
refclock_name = {'127.127.20.0': 'NMEA(0)',
                 '127.127.20.1': 'NMEA(1)',
                 '127.127.20.2': 'NMEA(2)',
                 '127.127.20.3': 'NMEA(3)',
                 '127.127.22.0': 'PPS(0)',
                 '127.127.22.1': 'PPS(1)',
                 '127.127.22.2': 'PPS(2)',
                 '127.127.22.3': 'PPS(3)',
                 '127.127.28.0': 'SHM(0)',
                 '127.127.28.1': 'SHM(1)',
                 '127.127.28.2': 'SHM(2)',
                 '127.127.28.3': 'SHM(3)',
                 '127.127.46.0': 'GPS(0)',
                 '127.127.46.1': 'GPS(1)',
                 '127.127.46.2': 'GPS(2)',
                 '127.127.46.3': 'GPS(3)'}


# Gack, python before 3.2 has no defined tzinfo for utc...
# define our own
class UTC(datetime.tzinfo):
    """UTC"""

    def utcoffset(self, dt):
        return datetime.timedelta(0)

    def tzname(self, dt):
        return "UTC"

    def dst(self, dt):
        return datetime.timedelta(0)


try:
    import ntp.statfiles
    import ntp.util
except ImportError as e:
    sys.stderr.write("ntpviz: ERROR: can't find Python NTP library.\n%s\n"
                     "Check your PYTHONPATH\n" % e)
    sys.exit(1)

# check Python version
Python26 = False
if (3 > sys.version_info[0]) and (7 > sys.version_info[1]):
    # running under Python version before 2.7
    Python26 = True


# overload ArgumentParser
class MyArgumentParser(argparse.ArgumentParser):
    "class to parse arguments"

    def convert_arg_line_to_args(self, arg_line):
        '''Make options file more tolerant'''
        # strip out trailing comments
        arg_line = re.sub('\s+#.*$', '', arg_line)

        # ignore blank lines
        if 0 == len(arg_line):
            return []
        # ignore comment lines
        if '#' == arg_line[0]:
            return []

        return arg_line.split()


def print_profile():
    """called by atexit() on normal exit to print profile data"""
    pr.disable()
    pr.print_stats('tottime')
    pr.print_stats('cumtime')


# standard deviation class
# use this until we can guarantee Python 3.4 and the statistics module
# http://stackoverflow.com/questions/15389768/standard-deviation-of-a-list#21505523

# class to calc:
#   Mean, Variance, Standard Deviation, Skewness and Kurtosis

class RunningStats(object):
    "Calculate mean, variance, sigma, skewness and kurtosis"

    def __init__(self, values):
        self.num = len(values)     # number of samples
        self.mu = 0.0              # simple arithmetic mean
        self.variance = 0.0        # variance
        self.sigma = 0.0           # aka standard deviation
        self.skewness = 0.0
        self.kurtosis = 3.0

        if 0 >= self.num:
            # no data??
            return

        self.mu = sum(values) / self.num
        self.variance = sum(pow((v-self.mu), 2) for v in values) / self.num
        self.sigma = math.sqrt(self.variance)

        if math.isnan(self.sigma) or 1e-12 >= abs(self.sigma):
            # punt
            self.skewness = float('nan')
            self.kurtosis = float('nan')
            return

        m3 = 0
        m4 = 0
        for val in values:
            m3 += pow(val - self.sigma, 3)
            m4 += pow(val - self.sigma, 4)

        self.skewness = m3 / (self.num * pow(self.sigma, 3))
        self.kurtosis = m4 / (self.num * pow(self.sigma, 4))

# end standard deviation class


# class for calced values
class VizStats(ntp.statfiles.NTPStats):
    "Class for calculated values"

    percs = {}          # dictionary of percentages
    title = ''          # title
    unit = 's'          # display units: s, ppm, etc.
    skip_summary = False
    clipped = False
    multiplier = 1

    # observe RFC 4180, end lines with CRLF
    csv_head = ["Name", "Min", "1%", "5%", "50%", "95%", "99%", "Max", "",
                "90% Range", "98% Range", "StdDev", "", "Mean", "Units",
                "Skewness", "Kurtosis"]

    table_head = """\
<br>
<table>
<thead>
  <tr style="font-weight:bold;text-align:left;">
    <td style="width:300px;"></td>
    <td colspan=8> Percentiles......</td>
    <td colspan=3> Ranges......</td>
    <td colspan=3></td>
    <td style="text-align:right;">Skew-</td>
    <td style="text-align:right;">Kurt-</td>
  </tr>
  <tr style="font-weight:bold;text-align:right;">
    <td style="text-align:left;">Name</td>
    <td>Min</td><td>1%</td><td>5%</td><td>50%</td><td>95%</td>
    <td>99%</td><td>Max</td> <td style="width:10px;">&nbsp;</td>
    <td>90%</td><td>98%</td><td>StdDev</td>
    <td style="width:10px;">&nbsp;</td><td>Mean</td><td>Units</td>
    <td>ness</td><td>osis</td>
  </tr>
</thead>
"""
    table_tail = """\
</table>
"""

    def __init__(self, values, title, freq=0, units=''):

        values.sort()
        self.percs = self.percentiles((100, 99, 95, 50, 5, 1, 0), values)

        # find the target for autoranging
        if args.clip:
            # keep 99% and 1% under 999 in selected units
            # clip to 1% and 99%
            target = max(self.percs["p99"], -self.percs["p1"])
        else:
            # keep 99% and 1% under 999 in selected units
            # but do not let 100% and 1% go over 5000 in selected units
            target = max(self.percs["p99"], -self.percs["p1"],
                         self.percs["p100"]/5, -self.percs["p0"]/5)

        if len(units):
            # fixed scale
            self.multiplier = 1
            self.unit = units

        elif 1 <= target:
            self.multiplier = 1
            if freq:
                # go to ppm
                self.unit = "ppm"
            else:
                # go to seconds
                self.unit = "s"

        elif S_PER_MS <= target:
            self.multiplier = MS_PER_S
            if freq:
                # go to ppb
                self.unit = "ppb"
            else:
                # go to millisec
                self.unit = "ms"

        elif S_PER_US <= target:
            self.multiplier = US_PER_S
            if freq:
                self.unit = "10e-12"
            else:
                # go to microsec
                self.unit = "µs"

        else:
            self.multiplier = NS_PER_S
            if freq:
                self.unit = "10e-15"
            else:
                # go to nanosec
                self.unit = "ns"

        sts = RunningStats(values)
        self.percs["mu"] = sts.mu
        self.percs["pstd"] = sts.sigma

        self.title = title

        # calculate ranges
        self.percs["r90"] = self.percs["p95"] - self.percs["p5"]
        self.percs["r98"] = self.percs["p99"] - self.percs["p1"]

        # calculate mean +/- std dev
        self.percs["m1sigma"] = self.percs["mu"] - self.percs["pstd"]
        self.percs["p1sigma"] = self.percs["mu"] + self.percs["pstd"]

        # pretty print the values
        self.percs_f = {}
        for k, v in self.percs.items():
            # range the data
            v *= self.multiplier
            self.percs[k] = round(v, 4)
            if 'ppm' == self.unit and 0.020 > abs(self.percs[k]):
                fmt = ".4f"
            else:
                fmt = ".3f"
            if not Python26:
                # Python 2.6 does not undertand the comma format option
                fmt = "," + fmt
            self.percs_f[k] = format(v, fmt)

        # don't scale skewness and kurtosis
        self.percs["skew"] = sts.skewness
        self.percs["kurt"] = sts.kurtosis
        if '°C' == units:
            # skip for temperatures.
            self.percs_f["skew"] = ''
            self.percs_f["kurt"] = ''
        else:
            self.percs_f["skew"] = format(self.percs["skew"], "6.4g")
            self.percs_f["kurt"] = format(self.percs["kurt"], "6.4g")

        if args.clip:
            self.percs["min_y"] = self.percs["p1"]
            self.percs["max_y"] = self.percs["p99"]
            self.percs["clipped"] = " (clipped)"
        else:
            self.percs["min_y"] = self.percs["p0"]
            self.percs["max_y"] = self.percs["p100"]
            self.percs["clipped"] = ""

        self.fmt = gnuplot_fmt(self.percs["min_y"], self.percs["max_y"])

        # Python is stupid about nested objects, so add in some other stuff
        self.percs_f["fmt"] = self.percs["fmt"] = self.fmt
        self.percs_f["multiplier"] = self.percs["multiplier"] = self.multiplier
        self.percs_f["title"] = self.percs["title"] = self.title
        self.percs_f["unit"] = self.percs["unit"] = self.unit

        s = ["%(title)s", "%(p0)s", "%(p1)s", "%(p5)s", "%(p50)s", "%(p95)s",
             " %(p99)s", "%(p100)s", "", "%(r90)s", "%(r98)s", "%(pstd)s",
             "", "%(mu)s", "%(unit)s", "%(skew)s", "%(kurt)s", ]

        # csv is raw, html table is autoranged
        self.csv = [x % self.percs for x in s]
        self.table = [x % self.percs_f for x in s]
        self.table = "</td>\n <td>".join(self.table)

        self.table = '''\
<tr>
 <td style="text-align:left;">%s</td>
</tr>
''' % self.table

        return


def gnuplot_fmt(min_val, max_val):
    "return optimal gnuplot format"
    span = max_val - min_val
    if 6 <= span:
        fmt = '%.0f'
    elif 0.6 <= span:
        fmt = '%.1f'
    elif 0.1 <= span:
        fmt = '%.2f'
    else:
        fmt = '%.3f'
    return fmt


# end calc things now

# RMS frequency jitter - Deviation from root-mean-square linear approximation?
# Investigate.

def gnuplot(template, outfile=None):
    "Run a specified gnuplot program."

    if not len(template):
        # silently ignore empty plots
        return ''
    if outfile is None:
        out = None
    else:
        out = open(outfile, "w", encoding='utf-8')
    ##

    # can be 30% faster to write to a tmp file than to pipe to gnuplot
    # bonus, we can keep the plot file for debug.
    if sys.version_info[0] == 2:
        tmp_file = tempfile.NamedTemporaryFile(mode='w',
                                               suffix='.plt', delete=False)
    else:
        tmp_file = tempfile.NamedTemporaryFile(mode='w', encoding='utf-8',
                                               suffix='.plt', delete=False)
    # note that tmp_file is a file handle, it is not a file object
    tmp_file.write(template)
    tmp_file.close()

    # shell=True is a security hazard, do not use
    try:
        rcode = subprocess.call(['gnuplot', tmp_file.name], stdout=out)
    except OSError as e:
        if e.errno == os.errno.ENOENT:
            # gnuplot not found
            sys.stderr.write("ntpviz: ERROR: gnuplot not found in path\n")
        else:
            # Something else went wrong while trying to run gnuplot
            sys.stderr.write("ntpviz: ERROR: gnuplot failed\n")
        raise SystemExit(1)

    if 0 != rcode:
        sys.stderr.write("ntpviz: WARNING: plot returned %s\n" % rcode)
        sys.stderr.write("ntpviz: WARNING: plot file %s\n" % tmp_file.name)
    elif 2 <= args.debug_level:
        sys.stderr.write("ntpviz: INFO: plot file %s\n" % tmp_file.name)
    else:
        # remove tmp file
        os.remove(tmp_file.name)

    return rcode


class NTPViz(ntp.statfiles.NTPStats):
    "Class for visualizing statistics from a single server."

    # Python takes single quotes here. Since no % substitution
    Common = """\
set grid
set autoscale xfixmin
set autoscale xfixmax
set xdata time
set xlabel "Time UTC"
set xtics format "%d %b\\n%H:%MZ"
set timefmt "%s"
set lmargin 10
set rmargin 10
"""

    def __init__(self, statsdir,
                 sitename=None, period=None, starttime=None, endtime=None):
        ntp.statfiles.NTPStats.__init__(self, statsdir=statsdir,
                                        sitename=sitename,
                                        period=period,
                                        starttime=starttime,
                                        endtime=endtime)

    def plot_slice(self, rows, item1, item2=None):
        "slice 0,item1, maybe item2, from rows, ready for gnuplot"
        # speed up by only sending gnuplot the data it will actually use
        # WARNING: this is hot code, only modify if you profile
        # since we are looping the data, get the values too
        plot_data = ''
        last_time = 0
        values1 = []
        values2 = []
        if item2:
            for row in rows:
                try:
                    values1.append(float(row[item1]))
                    values2.append(float(row[item2]))
                    if 2200000 < row[0] - last_time:
                        # more than 2,200 seconds between points
                        # data loss, add a break in the plot line
                        plot_data += '\n'
                    # fields: time, fld1, and fld2
                    plot_data += row[1] + ' ' + row[item1] + ' ' \
                        + row[item2] + '\n'
                    last_time = row[0]
                except IndexError:
                    pass
        else:
            for row in rows:
                try:
                    values1.append(float(row[item1]))
                    if 2200000 < row[0] - last_time:
                        # more than 2,200 seconds between points
                        # data loss, add a break in the plot line
                        plot_data += '\n'
                    # fields: time, fld
                    plot_data += row[1] + ' ' + row[item1] + '\n'
                    last_time = row[0]
                except IndexError:
                    pass

        # I know you want to replace the plot_data string concat with
        # or more join()s, do not do it, it is slower
        # next you'll want to try %-substitution.  it too is slower
        plot_data += "e\n"
        if item2:
            return (plot_data, values1, values2)

        return (plot_data, values1)

    def local_offset_gnuplot(self):
        "Generate gnuplot code graphing local clock loop statistics"
        if not len(self.loopstats):
            sys.stderr.write("ntpviz: WARNING: no loopstats to graph\n")
            return ''

        # speed up by only sending gnuplot the data it will actually use
        # fields: time, time offset, freq offset
        (plot_data, values, values_f) = self.plot_slice(self.loopstats, 2, 3)

        # compute clock offset
        stats = VizStats(values, "Local Clock Time Offset")

        # compute frequency offset
        stats_f = VizStats(values_f, "Local Clock Frequency Offset", freq=1)

        out = stats.percs
        out["fmt"] = stats.percs["fmt"]
        out["min_y2"] = stats_f.percs["min_y"]
        out["max_y2"] = stats_f.percs["max_y"]
        out["unit_f"] = stats_f.percs["unit"]
        out["fmt_f"] = stats_f.percs["fmt"]
        out["multiplier_f"] = stats_f.percs["multiplier"]
        out["sitename"] = self.sitename
        out['size'] = args.png_size

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: Local Clock Time/Frequency Offsets%(clipped)s"
set ytics format "%(fmt)s %(unit)s" nomirror textcolor rgb '#0060ad'
set yrange [%(min_y)s:%(max_y)s]
set y2tics format "%(fmt_f)s %(unit_f)s" nomirror textcolor rgb '#dd181f'
set y2range [%(min_y2)s:%(max_y2)s]
set key top right
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
plot \
 "-" using 1:($2*%(multiplier)s) title "clock offset %(unit)s" \
     with linespoints ls 1, \
 "-" using 1:($3*%(multiplier_f)s) title "frequency offset %(unit_f)s" \
     with linespoints ls 2 axis x1y2
""" % out

        exp = """\
<p>The time and frequency offsets between the ntpd calculated time
and the local system clock.  Showing frequency offset (red, in parts
per million, scale on right) and the time offset (blue, in μs, scale
on left).  Quick changes in time offset will lead to larger frequency
offsets.</p>

<p>These are fields 3 (time) and 4 (frequency) from the loopstats log
file.</p>

"""

        ret = {'html': VizStats.table_head + stats.table +
               stats_f.table + VizStats.table_tail + exp,
               'plot': plot_template + plot_data + plot_data,
               'stats': [stats, stats_f],
               'title': "Local Clock Time/Frequency Offsets"}
        return ret

    def local_freq_temps_plot(self):
        "Generate gnuplot code graphing local frequency and temps"
        if not len(self.loopstats):
            sys.stderr.write("ntpviz: WARNING: no loopstats to graph\n")
            return ''

        tempsmap = self.tempssplit()
        tempslist = list(tempsmap.keys())
        tempslist.sort()
        if not len(tempsmap) or not len(tempslist):
            sys.stderr.write("ntpviz: WARNING: no temps to graph\n")
            return ''

        # speed up by only sending gnuplot the data it will actually use
        # fields: time, freq offset
        (plot_data, values_f) = self.plot_slice(self.loopstats, 3)

        # compute frequency offset
        stats_f = VizStats(values_f, "Local Clock Frequency Offset", freq=1)

        stats = [stats_f]
        table = ''
        plot_data_t = ''
        max_temp = -300
        min_temp = 1000
        for key in tempslist:
            # speed up by only sending gnuplot the data it will actually use
            # fields: time, temp
            (p, v) = self.plot_slice(tempsmap[key], 3)
            plot_data_t += p
            s = VizStats(v, 'Temp %s' % key, units='°C')
            max_temp = max(s.percs["max_y"], max_temp)
            min_temp = min(s.percs["min_y"], min_temp)
            table += s.table
            stats.append(s)

        # out = stats.percs
        out = {}
        if args.clip:
            out["clipped"] = " (clipped)"
        else:
            out["clipped"] = ""
        out['fmt'] = gnuplot_fmt(min_temp, max_temp)
        out["fmt_f"] = stats_f.percs["fmt"]
        out["max_y2"] = stats_f.percs["max_y"]
        out["min_y2"] = stats_f.percs["min_y"]
        out["multiplier_f"] = stats_f.percs["multiplier"]
        out["sitename"] = self.sitename
        out['size'] = args.png_size
        out["unit"] = '°C'
        out["unit_f"] = stats_f.percs["unit"]

        # let temp autoscale
        # set yrange [%(min_y)s:%(max_y)s]
        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: Local Frequency Offset/Temps%(clipped)s"
set ytics format "%(fmt)s %(unit)s" nomirror textcolor rgb '#0060ad'
set y2tics format "%(fmt_f)s %(unit_f)s" nomirror textcolor rgb '#dd181f'
set y2range [%(min_y2)s:%(max_y2)s]
set key top right
set style line 1 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
plot \
 "-" using 1:($2*%(multiplier_f)s) title "frequency offset %(unit_f)s" \
     with linespoints ls 1 axis x1y2, \
""" % out

        for key in tempslist:
            out['key'] = key
            plot_template += "'-' using 1:2 title '%(key)s' with line, \\\n" \
                             % out

        # strip trailing ", \n"
        plot_template = plot_template[:-4] + "\n"

        exp = """\
<p>The frequency offsets and temperatures.
Showing frequency offset (red, in parts
per million, scale on right) and the temperatures.</p>

<p>These are field 4 (frequency) from the loopstats log
file, and field 3 from the tempstats log file.</p>

"""

        ret = {'html': VizStats.table_head + stats_f.table +
               table + VizStats.table_tail + exp,
               'plot': plot_template + plot_data + plot_data_t,
               'stats': stats,
               'title': "Local Frequency/Temp"}
        return ret

    def local_temps_gnuplot(self):
        "Generate gnuplot code graphing local temperature statistics"
        sitename = self.sitename
        tempsmap = self.tempssplit()
        tempslist = list(tempsmap.keys())
        tempslist.sort()

        if not len(tempsmap) or not len(tempslist):
            sys.stderr.write("ntpviz: WARNING: no temps to graph\n")
            return ''

        stats = []
        plot_data = ''
        max_temp = -300
        min_temp = 1000
        for key in tempslist:
            # speed up by only sending gnuplot the data it will actually use
            # fields: time, temp
            (p, v) = self.plot_slice(tempsmap[key], 3)
            s = VizStats(v, 'Temp %s' % key, units='°C')
            max_temp = max(s.percs["max_y"], max_temp)
            min_temp = min(s.percs["min_y"], min_temp)
            plot_data += p

        out = {}
        out['fmt'] = gnuplot_fmt(min_temp, max_temp)
        out['sitename'] = sitename
        out['size'] = args.png_size

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: Local Temperatures"
set ytics format "%(fmt)s °C" nomirror textcolor rgb '#0060ad'
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set key top right
plot \\
""" % out

        for key in tempslist:
            out['key'] = key
            plot_template += "'-' using 1:2 title '%(key)s' with line, \\\n" \
                             % out

        # strip the trailing ", \n"
        plot_template = plot_template[:-4] + "\n"
        exp = """\
<p>Local temperatures.  These will be site-specific depending upon what
temperature sensors you collect data from.
Temperature changes affect the local clock crystal frequency and
stability.  The math of how temperature changes frequency is
complex, and also depends on crystal aging.  So there is no easy
way to correct for it in software.  This is the single most important
component of frequency drift.</p>
<p>The Local Temperatures are from field 3 from the tempstats log file.</p>
"""

        ret = {'html': exp, 'stats': stats}
        ret['title'] = "Local Temperatures"
        ret['plot'] = plot_template + plot_data
        return ret

    def local_gps_gnuplot(self):
        "Generate gnuplot code graphing local GPS statistics"
        sitename = self.sitename
        gpsmap = self.gpssplit()
        gpslist = list(gpsmap.keys())
        gpslist.sort()

        if not len(gpsmap) or not len(gpslist):
            if 1 <= args.debug_level:
                sys.stderr.write("ntpviz: INFO: no GPS data to graph\n")
            return ''

        # build the output dictionary, because Python can not format
        # complex objects.
        values_nsat = []
        values_tdop = []
        plot_data = ""
        for key in gpslist:
            # fields: time, TDOP, nSats
            (ps, values_tdop, values_nsat) = self.plot_slice(gpsmap[key], 3, 4)
            plot_data += ps

        stats = VizStats(values_nsat, "nSats", units='nSat')
        stats_tdop = VizStats(values_tdop, "TDOP", units=' ')

        out = stats_tdop.percs
        out['sitename'] = sitename
        out['size'] = args.png_size
        if out['min_y'] == out['max_y']:
            # some GPS always output the same TDOP
            if 0 == out['min_y']:
                # scale 0:1
                out['max_y'] = 1
            else:
                # scale +/- 20%
                out['min_y'] = out['max_y'] * 0.8
                out['max_y'] = out['max_y'] * 1.2
        elif 2 > out['min_y']:
            # scale 0:max_x
            out['min_y'] = 0

        # recalc fmt
        out['fmt'] = gnuplot_fmt(out["min_y"], out["max_y"])

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: Local GPS%(clipped)s
set ytics format "%(fmt)s TDOP" nomirror textcolor rgb '#0060ad'
set yrange [%(min_y)s:%(max_y)s]
set y2tics format "%%2.0f nSat"  nomirror textcolor rgb '#dd181f'
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
set key top right
plot \\
""" % out

        for key in gpslist:
            plot_template += """\
'-' using 1:2 title '%(key)s TDOP' with line ls 1, \\
'-' using 1:3 title '%(key)s nSat' with line ls 2 axis x1y2, \\
""" % locals()

        # strip the trailing ", \\n"
        plot_template = plot_template[:-4] + "\n"
        exp = """\
<p>Local GPS.  The Time Dilution of Precision (TDOP) is plotted in blue.
The number of visible satellites (nSat) is plotted in red.</p>
<p>TDOP is field 3, and nSats is field 4, from the gpsd log file.  The
gpsd log file is created by the ntploggps program.</p>
<p>TDOP is a dimensionless error factor.  TDOP ranges from 1 to greater
than 20.  1 denotes the highest possible confidence level.  2 to 5 is good.
Greater than 20 means there will be significant inaccuracy and error.</p>
"""

        ret = {'html': VizStats.table_head + stats.table +
               stats_tdop.table + VizStats.table_tail + exp,
               'stats': [stats, stats_tdop],
               'title': "Local GPS",
               'plot': plot_template + plot_data + plot_data}
        return ret

    def local_error_gnuplot(self):
        "Plot the local clock frequency error."
        if not len(self.loopstats):
            sys.stderr.write("ntpviz: WARNING: no loopstats to graph\n")
            return ''

        # grab and sort the values, no need for the timestamp, etc.

        # speed up by only sending gnuplot the data it will actually use
        # fields: time, freq error
        (plot_data, values) = self.plot_slice(self.loopstats, 3)

        # compute frequency offset
        stats = VizStats(values, "Local Clock Frequency Offset", freq=1,)

        # build the output dictionary, because Python can not format
        # complex objects.
        out = stats.percs
        out["fmt"] = stats.percs["fmt"]
        out["sitename"] = self.sitename
        out['size'] = args.png_size

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: Local Clock Frequency Offset%(clipped)s"
set ytics format "%(fmt)s %(unit)s" nomirror
set yrange [%(min_y)s:%(max_y)s]
set key bottom right
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
plot \
 "-" using 1:($2 * %(multiplier)s) title "local clock error" \
     with linespoints ls 2, \
 %(p99)s title "99th percentile", \
 %(p95)s title "95th percentile", \
 %(p5)s title "5th percentile", \
 %(p1)s title "1st percentile"
""" % out

        exp = """\
<p>The frequency offset of the local clock (aka drift).  The
graph includes percentile data to show how much the frequency changes
over a longer period of time.  The majority of this change should come
from temperature changes (ex: HVAC, the weather, CPU usage causing local
heating).</p>

<p>Smaller changes are better.  An ideal result would be a horizontal
line at 0ppm.  Expected values of 99%-1% percentiles: 0.4ppm</p>

<p>The Frequency Offset comes from field 4 of the loopstats log file.</p>
"""
        ret = {'html': VizStats.table_head + stats.table +
               VizStats.table_tail + exp,
               'plot': plot_template + plot_data,
               'stats': [stats],
               'title': "Local Clock Frequency Offset"}
        return ret

    def loopstats_gnuplot(self, fld, title, legend, freq):
        "Generate gnuplot code of a given loopstats field"
        if not self.loopstats:
            sys.stderr.write("ntpviz: WARNING: no loopstats to graph\n")
            return ''

        # speed up by only sending gnuplot the data it will actually use
        # fields: time, fld
        (plot_data, values) = self.plot_slice(self.loopstats, fld)

        # process the values
        stats = VizStats(values, title, freq=freq)

        # build the output dictionary, because Python can not format
        # complex objects.
        out = stats.percs
        out["fld"] = fld
        out["fmt"] = stats.percs["fmt"]
        out["legend"] = legend
        out["min_y"] = '0'
        out["sitename"] = self.sitename
        out['size'] = args.png_size

        if freq:
            exp = """\
<p>The RMS Frequency Jitter (aka wander) of the local
clock's frequency.  In other words, how fast the local clock changes
frequency.</p>

<p>Lower is better.  An ideal clock would be a horizontal line at
0ppm.</p>

<p> RMS Frequency Jitter is field 6 in the loopstats log file.</p>
"""
        else:
            exp = """\
<p>The RMS Jitter of the local clock offset.  In other words,
how fast the local clock offset is changing.</p>

<p>Lower is better.  An ideal system would be a horizontal line at 0μs.</p>

<p>RMS jitter is field 5 in the loopstats log file.</p>
"""

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: %(title)s%(clipped)s"
set ytics format "%(fmt)s %(unit)s" nomirror
set yrange [%(min_y)s:%(max_y)s]
set key top right
set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 7 ps 0   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 1 pt 5 ps 0   # --- red
plot \
 "-" using 1:($2*%(multiplier)s) title "%(legend)s" with linespoints ls 1, \
 %(p99)s title "99th percentile", \
 %(p95)s title "95th percentile", \
 %(p5)s title "5th percentile", \
 %(p1)s title "1st percentile"
""" % out

        ret = {'html': VizStats.table_head + stats.table +
               VizStats.table_tail + exp,
               'plot': plot_template + plot_data,
               'stats': [stats],
               'title': title}
        return ret

    def local_offset_jitter_gnuplot(self):
        "Generate gnuplot code of local clock loop standard deviation"
        return self.loopstats_gnuplot(4, "Local RMS Time Jitter", "Jitter", 0)

    def local_offset_stability_gnuplot(self):
        "Generate gnuplot code graphing local clock stability"
        return self.loopstats_gnuplot(5, "Local RMS Frequency Jitter",
                                      "Stability", 1)

    def peerstats_gnuplot(self, peerlist, fld, title, ptype):
        "Plot a specified field from peerstats."

        peerdict = self.peersplit()
        if not peerlist:
            peerlist = list(peerdict.keys())
            if not peerlist:
                sys.stderr.write("ntpviz: WARNING: no server data to graph\n")
                return ''
        peerlist.sort()  # For stability of output
        namelist = []    # peer names

        ip_todo = []
        for key in peerlist:
            # Trickiness - we allow peerlist elements to be DNS names.
            # The socket.gethostbyname() call maps DNS names to IP addresses,
            # passing through literal IPv4 addresses unaltered.  However,
            # it barfs on either literal IPv6 addresses or refclock names.
            try:
                ip = socket.gethostbyname(key)
                namelist.append(key)
            except socket.error:
                # ignore it
                ip = key
                # socket.getfqdn() is also flakey...
                namelist.append(socket.getfqdn(key))

            if ip in peerdict:
                ip_todo.append(ip)
            else:
                # can this ever happen?
                sys.stderr.write("ntpviz: ERROR: No such server as %s" % key)
                raise SystemExit(1)

        rtt = 0
        percentages = ""
        stats = []
        if len(peerlist) == 1:
            # only one peer
            if "offset" == ptype:
                # doing offset, not jitter
                rtt = 1
                if "127.127." == peerlist[0][:8]:
                    # don't do rtt for reclocks
                    rtt = 0
                    title = "Refclock Offset " + str(peerlist[0])
                    exp = """\
<p>The offset of a local refclock in seconds.  This is
useful to see how the measured offset is behaving.</p>

<p>Closer to 0s is better.  An ideal system would be a horizontal line
at 0s. Typical 90% ranges may be: local serial GPS 200 ms; local PPS
20µs.</p>

<p>Clock Offset is field 5 in the peerstats log file.</p>
"""
                else:
                    title = "Server Offset " + str(peerlist[0])
                    exp = """\
<p>The offset of a server in seconds.  This is
useful to see how the measured offset is behaving.</p>

<p>The chart also plots offset±rtt,  where rtt is the round trip time
to the server.  NTP can not really know the offset of a remote chimer,
NTP computes it by subtracting rtt/2 from the offset.  Plotting the
offset±rtt reverses this calculation to more easily see the effects of
rtt changes.</p>

<p>Closer to 0s is better.  An ideal system would be a horizontal line
at 0s. Typical 90% ranges may be: local LAN server 80µs; 90% ranges for
WAN server may be 4ms and much larger. </p>

<p>Clock Offset is field 5 in the peerstats log file.  The Round Trip
Time (rtt) is field 6 in the peerstats log file.</p>
"""

            else:
                # doing jitter, not offset
                if "127.127." == peerlist[0][:8]:
                    title = "Refclock RMS Jitter " + str(peerlist[0])
                    exp = """\
<p>The RMS Jitter of a local refclock.  Jitter is the
current estimated dispersion, in other words the variation in offset
between samples.</p>

<p>Closer to 0s is better.  An ideal system would be a horizontal
line at 0s.</p>

<p>RMS Jitter is field 8 in the peerstats log file.</p>
"""
                else:
                    title = "Server Jitter " + str(peerlist[0])
                    exp = """\
<p>The RMS Jitter of a server.  Jitter is the
current estimated dispersion, in other words the variation in offset
between samples.</p>

<p>Closer to 0s is better.  An ideal system would be a horizontal line
at 0s.</p>

<p>RMS Jitter is field 8 in the peerstats log file.</p>
"""

            if namelist[0] and peerlist[0] != namelist[0]:
                # append hostname, if we have it
                # after stats to keep summary short
                title += " (%s)" % namelist[0]

        else:
            # many peers
            title += "s"

            if "offset" == ptype:
                title = "Server Offsets"
                exp = """\
<p>The offset of all refclocks and servers.
This can be useful to see if offset changes are happening in
a single clock or all clocks together.</p>

<p>Clock Offset is field 5 in the peerstats log file.</p>
"""
            else:
                title = "Server Jitters"
                exp = """\
<p>The RMS Jitter of all refclocks and servers. Jitter is the
current estimated dispersion, in other words the variation in offset
between samples.</p>

<p>Closer to 0s is better.  An ideal system would be a horizontal line
at 0s.</p>

<p>RMS Jitter is field 8 in the peerstats log file.</p>
"""

        if len(peerlist) == 1:
            if peerlist[0] in refclock_name:
                title += ' ' + refclock_name[peerlist[0]]

        plot_data = ""
        for ip in ip_todo:
            # 20% speed up by only sending gnuplot the data it will
            # actually use
            if rtt:
                # fields: time, fld, and rtt
                pt = self.plot_slice(peerdict[ip], fld, 5)
                plot_data += pt[0]
            else:
                # fields: time, fld
                pt = self.plot_slice(peerdict[ip], fld)
                plot_data += pt[0]

        stats = VizStats(pt[1], title)
        if len(peerlist) == 1:
            percentages = " %(p50)s title '50th percentile', " % stats.percs
        else:
            # skip stats on peers/offsets plots
            stats.skip_summary = True
            stats.table = ''

        out = stats.percs
        out['sitename'] = self.sitename
        out['size'] = args.png_size
        out['title'] = title
        out["fmt"] = stats.percs["fmt"]

        if 6 >= len(peerlist):
            out['set_key'] = "set key top right"
        elif 12 >= len(peerlist):
            # getting crowded
            out['set_key'] = "set key bmargin"
        else:
            # too many keys to show
            out['set_key'] = "set key off"

        plot_template = NTPViz.Common + """\
set terminal png size %(size)s
set title "%(sitename)s: %(title)s%(clipped)s"
set ylabel ""
set ytics format "%(fmt)s %(unit)s" nomirror
set yrange [%(min_y)s:%(max_y)s]
%(set_key)s
plot \
""" % out

        plot_template += percentages
        for key in peerlist:
            out['label'] = self.ip_label(key)
            plot_template += "'-' using 1:($2*%(multiplier)s) " \
                             " title '%(label)s' with line, \\\n" % out

        if 1 == rtt:
            plot_template += """\
'-' using 1:(($2+$3/2)*%(multiplier)s) title 'offset+rtt/2' with line, \\
'-' using 1:(($2-$3/2)*%(multiplier)s) title 'offset-rtt/2' with line
""" % stats.percs
            # sadly, gnuplot needs 3 identical copies of the data.
            plot_template += plot_data + plot_data
        else:
            # strip the trailing ", \n"
            plot_template = plot_template[:-4] + "\n"

        if len(peerlist) == 1:
            # skip stats for multiplots
            html = VizStats.table_head + stats.table \
                + VizStats.table_tail + exp,
        else:
            html = exp

        ret = {'html': html,
               'plot': plot_template + plot_data,
               'stats': [stats],
               'title': title}
        return ret

    def peer_offsets_gnuplot(self, peerlist=None):
        "gnuplot Peer Offsets"
        return self.peerstats_gnuplot(peerlist, 4, "Server Offset",
                                      "offset")

    def peer_jitters_gnuplot(self, peerlist=None):
        "gnuplot Peer Jitters"
        return self.peerstats_gnuplot(peerlist, 7, "Server Jitter",
                                      "jitter")

    def local_offset_histogram_gnuplot(self):
        "Plot a histogram of clock offset values from loopstats."
        if not self.loopstats:
            sys.stderr.write("ntpviz: WARNING: no loopstats to graph\n")
            return ''

        # TODO normalize to 0 to 100?

        # grab and sort the values, no need for the timestamp, etc.
        values = [float(row[2]) for row in self.loopstats]
        stats = VizStats(values, 'Local Clock Offset')
        out = stats.percs
        out["fmt_x"] = stats.percs["fmt"]
        out['sitename'] = self.sitename
        # flip the axis
        out['min_x'] = out['min_y']
        out['max_x'] = out['max_y']

        rnd1 = 7        # round to 100 ns boxes
        out['boxwidth'] = 1e-7

        # between -10us and 10us
        if 1e-5 > stats.percs["p99"] and -1e-5 < stats.percs["p1"]:
            # go to nanosec
            rnd1 = 9        # round to 1 ns boxes
            out['boxwidth'] = S_PER_NS

        # Python 2.6  has no collections.Counter(), so fake it.
        cnt = collections.defaultdict(int)
        for value in values:
            # put into buckets
            # for a +/- 50 microSec range that is 1,000 buckets to plot
            cnt[round(value, rnd1)] += 1

        sigma = True
        if args.clip:
            if stats.percs['p1sigma'] > stats.percs['p99'] or \
               stats.percs['m1sigma'] < stats.percs['p1']:
                # sigma out of range, do not plot
                sigma = ''

        out['sigma'] = ''
        if sigma:
            # plus/minus of one sigma range
            out['sigma'] = """\
set style arrow 1 nohead
set linestyle 1 linecolor rgb "#009900"
set arrow from %(m1sigma)s,graph 0 to %(m1sigma)s,graph 0.90 as 1 ls 1
set arrow from %(p1sigma)s,graph 0 to %(p1sigma)s,graph 0.90 as 1 ls 1
set label 1 "-1σ" at %(m1sigma)s, graph 0.96 left front offset -1,-1 \
    textcolor rgb "#009900"
set label 2 "+1σ" at %(p1sigma)s, graph 0.96 left front offset -1,-1 \
    textcolor rgb "#009900"
""" % out

        out['size'] = args.png_size

        # in 2016, 25% of screens are 1024x768, 42% are 1388x768
        # but leave some room for the browser frame
        plot_template = '''\
set terminal png size %(size)s
set title "%(sitename)s: Local Clock Time Offset Histogram%(clipped)s"
set grid
set boxwidth %(boxwidth)s
set xtics format "%(fmt_x)s %(unit)s" nomirror
set xrange [%(min_x)s:%(max_x)s]
set yrange [0:*]
set style arrow 3 nohead
set arrow from %(p99)s,graph 0 to %(p99)s,graph 0.30 as 3
set style arrow 4 nohead
set arrow from %(p95)s,graph 0 to %(p95)s,graph 0.45 as 4
set style arrow 5 nohead
set arrow from %(p5)s,graph 0 to %(p5)s,graph 0.45 as 5
set style arrow 6 nohead
set arrow from %(p1)s,graph 0 to %(p1)s,graph 0.30 as 6
set key off
set lmargin 10
set rmargin 10
set style fill solid 0.5
set label 3 "99%%" at %(p99)s, graph 0.35  left front offset -1,-1
set label 4 "95%%" at %(p95)s, graph 0.50  left front offset -1,-1
set label 5 "1%%" at %(p1)s, graph 0.35  left front offset -1,-1
set label 6 "5%%" at %(p5)s, graph 0.50  left front offset -1,-1
%(sigma)s
plot \
 "-" using ($1 * %(multiplier)s):2 title "histogram" with boxes
''' % out

        histogram_data = ["%s %s\n" % (k, v) for k, v in cnt.items()]

        exp = """\
<p>The clock offsets of the local clock as a histogram.</p>

<p>The Local Clock Offset is field 3 from the loopstats log file.</p>
"""
        # don't return stats, it's just a dupe
        ret = {'html': VizStats.table_head + stats.table +
               VizStats.table_tail + exp,
               'plot': plot_template + "".join(histogram_data) + "e\n",
               'stats': [],
               'title': "Local Clock Time Offset Histogram"}
        return ret


# Multiplotting can't live inside NTPViz because it consumes a list
# of such objects, not a single one.


def local_offset_multiplot(statlist):
    "Plot comparative local offsets for a list of NTPViz objects."

    out = []
    out['size'] = args.png_size

    plot = NTPViz.Common + '''\
set terminal png size %(size)s
set title "Multiplot Local Clock Offsets"
set ytics format "%1.2f μs" nomirror textcolor rgb "#0060ad"
set key bottom right box
plot \\
''' % out
    # FIXME: probably need to be more flexible about computing the plot label
    sitenames = [os.path.basename(os.path.dirname(dr))
                 for dr in args.statsdirs]
    for (i, stats) in enumerate(statlist):
        plot += '"-" using 1:($2*1000000) title "%s clock offset μs"  ' \
                'with linespoints, \\\n' % (sitenames[i])
    plot = plot[:-4] + "\n"

    plot_data = ''
    for stats in statlist:
        # speed up by only sending gnuplot the data it will actually use
        # fields: time, offset
        pt = NTPViz.plot_slice(stats.loopstats, 2)
        plot_data += pt[0]

    ret = {'html': '', 'stats': []}
    ret['title'] = "Multiplot"
    ret['plot'] = plot + plot_data
    return ret


# here is how to create the base64 from an image file:
# with open("path/to/file.png", "rb") as f:
#    data = f.read()
#    print data.encode("base64")
#
ntpsec_logo = """
iVBORw0KGgoAAAANSUhEUgAAAEAAAABKCAQAAACh+5ozAAAABGdBTUEAALGPC/xhBQAAAAFzUkdC
AK7OHOkAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAJiS0dE
AP7wiPwpAAAACXBIWXMAAAsTAAALEwEAmpwYAAAFKElEQVRo3s2ZT0wcVRzHPzMLKCwsNgqLkYPS
xBjbRF3TcKlC4VAhFU0AdRN7a+zBEsUEL0qImqoxMTWhBzEkTdqmREhMCgpeiiV6KVE46MVE1KQg
uxv/df81tLvzPOzsMjs7sztvd7b4ndPsfPf3vu/33vv93vs9yGCIJMLyWaKJXTSxZMMTCITilJ1k
KENRdeoB6rHGYboNb80cpAjEQZoNr90ctiHWcyBfgD0aCZTk2CFAYylKTd7bVZYNknycwGf5ryjT
RE2/OWVr9Bh9ahbwnuGtnRdsTZ5h0/Rbhr1PDYhNUZyt2guwRjdazi8+G0lZeMWoeExna3mzxwbO
BDgwlIWQYhefhCkSNl8SpCpkO/JAiHFO00D+kCokGa8JpRyylSTjIlSeAPiC7/AU/JomknLM9qRb
Ijv8XaaANNs4hyU7VcJE6UBUZeR7wLjgqgXT4jQL6JYw5Qqy/U3e6YazLWY9cJ5DDOc+/kvU9aHQ
8HFP7m2O8/kCwoyQYgAvAD8xwja1rjUugA7e15NzgnlGCRfSvATZII1A4yv1KIqL/R/iF9IIBCGC
itfOtEoHs/qeJURQ90elaGOCbQSCtLKhDOd/LJTiZ1KfDXGW+aFiP2h00o8CJJhX3m75PabdLMZX
jIrdfIq6vhDDhFxtfkV9xtqXlrmgjltzHGIMSBMhXcEAeGjFAyxrX1sTLAXcAvTsHuE5tixjgga6
NA92OUXjAS5zfzGFpXZEabb5w7Jn99LMAI3EmecGf9n4SS3lPydbskKjD3GcIM3ch4c0Y9xghgv8
hiZvrBwBg3zIgwj+1FN9LfsZ52Uu8ikhWWPyAoY5Swu/coEZYmio+DhGD31M8CgjViG2PEwgEFyn
3dR8GMEsHahAF+/SBezGjkums1A71xEIJtwR0K837zdwdk0HiRNnQE6ATNL1cpJWFjll4+YF5vFy
Qi6DyAhop5MkU0Rsvsd5hzC99FZLwAB+NlktwtjkGg08US0BDcDlogstwRoQkBkE2WVYePw6ondD
ZZUFAALssz2mVSwgHzFCPMwjAHhoY1HehKyAAF5D76aZNXyL6nF/jX+qI2CdJJ2087Ohyfw6iZcA
sOZ8AOQm4Sqb+HmpCKOXXhKsS9iUEhDiEnCc/TbfWzmJlytcqZYAuMgG+/kgF4qN8HOWfiJMyQxA
MRRLRoscy0s62e18GNOmu3QukF0Fc8AkfTzFN6zwJXEET9LF83QQ4RRz7vTe3gOg0McCMQQpQmyx
RRRBnAX6LPa9rnsABEt8yxG6eFavC8dZYYqrxMvpZ3mRMM4Ci3ycqwhFC+qmVRYAsvWjsgX4GC2/
d5SurNoK8Oo1ch9vuNFP+XN2kJjLR9Nh64asPNDEa7xKIxVNLgN8+PAzCVZRwurEGuQzGoEwr7Ni
USmVQ5ouPsFPpgzkIFBlD+a2TpOF6txmPtXVMpkTCZ5d2jaDblaoABjUqy4mCcZ2+jlHK3CTt/gc
xdUqmUDwIqepBzY4ykahgFbO0Q9AirCp6u8OFPz6qpvhlcLMMeZ6Wcr+iSu5E+TuTGvIyqzuA4BX
5E5P5kAUrZuucSP42CDl2zHdLhYI2DmzsylhURYFd5F7fmOy5wJqaFbb7h5Q65PdGoDvrtEqz4HM
APTUfn97HZW4whKPKy14sgvf9QhoQi7ARImi8KNSlZAjgewqcCfzy0DfrGUFTPORi1c0pXGbNzOb
vV0PuFZgdAjd4/+DZZjBnbgzNSJ3f7rnq0AltrcCPMR4mro9a3/9Pwl2Z1Rsm9zNAAAAJXRFWHRk
YXRlOmNyZWF0ZQAyMDE1LTA2LTI5VDE4OjMwOjA3LTA0OjAwZxkj2wAAACV0RVh0ZGF0ZTptb2Rp
ZnkAMjAxNS0wNi0yOVQxODozMDowNy0wNDowMBZEm2cAAAAASUVORK5CYII=
"""

ntpsec_ico = """\
AAABAAEAEBAAAAEAIABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAAAQAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/
AAAA/wAAAP8AAAD/AAAAAAAAAP+fn59gn5+fYJ+fn2Cfn59gn5+fYJ+fn2Cfn59gn5+fYJ+fn2Cf
n59gn5+fYJ+fn2B/f39/AAAA/wAAAAAAAAAAAAAA/5+fn2Cfn59gn5+fYJ+fn2Cfn59gn5+fYJ+f
n2Cfn59gn5+fYJ+fn2Cfn59gAAAA/wAAAAAAAAAAAAAAAAAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA
/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAAAAAAAAAAAAAAAAAAAAAAA/5+fn2Cfn59g
n5+fYJ+fn2Cfn59gn5+fYJ+fn2Cfn59gn5+fYAAAAP8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+f
n59gn5+fYAAAAP8AAAD/AAAA/wAAAP8AAAD/n5+fYJ+fn2AAAAD/AAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAD/n5+fYAAAAP8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+fn59gAAAA/wAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAA/5+fn2AAAAD/AAAAAAAAAAAAAAD/AAAA/01NTWAAAAD/n5+fYAAAAP8AAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAP+fn59gAAAA/wAAAAAAAAAAAAAA/wAAAAAAAAAAAAAA/5+fn2AA
AAD/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/n5+fYAAAAP8AAAAAAAAAAE1NTWAAAAAAAAAAAAAA
AP+fn59gAAAA/wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/5+fn2Cfn59gAAAA/wAAAP8AAAD/AAAA
/wAAAP+fn59gn5+fYAAAAP8AAAAAAAAAAAAAAAAAAAAAAAAAAAEBAf+fn59gn5+fYJ+fn2Cfn59g
n5+fYJ+fn2Cfn59gn5+fYJ+fn2AAAAD/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/AAAA/wAAAP8A
AAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAAAAAAAAAAAAAAAAAAAAAAD/AAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/AAAAAAAAAAAAAAAAAAAA
/wAAAAAAAAD/AAAA/wAAAP8AAAAAAAAAAAAAAAAAAAD/AAAA/wAAAP8AAAAAAAAA/wAAAAAAAAAA
AAAAAAAAAP8AAAD/AAAA/wAAAAAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAAAAAAD/AAAA/wAAAP8A
AAAAgAAAAL/+AADf/QAAwAEAAO/7AADsGwAA6+sAAOsrAADrawAA6+sAAOwbAADv+wAA4AMAAN/9
AADRxQAAxBEAAA==
"""

if __name__ == '__main__':

    parser = MyArgumentParser(description="ntpd stats visualizer",
                              fromfile_prefix_chars='@',
                              epilog="""
You can place command line options in a file, one per line.

See the manual page for details.

Python by ESR, concept and gnuplot code by Dan Drown.
""")
    group = parser.add_mutually_exclusive_group()

    parser.add_argument('-c', '--clip',
                        action="store_true",
                        dest='clip',
                        help="Clip plots at 1%% and 99%%")
    parser.add_argument('-d', '--datadir',
                        default="/var/log/ntpstats",
                        dest='statsdirs',
                        help="one or more log file directories to read",
                        type=str)
    parser.add_argument('-e', '--endtime',
                        dest='endtime',
                        help="End time in POSIX (seconds) or ISO 8601",
                        type=str)
    parser.add_argument('-g', '--generate',
                        action="store_true",
                        dest='generate',
                        help="Run plot through gnuplot to make png")
    parser.add_argument('-n', '--name',
                        default=socket.getfqdn(),
                        dest='sitename',
                        help="sitename (title)",
                        type=str)
    parser.add_argument('-o', '--outdir',
                        default="ntpgraphs",
                        dest='outdir',
                        help="output directory",
                        type=str)
    parser.add_argument('-p', '--period',
                        default=7,    # default to 7 days
                        dest='period',
                        help="period in days to graph (float)",
                        type=float)
    parser.add_argument('-s', '--starttime',
                        dest='starttime',
                        help="Start time in POSIX (seconds) or ISO 8601",
                        type=str)
    parser.add_argument('-w', '--width',
                        choices=['s', 'm', 'l'],
                        default='m',
                        dest='width',
                        help="PNG width: s, m, or l",
                        type=str)
    group.add_argument('--all-peer-jitters',
                       action="store_true",
                       dest='show_peer_jitters',
                       help="Plot all peer jitters")
    group.add_argument('--peer-jitters',
                       default='',
                       dest='peer_jitters',
                       help="Plot peer jitters.  Comma separated host list.",
                       type=str)
    group.add_argument('--all-peer-offsets',
                       action="store_true",
                       dest='show_peer_offsets',
                       help="Plot all peer offsets")
    group.add_argument('--peer-offsets',
                       default='',
                       dest='peer_offsets',
                       help="Plot peer offsets.  Comma separated host list.",
                       type=str)
    group.add_argument('--local-error',
                       action="store_true",
                       dest='show_local_error',
                       help="Plot local clock frequency offsets")
    group.add_argument('--local-freq-temps',
                       action="store_true",
                       dest='show_freq_temps',
                       help="Plot local frequency vs temperature data")
    group.add_argument('--local-gps',
                       action="store_true",
                       dest='show_gps',
                       help="Plot gpsd TDOP and nSats")
    group.add_argument('--local-jitter',
                       action="store_true",
                       dest='show_local_jitter',
                       help="Plot clock time jitter")
    group.add_argument('--local-offset',
                       action="store_true",
                       dest='show_local_offset',
                       help="Plot Clock frequency offset")
    group.add_argument('--local-offset-histogram',
                       action="store_true",
                       dest='show_local_offset_histogram',
                       help="Plot histogram of loopstats time offsets")
    group.add_argument('--local-offset-multiplot',
                       action="store_true",
                       dest='show_local_offset_multiplot',
                       help="Plot comparative local offsets for "
                            "multiple directories")
    group.add_argument('--local-stability',
                       action="store_true",
                       dest='show_local_stability',
                       help="Plot RMS frequency-jitter")
    group.add_argument('--local-temps',
                       action="store_true",
                       dest='show_temps',
                       help="Plot local temperature data")
    parser.add_argument('-D', '--debug',
                        default=0,
                        dest='debug_level',
                        help="debug level, 0 (none) to 9 (most)",
                        type=int)
    # some OS do not support os.nice()
    try:
        os.nice(0)
        parser.add_argument('-N', '--nice',
                            action="store_true",
                            dest='nice',
                            help="Run as lowest priority")
    except OSError:
        pass

    parser.add_argument('-V', '--version',
                        action="version",
                        version="ntpviz %s" % ntp.util.stdversion())

    args = parser.parse_args()

    if args.nice:
        # run at lowest possible priority
        nice = os.nice(19)
        if args.debug_level:
            sys.stderr.write("ntpviz: INFO: Now running at nice %s\n" % nice)

    if 's' == args.width:
        # fit in 1024x768 browser
        # in 2016 this is 22% of all browsers
        args.png_size = '1000,720'
    elif 'l' == args.width:
        # fit in 1920x1080 browser
        args.png_size = '1850,1000'
    else:
        # fit in 1388x768 browser
        # in 2016 this is 42% of all browsers
        args.png_size = '1340,720'

    args.period = int(float(args.period) * ntp.statfiles.NTPStats.SecondsInDay)
    if args.endtime is not None:
        args.endtime = ntp.statfiles.iso_to_posix(args.endtime)
    if args.starttime is not None:
        args.starttime = ntp.statfiles.iso_to_posix(args.starttime)

    args.statsdirs = [os.path.expanduser(path)
                      for path in args.statsdirs.split(",")]

    if args.show_peer_offsets is True:
        args.show_peer_offsets = []
    elif args.peer_offsets:
        args.show_peer_offsets = args.peer_offsets.split(",")
    else:
        args.show_peer_offsets = None

    if args.show_peer_jitters is True:
        args.show_peer_jitters = []
    elif args.peer_jitters:
        args.show_peer_jitters = args.peer_jitters.split(",")
    else:
        args.show_peer_jitters = None

    if 0 < args.debug_level:
        sys.stderr.write("ntpviz: INFO: now running at debug: %s\n" %
                         args.debug_level)
        sys.stderr.write("ntpviz: INFO: Version: %s\n" % ntp.util.stdversion())
        sys.stderr.write("ntpviz: INFO: Parsed Options %s\n" % args)

        if 9 == args.debug_level:
            # crazy debug, also profile
            import cProfile
            pr = cProfile.Profile()
            pr.enable()

            # register to dump debug on all normal exits
            atexit.register(print_profile)

    nice = 19       # always run nicely
    if 0 != nice:
        try:
            import psutil
            # set ionice() to idle
            p = psutil.Process(os.getpid())
            p.ionice(psutil.IOPRIO_CLASS_IDLE)

        except ImportError:
            if 0 < args.debug_level:
                sys.stderr.write("ntpviz: INFO: psutil not found\n")

        # set nice()
        nice = os.nice(nice)
        if 2 < args.debug_level:
            sys.stderr.write("ntpviz: INFO: now running at nice: %s\n" % nice)

    for fontpath in ("/usr/share/fonts/liberation",
                     "/usr/share/fonts/liberation-fonts",
                     "/usr/share/fonts/truetype/liberation"):

        if os.path.exists(fontpath):
            os.environ["GDFONTPATH"] = fontpath
            break
    else:
        sys.stderr.write(
            "ntpviz: WARNING: liberation truetype fonts not found\n")
    os.environ["GNUPLOT_DEFAULT_GDFONT"] = "LiberationSans-Regular"

    plot = None

    if 1 == len(args.statsdirs):
        statlist = [NTPViz(statsdir=args.statsdirs[0], sitename=args.sitename,
                           period=args.period, starttime=args.starttime,
                           endtime=args.endtime)]
    else:
        statlist = [NTPViz(statsdir=d, sitename=d,
                           period=args.period, starttime=args.starttime,
                           endtime=args.endtime)
                    for d in args.statsdirs]

    if len(statlist) == 1:
        stats = statlist[0]

        if args.show_local_offset or \
           args.show_local_error or \
           args.show_local_jitter or \
           args.show_local_stability or \
           args.show_local_offset_histogram:
            if not stats.loopstats:
                sys.stderr.write("ntpviz: ERROR: missing loopstats data\n")
                raise SystemExit(1)

            if args.show_local_offset:
                plot = stats.local_offset_gnuplot()
            elif args.show_local_error:
                plot = stats.local_error_gnuplot()
            elif args.show_local_jitter:
                plot = stats.local_offset_jitter_gnuplot()
            elif args.show_local_stability:
                plot = stats.local_offset_stability_gnuplot()
            elif args.show_local_offset_histogram:
                plot = stats.local_offset_histogram_gnuplot()

        if args.show_peer_offsets is not None or \
           args.show_peer_jitters is not None:
            if not stats.peerstats:
                sys.stderr.write("ntpviz: ERROR:  missing peerstats data\n")
                raise SystemExit(1)
            if args.show_peer_offsets is not None:
                plot = stats.peer_offsets_gnuplot(args.show_peer_offsets)
            if args.show_peer_jitters is not None:
                plot = stats.peer_jitters_gnuplot(args.show_peer_jitters)

        if args.show_freq_temps:
            if not stats.temps:
                sys.stderr.write("ntpviz: ERROR: missing temps data\n")
                raise SystemExit(1)
            plot = stats.local_freq_temps_plot()

        if args.show_temps:
            if not stats.temps:
                sys.stderr.write("ntpviz: ERROR: missing temps data\n")
                raise SystemExit(1)
            plot = stats.local_temps_gnuplot()

        if args.show_gps:
            if not stats.gpsd:
                sys.stderr.write("ntpviz: ERROR: missing gps data\n")
                raise SystemExit(1)
            plot = stats.local_gps_gnuplot()

    if args.show_local_offset_multiplot:
        plot = local_offset_multiplot(statlist)

    if plot is not None:
        # finish up the plot, and exit
        if args.generate:
            gnuplot(plot['plot'])
        else:
            sys.stdout.write(plot['plot'])
        raise SystemExit(0)

    # Fall through to HTML code generation
    if not os.path.isdir(args.outdir):
        try:
            os.mkdir(args.outdir)
        except SystemError:
            sys.stderr.write("ntpviz: ERROR: %s can't be created.\n"
                             % args.outdir)
            raise SystemExit(1)

    # if no ntpsec favicon.ico, write one.
    ico_filename = os.path.join(args.outdir, "favicon.ico")
    if not os.path.lexists(ico_filename):
        with open(ico_filename, "wb") as wp:
            wp.write(binascii.a2b_base64(ntpsec_ico))

    # if no ntpsec logo, write one.
    logo_filename = os.path.join(args.outdir, "ntpsec-logo.png")
    if not os.path.lexists(logo_filename):
        with open(logo_filename, "wb") as wp:
            wp.write(binascii.a2b_base64(ntpsec_logo))

    # report_time = datetime.datetime.utcnow() # the time now is...
    report_time = datetime.datetime.now(UTC())   # the time now is...
    report_time = report_time.strftime("%c %Z")  # format it nicely

    title = args.sitename

    index_header = '''\
<!DOCTYPE html>
<html lang="en">
<head>
<link rel="shortcut icon" href="favicon.ico">
<meta charset="UTF-8">
<meta http-equiv="refresh" content="1800">
<meta name="expires" content="0">
<title>%(title)s</title>
<style>
dt {
    font-weight: bold;
    margin-left: 20px;
}
dd {
    margin-top: 4px;
    margin-bottom: 10px;
}
table {
    text-align: right;
    width: 1300px;
    border-collapse: collapse;
}
thead {
    font-weight: bold;
}
tbody tr {
    vertical-align: top;
}
tbody tr:nth-child(6n+4),
tbody tr:nth-child(6n+5),
tbody tr:nth-child(6n+6) {
    background-color: rgba(0,255,0,0.2);
}
.section {
    color: #000000;
    text-decoration: none;
}
.section .site-title:visited {
    color: #000000;
}
</style>
</head>
<body>
<div style="width:910px">
<a href='https://www.ntpsec.org/'>
<img src="ntpsec-logo.png" alt="NTPsec" style="float:left;margin:20px 70px;">
</a>
<div>
<h1 style="margin-bottom:10px;">%(title)s</h1>
<b>Report generated:</b> %(report_time)s <br>
''' % locals()

    # Ugh.  Not clear what to do in the multiplot case
    if len(statlist) == 1:
        start_time = datetime.datetime.utcfromtimestamp(
            stats.starttime).strftime('%c')
        end_time = datetime.datetime.utcfromtimestamp(
            stats.endtime).strftime('%c')

        index_header += '<b>Start Time:</b> %s UTC<br>\n' \
                        '<b>End Time:</b> %s UTC<br>\n' \
            % (start_time, end_time)
        if 1 > stats.period:
            # less than a day, report hours
            index_header += ('<b>Report Period:</b> %1.1f hours <br>\n' %
                             (float(stats.period) / (24 * 60)))
        else:
            # more than a day, report days
            index_header += ('<b>Report Period:</b> %1.1f days <br>\n' %
                             (float(stats.period) /
                              ntp.statfiles.NTPStats.SecondsInDay))

    if args.clip:
        index_header += """\
<span style="color:red;font-weight:bold;">Warning: plots clipped</span><br>
"""

    index_header += '</div>\n<div style="clear:both;"></div>'

    index_trailer = '''\
<h2>Glossary:</h2>

<dl>
<dt>frequency offset:</dt>
<dd>The difference between the ntpd calculated frequency and the local
system clock frequency (usually in parts per million, ppm)</dd>

<dt>jitter, dispersion:</dt>
<dd>The short term change in a value.  NTP measures Local Time Jitter,
Refclock Jitter, and Server Jitter in seconds.  Local Frequency Jitter is
in ppm or ppb.
</dd>

<dt>kurtosis, Kurt:</dt>
<dd>The kurtosis of a random variable X is the fourth standardized
moment and is a dimension-less ratio. ntpviz uses the Pearson's moment
coefficient of kurtosis.  A normal distribution has a kurtosis of three.
NIST describes a kurtosis over three as "heavy tailed" and one under
three as "light tailed".</dd>

<dt>ms, millisecond:</dt>
<dd>One thousandth of a second = 0.001 seconds, 1e-3 seconds</dd>

<dt>mu, mean:</dt>
<dd>The arithmetic mean: the sum of all the values divided by the
number of values.
The formula for mu is: "mu = (∑x<sub>i</sub>) / N".
Where x<sub>i</sub> denotes the data points and N is the number of data
points.</dd>

<dt>ns, nanosecond:</dt>
<dd>One billionth of a second, also one thousandth of a microsecond,
0.000000001 seconds and 1e-9 seconds.</dd>

<dt>percentile:</dt>
<dd>The value below which a given percentage of values fall.</dd>

<dt>ppb, parts per billion:</dt>
<dd>Ratio between two values. These following are all the same:
    1 ppb, one in one billion, 1/1,000,000,000, 0.000,000,001, 1e-9 and
    0.000,000,1%</dd>

<dt>ppm, parts per million:</dt>
<dd>Ratio between two values. These following are all the same:
    1 ppm, one in one million, 1/1,000,000, 0.000,001, and 0.000,1%</dd>

<dt>‰, parts per thousand:</dt>
<dd>Ratio between two values. These following are all the same:
    1 ‰. one in one thousand, 1/1,000, 0.001, and 0.1%</dd>

<dt>refclock:</dt>
<dd>Reference clock, a local GPS module or other local source of time.</dd>

<dt>remote clock:</dt>
<dd>Any clock reached over the network, LAN or WAN.  Also called a peer
or server.</dd>

<dt>time offset:</dt>
<dd>The difference between the ntpd calculated time and the local system
 clock's time.  Also called phase offset.</dd>

<dt>σ, sigma:</dt>
<dd>Sigma denotes the standard deviation (SD) and is centered on the
arithmetic mean of the data set. The SD is simply the square root of
the variance of the data set.  Two sigma is simply twice the standard
deviation.  Three sigma is three times sigma.  Smaller is better.<br>
The formula for sigma is: "σ = √[ ∑(x<sub>i</sub>-mu)^2 / N ]".
Where x<sub>i</sub> denotes the data points and N is the number of data
points.</dd>

<dt>skewness, Skew:</dt>
<dd>The skewness of a random variable X is the third standardized
moment and is a dimension-less ratio. ntpviz uses the Pearson's moment
coefficient of skewness.  Wikipedia describes it best: "The qualitative
interpretation of the skew is complicated and unintuitive."<br> A normal
distribution has a skewness of zero. </dd>

<dt>upstream clock:</dt>
<dd>Any server or reference clock used as a source of time.</dd>

<dt>µs, us, microsecond:</dt>
<dd>One millionth of a second, also one thousandth of a millisecond,
0.000,001 seconds, and 1e-6 seconds.</dd>
</dl>

<br>
<br>
<br>
<div style="float:left">
This page autogenerated by
<a href="https://docs.ntpsec.org/latest/ntpviz.html">
ntpviz</a>, part of the <a href="https://www.ntpsec.org/">NTPsec project</a>
</div>
<div style="float:left;margin-left:350px;">
    <a href="https://validator.w3.org/nu/">
    <img src="https://www.w3.org/html/logo/downloads/HTML5_Logo_32.png"
        alt="html 5">
    </a>
&nbsp;&nbsp;
    <a href="https://jigsaw.w3.org/css-validator/check/referer">
        <img style="border:0;width:88px;height:31px"
            src="https://jigsaw.w3.org/css-validator/images/vcss"
            alt="Valid CSS!" />
    </a>
</div>
<div style="clear:both;"></div>
</div>
</body>
</html>
'''
    imagewrapper = "<img src='%s.png' alt='%s plot'>\n"

    # buffer the index.html output so the index.html is not empty
    # during the run
    index_buffer = index_header
    # if header file, add it to index.html
    header = os.path.join(args.outdir, "header")
    if os.path.isfile(header):
        try:
            header_file = open(header, 'r', encoding='utf-8')
            header_txt = header_file.read()
            index_buffer += '<br>\n' + header_txt + '\n'
        except IOError:
            pass

    if len(statlist) > 1:
        index_buffer += local_offset_multiplot(statlist)
    else:
        # imagepairs in the order of the html entries
        imagepairs = [
            ("local-offset", stats.local_offset_gnuplot()),
            # skipa next one, redundant to one above
            # ("local-error", stats.local_error_gnuplot()),
            ("local-jitter", stats.local_offset_jitter_gnuplot()),
            ("local-stability", stats.local_offset_stability_gnuplot()),
            ("local-offset-histogram", stats.local_offset_histogram_gnuplot()),
            ("local-temps", stats.local_temps_gnuplot()),
            ("local-freq-temps", stats.local_freq_temps_plot()),
            ("local-gps", stats.local_gps_gnuplot()),
            ("peer-offsets", stats.peer_offsets_gnuplot()),
        ]

        peerlist = list(stats.peersplit().keys())
        # sort for output order stability
        peerlist.sort()
        for key in peerlist:
            imagepairs.append(("peer-offset-" + key,
                               stats.peer_offsets_gnuplot([key])))

        imagepairs.append(("peer-jitters",
                           stats.peer_jitters_gnuplot()))
        for key in peerlist:
            imagepairs.append(("peer-jitter-" + key,
                               stats.peer_jitters_gnuplot([key])))

        stats = []
        for (imagename, image) in imagepairs:
            if not image:
                continue
            if 1 <= args.debug_level:
                sys.stderr.write("ntpviz: plotting %s\n" % image['title'])
            stats.append(image['stats'])
            # give each H2 an unique ID.
            div_id = image['title'].lower().replace(' ', '_').replace(':', '_')

            index_buffer += """\
<div id="%s">\n<h2><a class="section" href="#%s">%s</a></h2>
""" % (div_id, div_id, image['title'])

            div_name = imagename.replace('-', ' ')
            # Windows hates colons in filename
            imagename = imagename.replace(':', '-')
            index_buffer += imagewrapper % (imagename, div_name)

            if image['html']:
                index_buffer += "<div>\n%s</div>\n" % image['html']
            index_buffer += "<br><br>\n"
            gnuplot(image['plot'], os.path.join(args.outdir,
                    imagename + ".png"))
            index_buffer += "</div>\n"

    # dump stats
    csvs = []

    stats_to_output = {}
    for stat in stats:
        if [] == stat:
            continue
        for sta in stat:
            if sta.skip_summary:
                continue
            # This removes duplicates
            stats_to_output[sta.title] = sta

    index_buffer += '<div id="Summary">\n' \
        '<h2><a class="section" href="#Summary">Summary</a></h2>\n'
    index_buffer += VizStats.table_head

    for key in sorted(stats_to_output.keys()):
        index_buffer += str(stats_to_output[key].table)
        csvs.append(stats_to_output[key].csv)

    # RFC 4180 specifies the mime-type of a csv: text/csv
    # your webserver should be programmed the same
    index_buffer += VizStats.table_tail
    index_buffer += """\
<a href="summary.csv" target="_blank"
  type="text/csv;charset=UTF-8;header=present">Summary as CSV file</a><br>
</div>
"""

    # if footer file, add it to index.html
    footer = os.path.join(args.outdir, "footer")
    if os.path.isfile(footer):
        try:
            footer_file = open(footer, 'r', encoding='utf-8')
            footer_txt = footer_file.read()
            index_buffer += '<br>\n' + footer_txt + '\n'
        except IOError:
            pass
    index_buffer += index_trailer

    # and send the file buffer
    index_filename = os.path.join(args.outdir, "index.html")
    with open(index_filename + ".tmp", "w", encoding='utf-8') as ifile:
        ifile.write(index_buffer)

    # create csv file, as a tmp file
    csv_filename = os.path.join(args.outdir, "summary.csv")
    with open(csv_filename + ".tmp", "w", encoding='utf-8') as csv_file:
        csv_ob = csv.writer(csv_file)
        csv_ob.writerow(VizStats.csv_head)
        for row in csvs:
            csv_ob.writerow(row)

    # move new index and summary into place
    # windows python 2.7, 3.6 has no working rename, so delete and move
    try:
        os.remove(csv_filename)
        os.remove(index_filename)
    except OSError:
        pass

    os.rename(csv_filename + ".tmp", csv_filename)
    os.rename(index_filename + ".tmp", index_filename)


# end
