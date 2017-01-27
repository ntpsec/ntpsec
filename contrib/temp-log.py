#!/usr/bin/env python
# temp-log.py: A script that will be run eventually as a daemon
#              to log temperatures of a system
# Usage:
# temp-log.py [-h] [-q] [-v] [-V] [-l LOGFILE]
# Requires root to run

import argparse
import glob
import logging
import logging.handlers
import os
import re
import subprocess
import sys
import time

try:
    import ntp.util
except ImportError as e:
    sys.stderr.write("temp-log: can't find Python NTP modules "
                     "-- check PYTHONPATH.\n%s\n" % e)
    sys.exit(1)


class CpuTemp:
    "Sensors on the CPU Core"
    no_sensors = False

    def __init__(self):
        # check for sensors binary
        try:
            # sadly subprocess.check_output() is not in Python 2.6
            subprocess.check_output(["which", "sensors"],
                                    stderr=subprocess.STDOUT,)
        except:
            # skip sensors, many systems can not use sensors
            self.no_sensors = True
            if args.verbose:
                sys.stderr.write("Unable to find sensors binary")

        # pattern that matches the string that has the cpu temp
        self._pattern = re.compile('^\s+temp\d+_input:\s+([\d\.]+).*$')

    def get_data(self):
        "Collects the data and return the output as an array"
        if self.no_sensors:
            return None

        _index = 0
        _data = []
        # grab the needed output
        # sadly subprocess.check_output() is not in Python 2.6
        _output = subprocess.check_output(['sensors', "-u"],
                                          universal_newlines=True).split('\n')
        for record in _output:
            match = self._pattern.match(record)
            if match and match.group(1):
                _now = int(time.time())
                _cpu_temprature = match.group(1)
                _data.append('%d LM%s %s' % (_now, _index, _cpu_temprature))
                _index += 1

        return _data


class SmartCtl:
    "Sensor on the Hard Drive"
    _drives = []

    def __init__(self):
        # Which drive to watch
        for child in glob.glob('/dev/sd?'):
            self._drives.append(child)
        self._drives = sorted(self._drives)

    def get_data(self):
        "Collects the data and return the output as an array"
        data = []
        for _device in self._drives[:]:
            try:
                # sadly subprocess.check_output() is not in Python 2.6
                _output = subprocess.check_output(["smartctl", "-A",
                                                  _device],
                                                  universal_newlines=True
                                                  ).split('\n')
                for line in _output:
                    if line.startswith('194 '):
                        now = int(time.time())
                        temp = line.split()[9]
                        data.append('%d %s %s' % (now, _device, temp))
            except:
                # do not keep trying on failure
                self._drives.remove(_device)
        return data


class Temper:
    """\
    Reads 'temper-poll -c' for room temperature data.

    Before you can use this class you must have a TEMPer USB thermometer
    plugged in, and the temper-python package must be installed and configured.
    See their documentation for that procedure.
"""
    no_temper = False

    def __init__(self):
        # check for temper-poll binary
        try:
            # sadly subprocess.check_output() is not in Python 2.6
            subprocess.check_output(["which", "temper-poll"],
                                    stderr=subprocess.STDOUT,)
        except:
            # skip temper, many systems do not have it
            self.no_temper = True
            if args.verbose:
                sys.stderr.write("Unable to find temper-poll binary")

    def get_data(self):
        "Collects the data and return the output as an array"
        if self.no_temper:
            return None

        data = []
        _device = 'TEMPER0'
        # grab the needed output
        # sadly subprocess.check_output() is not in Python 2.6
        try:
            proc = subprocess.Popen(["temper-poll", "-c"],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT,
                                    universal_newlines=True)
            temp = proc.communicate()[0]
            # make sure it is a temperature
            temp = float(temp)
            now = int(time.time())
            data.append('%d %s %s' % (now, _device, temp))
        except:
            # bad data, ignore it
            self.no_temper = True
        return data


class ZoneTemp:
    "Zone sensors"
    def __init__(self):
        self._base_dir = '/sys/class/thermal/'
        self.zone_directories = []
        for child in os.listdir(self._base_dir):
            if re.compile('thermal_zone').match(child):
                self.zone_directories.append(child)

    def get_data(self):
        "Collects the data and return the output as an array"
        _zone = 0
        _data = []
        for zone in self.zone_directories:
            _zone_data = open(os.path.join(os.path.join(self._base_dir, zone),
                                           'temp'))
            for line in _zone_data:
                temp = float(line) / 1000
                _now = int(time.time())
                _data.append('%d ZONE%s %s' % (_now, _zone, temp))
                _zone = _zone+1
            _zone_data.close()
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
parser.add_argument('-V', '--version',
                    action="version",
                    version="temp-log %s" % ntp.util.stdversion())
parser.add_argument('-w', '--wait',
                    default=[60],
                    dest='wait',
                    help="Set delay time in seconds, default is 60",
                    nargs=1,
                    type=int)
args = parser.parse_args()


def logging_setup(fileName, logLevel):
    "Create logging object"
    logFormat = logging.Formatter('%(message)s')
    # Create logger for cpuTemp
    tempLogger = logging.getLogger()
    tempLogger.setLevel(logLevel)
    # Create file handler
    if args.logfile:
        _file = logging.handlers.TimedRotatingFileHandler(
            fileName,
            when='midnight',
            interval=1)
    else:
        _file = logging.StreamHandler(sys.stdout)

    _file.setLevel(logLevel)
    # Create the formatter and add it to the handler
    _file.setFormatter(logFormat)
    # Add the handler to the logger
    tempLogger.addHandler(_file)
    return tempLogger


def logData(log, data):
    "log the data"
    if data is None:
        return

    if type(data) in (tuple, list):
        for _item in data:
            log.info(_item)
    else:
        if data is not "":
            log.info(data)


def log_data():
    "Write all temperature readings to one file"
    # Create objects
    cpu = CpuTemp()
    zone = ZoneTemp()
    hdd = SmartCtl()
    temper = Temper()

    # Create the logger instance
    Logger = logging_setup(log, logging.INFO)

    # Create data layout
    logData(Logger, "# seconds since epoch, sensor, sensor value")

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
    sys.stderr.write("You must be root!")
    sys.exit(1)

try:
    if args.logfile:
        log = args.logfile[0]
    else:
        log = sys.stdout
    log_data()
except (KeyboardInterrupt, SystemExit):
    sys.exit(0)
