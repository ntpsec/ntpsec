#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import unittest
import ntp.util
import ntp.packet
import sys
import socket

import jigs


class TestPylibUtilMethods(unittest.TestCase):

    def test_deunicode_units(self):
        u = ntp.util

        self.assertEqual(u.UNIT_US, u"µs")
        self.assertEqual(u.UNIT_PPK, u"‰")
        self.assertEqual(u.UNITS_SEC, ["ns",  u"µs", "ms", "s", "ks"])
        self.assertEqual(u.UNITS_PPX, ["ppt", "ppb", "ppm", u"‰"])
        u.deunicode_units()
        self.assertEqual(u.UNIT_US, "us")
        self.assertEqual(u.UNIT_PPK, "ppk")
        self.assertEqual(u.UNITS_SEC, ["ns",  "us", "ms", "s", "ks"])
        self.assertEqual(u.UNITS_PPX, ["ppt", "ppb", "ppm", "ppk"])
        u.UNIT_US = u"µs"
        u.UNIT_PPK = u"‰"
        u.UNITS_SEC[1] = u.UNIT_US
        u.UNITS_PPX[3] = u.UNIT_PPK

    def test_dolog(self):
        f = ntp.util.dolog

        faketimemod = jigs.TimeModuleJig()
        # We need a test jig

        class LogTester:
            def __init__(self):
                self.written = None
                self.flushed = False

            def write(self, text):
                if self.written is None:
                    self.written = ""
                self.written += text

            def flush(self):
                self.flushed = True
        try:
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            # Test with logging off (fd == None)
            #   uh... if someone can think of a way to do that please tell me
            # Test with logging on, below threshold
            jig = LogTester()
            faketimemod.time_returns = [0, 1]
            f(jig, "blah", 0, 3)
            self.assertEqual((jig.written, jig.flushed), (None, False))
            # Test with logging on, above threshold
            jig.__init__()  # reset
            f(jig, "blah", 4, 3)
            self.assertEqual((jig.written, jig.flushed),
                             ("1970-01-01T00:00:01Z blah\n", True))
        finally:
            ntp.util.time = timetemp

    def test_safeargcast(self):
        f = ntp.util.safeargcast

        errjig = jigs.FileJig()
        try:
            errtemp = sys.stderr
            sys.stderr = errjig
            # Test successful int
            self.assertEqual(f("42", int, "blah %s", "\nDo the needful\n"), 42)
            self.assertEqual(errjig.data, [])
            # Test successful float
            self.assertAlmostEqual(f("5.23", float, "blah %s",
                                     "\nDo the needful\n"),
                                   5.23)
            self.assertEqual(errjig.data, [])
            # Test failure
            try:
                f("23.5", int, "blah %s", "\nDo the needful\n")
                errored = False
            except SystemExit:
                errored = True
            self.assertEqual(errored, True)
            self.assertEqual(errjig.data, ["blah 23.5", "\nDo the needful\n"])
        finally:
            sys.stderr = errtemp

    def test_stdversion(self):
        f = ntp.util.stdversion

        ver = "@NTPSEC_VERSION_EXTENDED@"
        self.assertEqual(f(), "ntpsec-" + ver)

    def test_rfc3339(self):
        f = ntp.util.rfc3339

        self.assertEqual(f(1480999786),
                         '2016-12-06T04:49:46Z')
        self.assertEqual(f(1480999786.5),
                         '2016-12-06T04:49:46.5Z')
        # RFC 3339, 2 digits of seconds.
        # we round, but the spec is silent on rounding
        # Python 2 and 3 round differently
        if sys.version_info[0] < 3:
            self.assertEqual(f(1480999786.025), "2016-12-06T04:49:46.03Z")
        else:
            self.assertEqual(f(1480999786.025), "2016-12-06T04:49:46.025Z")

    def test_deformatNTPTime(self):
        f = ntp.util.deformatNTPTime

        # Test standard
        self.assertEqual(f("0x0001020304050607.08090A0B0C0D0E0F"),
                         "\x00\x01\x02\x03\x04\x05\x06\x07"
                         "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F")
        # Test empty
        self.assertEqual(f(""), "")

    def test_hexstr2octets(self):
        f = ntp.util.hexstr2octets

        # Test
        self.assertEqual(f("f00dface"), "\xF0\x0D\xFA\xCE")
        # Test odd length
        self.assertEqual(f("cafebabe1"), "\xCA\xFE\xBA\xBE")

    def test_slicedata(self):
        f = ntp.util.slicedata

        # Test normal
        self.assertEqual(f("foobaz", 2), ("fo", "obaz"))
        # Test overflow
        self.assertEqual(f("foobaz", 8), ("foobaz", ""))
        # Test underflow
        self.assertEqual(f("foobaz", -2), ("foob", "az"))

    def test_portsplit(self):
        self.assertEqual(ntp.util.portsplit("host.invalid"),
                         ("host.invalid", ""))
        self.assertEqual(ntp.util.portsplit("host.invalid:123"),
                         ("host.invalid", ":123"))
        self.assertEqual(ntp.util.portsplit(
            "[0000:1111:2222:3333:4444:5555:6666:7777]:123"),
            ("0000:1111:2222:3333:4444:5555:6666:7777", ":123"))

    def test_parseConf(self):
        data = """foo bar 42 #blah blah blah
        "abcd"   poi? 3.14
        "\\"\\n\\\\foo\\c" \\
        poi! """
        parsed = ntp.util.parseConf(data)
        self.assertEqual(parsed,
                         [[(False, "foo"), (False, "bar"), (False, 42)],
                          [(True, "abcd"), (False, "poi?"), (False, 3.14)],
                          [(True, "\"\n\\foo"), (False, "poi!")]])

    def test_stringfilt(self):
        f = ntp.util.stringfilt

        self.assertEqual(f("1.2345 2.3456 3.4567 4.5678 5.6789 6.789"),
                         " 1.2345  2.3456  3.4567  4.5678  5.6789   6.789")

    def test_oomsbetweenunits(self):
        f = ntp.util.oomsbetweenunits

        self.assertEqual(f(ntp.util.UNIT_KS, ntp.util.UNIT_MS), 6)
        self.assertEqual(f(ntp.util.UNIT_PPM, ntp.util.UNIT_PPB), 3)
        self.assertEqual(f(ntp.util.UNIT_S, None), 9)

    def test_scalestring(self):
        f = ntp.util.scalestring

        # Scale all decimals
        self.assertEqual(f("0.042"), ("42", -1))
        # Unscalable
        self.assertEqual(f(".23"), ("0.23", 0))
        # Typical length, positive value, no scaling
        self.assertEqual(f("1.23450"), ("1.23450", 0))
        # Ditto, negative
        self.assertEqual(f("-1.23450"), ("-1.23450", 0))
        # Long, positive value, no scaling
        self.assertEqual(f("1.234567890123456"), ("1.234567890123456", 0))
        # ditto, negative
        self.assertEqual(f("-1.234567890123456"), ("-1.234567890123456", 0))
        # Zero
        self.assertEqual(f("0.000000"), ("0", -2))
        # Zero with point
        self.assertEqual(f("0.0000"), ("0.0", -1))
        # Negative zero
        self.assertEqual(f("-0.000"), ("0", -1))
        # Large, positive, non scaled
        self.assertEqual(f("987.654"), ("987.654", 0))
        # ditto, negative
        self.assertEqual(f("-987.654"), ("-987.654", 0))
        # Scaled to larger unit, positive
        self.assertEqual(f("12345.67890"), ("12.34567890", 1))
        # ditto, negative
        self.assertEqual(f("-12345.67890"), ("-12.34567890", 1))
        # Scaled to smaller unit, position
        self.assertEqual(f("0.1234567890"), ("123.4567890", -1))
        # ditto, negative
        self.assertEqual(f("-0.1234567890"), ("-123.4567890", -1))
        # Bizarre 1
        self.assertEqual(f("-000.000012345678900987654321"),
                         ("-12.345678900987654321", -2))
        # Bizarre 2
        self.assertEqual(f("1234567890987654321000.00000000000042"),
                         ("1.23456789098765432100000000000000042", 7))
        # Bizarre 3
        self.assertEqual(f("00000000.000000000000"),
                         ("0", -4))

    def test_rescalestring(self):
        f = ntp.util.rescalestring

        # No-op
        self.assertEqual(f("1000.42", 0), "1000.42")
        # Scale to higher unit
        self.assertEqual(f("123.456", 1), "0.123456")
        # ditto, negative
        self.assertEqual(f("-123.456", 1), "-0.123456")
        # Scale to higher unit, beyond available digits
        self.assertEqual(f("12.34", 2), "0.00001234")
        # ditto, negative
        self.assertEqual(f("-12.34", 2), "-0.00001234")
        # Scale to lower unit
        self.assertEqual(f("1.23456", -1), "1234.56")
        # ditto, negative
        self.assertEqual(f("-1.23456", -1), "-1234.56")
        # Scale to lower unit, beyond available digits
        self.assertEqual(f("1.23456", -2), None)
        # ditto, negative
        self.assertEqual(f("-1.23456", -2), None)
        # Scale from below the decimal
        self.assertEqual(f("0.420", -1), "420")

    def test_breaknumberstring(self):
        f = ntp.util.breaknumberstring

        # No decimals, positive
        self.assertEqual(f("1234567890"), ("1234567890", "", False))
        # ditto, negative
        self.assertEqual(f("-1234567890"), ("1234567890", "", True))
        # No whole, positive
        self.assertEqual(f(".12345"), ("", "12345", False))
        # ditto, negative
        self.assertEqual(f("-.12345"), ("", "12345", True))
        # Both sides, position
        self.assertEqual(f("123.456"), ("123", "456", False))
        # ditto, negative
        self.assertEqual(f("-123.456"), ("123", "456", True))

    def test_gluenumberstring(self):
        f = ntp.util.gluenumberstring

        # No decimals, positive
        self.assertEqual(f("123", "", False), "123")
        # ditto, negative
        self.assertEqual(f("123", "", True), "-123")
        # No whole, positive
        self.assertEqual(f("", "456", False), "0.456")
        # ditto, negative
        self.assertEqual(f("", "456", True), "-0.456")
        # Both sides, positive
        self.assertEqual(f("123", "456", False), "123.456")
        # ditto, negative
        self.assertEqual(f("123", "456", True), "-123.456")

    def test_fitinfield(self):
        f = ntp.util.fitinfield

        # Field too small, crop decimals
        self.assertEqual(f("123.456", 5), "123.5")
        # ditto, negative
        self.assertEqual(f("-123.456", 5), "-123.")
        # Field too small, blow field
        self.assertEqual(f("12345.678", 4), "12346.")
        # Goldilocks
        self.assertEqual(f("123.456", 7), "123.456")
        # Field too big
        self.assertEqual(f("123.456", 10), "   123.456")
        # Rounding test, round down
        self.assertEqual(f("1.994", 4), "1.99")
        # Rounding test, round up
        self.assertEqual(f("1.995", 4), "2.00")
        # Attempt to catch bug
        self.assertEqual(f("15937.5", None), "15937.5")
        # Test to bid and no decimals
        self.assertEqual(f("123456", 4), "123456")

    def test_cropprecision(self):
        f = ntp.util.cropprecision

        # No decimals
        self.assertEqual(f("1234", 6), "1234")
        # Decimals, no crop
        self.assertEqual(f("12.3456", 6), "12.3456")
        # Decimals, crop
        self.assertEqual(f("12.3456", 3), "12.345")
        # At baseunit
        self.assertEqual(f("1.234", 0), "1")

    def test_isstringzero(self):
        f = ntp.util.isstringzero

        # Non-zero
        self.assertEqual(f("0.0000001"), False)
        # Zero
        self.assertEqual(f("-0.00"), True)

    def test_unitify(self):
        f = ntp.util.unitify
        nu = ntp.util

        # Zero
        self.assertEqual(f("0.000", nu.UNIT_MS), u"     0µs")
        # Zero, spaced
        self.assertEqual(f("0.000", nu.UNIT_MS, unitSpace=True), u"    0 µs")
        # Standard, width=8
        self.assertEqual(f("1.234", nu.UNIT_MS), " 1.234ms")
        # ditto, negative
        self.assertEqual(f("-1.234", nu.UNIT_MS), "-1.234ms")
        # Scale to larger unit, width=8
        self.assertEqual(f("1234.5", nu.UNIT_MS), " 1.2345s")
        # ditto, negative
        self.assertEqual(f("-1234.5", nu.UNIT_MS), "-1.2345s")
        # Scale to smaller unit, width=8
        self.assertEqual(f("0.01234", nu.UNIT_MS), u" 12.34µs")
        # ditto, negative
        self.assertEqual(f("-0.01234", nu.UNIT_MS), u"-12.34µs")
        # At baseunit
        self.assertEqual(f("12.0", nu.UNIT_NS), "    12ns")
        # Scale to baseunit
        self.assertEqual(f(".042", nu.UNIT_US), "    42ns")
        # Below baseunit
        self.assertEqual(f("23.42", nu.UNIT_NS), "    23ns")
        # Different units
        self.assertEqual(f("12.345", nu.UNIT_PPM), "12.35ppm")
        # Strip
        self.assertEqual(f("1.23", nu.UNIT_MS, width=None), "1.23ms")
        # Different width
        self.assertEqual(f("1.234", nu.UNIT_MS, width=12), "     1.234ms")
        # Outside of available units
        self.assertEqual(f("1234.5", nu.UNIT_KS), "1234.5ks")
        # Seconds
        self.assertEqual(f("42.23", nu.UNIT_S), "  42.23s")
        # Attempt to catch bug
        self.assertEqual(f("15937.5", nu.UNIT_MS, width=None), "15.9375s")

    def test_stringfiltcooker(self):
        # No scale
        self.assertEqual(ntp.util.stringfiltcooker(
            "1.02 34.5 0.67835 -23.0 9 6.7 1.00 .1"),
            "   1.02    34.5 0.67835   -23.0       9     6.7    1.00     "
            "0.1 ms"
        )
        # Scale to larger unit
        self.assertEqual(ntp.util.stringfiltcooker(
            "1000.02 3400.5 0.67835 -23.0 9001 6.7 1.00 1234"),
            "1.00002  3.4005 0.00068 -0.0230   9.001  0.0067 0.00100   "
            "1.234 s"
        )
        # Scale to smaller unit
        self.assertEqual(ntp.util.stringfiltcooker(
            "0.470 0.420 0.430 0.500 0.460 0.4200 0.490 0.480"),
            u"    470     420     430     500     460   420.0     490     "
            u"480 µs")
        # Can't scale
        self.assertEqual(ntp.util.stringfiltcooker(
            "0.47 0.42 0.43 0.50 0.46 0.42 0.49 0.48"),
            "   0.47    0.42    0.43    0.50    0.46    0.42    0.49    "
            "0.48 ms")
        # Can't scale, only one value blocking
        self.assertEqual(ntp.util.stringfiltcooker(
            "0.47 0.4200 0.4300 0.5000 0.4600 0.4200 0.4900 0.4800"),
            "   0.47  0.4200  0.4300  0.5000  0.4600  0.4200  0.4900  "
            "0.4800 ms")

    def test_unitrelativeto(self):
        f = ntp.util.unitrelativeto

        # Scale to smaller unit
        self.assertEqual(f(ntp.util.UNIT_S, -1), ntp.util.UNIT_MS)
        # Scale to larger unit
        self.assertEqual(f(ntp.util.UNIT_S, 1), ntp.util.UNIT_KS)
        # Scale smaller, outside of range
        self.assertEqual(f(ntp.util.UNIT_S, -10), None)
        # Scale larger, outside of range
        self.assertEqual(f(ntp.util.UNIT_NS, 10), None)
        # Get base unit
        self.assertEqual(f(ntp.util.UNIT_KS, None), ntp.util.UNIT_NS)
        # Different unitgroup
        self.assertEqual(f(ntp.util.UNIT_PPM, -1), ntp.util.UNIT_PPB)

    def test_formatzero(self):
        f = ntp.util.formatzero

        # No scaling possible
        self.assertEqual(f("0.00"), ("0.00", 0))
        # Scaling possible
        self.assertEqual(f("0.0000000"), ("0.0", -2))
        # Scaling without a remainder
        self.assertEqual(f("0.000"), ("0", -1))

    def test_unitifyvar(self):
        f = ntp.util.unitifyvar

        # Second var
        self.assertEqual(f("1.234", "tai"), "  1.234s")
        # Millisecond var
        self.assertEqual(f("1.234", "offset"), " 1.234ms")
        # PPM var
        self.assertEqual(f("1.234", "frequency"), "1.234ppm")
        # No unit type
        self.assertEqual(f("1.234", "blahblahblah"), "1.234")

    def test_f8dot4(self):
        f = ntp.util.f8dot4

        # Test signal
        self.assertEqual(f("foo"), "     foo")
        # Test bad type
        self.assertEqual(f({"food": "problematic"}), "       X")
        # Test not a number
        self.assertEqual(f(float("nan")), "     nan")
        # Test Positives
        # Test xxxxxxxx
        self.assertEqual(f(12345678.9), "12345678")
        # Test xxxxxx.x
        self.assertEqual(f(123456.789), "123456.8")
        # Test xxxxx.xx
        self.assertEqual(f(12345.6789), "12345.68")
        # Test xxxx.xxx
        self.assertEqual(f(1234.56789), "1234.568")
        # Test xxx.xxxx
        self.assertEqual(f(123.456789), "123.4568")
        # Test Negatives
        # Test -xxxxxxx
        self.assertEqual(f(-1234567.89), "-1234567")
        # Test -xxxxx.x
        self.assertEqual(f(-12345.6789), "-12345.7")
        # Test -xxxx.xx
        self.assertEqual(f(-1234.56789), "-1234.57")
        # Test -xxx.xxx
        self.assertEqual(f(-123.456789), "-123.457")
        # Test -xx.xxxx
        self.assertEqual(f(-12.3456789), "-12.3457")

    def test_f8dot3(self):
        f = ntp.util.f8dot3

        # Test signal
        self.assertEqual(f("foo"), "     foo")
        # Test bad type
        self.assertEqual(f({"food": "problematic"}), "       X")
        # Test not a number
        self.assertEqual(f(float("nan")), "     nan")
        # Test Positives
        # Test xxxxxxxx
        self.assertEqual(f(12345678.9), "12345678")
        # Test xxxxxx.x
        self.assertEqual(f(123456.789), "123456.8")
        # Test xxxxx.xx
        self.assertEqual(f(12345.6789), "12345.68")
        # Test xxxx.xxx
        self.assertEqual(f(1234.56789), "1234.568")
        # Test Negatives
        # Test -xxxxxxx
        self.assertEqual(f(-1234567.89), "-1234567")
        # Test -xxxxx.x
        self.assertEqual(f(-12345.6789), "-12345.7")
        # Test -xxxx.xx
        self.assertEqual(f(-1234.56789), "-1234.57")
        # Test -xxx.xxx
        self.assertEqual(f(-123.456789), "-123.457")

    def test_Cache(self):
        c = ntp.util.Cache

        monodata = []

        def monoclock_jig():
            return monodata.pop(0)

        # Test init
        cls = c()
        self.assertEqual(cls._cache, {})
        try:
            monotemp = ntp.util.monoclock
            ntp.util.monoclock = monoclock_jig
            # Test set
            monodata = [5, 10, 315, 20]
            cls.set("foo", 42)
            cls.set("bar", 23)
            self.assertEqual(cls._cache, {"foo": (42, 5, 300),
                                          "bar": (23, 10, 300)})
            self.assertEqual(monodata, [315, 20])
            # Test get, expired
            result = cls.get("foo")
            self.assertEqual(result, None)
            self.assertEqual(monodata, [20])
            self.assertEqual(cls._cache, {"bar": (23, 10, 300)})
            # Test get, valid
            result = cls.get("bar")
            self.assertEqual(result, 23)
            self.assertEqual(monodata, [])
            self.assertEqual(cls._cache, {"bar": (23, 10, 300)})
            # Test set, custom TTL
            monodata = [0, 0, 11, 15]
            cls.set("foo", 42, 10)
            cls.set("bar", 23, 20)
            self.assertEqual(cls._cache, {"foo": (42, 0, 10),
                                          "bar": (23, 0, 20)})
            self.assertEqual(monodata, [11, 15])
            # Test get, expired, custom TTL
            result = cls.get("foo")
            self.assertEqual(result, None)
            self.assertEqual(monodata, [15])
            self.assertEqual(cls._cache, {"bar": (23, 0, 20)})
            # Test get, valid, custom TTL
            result = cls.get("bar")
            self.assertEqual(result, 23)
            self.assertEqual(monodata, [])
            self.assertEqual(cls._cache, {"bar": (23, 0, 20)})
        finally:
            ntp.util.monoclock = monotemp

    def test_canonicalize_dns(self):
        f = ntp.util.canonicalize_dns

        fakesockmod = jigs.SocketModuleJig()
        mycache = ntp.util.Cache()
        mycache.set("foo", "bar")
        try:
            cachetemp = ntp.util.canonicalization_cache
            ntp.util.canonicalization_cache = mycache
            sockettemp = ntp.util.socket
            ntp.util.socket = fakesockmod
            # Test cache hit
            self.assertEqual(f("foo"), "bar")
            self.assertEqual(fakesockmod.gai_calls, [])
            # Test addrinfo fail
            fakesockmod.__init__()
            fakesockmod.gai_error_count = 1
            self.assertEqual(f("nothing"), "DNSFAIL:nothing")
            self.assertEqual(fakesockmod.gai_calls,
                             [("nothing", None, 0, 0, 0,
                               socket.AI_CANONNAME)])
            self.assertEqual(fakesockmod.gni_calls, [])
            # Test nameinfo fail
            fakesockmod.__init__()
            fakesockmod.gni_error_count = 1
            fakesockmod.gni_returns = [("www.Hastur.invalid", 42)]
            fakesockmod.gai_returns = [(("family", "socktype", "proto",
                                         "san.Hastur.invalid",
                                         "42.23.%$.(#"),)]
            self.assertEqual(f("bar:42"), "san.hastur.invalid:42")
            # Test nameinfo fail, no canonname
            fakesockmod.__init__()
            mycache.__init__()
            fakesockmod.gni_error_count = 1
            fakesockmod.gni_returns = [("www.Hastur.invalid", 42)]
            fakesockmod.gai_returns = [(("family", "socktype", "proto",
                                         None, "42.23.%$.(#"),)]
            self.assertEqual(f("bar:42"), "bar:42")
            # Test success
            fakesockmod.__init__()
            mycache.__init__()
            fakesockmod.gni_returns = [("www.Hastur.invalid", 42)]
            fakesockmod.gai_returns = [(("family", "socktype", "proto",
                                         None, "42.23.%$.(#"),)]
            self.assertEqual(f("bar:42"), "www.hastur.invalid:42")
        finally:
            ntp.util.canonicalization_cache = cachetemp
            ntp.util.socket = sockettemp

    def test_termsize(self):
        f = ntp.util.termsize

        fakeosmod = jigs.OSModuleJig()
        fakefcntlmod = jigs.FcntlModuleJig()
        fakeshutilmod = jigs.ShutilModuleJig()
        try:
            ostemp = ntp.util.os
            ntp.util.os = fakeosmod
            # Test default
            fakeosmod.isatty_returns = [False]
            self.assertEqual(f(), (80, 24))
            self.assertEqual(fakeosmod.isatty_calls, [1])
            # termsize takes different code paths for different
            # versions of Python
            if str is not bytes:
                # Python 3 version
                try:
                    shutiltemp = ntp.util.shutil
                    ntp.util.shutil = fakeshutilmod
                    fakeosmod.isatty_returns = [True]
                    fakeshutilmod.gts_returns = [(42, 23)]
                    self.assertEqual(f(), (42, 23))
                finally:
                    ntp.util.shutil = shutiltemp
            else:
                # Python 2.x version
                try:
                    import fcntl
                    fcntltemp = fcntl
                    ntp.util.fcntl = fakefcntlmod
                    fakeosmod.isatty_returns = [True]
                    data = ["\x11\x11\x22\x22\x33\x33\x44\x44"]
                    fakefcntlmod.ioctl_returns = data
                    self.assertEqual(f(), (0x2222, 0x1111))
                    self.assertEqual(fakefcntlmod.ioctl_calls,
                                     [(2, ntp.util.termios.TIOCGWINSZ,
                                       "\0\0\0\0\0\0\0\0", False)])
                finally:
                    ntp.util.fcntl = fcntltemp
        finally:
            ntp.util.os = ostemp

    def test_PeerStatusWord(self):
        c = ntp.util.PeerStatusWord

        # Test blank status
        cls = c(0)
        self.assertEqual(cls.event, 0)
        self.assertEqual(cls.event_count, 0)
        self.assertEqual(cls.conf, "no")
        self.assertEqual(cls.reach, "no")
        self.assertEqual(cls.auth, "none")
        self.assertEqual(cls.condition, "reject")
        self.assertEqual(cls.last_event, "")
        # Test max status
        cls = c(0xFFFF)
        self.assertEqual(cls.event, 15)
        self.assertEqual(cls.event_count, 15)
        self.assertEqual(cls.conf, "yes")
        self.assertEqual(cls.reach, "none")
        self.assertEqual(cls.auth, "ok ")
        self.assertEqual(cls.condition, "pps.peer")
        self.assertEqual(cls.last_event, "")
        # Test __str__ of max status
        self.assertEqual(str(cls),
                         "conf=yes, reach=none, auth=ok , "
                         "cond=pps.peer, event= ec=15")
        # Test third options
        cls = c(0x57FF)
        self.assertEqual(cls.event, 15)
        self.assertEqual(cls.event_count, 15)
        self.assertEqual(cls.conf, "no")
        self.assertEqual(cls.reach, "yes")
        self.assertEqual(cls.auth, "bad")
        self.assertEqual(cls.condition, "pps.peer")
        self.assertEqual(cls.last_event, "")
        # Test all newer than OLDVERSION conditions
        cls = c(0x0000)
        self.assertEqual(cls.condition, "reject")
        cls = c(0x0100)
        self.assertEqual(cls.condition, "falsetick")
        cls = c(0x0200)
        self.assertEqual(cls.condition, "excess")
        cls = c(0x0300)
        self.assertEqual(cls.condition, "outlier")
        cls = c(0x0400)
        self.assertEqual(cls.condition, "candidate")
        cls = c(0x0500)
        self.assertEqual(cls.condition, "backup")
        cls = c(0x0600)
        self.assertEqual(cls.condition, "sys.peer")
        cls = c(0x0700)
        self.assertEqual(cls.condition, "pps.peer")
        # Test all older than OLDVERSION conditions
        cls = c(0xF400, 0)
        self.assertEqual(cls.condition, "insane")
        cls = c(0xF800, 0)
        self.assertEqual(cls.condition, "hi_disp")
        cls = c(0xFC00, 0)
        self.assertEqual(cls.condition, "")
        cls = c(0xF100, 0)
        self.assertEqual(cls.condition, "sel_cand")
        cls = c(0xF200, 0)
        self.assertEqual(cls.condition, "sync_cand")
        cls = c(0xF300, 0)
        self.assertEqual(cls.condition, "sys_peer")
        # Test all last events
        cls = c(0xFFF1)
        self.assertEqual(cls.last_event, "mobilize")
        cls = c(0xFFF2)
        self.assertEqual(cls.last_event, "demobilize")
        cls = c(0xFFF3)
        self.assertEqual(cls.last_event, "unreachable")
        cls = c(0xFFF4)
        self.assertEqual(cls.last_event, "reachable")
        cls = c(0xFFF5)
        self.assertEqual(cls.last_event, "restart")
        cls = c(0xFFF6)
        self.assertEqual(cls.last_event, "no_reply")
        cls = c(0xFFF7)
        self.assertEqual(cls.last_event, "rate_exceeded")
        cls = c(0xFFF8)
        self.assertEqual(cls.last_event, "access_denied")
        cls = c(0xFFF9)
        self.assertEqual(cls.last_event, "leap_armed")
        cls = c(0xFFFA)
        self.assertEqual(cls.last_event, "sys_peer")
        cls = c(0xFFFB)
        self.assertEqual(cls.last_event, "clock_alarm")

    def test_cook(self):
        f = ntp.util.cook
        od = ntp.util.OrderedDict

        termsize = (80, 24)

        def termsize_jig():
            return ntp.util.TermSize(*termsize)

        try:
            termtemp = ntp.util.termsize
            ntp.util.termsize = termsize_jig
            # Test empty
            self.assertEqual(f(od()), "\n")
            # Test prettydates
            data = od((("reftime",
                        ("0x00000000.00000000", "0x00000000.00000000")),
                       ("clock",
                        ("0x10000000.00000000", "0x10000000.00000000")),
                       ("org",
                        ("0x20000000.00000000", "0x20000000.00000000")),
                       ("rec",
                        ("0x30000000.00000000", "0x30000000.00000000")),
                       ("xmt",
                        ("0x40000000.00000000", "0x40000000.00000000"))))
            self.assertEqual(f(data),
                             "reftime=00000000.00000000 "
                             "2036-02-07T06:28:16.000Z,\n"
                             "clock=10000000.00000000 "
                             "1908-07-04T21:24:16.000Z,\n"
                             "org=20000000.00000000 "
                             "1917-01-05T18:48:32.000Z,\n"
                             "rec=30000000.00000000 "
                             "1925-07-09T16:12:48.000Z,\n"
                             "xmt=40000000.00000000 "
                             "1934-01-10T13:37:04.000Z\n")
            # Test prettydates, with units
            self.assertEqual(f(data, showunits=True),
                             "reftime=00000000.00000000 "
                             "2036-02-07T06:28:16.000Z,\n"
                             "clock=10000000.00000000 "
                             "1908-07-04T21:24:16.000Z,\n"
                             "org=20000000.00000000 "
                             "1917-01-05T18:48:32.000Z,\n"
                             "rec=30000000.00000000 "
                             "1925-07-09T16:12:48.000Z,\n"
                             "xmt=40000000.00000000 "
                             "1934-01-10T13:37:04.000Z\n")
            # Test wide terminal
            termsize = (160, 24)
            self.assertEqual(f(data),
                             "reftime=00000000.00000000 "
                             "2036-02-07T06:28:16.000Z, "
                             "clock=10000000.00000000 "
                             "1908-07-04T21:24:16.000Z, "
                             "org=20000000.00000000 "
                             "1917-01-05T18:48:32.000Z,\n"
                             "rec=30000000.00000000 "
                             "1925-07-09T16:12:48.000Z, "
                             "xmt=40000000.00000000 "
                             "1934-01-10T13:37:04.000Z\n")
            # Test narrow terminal
            termsize = (40, 24)
            self.assertEqual(f(data),
                             "\nreftime=00000000.00000000 "
                             "2036-02-07T06:28:16.000Z,\n"
                             "clock=10000000.00000000 "
                             "1908-07-04T21:24:16.000Z,\n"
                             "org=20000000.00000000 "
                             "1917-01-05T18:48:32.000Z,\n"
                             "rec=30000000.00000000 "
                             "1925-07-09T16:12:48.000Z,\n"
                             "xmt=40000000.00000000 "
                             "1934-01-10T13:37:04.000Z\n")
            termsize = (80, 24)
            # Test ex-obscure cooking
            data = od((("srcadr", ("1.1.1.1", "1.1.1.1")),
                       ("peeradr", ("2.2.2.2", "2.2.2.2")),
                       ("dstadr", ("3.3.3.3", "3.3.3.3")),
                       ("refid", ("blah", "blah"))))
            self.assertEqual(f(data),
                             "srcadr=1.1.1.1, peeradr=2.2.2.2, "
                             "dstadr=3.3.3.3, refid=blah\n")
            # Test ex-obscure cooking
            self.assertEqual(f(data, showunits=True),
                             "srcadr=1.1.1.1, peeradr=2.2.2.2, "
                             "dstadr=3.3.3.3, refid=blah\n")
            # Test leap
            self.assertEqual(f(od((("leap", (0, "0")),)), showunits=True),
                             "leap=00\n")
            self.assertEqual(f(od((("leap", (1, "1")),)), showunits=True),
                             "leap=01\n")
            self.assertEqual(f(od((("leap", (2, "2")),)), showunits=True),
                             "leap=10\n")
            self.assertEqual(f(od((("leap", (3, "3")),)), showunits=True),
                             "leap=11\n")
            # Test leap
            self.assertEqual(f(od((("leap", (0, "0")),))), "leap=00\n")
            self.assertEqual(f(od((("leap", (1, "1")),))), "leap=01\n")
            self.assertEqual(f(od((("leap", (2, "2")),))), "leap=10\n")
            self.assertEqual(f(od((("leap", (3, "3")),))), "leap=11\n")
            # Test leap, with units
            self.assertEqual(f(od((("leap", (0, "0")),)), showunits=True),
                             "leap=00\n")
            self.assertEqual(f(od((("leap", (1, "1")),)), showunits=True),
                             "leap=01\n")
            self.assertEqual(f(od((("leap", (2, "2")),)), showunits=True),
                             "leap=10\n")
            self.assertEqual(f(od((("leap", (3, "3")),)), showunits=True),
                             "leap=11\n")
            # Test reach
            self.assertEqual(f(od((("reach", (1, "1")),))), "reach=001\n")
            # Test reach, with units
            self.assertEqual(f(od((("reach", (1, "1")),)), showunits=True),
                             "reach=001\n")
            # Test specials
            data = od((("filtdelay", ("1 2 3", "1 2 3")),
                       ("filtoffset", ("2 3 4", "2 3 4")),
                       ("filtdisp", ("3 4 5", "3 4 5")),
                       ("filterror", ("4 5 6", "4 5 6"))))
            self.assertEqual(f(data),
                             " filtdelay =1\t2\t3, filtoffset =2\t3\t4,   "
                             "filtdisp =3\t4\t5,  filterror =4\t5\t6\n")
            # Test specials, with units
            self.assertEqual(f(data, showunits=True),
                             " filtdelay =      1       2       3 ms,\n"
                             "filtoffset =      2       3       4 ms,\n"
                             "  filtdisp =      3       4       5 ms,\n"
                             " filterror =      4       5       6 ms\n")
            # Test flash null
            self.assertEqual(f(od((("flash", (0, "0")),))), "flash=00 ok\n")
            # Test flash all bits
            self.assertEqual(f(od((("flash", (65535, "65535")),))),
                             "\nflash=ffff pkt_dup pkt_bogus pkt_unsync "
                             "pkt_denied pkt_auth pkt_stratum pkt_header "
                             "pkt_autokey pkt_crypto peer_stratum "
                             "peer_dist peer_loop peer_unreach\n")
            # Test MS_VARS
            data = od((("rootdelay", (0, "0")), ("rootdisp", (1, "1")),
                       ("offset", (2, "2")), ("sys_jitter", (3, "3")),
                       ("clk_jitter", (4, "4")), ("leapsmearoffset", (5, "5")),
                       ("authdelay", (6, "6")), ("koffset", (7, "7")),
                       ("kmaxerr", (8, "8")), ("kesterr", (9, "9")),
                       ("kprecis", (10, "10")), ("kppsjitter", (11, "11")),
                       ("fuzz", (12, "12")),
                       ("clk_wander_threshold", (13, "13")),
                       ("tick", (14, "14")), ("in", (15, "15")),
                       ("out", (16, "16")), ("bias", (17, "17")),
                       ("delay", (18, "18")), ("jitter", (19, "19")),
                       ("dispersion", (20, "20")), ("fudgetime1", (21, "21")),
                       ("fudgetime2", (21, "21"))))
            self.assertEqual(f(data), "rootdelay=0, rootdisp=1, offset=2, "
                             "sys_jitter=3, clk_jitter=4,\nleapsmearoffset=5, "
                             "authdelay=6, koffset=7, kmaxerr=8, kesterr=9, "
                             "kprecis=10,\nkppsjitter=11, fuzz=12, "
                             "clk_wander_threshold=13, tick=14, in=15, "
                             "out=16,\nbias=17, delay=18, jitter=19, "
                             "dispersion=20, fudgetime1=21, fudgetime2=21\n")
            # Test MS_VARS, with units
            self.assertEqual(f(data, showunits=True),
                             "rootdelay=0ms, rootdisp=1ms, offset=2ms, "
                             "sys_jitter=3ms, clk_jitter=4ms,\n"
                             "leapsmearoffset=5ms, authdelay=6ms, "
                             "koffset=7ms, kmaxerr=8ms, kesterr=9ms,\n"
                             "kprecis=10ms, kppsjitter=11ms, fuzz=12ms, "
                             "clk_wander_threshold=13ms,\ntick=14ms, in=15ms, "
                             "out=16ms, bias=17ms, delay=18ms, jitter=19ms,\n"
                             "dispersion=20ms, fudgetime1=21ms, "
                             "fudgetime2=21ms\n")
            # Test S_VARS
            data = od((("tai", (0, "0")), ("poll", (1, "1"))))
            self.assertEqual(f(data), "tai=0, poll=1\n")
            # Test S_VARS, with units
            self.assertEqual(f(data, showunits=True), "tai=0s, poll=1s\n")
            # Test PPM_VARS
            data = od((("frequency", (0, "0")), ("clk_wander", (1, "1"))))
            self.assertEqual(f(data), "frequency=0, clk_wander=1\n")
            # Test PPM_VARS, with units
            self.assertEqual(f(data, showunits=True),
                             "frequency=0ppm, clk_wander=1ppm\n")
            # Test unrecognized variable
            data = od((("yeahyeah", (1, "1")),))
            self.assertEqual(f(data), "yeahyeah=1\n")
        finally:
            ntp.util.termsize = termtemp

    def test_MRUSummary(self):
        c = ntp.util.MRUSummary
        e = ntp.packet.MRUEntry

        fakesockmod = jigs.SocketModuleJig()
        mycache = ntp.util.Cache()

        cdns_jig_calls = []
        cdns_jig_returns = []

        def cdns_jig(ip):  # canonicalize_dns()
            cdns_jig_calls.append(ip)
            return cdns_jig_returns.pop(0)

        # Test init
        cls = c(False, debug=3, logfp=sys.stderr)
        self.assertEqual(cls.debug, 3)
        self.assertEqual(cls.logfp, sys.stderr)
        self.assertEqual(cls.now, None)
        self.assertEqual(cls.showhostnames, False)
        self.assertEqual(cls.wideremote, False)
        try:
            socktemp = ntp.util.socket
            ntp.util.socket = fakesockmod
            cachetemp = ntp.util.canonicalization_cache
            ntp.util.canonicalization_cache = mycache
            cdnstemp = ntp.util.canonicalize_dns
            ntp.util.canonicalize_dns = cdns_jig
            # Test summary, first options
            ent = e()
            ent.first = "0x00000000.00000000"
            ent.last = "0xFFFFFFFF.FFFFFFFF"
            ent.ct = 1
            ent.rs = ntp.magic.RES_KOD
            ent.addr = "1.2.3.4:42"
            ent.mv = 0x17
            fakesockmod.gai_returns = [("fam", "type", "proto",
                                        "foo.bar.com", "1.2.3.4")]
            self.assertEqual(cls.summary(ent),
                             "64730 23296      0  400 K 7 2"
                             "      1    42 1.2.3.4")
            # Test summary, second options
            mycache._cache = {}
            cls.now = 0x00000000
            cls.showhostnames = True
            ent.first = "0x00000100.00000000"
            ent.last = "0x00000200.00000000"
            ent.ct = 65
            ent.rs = ntp.magic.RES_LIMITED
            fakesockmod.gai_returns = [[("fam", "type", "proto",
                                         "foo.bar.com", ("1.2.3.4", 42))]]
            cdns_jig_returns = ["foo.com"]
            self.assertEqual(cls.summary(ent),
                             "64730 23808   4.00   20 L 7 2     65"
                             "    42 foo.com")
            # Test summary, third options
            mycache._cache = {}
            ent.ct = 2
            ent.rs = 0
            fakesockmod.gai_error_count = 1
            cdns_jig_returns = ["foobarbaz" * 5]  # 45 chars, will be cropped
            self.assertEqual(cls.summary(ent),
                             "64730 23808    256    0 . 7 2      2    42"
                             " 1.2.3.4 (foobarbazfoobarbazfoobarbazfoob")
            # Test summary, wide
            mycache._cache = {}
            cls.wideremote = True
            fakesockmod.gai_error_count = 1
            cdns_jig_returns = ["foobarbaz" * 5]  # 45 chars, will be cropped
            self.assertEqual(cls.summary(ent),
                             "64730 23808    256    0 . 7 2      2"
                             "    42 1.2.3.4 "
                             "(foobarbazfoobarbazfoobarbazfoobarbazfoobarbaz)")
        finally:
            ntp.util.socket = socktemp
            ntp.util.canonicalization_cache = cachetemp
            ntp.util.canonicalize_dns = cdnstemp

    def test_ReslistSummary(self):
        c = ntp.util.ReslistSummary
        m = ntp.util.ReslistSummary._ReslistSummary__getPrefix

        # Test __getPrefix
        #   Test empty
        self.assertEqual(m(""), "/0")
        #   Test base 16
        self.assertEqual(m("FF:FF:F0:00"), "/20")
        #   Test base 10
        self.assertEqual(m("255.240.0.0"), "/12")
        # Test summary
        # Test with everything
        cls = c()
        data = {"hits": "blah", "addr": "42.23.1.2",
                "mask": "FF:FF:0:0", "flags": "qwerty"}
        self.assertEqual(cls.summary(data),
                         "      blah 42.23.1.2/16\n           qwerty\n")
        # Test with missing data
        data = {"addr": "42.23.1.2", "mask": "FF:FF:0:0"}
        self.assertEqual(cls.summary(data), "")

    def test_IfstatsSummary(self):
        c = ntp.util.IfstatsSummary
        od = ntp.util.OrderedDict

        cls = c()
        # Test with all variables available
        data = od((("addr", "1.2.3.4"), ("bcast", "foo"),
                   ("name", "Namus Maximus"), ("en", True), ("flags", 0xFACE),
                   ("rx", 12), ("tx", 34), ("txerr", 56), ("pc", 78),
                   ("up", 90)))
        self.assertEqual(cls.summary(1, data),
                         "  1 Namus Maximus            . face     12     34"
                         "     56    78       90\n    1.2.3.4\n    foo\n")
        # Test without bcast
        data = od((("addr", "1.2.3.4"), ("name", "Namus Maximus"),
                   ("en", True), ("flags", 0xFACE), ("rx", 12), ("tx", 34),
                   ("txerr", 56), ("pc", 78), ("up", 90)))
        self.assertEqual(cls.summary(1, data),
                         "  1 Namus Maximus            . face     12     34"
                         "     56    78       90\n    1.2.3.4\n")
        # Test with missing data
        self.assertEqual(cls.summary(1, od()), "")


class TestPeerSummary(unittest.TestCase):
    target = ntp.util.PeerSummary

    def test___init__(self):
        cls = self.target("peers", 4, True, False, logfp=sys.stderr)
        self.assertEqual(cls.displaymode, "peers")
        self.assertEqual(cls.pktversion, 4)
        self.assertEqual(cls.showhostnames, True)
        self.assertEqual(cls.showunits, False)
        self.assertEqual(cls.wideremote, False)
        self.assertEqual(cls.debug, 0)
        self.assertEqual(cls.logfp, sys.stderr)
        self.assertEqual(cls.termwidth, None)
        self.assertEqual(cls.horizontal_slack, 0)
        self.assertEqual(cls.namewidth, 15)
        self.assertEqual(cls.refidwidth, 15)
        self.assertEqual(cls._PeerSummary__remote, "     remote    ")
        self.assertEqual(cls._PeerSummary__common,
                         "st t when poll reach   delay   offset   ")
        self.assertEqual(cls._PeerSummary__header, None)
        self.assertEqual(cls.polls, [])

    def test_prettyinterval(self):
        m = self.target.prettyinterval

        # Test invalid
        self.assertEqual(m("Failure"), "-")
        # Test <=2048
        self.assertEqual(m(2048), "2048")
        # Test <=300
        self.assertEqual(m(17971), "300m")
        # Test <=96
        self.assertEqual(m(343831), "96h")
        # Test final
        self.assertEqual(m(350000), "4d")

    def test_high_truncate(self):
        m = self.target.high_truncate

        # Test fit
        self.assertEqual(m("foo", 10), "foo")
        # Test doesn't fit
        self.assertEqual(m("lorem ipsum", 5), "-psum")

    def test_is_clock(self):
        m = self.target.is_clock

        # Test True
        data = {"srchost": "(would)"}
        self.assertEqual(m(data), True)
        # Test False, not a clock
        data = {"srchost": "wouldn't"}
        self.assertEqual(m(data), False)
        # Test False, no host
        data = {"blah": "definitely wouldn't"}
        self.assertEqual(m(data), False)

    def test_header(self):
        # Test peers
        cls = self.target("peers", 4, True, False)
        self.assertEqual(cls.header(),
                         "     remote           refid      st t when "
                         "poll reach   delay   offset   jitter")
        # Test opeers
        cls = self.target("opeers", 4, True, False)
        self.assertEqual(cls.header(),
                         "     remote           local      st t when "
                         "poll reach   delay   offset     disp")
        # Test apeers
        cls = self.target("apeers", 4, True, False)
        self.assertEqual(cls.header(),
                         "     remote       refid   assid  st t when "
                         "poll reach   delay   offset   jitter")

    def test_width(self):
        cls = self.target("peers", 4, True, False)
        self.assertEqual(cls.width(), 79)
        cls.horizontal_slack = 10
        self.assertEqual(cls.width(), 89)

    def test_summary(self):
        cls = self.target("peers", 4, True, False)
        cls.header()

        cdns_jig_calls = []
        cdns_jig_returns = []

        def cdns_jig(ip):  # canonicalize_dns()
            cdns_jig_calls.append(ip)
            return cdns_jig_returns.pop(0)

        data = ntp.util.OrderedDict((("delay", (1.234567, "1.234567")),
                                     ("dstadr", ("1.2.3.4", "1.2.3.4")),
                                     ("dstport", ("blah0", "blah0")),
                                     ("filtdelay", ("blah1", "blah1")),
                                     ("filtdisp", ("blah2", "blah2")),
                                     ("filtoffset", ("blah3", "blah3")),
                                     ("flash", ("blah4", "blah4")),
                                     ("headway", ("blah5", "blah5")),
                                     ("hmode", (6, "6")),
                                     ("hpoll", (12, "12")),
                                     ("jitter", (3.14159, "3.14159")),
                                     ("keyid", ("blah6", "blah6")),
                                     ("leap", ("blah7", "blah7")),
                                     ("offset", (2.71828, "2.71828")),
                                     ("pmode", ("blah8", "blah8")),
                                     ("ppoll", (5, "5")),
                                     ("precision", ("blah9", "blah9")),
                                     ("reach", (500, "500")),
                                     ("refid", ("FAIL", "FAIL")),
                                     ("rec", ("0x00000000.00000000",
                                              "0x00000000.00000000")),
                                     ("reftime", ("0x00001000.00000000",
                                                  "0x00001000.00000000")),
                                     ("rootdelay", ("blah10", "blah10")),
                                     ("rootdisp", (299792.458, "299792.458")),
                                     ("srcadr", ("10.20.30.40",
                                                 "10.20.30.40")),
                                     ("srchost", ("15.25.35.45",
                                                  "15.25.35.45")),
                                     ("scrport", ("blah11", "blah11")),
                                     ("stratum", (8, "8")),
                                     ("ttl", ("blah12", "blah12")),
                                     ("unreach", ("blah13", "blah13")),
                                     ("xmt", ("blah14", "blah14")),
                                     ("randomness!", ("foo!", "bar!"))))
        faketimemod = jigs.TimeModuleJig()
        try:
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            cdnstemp = ntp.util.canonicalize_dns
            ntp.util.canonicalize_dns = cdns_jig
            # Test, no units, hmode=BCLIENTX, peers
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 b 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=BROADCAST, not multicast
            data["hmode"] = (5, "5")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 B 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=BROADCAST, not multicast
            data["srcadr"] = ("224.2.3.4", "224.2.3.4")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 M 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=CLIENT, local refclock
            data["srcadr"] = ("10.20.30.40", "10.20.30.40")
            data["hmode"] = (3, "3")
            data["srchost"] = ("(blah)", "(blah)")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 l 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=CLIENT, pool
            data["srchost"] = ("15.25.35.45", "15.25.35.45")
            data["refid"] = ("POOL", "POOL")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .POOL.           8 p 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=CLIENT, manycast client
            data["srcadr"] = ("224.2.3.4", "224.2.3.4")
            data["refid"] = ("FAIL", "FAIL")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 a 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=CLIENT, unicast
            data["srcadr"] = ("10.20.30.40", "10.20.30.40")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 u 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=ACTIVE
            data["hmode"] = (1, "1")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 s 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, hmode=PASSIVE
            data["hmode"] = (2, "2")
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 S 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, don't show hostnames
            cls.showhostnames = False
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#10.20.30.40     .FAIL.           8 S 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, name crop
            cls.showhostnames = True
            cdns_jig_returns = ["clock_canon_blah_jabber_quantum"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon_bla .FAIL.           8 S 6926"
                             "   32  764   1.2346   2.7183   3.1416\n")
            # Test, no units, no name crop
            cls.wideremote = True
            cdns_jig_returns = ["clock_canon_blah_jabber_quantum"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon_blah_jabber_quantum\n"
                             "                 .FAIL.           8 S"
                             " 6926   32  764   1.2346   2.7183   3.1416\n")
            # Test, with units
            cls.showunits = True
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.           8 S 6926"
                             "   32  764 1.2346ms 2.7183ms 3.1416ms\n")
            # Test, low precision formatting with units
            lowpdata = data.copy()
            lowpdata["delay"] = (1.234, "1.234")
            cls.showunits = True
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, lowpdata, 12345),
                             "#clock_canon     .FAIL.           8 S 6926"
                             "   32  764  1.234ms 2.7183ms 3.1416ms\n")
            # Test, low precision formatting with units
            cls.showunits = False
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, lowpdata, 12345),
                             "#clock_canon     .FAIL.           8 S 6926"
                             "   32  764    1.234    2.718    3.142\n")
            # Test, apeers
            cls.showunits = True
            cls.displaymode = "apeers"
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            self.assertEqual(cls.summary(0x500, data, 12345),
                             "#clock_canon     .FAIL.   12345   8 S 6926"
                             "   32  764 1.2346ms 2.7183ms 3.1416ms\n")
            # Test rstatus, previous version
            cls.showunits = True
            cls.displaymode = "apeers"
            cdns_jig_returns = ["clock_canon"]
            faketimemod.time_returns = [0xA0000000]
            cls.pktversion = 0
            self.assertEqual(cls.summary(0x300, data, 12345),
                             "*clock_canon     .FAIL.   12345   8 S 6926"
                             "   32  764 1.2346ms 2.7183ms 3.1416ms\n")
        finally:
            ntp.util.time = timetemp
            ntp.util.canonicalize_dns = cdnstemp

    def test_intervals(self):
        cls = self.target("peers", 4, True, False)
        cls.polls = [1, 2, 3, 4, 5]
        self.assertEqual(cls.intervals(), [1, 2, 3, 4, 5])
        self.assertEqual(cls.polls, [])


if __name__ == '__main__':
    unittest.main()
