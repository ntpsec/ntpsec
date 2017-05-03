#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.statfiles


class TestPylibStatfilesMethods(unittest.TestCase):

    def test_iso_to_posix(self):
        self.assertEqual(ntp.statfiles.iso_to_posix(1480999786),
                         1480999786)
        self.assertEqual(ntp.statfiles.iso_to_posix("2016-12-06T04:49:46"),
                         1480999786)

    def test_posix_to_iso(self):
        self.assertEqual(ntp.statfiles.posix_to_iso(1480999786),
                         "2016-12-06T04:49:46")

    def test_iso_to_posix_inverts_posix_to_iso(self):
        self.assertEqual(ntp.statfiles.iso_to_posix(
            ntp.statfiles.posix_to_iso(1480999786)), 1480999786)

    def test_posix_to_iso_inverts_iso_to_posix(self):
        self.assertEqual(ntp.statfiles.posix_to_iso(
            ntp.statfiles.iso_to_posix("2016-12-06T04:49:46")),
            "2016-12-06T04:49:46")

if __name__ == '__main__':
    unittest.main()
