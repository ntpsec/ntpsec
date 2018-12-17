#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""\
usage: ntploggps [-h] [-o] [-l LOGFILE] [-v] [-V]

gpsd log file generator

optional arguments:
  -h, --help            show this help message and exit
  -l LOGFILE, --logfile LOGFILE
                        append log data to LOGFILE instead of stdout
  -o, --once            log one line, then exit
  -w WAIT, --wait WAIT  wait WAIT seconds after each log line, default 5
  -v, --verbose         be verbose
  -V, --version         show program's version number and exit

See the manual page for details.
"""

from __future__ import print_function

import io
import logging
import logging.handlers
import sys
import threading
import time

try:
    import argparse
except ImportError:
    sys.stderr.write("""
ntploggps: can't find the Python argparse module
         If your Python version is < 2.7, then manual installation is needed:
         # pip install argparse
""")
    sys.exit(1)

try:
    import gps
except ImportError as e:
    sys.stderr.write("ntploggps: can't find Python GPSD library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)

try:
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntploggps: can't find Python NTP library -- check PYTHONPATH.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


def logging_setup():
    "Create logging object"
    logFormat = logging.Formatter('%(message)s')
    # Create logger for gpsd
    Logger = logging.getLogger()
    Logger.setLevel(logging.INFO)
    # Create file handler
    if args.logfile:
        # log to logfile
        file = logging.handlers.TimedRotatingFileHandler(
            args.logfile[0],
            when='midnight',
            interval=1)
    else:
        # log to stdout
        file = logging.StreamHandler(sys.stdout)

    file.setLevel(logging.INFO)
    # Create the formatter and add it to the handler
    file.setFormatter(logFormat)
    # Add the handler to the logger
    Logger.addHandler(file)
    return Logger


parser = argparse.ArgumentParser(description="gpsd log file generator",
                                 epilog="""
See the manual page for details.
""")

parser.add_argument('-l', '--logfile',
                    dest='logfile',
                    help="append log data to LOGFILE instead of stdout",
                    nargs=1)

parser.add_argument('-o', '--once',
                    action="store_true",
                    dest='once',
                    help="log one line, then exit")

parser.add_argument('-w', '--wait',
                    default=[5],
                    dest='wait',
                    help="wait WAIT seconds after each log line, default 5",
                    nargs=1,
                    type=int)

parser.add_argument('-v', '--verbose',
                    action="store_true",
                    dest='verbose',
                    help="be verbose")

parser.add_argument('-V', '--version',
                    action="version",
                    version="ntploggps %s" % ntp.util.stdversion())

args = parser.parse_args()

if args.verbose:
    print("ntploggps: arguments:")
    print(args)

if args.logfile:
    # log to logfile
    try:
        out = open(args.logfile[0], mode='a')
    except io.UnsupportedOperation as e:
        sys.stderr.write("ntploggps: can't open logfile %s\n" % args.logfile)
        sys.stderr.write("%s\n" % e)
        sys.exit(1)

    if args.verbose:
        print("ntploggps: opened log file %s" % args.logfile[0])

else:
    # log to stdout
    out = sys.stdout


class GpsPoller(threading.Thread):
    running = False       # True when thread is running. Quit when set False

    def __init__(self):
        threading.Thread.__init__(self)
        self.device = None
        self.satellites_used = None
        self.tdop = None
        # start the streaming of gps data
        try:
            self.gpsd = gps.gps(mode=gps.WATCH_ENABLE)
        except BaseException as e:
            sys.stderr.write("ntploggps: Can't connect to gpsd, %s\n"
                             "         Is gpsd running?\n" % e)
            sys.exit(1)
        self.running = True

    def run(self):
        while gpsp.running:
            if self.gpsd.read() == -1:
                self.running = False
                break
            if hasattr(self.gpsd, "data"):
                if self.gpsd.data.get("class") == "SKY":
                    self.satellites_used = 0
                    self.tdop = self.gpsd.data.get("tdop", 0)
                    for sat in self.gpsd.data.get("satellites", []):
                        if sat["used"]:
                            self.satellites_used += 1
                elif self.gpsd.data.get("class") == "TPV":
                    self.device = self.gpsd.data.get("device")

    @property
    def time(self):
        "Return the gpsd time fix"
        t = self.gpsd.fix.time
        if isinstance(t, int):
            return t
        if isinstance(t, float):
            if not gps.isfinite(t):
                return None
            return t
        return gps.isotime(t)


if __name__ == '__main__':
    # this is the main thread
    if args.verbose:
        print("ntploggps: creating poll thread")

    gpsp = GpsPoller()    # create the thread
    try:
        # Create the logger instance
        Logger = logging_setup()

        # Create data layout
        Logger.info("# Time       Device     TDOP     nSat")

        gpsp.start()      # start it up
        last_time = 0
        while gpsp.running:
            # It may take a second or two to get good data

            try:
                current_time = gpsp.time
                device = gpsp.device
                tdop = gpsp.tdop
                satellites_used = gpsp.satellites_used

                if current_time is not None and \
                   device is not None and \
                   satellites_used is not None and \
                   tdop is not None:
                    if last_time != current_time:
                        s = '%i %s %f %d' % (current_time, device, tdop,
                                             satellites_used)
                        Logger.info(s)
                        last_time = current_time
                    if args.once:
                        # just once
                        break

            except AttributeError as e:
                print('parse error\n')

            # wait a bit before next log
            time.sleep(args.wait[0])

    except (KeyboardInterrupt, SystemExit):    # when you press ctrl+c
        args.once = True        # stop the retry loop
        if args.verbose:
            print("\nKilling Thread...")
        else:
            # print a blank line to make bash happy
            print("")
    except Exception as e:       # any error, signal
        print(e)

    # tell the thread to die
    gpsp.running = False

    # wait for the thread to finish what it's doing
    gpsp.join()

    if args.verbose:
        print("ntploggps: Done -- Exiting.")
