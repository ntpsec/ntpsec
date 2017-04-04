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
        self.assertEqual(ntp.util.oomsbetweenunits(3, 2),
                         3)
        self.assertEqual(ntp.util.oomsbetweenunits(2, 3),
                         3)

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

if __name__ == '__main__':
    unittest.main()
