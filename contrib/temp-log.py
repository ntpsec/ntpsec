#!/usr/bin/env python
# temp-log.py: A script that will be run eventually as a daemon to log temperature of a system
# Usage:
# temp-log.py [-h] [-V] [-s] [-m]
# Writes logs to /var/log/ntpstats
# Requires root to run

import temp_classes
import time, os, logging, sys, logging.handlers

# Global vars
version = 1.0
# Create objects
cpu  = temp_classes.CpuTemp()
zone = temp_classes.ZoneTemp()
hdd  = temp_classes.SmartCtl('/dev/sda')

def logging_setup(fileName, levelName, logLevel):
    "Create logging object with midnight rotation"
    logFormat = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    # Create logger for cpuTemp
    tempLogger = logging.getLogger(levelName)
    tempLogger.setLevel(logLevel)
    # Create file handler
    _file = logging.handlers.TimedRotatingFileHandler(fileName, when='midnight', interval=1, backupCount=5, encoding=None, delay=False, utc=False)
    _file.setLevel(logLevel)
    # Create the formatter and add it to the handler
    _file.setFormatter(logFormat)
    # Add the handler to the logger
    tempLogger.addHandler(_file)
    # Logging rotate
    return tempLogger

def log_data(logger, data):
    "Write data to the log"
    if type(data) in (tuple, list):
        for _item in data:
            logger.info(_item)
    else:
        logger.info(data)

def log_with_multiple_files():
    "Write the logs to individual files depending on sensor"
    # Create logger instances
    zoneLogger = logging_setup('/var/log/ntpstats/zoneTemp.log', 'zone_levelname', logging.INFO)
    cpuLogger  = logging_setup('/var/log/ntpstats/cpuTemp.log', 'cpu_levelname', logging.INFO)
    hddLogger  = logging_setup('/var/log/ntpstats/hddTemp.log', 'hdd_levelname', logging.INFO)

    # Write data to their respective logs forever
    while True:
        log_data(zoneLogger, zone.get_data())
        log_data(cpuLogger, cpu.get_data())
        log_data(hddLogger, hdd.get_data())
        # Sleep 15 seconds
        time.sleep(15)

def log_with_one_file():
    "Write all temperature readings to one file"
    # Create the logger instance
    zoneLogger = logging_setup('/var/log/ntpstats/temp.log', 'zone_levelname', logging.INFO)
    cpuLogger  = logging_setup('/var/log/ntpstats/temp.log', 'cpu_levelname', logging.INFO)
    hddLogger  = logging_setup('/var/log/ntpstats/temp.log', 'hdd_levelname', logging.INFO)

    # Write data to their respective logs forever
    while True:
        log_data(zoneLogger, zone.get_data())
        log_data(cpuLogger, cpu.get_data())
        log_data(hddLogger, hdd.get_data())
        # Sleep 15 seconds
        time.sleep(15)

def help_text():
    "Help message"
    print("temp-log.py [-f filepath] [-h] [-s [-r {hour|day|week|month|year}] [-m] [-V]")
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
