#!/usr/bin/env python
# temp-log.py: A script that will be run eventually as a daemon
#              to log temperature of a system
# Usage:
# temp-log.py [-h] [-V] [-l]
# Writes logs to /var/log/ntpstats
# Requires root to run

<<<<<<< Updated upstream
import logging
import logging.handlers
import os
import re
import subprocess
import sys
import time

=======
import sys, re, time, subprocess, os, logging, logging.handlers, argparse

# Global vars
version = 1.0
>>>>>>> Stashed changes

class CpuTemp:
    "Sensors on the CPU Core"
    def __init__(self):
        # pattern that matches the string that has the cpu temp
        self._pattern = re.compile('^\s+temp\d+_input:\s+([\d\.]+).*$')
        # Find the sensors binary and stores the path
        self._sensors_path = subprocess.check_output(
            ["which", "sensors"], universal_newlines=False).replace('\n', '')
        if self._sensors_path is None:
            raise Exception("Unable to find sensors binary")

    def get_data(self):
        "Collects the data and return the output as an array"
        _index = 0
        _data = []
        # grab the needed output
        _output = subprocess.check_output([self._sensors_path, "-u"], universal_newlines=True).split('\n')
        for record in _output:
            match = self._pattern.match(record)
            if match and match.group(1):
                _now = int(time.time())
                _cpu_temprature = match.group(1)
                _data.append('{},{},{}'.format(_now, _index, _cpu_temprature))
                _index += 1

        return _data

<<<<<<< Updated upstream
    def _record_temp(self):
        "Call the external command 'sensors -u' and get the data"
        # grab the data from the "sensors -u" command
        output = subprocess.check_output([self._sensors_path, "-u"],
                                         universal_newlines=True)
        self.sensors_output = output.split('\n')


=======
>>>>>>> Stashed changes
class SmartCtl:
    "Sensor on the Hard Drive"
    def __init__(self):
        if os.getuid() != 0:
            raise IOError("You must be root!")
        # Which drive to watch
        self._drives = []
        for child in os.listdir('/dev/'):
            if re.compile('sd[a-z]$').match(child):
                self._drives.append("/dev/"+str(child))
        # this regex matches temperature output lines from smartctl -a
        self._pat = re.compile('194 Temperature_Celsius\s+\S+\s+(\d+)\s+')

    def get_data(self):
        "Collects the data and return the output as an array"
<<<<<<< Updated upstream
        _output = subprocess.check_output(["smartctl", "-a", self._device],
                                          universal_newlines=True)
        _lines = _output.split('\n')
        for line in _lines:
            match = self._pat.match(line)
            now = int(time.time())
            if match and match.group(1):
                temp = match.group(1)
                return ('%d SMART %s' % (now, temp))
=======
        for _device in self._drives:
            _output = subprocess.check_output(["smartctl", "-a", _device], universal_newlines=True).split('\n')
            for line in _output:
                match = self._pat.match(line)
                now = int(time.time())
                if match and match.group(1):
                    temp = match.group(1)
                    return ('%d,%s,%s' % (now, _device, temp))
>>>>>>> Stashed changes


class ZoneTemp:
    "Sensors on the CPU Zones"
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
                _data.append('{},{},{}'.format(_now, _zone, temp))
                _zone = _zone+1
            _zone_data.close()
        return _data

<<<<<<< Updated upstream
# Global vars
version = 1.0
# Create objects
cpu = CpuTemp()
zone = ZoneTemp()
hdd = SmartCtl('/dev/sda')


def logging_setup(fileName, levelName, logLevel):
    "Create logging object with midnight rotation"
    logFormat = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s')
=======
def logging_setup(fileName, levelName, logLevel):
    "Create logging object"
    logFormat = logging.Formatter('%(asctime)s,%(name)s,%(levelname)s,%(message)s')
>>>>>>> Stashed changes
    # Create logger for cpuTemp
    tempLogger = logging.getLogger(levelName)
    tempLogger.setLevel(logLevel)
    # Create file handler
    _file = logging.handlers.TimedRotatingFileHandler(fileName,
                                                      when='midnight',
                                                      interval=1,
                                                      backupCount=5,
                                                      encoding=None,
                                                      delay=False,
                                                      utc=False)
    _file.setLevel(logLevel)
    # Create the formatter and add it to the handler
    _file.setFormatter(logFormat)
    # Add the handler to the logger
    tempLogger.addHandler(_file)
    return tempLogger

<<<<<<< Updated upstream

def log_data(logger, data):
    "Write data to the log"
=======
def console_log_setup(levelName, logLevel):
    "Create logging object that writes to STDOUT"
    logFormat = logging.Formatter('%(asctime)s,%(name)s,%(levelname)s,%(message)s')
    # Create the logging object
    tempLog = logging.getLogger(levelName)
    tempLog.setLevel(logLevel)
    # Create the output handler
    ch = logging.StreamHandler()
    ch.setLevel(logLevel)
    # Apply the format to the handler
    ch.setFormatter(logFormat)
    # Apply the handler object to the logging object
    tempLog.addHandler(ch)
    return tempLog

def logData(log, data):
    "log the data"
>>>>>>> Stashed changes
    if type(data) in (tuple, list):
        for _item in data:
            log.info(_item)
    else:
        log.info(data)

<<<<<<< Updated upstream

def log_with_multiple_files():
    "Write the logs to individual files depending on sensor"
    # Create logger instances
    zoneLogger = logging_setup('/var/log/ntpstats/zoneTemp.log',
                               'zone_levelname', logging.INFO)
    cpuLogger = logging_setup('/var/log/ntpstats/cpuTemp.log',
                              'cpu_levelname', logging.INFO)
    hddLogger = logging_setup('/var/log/ntpstats/hddTemp.log',
                              'hdd_levelname', logging.INFO)
=======
def log_data():
    "Write all temperature readings to one file"
    try:
        # Create objects
        cpu  = CpuTemp()
        zone = ZoneTemp()
        hdd  = SmartCtl()
    except IOError as ioe:
        sys.stderr.write("Unable to run: " + str(ioe) + "\n")
        sys.exit(1)
    # Create the logger instance
    zoneLogger = logging_setup(log, 'zone', logging.INFO)
    cpuLogger  = logging_setup(log, 'cpu', logging.INFO)
    hddLogger  = logging_setup(log, 'hdd', logging.INFO)
>>>>>>> Stashed changes

    # Write data to their respective logs forever
    while True:
        logData(zoneLogger, zone.get_data())
        logData(cpuLogger, cpu.get_data())
        logData(hddLogger, hdd.get_data())
        # Sleep 15 seconds
        time.sleep(15)

<<<<<<< Updated upstream

def log_with_one_file():
    "Write all temperature readings to one file"
    # Create the logger instance
    zoneLogger = logging_setup('/var/log/ntpstats/temp.log',
                               'zone_levelname', logging.INFO)
    cpuLogger = logging_setup('/var/log/ntpstats/temp.log',
                              'cpu_levelname', logging.INFO)
    hddLogger = logging_setup('/var/log/ntpstats/temp.log',
                              'hdd_levelname', logging.INFO)

    # Write data to their respective logs forever
=======
def display():
    try:
        # Create objects
        cpu  = CpuTemp()
        zone = ZoneTemp()
        hdd  = SmartCtl()
    except IOError as ioe:
        sys.stderr.write("Unable to run: " + str(ioe) + "\n")
        sys.exit(1)

    zoneLogger = console_log_setup('zone', logging.INFO)
    cpuLogger  = console_log_setup('cpu', logging.INFO)
    hddLogger  = console_log_setup('hdd', logging.INFO)
>>>>>>> Stashed changes
    while True:
        logData(zoneLogger, zone.get_data())
        logData(cpuLogger, cpu.get_data())
        logData(hddLogger, hdd.get_data())
        # Sleep 15 seconds
        time.sleep(15)

<<<<<<< Updated upstream

def help_text():
    "Help message"
    print("temp-log.py [-f filepath] [-h] "
          "[-s [-r {hour|day|week|month|year}] [-m] [-V]")
    print("-h\tPrints this help message")
    print("-s\tWrites all data to a single log file")
    print("-m\tWrites data for each sensor type to their own log file")
    print("-V\tPrints the version")

try:
    if sys.argv[1] == "-h" or sys.argv[1] == "--help":
        help_text()

    if sys.argv[1] == "-V" or sys.argv[1] == "--version":
        print("Version: " + str(version))

    if sys.argv[1] == "-s":
        log_with_one_file()

    if sys.argv[1] == "-m":
        log_with_multiple_files()

except IndexError:
    help_text()
except Exception as e:
    sys.stderr.write("Unable to run: " + str(e))
    sys.exit(1)
except IOError as ioe:
    sys.stderr.write("Unable to run: " + str(ioe))
    sys.exit(1)
except KeyboardInterrupt:
    sys.exit(0)
except:
    help_text()
    sys.exit(1)
=======
# Work with argvars
parser = argparse.ArgumentParser(description="Temperature sensor daemon",
                                 epilog="""See the manual page for details.""")
parser.add_argument('-l', '--logfile',
                    dest='logfile',
                    help="append log data to LOGFILE instead of stdout",
                    nargs=1)
parser.add_argument('-V', '--version',
                    dest='version',
                    help="Display the version and exit",
                    action='store_true')

args = parser.parse_args()
if args.version:
    print("temp-log v. " + str(version))

elif args.logfile:
    try:
        log = args.logfile[0]
        log_data()
    except (KeyboardInterrupt, SystemExit):
        sys.exit(0)
else:
    try:
        display()
    except (KeyboardInterrupt, SystemExit):
        sys.exit(0)
>>>>>>> Stashed changes
