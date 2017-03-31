import unittest
import ntp.util


class TestPylibUtilMethods(unittest.TestCase):

    def test_rfc3339(self):
        self.assertEqual(ntp.util.rfc3339(1480999786),
                         '2016-12-06T04:49:46Z')
        self.assertEqual(ntp.util.rfc3339(1480999786.5),
                         '2016-12-06T04:49:46.5Z')
        self.assertEqual(ntp.util.rfc3339(1480999786.025),
                         '2016-12-06T04:49:46.025Z')

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
        self.assertEqual(ntp.util.scaleforunit(1.0),
                         (1.0, 0))
        self.assertEqual(ntp.util.scaleforunit(999.2342),
                         (999.2342, 0))
        self.assertEqual(ntp.util.scaleforunit(5042.7),
                         (5.0427, 1))
        self.assertEqual(ntp.util.scaleforunit(0.1912),
                         (191.2, -1))
        self.assertEqual(ntp.util.scaleforunit(0.00000042),
                         (420.0, -3))
        self.assertEqual(ntp.util.scaleforunit(-1.0),
                         (-1.0, 0))
        self.assertEqual(ntp.util.scaleforunit(-999.2342),
                         (-999.2342, 0))
        self.assertEqual(ntp.util.scaleforunit(-5042.7),
                         (-5.0427, 1))
        self.assertEqual(ntp.util.scaleforunit(-0.1912),
                         (-191.2, -1))
        self.assertEqual(ntp.util.scaleforunit(-0.00000042),
                         (-420.0, -3))

    def test_oomsbetweenunits(self):
        self.assertEqual(ntp.util.oomsbetweenunits(3, 2),
                         3)
        self.assertEqual(ntp.util.oomsbetweenunits(2, 3),
                         3)

    def test_filtcooker(self):
        self.assertEqual(ntp.util.filtcooker(
            "1.02 34.5 0.67835 -23.0 9 6.7 1.0 .1"),
                         "   1.02    34.5 0.67835     -23       9     6.7       1     0.1 ms")


if __name__ == '__main__':
    unittest.main()
