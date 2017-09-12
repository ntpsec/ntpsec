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

    def open_jig(self, filename, filemode):
        self.open_calls.append((filename, filemode))
        ret = self.open_returns.pop(0)
        if ret is None:
            raise IOError
        else:
            return ret

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
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            fakeosmod.path.getmtime_returns = [TDP+1, TDP+2, TDP+3, TDP+4,
                                               TDP+5, TDP+6, TDP+7, TDP+8,
                                               TDP+9, TDP+10, TDP+11, TDP-1]
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
            fakeglobmod.glob_returns = [("/foo/bar/clockstats.0",
                                         "/foo/bar/clockstats.1gz"),
                                        ("/foo/bar/peerstats.0",
                                         "/foo/bar/peerstats.1"),
                                        ("/foo/bar/loopstats.0",
                                         "/foo/bar/loopstats.1"),
                                        ("/foo/bar/rawstats.0",
                                         "/foo/bar/rawstats.1"),
                                        ("/foo/bar/temps0",
                                         "/foo/bar/temps1"),
                                        ("/foo/bar/gpsd0",
                                         "/foo/bar/gpsd1")]  # time kicked
            cls = self.target("/foo/bar")
            self.assertEqual(cls.endtime, TDP * 2)
            self.assertEqual(cls.starttime, TDP)
            self.assertEqual(cls.period, TDP)
            self.assertEqual(cls.sitename, "bar")
            self.assertEqual(logjig.data, [])
            self.assertEqual(cls.clockstats,
                             [[604810000, '604810.0'],
                              [604811000, '604811.0'],
                              [604820000, '604820.0'],
                              [604821000, '604821.0']])
            self.assertEqual(cls.peerstats,
                             [[604830000, '604830.0'],
                              [604831000, '604831.0'],
                              [604840000, '604840.0'],
                              [604841000, '604841.0']])
            self.assertEqual(cls.loopstats,
                             [[604850000, '604850.0'],
                              [604851000, '604851.0'],
                              [604860000, '604860.0'],
                              [604861000, '604861.0']])
            self.assertEqual(cls.rawstats,
                             [[604870000, '604870.0'],
                              [604871000, '604871.0'],
                              [604880000, '604880.0'],
                              [604881000, '604881.0']])
            self.assertEqual(cls.temps,
                             [[604801250, '604801.25', '40594', '90'],
                              [604802250, '604802.25', '40594', '91'],
                              [604803250, '604803.25', '40594', '100'],
                              [604804250, '604804.25', '40594', '101']])
            self.assertEqual(cls.gpsd,
                             [[604805250, '604805.25', '40594', '110'],
                              [604806250, '604806.25', '40594', '111']])
            # Test all arguments
            faketimemod.time_returns = [TDP * 2]
            fakesockmod.fqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            self.open_returns = [None]
            fakeglobmod.glob_returns = [(["/foo/bar/clockstats.0"]),
                                        ([]), ([]), ([]), ([]), ([])]
            fakeosmod.path.getmtime_returns = [101, 102, 103, 104, 105, 106]
            cls = self.target("/foo/bar", "Sitename", 100, 50, 150)
            self.assertEqual(cls.endtime, 150)
            self.assertEqual(cls.starttime, 50)
            self.assertEqual(cls.period, 100)
            self.assertEqual(cls.sitename, "Sitename")
            self.assertEqual(logjig.data,
                             ["ntpviz: WARNING: could not read "
                              "/foo/bar/clockstats.0\n"])
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
            open_returns = [None]
            fakeglobmod.glob_returns = [([]), ([]), ([]), ([]), ([]), ([])]
            fakeosmod.path.getmtime_returns = []
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
            ntp.statfiles.gzip = gziptemp
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = fakeglobmod
            ntp.statfiles.open = opentemp
            sys.stderr = errtemp

    def test_peersplit(self):
        # Yes, this copies much of test___init__. I already know it works
        #  and __init__ is very complicated
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
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            fakeosmod.path.getmtime_returns = [TDP+1, TDP+2, TDP+3, TDP+4,
                                               TDP+5, TDP+6, TDP+7, TDP+8,
                                               TDP+9, TDP+10, TDP+11, TDP-1]
            self.open_returns = [jigs.FileJig(["40594 10\n", "40594 11"]),
                                 jigs.FileJig(["40594 30 1.2.3.4\n",
                                               "50594 -12\n",
                                               "40594 31 1.2.3.4\n",
                                               "40594 31.5 5.6.7.8\n"]),
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
            fakeglobmod.glob_returns = [("/foo/bar/clockstats.0",
                                         "/foo/bar/clockstats.1gz"),
                                        ("/foo/bar/peerstats.0",
                                         "/foo/bar/peerstats.1"),
                                        ("/foo/bar/loopstats.0",
                                         "/foo/bar/loopstats.1"),
                                        ("/foo/bar/rawstats.0",
                                         "/foo/bar/rawstats.1"),
                                        ("/foo/bar/temps0",
                                         "/foo/bar/temps1"),
                                        ("/foo/bar/gpsd0",
                                         "/foo/bar/gpsd1")]  # time kicked
            cls = self.target("/foo/bar")
            # Test
            self.assertEqual(cls.peersplit(),
                             {"1.2.3.4": [[604830000, '604830.0', "1.2.3.4"],
                                          [604831000, '604831.0', "1.2.3.4"]],
                              "5.6.7.8": [[604831500, '604831.5', "5.6.7.8"]]})
            self.target.peermap["1.2.3.4"][0][0] = 42
            # Test that it uses cache
            self.assertEqual(cls.peersplit(),
                             {"1.2.3.4": [[42, '604830.0', "1.2.3.4"],
                                          [604831000, '604831.0', "1.2.3.4"]],
                              "5.6.7.8": [[604831500, '604831.5', "5.6.7.8"]]})
        finally:
            ntp.statfiles.gzip = gziptemp
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = fakeglobmod
            ntp.statfiles.open = opentemp
            sys.stderr = errtemp

    def test_gpssplit(self):
        # Yes, this copies much of test___init__. I already know it works
        #  and __init__ is very complicated
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
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            fakeosmod.path.getmtime_returns = [TDP+1, TDP+2, TDP+3, TDP+4,
                                               TDP+5, TDP+6, TDP+7, TDP+8,
                                               TDP+9, TDP+10, TDP+11, TDP-1]
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
            fakeglobmod.glob_returns = [("/foo/bar/clockstats.0",
                                         "/foo/bar/clockstats.1gz"),
                                        ("/foo/bar/peerstats.0",
                                         "/foo/bar/peerstats.1"),
                                        ("/foo/bar/loopstats.0",
                                         "/foo/bar/loopstats.1"),
                                        ("/foo/bar/rawstats.0",
                                         "/foo/bar/rawstats.1"),
                                        ("/foo/bar/temps0",
                                         "/foo/bar/temps1"),
                                        ("/foo/bar/gpsd0",
                                         "/foo/bar/gpsd1")]  # time kicked
            cls = self.target("/foo/bar")
            self.assertEqual(cls.gpssplit(),
                             {"40594": [[604805250, '604805.25', '40594',
                                         '110'],
                                        [604806250, '604806.25', '40594',
                                         '111']]})
        finally:
            ntp.statfiles.gzip = gziptemp
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = fakeglobmod
            ntp.statfiles.open = opentemp
            sys.stderr = errtemp

    def test_tempssplit(self):
        # Yes, this copies much of test___init__. I already know it works
        #  and __init__ is very complicated
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
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 2]
            fakeosmod.path.isdir_returns = [True]
            fakeosmod.path.getmtime_returns = [TDP+1, TDP+2, TDP+3, TDP+4,
                                               TDP+5, TDP+6, TDP+7, TDP+8,
                                               TDP+9, TDP+10, TDP+11, TDP-1]
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
                                               "604804.25 40594 101",
                                               "604804.25 40595 101"]),
                                 jigs.FileJig(["604805.25 40594 110\n",
                                               "#blah",
                                               "604806.25 40594 111"]),
                                 jigs.FileJig(["604807.25 40594 120\n",
                                               "#blah",
                                               "604808.25 40594 121"])]
            fakegzipmod.files_returned = [jigs.FileJig(["40594 20\n",
                                                        "40594 21"])]
            fakeglobmod.glob_returns = [("/foo/bar/clockstats.0",
                                         "/foo/bar/clockstats.1gz"),
                                        ("/foo/bar/peerstats.0",
                                         "/foo/bar/peerstats.1"),
                                        ("/foo/bar/loopstats.0",
                                         "/foo/bar/loopstats.1"),
                                        ("/foo/bar/rawstats.0",
                                         "/foo/bar/rawstats.1"),
                                        ("/foo/bar/temps0",
                                         "/foo/bar/temps1"),
                                        ("/foo/bar/gpsd0",
                                         "/foo/bar/gpsd1")]  # time kicked
            cls = self.target("/foo/bar")
            self.assertEqual(cls.tempssplit(),
                             {"40594": [[604801250, '604801.25',
                                         '40594', '90'],
                                        [604802250, '604802.25',
                                         '40594', '91'],
                                        [604803250, '604803.25',
                                         '40594', '100'],
                                        [604804250, '604804.25',
                                         '40594', '101']],
                              "40595": [[604804250, '604804.25',
                                         '40595', '101']]})
        finally:
            ntp.statfiles.gzip = gziptemp
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = fakeglobmod
            ntp.statfiles.open = opentemp
            sys.stderr = errtemp


if __name__ == '__main__':
    unittest.main()
