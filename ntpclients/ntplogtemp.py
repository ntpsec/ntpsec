#! @PYSHEBANG@
# -*- coding: utf-8 -*-

# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""\
usage: ntplogtemp [-h] [-l LOGFILE] [-o] [-q] [-v] [-w WAIT] [-V]

Program to log system temperatures

optional arguments:
  -h, --help            show this help message and exit
  -l LOGFILE, --logfile LOGFILE
                        append log data to LOGFILE instead of stdout
  -o, --once            Run the output once and exit
  -q, --quiet           be quite
  -v, --verbose         be verbose
  -w WAIT, --wait WAIT  Set delay time in seconds, default is 60
  -V, --version         show program's version number and exit

See the manual page for details.
"""

from __future__ import print_function, division

import argparse
import glob
import logging
import logging.handlers
import os
import re
import subprocess
import sys
import time


class logfile_header_class(logging.handlers.TimedRotatingFileHandler):
    'A class to modify the file logging handler.'
    def doRollover(self):
        'function to add header to new file on rotation.'
        if str is bytes:
            super(logfile_header_class, self).doRollover()
        else:
            super().doRollover()
        self.stream.write('# time, sensor, value\n')


def run_binary(cmd):
    """\
Run a binary
Return output if good, None if bad
"""

    try:
        # sadly subprocess.check_output() is not in Python 2.6
        # so use Popen()
        # this throws an exception if not found
        proc = subprocess.Popen(cmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT,
                                universal_newlines=True)
        output = proc.communicate()[0].split("\n")

        if proc.returncode:
            # non-zero return code, fail
            return None

    except:
        if args.verbose:
            sys.stderr.write("Unable to run %s binary\n" % cmd[0])
        # throws exception
        return None
    return output


class CpuTemp:
    "Sensors on the CPU Core"
    has_sensors = False

    def __init__(self):
        # check for sensors binary
        ret = run_binary(["sensors", "-h"])
        if ret is not None:
            self.has_sensors = True

        # pattern that matches the string that has the cpu temp
        self._pattern = re.compile(r'^\s+temp\d+_input:\s+([\d\.]+).*$')

    def get_data(self):
        "Collects the data and return the output as an array"
        if not self.has_sensors:
            return None

        _index = 0
        _data = []
        # grab the needed output
        output = run_binary(["sensors", "-u"])

        if output is not None:
            for record in output:
                match = self._pattern.match(record)
                if match and match.group(1):
                    _now = int(time.time())
                    _cpu_temperature = match.group(1)
                    _data.append('%d LM%s %s' % (_now, _index,
                                                 _cpu_temperature))
                    _index += 1
        else:
            self.has_sensors = False
            if args.verbose:
                sys.stderr.write("No sensors returned temperatures. ",
                                 "Have you run sensors-detect?\n")

        return _data


class SmartCtl:
    "Sensor on the Hard Drive"
    _drives = []
    has_smartctl = False

    def __init__(self):
        ret = run_binary(["smartctl", "-h"])
        if ret is not None:
            self.has_smartctl = True

        if self.has_smartctl:
            # Which drive to watch
            for child in glob.glob('/dev/sd?'):
                self._drives.append(child)
            for child in glob.glob('/dev/nvme?n?'):
                self._drives.append(child)
            self._drives = sorted(self._drives)

    def get_data(self):
        "Collects the data and return the output as an array"
        if not self.has_smartctl:
            return None

        data = []
        for _device in self._drives[:]:
            output = run_binary(["smartctl", "-A", _device])

            if output is None:
                # do not keep trying on failure
                self._drives.remove(_device)
            else:
                for line in output:
                    if line.startswith('194 '):
                        now = int(time.time())
                        temp = line.split()[9]
                        data.append('%d %s %s' % (now, _device, temp))
                        break
                    if line.startswith('Temperature:'):
                        now = int(time.time())
                        temp = line.split()[1]
                        data.append('%d %s %s' % (now, _device, temp))
                        break
        return data


class Temper:
    """\
    Reads 'temper-poll -c' for room temperature data.

    Before you can use this class you must have a TEMPer USB thermometer
    plugged in, and the temper-python package must be installed and configured.
    See their documentation for that procedure.
"""
    has_temper = False

    def __init__(self):
        # check for sensors binary
        ret = run_binary(["temper-poll", "-h"])
        if ret is not None:
            self.has_temper = True

    def get_data(self):
        "Collects the data and return the output as an array"
        if not self.has_temper:
            return None

        data = []
        _device = 'TEMPER0'
        # only one device can read the TEMPer at a time
        # collisions will happen, so retry a few times
        for attempt in range(0, 3):
            # grab the needed output
            output = run_binary(["temper-poll", "-c"])
            try:
                # make sure it is a temperature
                temp = float(output[0])
                now = int(time.time())
                data.append('%d %s %s' % (now, _device, temp))
                break
            except (TypeError, ValueError):
                # bad data, ignore it, for a bit
                if args.verbose:
                    sys.stderr.write("TEMPer-poll failed\n")

        if not data:
            self.has_temper = False
        return data


class ZoneTemp:
    "Zone sensors"

    def __init__(self):
        base_dir = '/sys/class/thermal/thermal_zone?/'
        self.zones = []
        for child in glob.glob(base_dir):
            self.zones.append(child)

    def get_data(self):
        "Collects the data and return the output as an array"
        _zone = 0
        _data = []
        for zone in self.zones:
            try:
                with open(os.path.join(zone, 'temp')) as _zone_data:
                    for line in _zone_data:
                        temp = float(line) / 1000
                        _now = int(time.time())
                        _data.append('%d ZONE%s %s' % (_now, _zone, temp))
                        _zone += 1
            except OSError:
                # temp might not be readable, if so continue on to the next one
                continue
        return _data


# Work with argvars
parser = argparse.ArgumentParser(description="Temperature sensor daemon",
                                 epilog="""See the manual page for details.""")
parser.add_argument('-l', '--logfile',
                    dest='logfile',
                    help="append log data to LOGFILE instead of stdout",
                    nargs=1)
parser.add_argument('-o', '--once',
                    dest='once',
                    help="Run the output once and exit",
                    action='store_true')
parser.add_argument('-q', '--quiet',
                    action="store_true",
                    dest='quiet',
                    help="be quite")
parser.add_argument('-v', '--verbose',
                    action="store_true",
                    dest='verbose',
                    help="be verbose")
parser.add_argument('-w', '--wait',
                    default=[60],
                    dest='wait',
                    help="Set delay time in seconds, default is 60",
                    nargs=1,
                    type=int)
parser.add_argument('-V', '--version',
                    action="version",
                    version="ntplogtemp ntpsec-@NTPSEC_VERSION_EXTENDED@")
args = parser.parse_args()


def logging_setup():
    "Create logging object"
    logFormat = logging.Formatter('%(message)s')
    # Create logger for cpuTemp
    tempLogger = logging.getLogger()
    tempLogger.setLevel(logging.INFO)
    # Create file handler
    if args.logfile:
        # log to logfile
        file = logfile_header_class(
            args.logfile[0],
            utc=True,
            when='midnight',
            interval=1)
    else:
        # log to stdout
        file = logging.StreamHandler(sys.stdout)

    file.setLevel(logging.INFO)
    # Create the formatter and add it to the handler
    file.setFormatter(logFormat)
    # Add the handler to the logger
    tempLogger.addHandler(file)
    return tempLogger


def logData(log, data):
    "log the data"
    if data is not None:
        for _item in data:
            log.info(_item)


def log_data():
    "Write all temperature readings to one file"
    # Create objects
    cpu = CpuTemp()
    zone = ZoneTemp()
    hdd = SmartCtl()
    temper = Temper()

    # Create the logger instance
    Logger = logging_setup()

    # Create data layout
    logData(Logger, ["# time, sensor, value"])

    # Write data to their respective logs
    while True:
        logData(Logger, zone.get_data())
        logData(Logger, cpu.get_data())
        logData(Logger, hdd.get_data())
        logData(Logger, temper.get_data())
        if args.once:
            sys.exit(0)
        time.sleep(args.wait[0])


args = parser.parse_args()
if os.getuid():
    sys.stderr.write("You must be root!\n")
    sys.exit(1)

try:
    log_data()
except KeyboardInterrupt:
    print("")    # be nice to bash
