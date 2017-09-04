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

    def test___init__(self):
        open_calls = []
        open_returns = []

        def open_jig(filename, filemode):
            open_calls.append((filename, filemode))
            return open_returns.pop(0)

        fakesockmod = jigs.SocketModuleJig()
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        fakeosmod = jigs.OSModuleJig()
        fakeglobmod = jigs.GlobModuleJig()
        try:
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
            global open
            opentemp = open
            open = open_jig
            # Test simplest
            TDP = self.target.DefaultPeriod
            faketimemod.time_returns = [TDP * 3]
            fakesockmod.fqdn_returns = ["jabber"]
            fakeosmod.path.isdir_returns = [True]
            TM = TDP * 2  # equal to starttime
            fakeosmod.path.getmtime_returns = [TM+1, TM+2, TM+3, TM+4,
                                               TM+5, TM+6, TM+7, TM+8,
                                               TM+9, TM+10, TM+11, TM-1]
            fakeglobmod.glob_returns = [("/foo/bar/clockstats0",
                                         "/foo/bar/clockstats1"),
                                        ("/foo/bar/peerstats0",
                                         "/foo/bar/peerstats1"),
                                        ("/foo/bar/loopstats0",
                                         "/foo/bar/loopstats1"),
                                        ("/foo/bar/rawstats0",
                                         "/foo/bar/rawstats1"),
                                        ("/foo/bar/temps.0",
                                         "/foo/bar/temps.1"),
                                        ("/foo/bar/gpsd.0",
                                         "/foo/bar/gpsd.1")]  # time kicked
            cls = self.target("/foo/bar")
            self.assertEqual(cls.endtime, TDP * 3)
            self.assertEqual(cls.starttime, TDP * 2)
            self.assertEqual(cls.sitename, "bar")
            self.assertEqual(logjig.data, [])
        finally:
            ntp.statfiles.socket = socktemp
            ntp.statfiles.os = ostemp
            ntp.statfiles.time = timetemp
            ntp.statfiles.glob = fakeglobmod
            #open = opentemp
            sys.stderr = errtemp

if __name__ == '__main__':
    unittest.main()
