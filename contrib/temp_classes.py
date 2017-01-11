import sys, re, time, subprocess, os

class CpuTemp:
    "Sensors on the CPU Core"
    def __init__(self):
        # pattern that matches the string that has the cpu temp
        self._pattern = re.compile('^\s+temp\d+_input:\s+([\d\.]+).*$')
        # Find the sensors binary and stores the path
        self._sensors_path = subprocess.check_output(["which", "sensors"], universal_newlines=False).replace('\n', '')
        if self._sensors_path == None:
            raise Exception("Unable to find sensors binary")

    def get_data(self):
        "Collects the data and return the output as an array"
        _index = 0
        _data = []
        self._record_temp()
        # grab the needed output and format it to log
        for record in self.sensors_output:
            match = self._pattern.match(record)
            if match and match.group(1):
                _now = int(time.time())
                _cpu_temprature = match.group(1)
                _data.append('{} {} {}'.format(_now, _index, _cpu_temprature))
                _index += 1

        return _data

    def _record_temp(self):
        "Call the external command 'sensors -u' and get the data"
        # grab the data from the "sensors -u" command
        output = subprocess.check_output([self._sensors_path, "-u"], universal_newlines=True)
        self.sensors_output = output.split('\n')

class SmartCtl:
    "Sensor on the Hard Drive"
    def __init__(self, device):
        if os.getuid() != 0:
            raise IOError("You must be root!")
        # Which drive to watch
        self._device = device
        # this regex matches temperature output lines from smartctl -a
        self._pat = re.compile('194 Temperature_Celsius\s+\S+\s+(\d+)\s+')

    def get_data(self):
        "Collects the data and return the output as an array"
        _output = subprocess.check_output(["smartctl", "-a", self._device], universal_newlines=True)
        _lines = _output.split('\n')
        for line in _lines:
            match = self._pat.match(line)
            now = int(time.time())
            if match and match.group(1):
                temp = match.group(1)
                return ('%d SMART %s' % (now, temp))

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
            _zone_data = open(os.path.join(os.path.join(self._base_dir, zone), 'temp'))
            for line in _zone_data:
                temp = float(line) / 1000
                _now = int(time.time())
                _data.append('{} {} {}'.format(_now, _zone, temp))
                _zone = _zone+1
            _zone_data.close()
        return _data
