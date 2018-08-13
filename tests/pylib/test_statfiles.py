#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.statfiles
import jigs
import sys


class TestPylibStatfiles(unittest.TestCase):

    def test_iso_to_posix(self):
        f = ntp.statfiles.iso_to_posix

        self.assertEqual(f(1480999786), 1480999786)
        self.assertEqual(f("2016-12-06T04:49:46"), 1480999786)

    def test_posix_to_iso(self):
        f = ntp.statfiles.posix_to_iso

        self.assertEqual(f(1480999786), "2016-12-06T04:49:46")

    def test_iso_to_posix_inverts_posix_to_iso(self):
        self.assertEqual(ntp.statfiles.iso_to_posix(
            ntp.statfiles.posix_to_iso(1480999786)), 1480999786)

    def test_posix_to_iso_inverts_iso_to_posix(self):
        self.assertEqual(ntp.statfiles.posix_to_iso(
            ntp.statfiles.iso_to_posix("2016-12-06T04:49:46")),
            "2016-12-06T04:49:46")


class TestNTPStats(unittest.TestCase):
    target = ntp.statfiles.NTPStats

    open_calls = []
    open_returns = []
    load_stem_calls = []
    load_stem_returns = []
    process_stem_calls = []
    process_stem_returns = []

    def open_jig(self, filename, filemode):
        self.open_calls.append((filename, filemode))
        ret = self.open_returns.pop(0)
        if ret is None:
            raise IOError
        else:
            return ret

    @staticmethod
    def load_stem_jig(cls, statsdir, stem):
        TestNTPStats.load_stem_calls.append((statsdir, stem))
        ret = TestNTPStats.load_stem_returns.pop(0)
        if ret is None:
            raise IOError
        return ret

    @staticmethod
    def process_stem_jig(cls, stem, lines):
        TestNTPStats.process_stem_calls.append((stem, lines))
        return TestNTPStats.process_stem_returns.pop(0)

    def test_unixize(self):
        f = self.target.unixize

        # Test empty
        self.assertEqual(f([], 0, 0xFFFFFFFF), [])
        # Test everything in range
        self.assertEqual(f(["5 3600", "6 86399", "40587 .125"],
                           -3600000000, 10000),
                         [[-3506281200000, "-3506281200.0"],
                          [-3506112001000, "-3506112001.0"],
                          [125, "0.125"]])
        # Test with unparseables, all in range
        self.assertEqual(f(["heeeey duuuude!", "40588 0"], 0, 86401),
                         [[86400000, "86400.0"]])
        # Test something not in range
        self.assertEqual(f(["40587 0", "40587 1.0625",
                            "40587 86399", "40588 1"], 1, 86400),
                         [[1062, "1.0625"], [86399000, "86399.0"]])

    def test_timestamp(self):
        f = self.target.timestamp

        # Test
        self.assertEqual(f("12345.6789 blah blah"), 12345.6789)

    def test_percentiles(self):
        f = self.target.percentiles

        # Test empty
        self.assertEqual(f([], []), {})
        # Test 1 item, empty percentile
        self.assertEqual(f([], [42]), {})
        # Test 1 item, non-empty percentile
        self.assertEqual(f([10, 90], [42]), {"p10": 42, "p90": 42})
        # Test several items, empty percentile
        self.assertEqual(f([], [1, 23, 42, 99]), {})
        # Test several items, non-empty percentile
        self.assertEqual(f([10, 25, 50, 90, 100], [1, 23, 42, 99]),
                         {"p10": 1, "p25": 23, "p90": 99,
                          "p50": 42, "p100": 99})

    def test_ip_label(self):
        f = self.target.ip_label

        fakesockmod = jigs.SocketModuleJig()
        try:
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            # Test no label
            self.assertEqual(f("blah"), "blah")
            # Test hostname, success
            fakesockmod.ghba_returns = [("result.com", None, None)]
            self.assertEqual(f("1.2.3.4"), "result.com")
            # Test hostname, failure
            fakesockmod.ghba_returns = [None]
            self.assertEqual(f("1.2.3.4"), "1.2.3.4")
            # Test old style NTP
            fakesockmod.ghba_returns = [("foo.org", None, None)]
            self.assertEqual(f("127.127.42.23"), "REFCLOCK(type=42,unit=23)")
        finally:
            ntp.statfiles.socket = socktemp

    def test___init__(self):
        # Create jigs
        fakegzipmod = jigs.GzipModuleJig()
        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        fakeglobmod = jigs.GlobModuleJig()
        load_args = []
        load_returns = []

        def loadjig(self, statsdir, stem):
            load_args.append((statsdir, stem))
            return load_returns.pop(0)
        process_args = []

        def processjig(self, stem, lines):
            process_args.append((stem, lines))
            return [stem + " " + line for line in lines]
        try:
            # Splice in jigs
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            errtemp = sys.stderr
            sys.stderr = logjig
            timetemp = ntp.statfiles.time
            ntp.statfiles.time = faketimemod
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            stemtemp = self.target._NTPStats__load_stem
            self.target._NTPStats__load_stem = loadjig
            processtemp = self.target._NTPStats__process_stem
            self.target._NTPStats__process_stem = processjig
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            load_returns = [["clock0", "clock1"], ["peer0", "peer1"],
                            ["loop0", "loop1"], ["raw0", "raw1"],
                            ["temp0", "temp1"], ["gpsd0", "gpsd1"]]
            cls = self.target("/foo/bar")
            self.assertEqual(cls.endtime, TDP * 2)
            self.assertEqual(cls.starttime, TDP)
            self.assertEqual(cls.period, TDP)
            self.assertEqual(cls.sitename, "bar")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats,
                             ["clockstats clock0", "clockstats clock1"])
            self.assertEqual(cls.peerstats,
                             ["peerstats peer0", "peerstats peer1"])
            self.assertEqual(cls.loopstats,
                             ["loopstats loop0", "loopstats loop1"])
            self.assertEqual(cls.rawstats,
                             ["rawstats raw0", "rawstats raw1"])
            self.assertEqual(cls.temps,
                             ["temps temp0", "temps temp1"])
            self.assertEqual(cls.gpsd,
                             ["gpsd gpsd0", "gpsd gpsd1"])
            # Test all arguments
            faketimemod.time_returns = [TDP * 2]
            fakesockmod.fqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            load_returns = [[], [], [], [], [], []]
            cls = self.target("/foo/bar", "Sitename", 200, 50, 150)
            self.assertEqual(cls.endtime, 150)
            self.assertEqual(cls.starttime, 50)
            self.assertEqual(cls.period, 200)
            self.assertEqual(cls.sitename, "Sitename")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats, [])
            self.assertEqual(cls.peerstats, [])
            self.assertEqual(cls.loopstats, [])
            self.assertEqual(cls.rawstats, [])
            self.assertEqual(cls.temps, [])
            self.assertEqual(cls.gpsd, [])
            # Test endtime, but no starttime
            faketimemod.time_returns = [TDP * 2]
            fakesockmod.fqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            load_returns = [[], [], [], [], [], []]
            cls = self.target("/foo/bar", "Sitename", 100, endtime=150)
            self.assertEqual(cls.endtime, 150)
            self.assertEqual(cls.starttime, 50)
            self.assertEqual(cls.period, 100)
            self.assertEqual(cls.sitename, "Sitename")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats, [])
            self.assertEqual(cls.peerstats, [])
            self.assertEqual(cls.loopstats, [])
            self.assertEqual(cls.rawstats, [])
            self.assertEqual(cls.temps, [])
            self.assertEqual(cls.gpsd, [])
            # Test endtime, but no starttime
            faketimemod.time_returns = [TDP * 2]
            fakesockmod.fqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            load_returns = [[], [], [], [], [], []]
            cls = self.target("/foo/bar", "Sitename", 100, starttime=150)
            self.assertEqual(cls.endtime, 250)
            self.assertEqual(cls.starttime, 150)
            self.assertEqual(cls.period, 100)
            self.assertEqual(cls.sitename, "Sitename")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats, [])
            self.assertEqual(cls.peerstats, [])
            self.assertEqual(cls.loopstats, [])
            self.assertEqual(cls.rawstats, [])
            self.assertEqual(cls.temps, [])
            self.assertEqual(cls.gpsd, [])
            # Test fqdn sitename
            logjig.data = []
            faketimemod.time_returns = [TDP * 2]
            fakesockmod.getfqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            load_returns = [[], [], [], [], [], []]
            cls = self.target("/foo/bar", "ntpstats", 100, 50, 150)
            self.assertEqual(cls.endtime, 150)
            self.assertEqual(cls.starttime, 50)
            self.assertEqual(cls.period, 100)
            self.assertEqual(cls.sitename, "jabber")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats, [])
            self.assertEqual(cls.peerstats, [])
            self.assertEqual(cls.loopstats, [])
            self.assertEqual(cls.rawstats, [])
            self.assertEqual(cls.temps, [])
            self.assertEqual(cls.gpsd, [])
        finally:
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            self.target._NTPStats__load_stem = stemtemp
            self.target._NTPStats__process_stem = processtemp
            sys.stderr = errtemp

    def test___load_statfiles(self):
        # Create jigs
        fakegzipmod = jigs.GzipModuleJig()
        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        fakeglobmod = jigs.GlobModuleJig()
        try:
            # Splice in jigs
            gziptemp = ntp.statfiles.gzip
            ntp.statfiles.gzip = fakegzipmod
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            errtemp = sys.stderr
            sys.stderr = logjig
            timetemp = ntp.statfiles.time
            ntp.statfiles.time = faketimemod
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            globtemp = ntp.statfiles.glob
            ntp.statfiles.glob = fakeglobmod
            opentemp = open
            ntp.statfiles.open = self.open_jig
            prostemp = self.target._NTPStats__process_stem
            self.target._NTPStats__process_stem = self.process_stem_jig
            # Set up repetable data
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            getmtime_data = [TDP+1, TDP+2, TDP+3, TDP+4, TDP+5, TDP+6,
                             TDP+7, TDP+8, TDP+9, TDP+10, TDP+11, TDP-1]
            glob_data = [("/foo/bar/clockstats.0", "/foo/bar/clockstats.1gz"),
                         ("/foo/bar/peerstats.0", "/foo/bar/peerstats.1"),
                         ("/foo/bar/loopstats.0", "/foo/bar/loopstats.1"),
                         ("/foo/bar/rawstats.0", "/foo/bar/rawstats.1"),
                         ("/foo/bar/temps0", "/foo/bar/temps1"),
                         ("/foo/bar/gpsd0", "/foo/bar/gpsd1")]  # time kicked
            # Load data into jigs
            # need to do it this time so that __init__ will work
            fakeosmod.path.getmtime_returns = getmtime_data[:]
            fakeglobmod.glob_returns = glob_data[:]
            self.open_returns = [jigs.FileJig(["40594 10\n", "40594 11"]),
                                 jigs.FileJig(["40594 30\n", "40594 31"]),
                                 jigs.FileJig(["40594 40\n", "40594 41"]),
                                 jigs.FileJig(["40594 50\n", "40594 51"]),
                                 jigs.FileJig(["40594 60\n", "40594 61"]),
                                 jigs.FileJig(["40594 70\n", "40594 71"]),
                                 jigs.FileJig(["40594 80\n", "40594 81"]),
                                 jigs.FileJig(["604801.25 40594 90\n",
                                               "#blah",
                                               "604802.25 40594 91"]),
                                 jigs.FileJig(["604803.25 40594 100\n",
                                               "#blah",
                                               "604804.25 40594 101"]),
                                 jigs.FileJig(["604805.25 40594 110\n",
                                               "#blah",
                                               "604806.25 40594 111"]),
                                 jigs.FileJig(["604807.25 40594 120\n",
                                               "#blah",
                                               "604808.25 40594 121"])]
            fakegzipmod.files_returned = [jigs.FileJig(["40594 20\n",
                                                        "40594 21"])]
            TestNTPStats.process_stem_returns = [[]] * 6
            cls = self.target("/foo/bar", "sitename")
            # Reload and test
            fakeosmod.path.getmtime_returns = getmtime_data[:]
            fakeglobmod.glob_returns = glob_data[:]
            self.open_returns = [jigs.FileJig(["40594 10\n", "40594 11"]),
                                 jigs.FileJig(["40594 30\n", "40594 31"]),
                                 jigs.FileJig(["40594 40\n", "40594 41"]),
                                 jigs.FileJig(["40594 50\n", "40594 51"]),
                                 jigs.FileJig(["40594 60\n", "40594 61"]),
                                 jigs.FileJig(["40594 70\n", "40594 71"]),
                                 jigs.FileJig(["40594 80\n", "40594 81"]),
                                 jigs.FileJig(["604801.25 40594 90\n",
                                               "#blah",
                                               "604802.25 40594 91"]),
                                 jigs.FileJig(["604803.25 40594 100\n",
                                               "#blah",
                                               "604804.25 40594 101"]),
                                 jigs.FileJig(["604805.25 40594 110\n",
                                               "#blah",
                                               "604806.25 40594 111"]),
                                 jigs.FileJig(["604807.25 40594 120\n",
                                               "#blah",
                                               "604808.25 40594 121"])]
            fakegzipmod.files_returned = [jigs.FileJig(["40594 20\n",
                                                        "40594 21"])]
            TestNTPStats.process_stem_returns = [[]] * 6
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "clockstats"),
                             ['40594 10\n', '40594 11',
                              '40594 20\n', '40594 21'])
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "peerstats"),
                             ['40594 30\n', '40594 31',
                              '40594 40\n', '40594 41'])
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "loopstats"),
                             ['40594 50\n', '40594 51',
                              '40594 60\n', '40594 61'])
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "rawstats"),
                             ['40594 70\n', '40594 71',
                              '40594 80\n', '40594 81'])
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "temps"),
                             ["604801.25 40594 90\n",
                              "#blah",
                              "604802.25 40594 91",
                              "604803.25 40594 100\n",
                              "#blah",
                              "604804.25 40594 101"])
            self.assertEqual(cls._NTPStats__load_stem("/foo/bar",
                                                      "gpsd"),
                             ['604805.25 40594 110\n', '#blah',
                              '604806.25 40594 111'])
        finally:
            ntp.statfiles.gzip = gziptemp
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = globtemp
            ntp.statfiles.open = opentemp
            sys.stderr = errtemp
            self.target._NTPStats__process_stem = prostemp

    def test___process_stem_lines(self):
        try:
            fakeosmod = jigs.OSModuleJig()
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            loadtemp = self.target._NTPStats__load_stem
            self.target._NTPStats__load_stem = self.load_stem_jig
            dataOne = ["#clockstats0\n", "40587 5 foo\n", "40587 10 bar\n",
                       "40587 15 baz\n", "#clockstats1\n", "40587 20 quux"]
            dataTwo = ["#gpsdstats0 foo bar quux\n",
                       "20.5 5 foo\n", "21.5 10 bar\n", "22.5 15 baz\n",
                       "#gpsdstats1\n", "23.5 20 quux"]
            TestNTPStats.load_stem_returns = [[]] * 6
            fakeosmod.path.isdir_returns = [True] * 10
            cls = self.target("/foo/bar", "sitename", starttime=0,
                              endtime=(86400 * 7))
            self.assertEqual(cls._NTPStats__process_stem("clockstats",
                                                         dataOne),
                             [[5000, '5.0', 'foo'], [10000, '10.0', 'bar'],
                              [15000, '15.0', 'baz'], [20000, '20.0', 'quux']])
            self.assertEqual(cls._NTPStats__process_stem("peerstats",
                                                         dataOne),
                             [[5000, '5.0', 'foo'], [10000, '10.0', 'bar'],
                              [15000, '15.0', 'baz'], [20000, '20.0', 'quux']])
            self.assertEqual(cls._NTPStats__process_stem("loopstats",
                                                         dataOne),
                             [[5000, '5.0', 'foo'], [10000, '10.0', 'bar'],
                              [15000, '15.0', 'baz'], [20000, '20.0', 'quux']])
            self.assertEqual(cls._NTPStats__process_stem("rawstats",
                                                         dataOne),
                             [[5000, '5.0', 'foo'], [10000, '10.0', 'bar'],
                              [15000, '15.0', 'baz'], [20000, '20.0', 'quux']])
            self.assertEqual(cls._NTPStats__process_stem("temps", dataTwo),
                             [[20500, '20.5', '5', 'foo'],
                              [21500, '21.5', '10', 'bar'],
                              [22500, '22.5', '15', 'baz'],
                              [23500, '23.5', '20', 'quux']])
            self.assertEqual(cls._NTPStats__process_stem("gpsd", dataTwo),
                             [[20500, '20.5', '5', 'foo'],
                              [21500, '21.5', '10', 'bar'],
                              [22500, '22.5', '15', 'baz'],
                              [23500, '23.5', '20', 'quux']])
        finally:
            self.target._NTPStats__load_stem = loadtemp
            ntp.statfiles.os = ostemp

    def test_peersplit(self):
        # Create jigs
        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        try:
            # Splice in jigs
            loadtemp = self.target._NTPStats__load_stem
            self.target._NTPStats__load_stem = self.load_stem_jig
            prostemp = self.target._NTPStats__process_stem
            self.target._NTPStats__process_stem = self.process_stem_jig
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            errtemp = sys.stderr
            sys.stderr = logjig
            timetemp = ntp.statfiles.time
            ntp.statfiles.time = faketimemod
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            TestNTPStats.load_stem_returns = [[]] * 6
            TestNTPStats.process_stem_returns = [[]] * 6
            cls = self.target("/foo/bar")
            cls.peerstats = [[604830000, "604830.0", "1.2.3.4"],
                             [604831000, "604831.0", "1.2.3.4"],
                             [604840000, "604840.0", "1.2.3.4"],
                             [604841000, "604841.0", "5.6.7.8"]]
            # Test
            self.assertEqual(cls.peersplit(),
                             {'1.2.3.4': [[604830000, '604830.0', '1.2.3.4'],
                                          [604831000, '604831.0', '1.2.3.4'],
                                          [604840000, '604840.0', '1.2.3.4']],
                              '5.6.7.8': [[604841000, '604841.0', '5.6.7.8']]})
            self.target.peermap["1.2.3.4"][0][0] = 42
            # Test that it uses cache
            self.assertEqual(cls.peersplit(),
                             {'1.2.3.4': [[42, '604830.0', '1.2.3.4'],
                                          [604831000, '604831.0', '1.2.3.4'],
                                          [604840000, '604840.0', '1.2.3.4']],
                              '5.6.7.8': [[604841000, '604841.0', '5.6.7.8']]})
        finally:
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            sys.stderr = errtemp
            self.target._NTPStats__load_stem = loadtemp
            self.target._NTPStats__process_stem = prostemp

    def test_gpssplit(self):
        # Create jigs
        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        try:
            # Splice in jigs
            loadtemp = self.target._NTPStats__load_stem
            self.target._NTPStats__load_stem = self.load_stem_jig
            prostemp = self.target._NTPStats__process_stem
            self.target._NTPStats__process_stem = self.process_stem_jig
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            errtemp = sys.stderr
            sys.stderr = logjig
            timetemp = ntp.statfiles.time
            ntp.statfiles.time = faketimemod
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            TestNTPStats.load_stem_returns = [[]] * 6
            TestNTPStats.process_stem_returns = [[]] * 6
            cls = self.target("/foo/bar")
            cls.gpsd = [["604801.25", "40594", "90"],
                        ["604802.25", "40594", "90"],
                        ["argh!"],
                        ["604803.25", "40594", "100"],
                        ["604804.25", "40594", "100"],
                        ["604804.25", "40594", "110"],
                        ["604804.25", "40594", "110"],
                        ["604804.25", "40594", "120"],
                        ["604804.25", "40594", "120"]]
            # Test
            self.assertEqual(cls.gpssplit(),
                             {'90': [['604801.25', '40594', '90'],
                                     ['604802.25', '40594', '90']],
                              '100': [['604803.25', '40594', '100'],
                                      ['604804.25', '40594', '100']],
                              '110': [['604804.25', '40594', '110'],
                                      ['604804.25', '40594', '110']],
                              '120': [['604804.25', '40594', '120'],
                                      ['604804.25', '40594', '120']]})
        finally:
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            sys.stderr = errtemp
            self.target._NTPStats__load_stem = loadtemp
            self.target._NTPStats__process_stem = prostemp

    def test_tempssplit(self):
        # Create jigs
        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        try:
            # Splice in jigs
            loadtemp = self.target._NTPStats__load_stem
            self.target._NTPStats__load_stem = self.load_stem_jig
            prostemp = self.target._NTPStats__process_stem
            self.target._NTPStats__process_stem = self.process_stem_jig
            socktemp = ntp.statfiles.socket
            ntp.statfiles.socket = fakesockmod
            errtemp = sys.stderr
            sys.stderr = logjig
            timetemp = ntp.statfiles.time
            ntp.statfiles.time = faketimemod
            ostemp = ntp.statfiles.os
            ntp.statfiles.os = fakeosmod
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            TestNTPStats.load_stem_returns = [[]] * 6
            TestNTPStats.process_stem_returns = [[]] * 6
            cls = self.target("/foo/bar")
            cls.temps = [["604801.25", "40594", "90"],
                         ["604802.25", "40594", "90"],
                         ["argh!"],
                         ["604803.25", "40594", "100"],
                         ["604804.25", "40594", "100"],
                         ["604804.25", "40594", "110"],
                         ["604804.25", "40594", "110"],
                         ["604804.25", "40594", "120"],
                         ["604804.25", "40594", "120"]]
            # Test
            self.assertEqual(cls.tempssplit(),
                             {'90': [['604801.25', '40594', '90'],
                                     ['604802.25', '40594', '90']],
                              '100': [['604803.25', '40594', '100'],
                                      ['604804.25', '40594', '100']],
                              '110': [['604804.25', '40594', '110'],
                                      ['604804.25', '40594', '110']],
                              '120': [['604804.25', '40594', '120'],
                                      ['604804.25', '40594', '120']]})
        finally:
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            sys.stderr = errtemp
            self.target._NTPStats__load_stem = loadtemp
            self.target._NTPStats__process_stem = prostemp


if __name__ == '__main__':
    unittest.main()
