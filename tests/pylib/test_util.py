#!/usr/bin/env python
# coding: utf-8

import unittest
import ntp.util


class TestPylibUtilMethods(unittest.TestCase):

    def test_rfc3339(self):
        self.assertEqual(ntp.util.rfc3339(1480999786),
                         '2016-12-06T04:49:46Z')
        self.assertEqual(ntp.util.rfc3339(1480999786.5),
                         '2016-12-06T04:49:46.5Z')
        # RFC 3339, 2 digits of seconds.
        # we round, but the spec is silent on rounding
        self.assertEqual(ntp.util.rfc3339(1480999786.025),
                         '2016-12-06T04:49:46.03Z')

    def test_portsplit(self):
        self.assertEqual(ntp.util.portsplit("host.invalid"),
                         ("host.invalid", ""))
        self.assertEqual(ntp.util.portsplit("host.invalid:123"),
                         ("host.invalid", ":123"))
        self.assertEqual(ntp.util.portsplit(
            "[0000:1111:2222:3333:4444:5555:6666:7777]:123"),
            ("0000:1111:2222:3333:4444:5555:6666:7777", ":123"))

    def test_rescaleunit(self):
        self.assertEqual(ntp.util.rescaleunit(1.0, 0),
                         1.0)
        self.assertEqual(ntp.util.rescaleunit(10.0, 1),
                         10000.0)
        self.assertEqual(ntp.util.rescaleunit(0.1, 1),
                         100.0)
        self.assertEqual(ntp.util.rescaleunit(10.0, -1),
                         0.01)
        self.assertEqual(ntp.util.rescaleunit(0.1, -1),
                         0.0001)
        self.assertEqual(ntp.util.rescaleunit(-100.0, 1),
                         -100000.0)
        self.assertEqual(ntp.util.rescaleunit(-.001, 1),
                         -1.0)
        self.assertEqual(ntp.util.rescaleunit(-100.0, -1),
                         -0.1)
        self.assertEqual(ntp.util.rescaleunit(-.001, -1),
                         -0.000001)

    def test_scaleforunit(self):
        self.assertEqual(ntp.util.scaleforunit(1.0, 9),
                         (1.0, 0))
        self.assertEqual(ntp.util.scaleforunit(999.2342, 9),
                         (999.2342, 0))
        self.assertEqual(ntp.util.scaleforunit(5042.7, 9),
                         (5.0427, 1))
        self.assertEqual(ntp.util.scaleforunit(0.1912, 9),
                         (191.2, -1))
        self.assertEqual(ntp.util.scaleforunit(0.00000042, 9),
                         (420.0, -3))
        self.assertEqual(ntp.util.scaleforunit(-1.0, 9),
                         (-1.0, 0))
        self.assertEqual(ntp.util.scaleforunit(-999.2342, 9),
                         (-999.2342, 0))
        self.assertEqual(ntp.util.scaleforunit(-5042.7, 9),
                         (-5.0427, 1))
        self.assertEqual(ntp.util.scaleforunit(-0.1912, 9),
                         (-191.2, -1))
        self.assertEqual(ntp.util.scaleforunit(-0.00000042, 9),
                         (-420.0, -3))
        self.assertEqual(ntp.util.scaleforunit(1.0000004, 6),
                         (1.0, 0))
        self.assertEqual(ntp.util.scaleforunit(1.0000005, 6),
                         (1.000001, 0))

    def test_oomsbetweenunits(self):
        f = ntp.util.oomsbetweenunits

        self.assertEqual(f(ntp.util.UNIT_KS, ntp.util.UNIT_MS), 6)
        self.assertEqual(f(ntp.util.UNIT_PPM, ntp.util.UNIT_PPB), 3)

    def test_filtcooker(self):
        self.assertEqual(ntp.util.filtcooker(
            "1.02 34.5 0.67835 -23.0 9 6.7 1.0 .1"),
            "1.02000 34.5000 0.67835 -23.000 9.00000 6.70000 1.00000 0.10000 ms"
        )

    def test_formatdigitsplit(self):
        self.assertEqual(ntp.util.formatdigitsplit(10.0, 5, 9),
                         "%5.2f")
        self.assertEqual(ntp.util.formatdigitsplit(100.52, 5, 9),
                         "%5.1f")
        self.assertEqual(ntp.util.formatdigitsplit(10.123456789, 8, 9),
                         "%8.5f")
        self.assertEqual(ntp.util.formatdigitsplit(1.123456789, 8, 9),
                         "%8.6f")
        self.assertEqual(ntp.util.formatdigitsplit(0.123456789, 8, 9),
                         "%8.6f")
        self.assertEqual(ntp.util.formatdigitsplit(1.234, 10, 9),
                         "%10.8f")
        self.assertEqual(ntp.util.formatdigitsplit(-1.23456789, 6, 9),
                         "%6.3f")

    def test_unitformatter(self):
        f = ntp.util.unitformatter
        usec = ntp.util.UNITS_SEC

        sec_t = [
            # 1.0000000005 s rounds to 1s
            [1.0000000005, "1.00000s"],
            # -1.0000000005 s rounds to 1s
            [-1.0000000005, "-1.0000s"],
            # 0.9999999999 s round to 1s
            [0.9999999999, "1.00000s"],
            # 0.00000000049 s rounds to 0ns
            [0.00000000049, "     0ns"],
            # 0.00000000051 s rounds to 1ns
            [0.00000000051, "     1ns"],
        ]

        for t in sec_t:
            self.assertEqual(f(t[0], usec, ntp.util.UNIT_S), t[1])

        # 0.4 ms rounds to 0ns
        self.assertEqual(f(0.0000004, usec, ntp.util.UNIT_MS),
                         "     0ns")
        # 0.5 ms rounds to 1ns
        self.assertEqual(f(0.0000005, usec, ntp.util.UNIT_MS),
                         "     1ns")
        # 0.5 ms rounds to 1ns, with strip
        self.assertEqual(f(0.0000005, usec, ntp.util.UNIT_MS, strip=True),
                         "1ns")
        self.assertEqual(f(12.45, usec, ntp.util.UNIT_MS),
                         "12.450ms")  # Checking normal
        self.assertEqual(f(12.45, usec, ntp.util.UNIT_MS, strip=True),
                         "12.450ms")  # Checking normal, strip
        # 12.499999 ms rounds to 12.45 ms
        self.assertEqual(f(12.499999, usec, ntp.util.UNIT_MS,
                           strip=True, width=6),
                         "12.5ms")  # Checking normal, strip
        # 12.451 ms rounds to 12.45 ms
        self.assertEqual(f(12.451, usec, ntp.util.UNIT_MS,
                           strip=True, width=7),
                         "12.45ms")  # Checking normal, strip
        self.assertEqual(f(123456789.1234, usec, ntp.util.UNIT_MS, width=None),
                         "123.4567891234ks")  # Checking normal, no width
        self.assertEqual(f(0.000005, usec, ntp.util.UNIT_MS),
                         "     5ns")  # Unit == base
        self.assertEqual(f(0.000005, usec, ntp.util.UNIT_MS, strip=True),
                         "5ns")  # Unit == base, strip
        self.assertEqual(f(0.000005, usec, ntp.util.UNIT_MS, width=None),
                         "5ns")  # Unit == base, no width
        self.assertEqual(f(10000.1, usec, ntp.util.UNIT_KS),
                         "10000.1ks")  # Value outside of unit ranges

    def test_scalestring(self):
        f = ntp.util.scalestring

        # Scale all decimals
        self.assertEqual(f("0.042"), ("42", -1))
        # Typical length, positive value, no scaling
        self.assertEqual(f("1.23450"), ("1.23450", 0))
        # Ditto, negative
        self.assertEqual(f("-1.23450"), ("-1.23450", 0))
        # Long, positive value, no scaling
        self.assertEqual(f("1.234567890123456"), ("1.234567890123456", 0))
        # ditto, negative
        self.assertEqual(f("-1.234567890123456"), ("-1.234567890123456", 0))
        # Zero
        self.assertEqual(f("0.000000"), ("0.000000", 0))
        # Negative zero
        self.assertEqual(f("-0.000"), ("0.000", 0))
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
        # Bizzare 1
        self.assertEqual(f("-000.000012345678900987654321"),
                         ("-12.345678900987654321", -2))
        # Bizzare 2
        self.assertEqual(f("1234567890987654321000.00000000000042"),
                           ("1.23456789098765432100000000000000042", 7))
        # Bizzare 3
        self.assertEqual(f("00000000.000000000000"),
                         ("00000000.000000000000", 0))

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
        self.assertEqual(f("1.23456", -2), "1234560")
        # ditto, negative
        self.assertEqual(f("-1.23456", -2), "-1234560")

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
        self.assertEqual(f("0.000", nu.UNIT_MS), "     0ns")
        # Standard, width=8
        self.assertEqual(f("1.234", nu.UNIT_MS), " 1.234ms")
        # ditto, negative
        self.assertEqual(f("-1.234", nu.UNIT_MS), "-1.234ms")
        # Scale to larger unit, width=8
        self.assertEqual(f("1234.5", nu.UNIT_MS), " 1.2345s")
        # ditto, negative
        self.assertEqual(f("-1234.5", nu.UNIT_MS), "-1.2345s")
        # Scale to smaller unit, width=8
        self.assertEqual(f("0.01234", nu.UNIT_MS), u" 12.34\u03bcs")
        # ditto, negative
        self.assertEqual(f("-0.01234", nu.UNIT_MS), u"-12.34\u03bcs")
        # At baseunit
        self.assertEqual(f("12.0", nu.UNIT_NS), "    12ns")
        # Scale to baseunit
        self.assertEqual(f(".042", nu.UNIT_US), "    42ns")
        # Below baseunit
        self.assertEqual(f("23.42", nu.UNIT_NS), "    23ns")
        # Different units
        self.assertEqual(f("12.345", nu.UNIT_PPM), "12.35ppm")
        # Strip
        self.assertEqual(f("1.23", nu.UNIT_MS, strip=True), "1.23ms")
        # Different width
        self.assertEqual(f("1.234", nu.UNIT_MS, width=12), "     1.234ms")
        # Outside of available units
        self.assertEqual(f("1234.5", nu.UNIT_KS), "1234.5ks")
        # Seconds
        self.assertEqual(f("42.23", nu.UNIT_S), "  42.23s")

    def test_stringfiltcooker(self):
        # No scale
        self.assertEqual(ntp.util.stringfiltcooker(
            "1.02 34.5 0.67835 -23.0 9 6.7 1.00 .1"),
            "   1.02    34.5 0.67835   -23.0       9     6.7    1.00     0.1 ms"
        )
        # Scale
        self.assertEqual(ntp.util.stringfiltcooker(
            "1000.02 3400.5 0.67835 -23.0 9001 6.7 1.00 1234"),
            "1.00002  3.4005 0.00068 -0.0230   9.001  0.0067 0.00100   1.234 s"
        )

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

if __name__ == '__main__':
    unittest.main()
