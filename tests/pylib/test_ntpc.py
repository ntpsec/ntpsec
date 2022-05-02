# -*- coding: utf-8 -*-
# SPDX-License-Identifier: BSD-2-Clause
import unittest
import ntp.ntpc
import ntp.poly
import ntp.util


class TestPylibNtpc(unittest.TestCase):
    lfp_set = [
        ("0xcfba1ce0.80000000", 1276092000.5,
         "cfba1ce0.80000000 2010-06-09T14:00:00.500Z"),
        ("==cfba1ce0.80000000", 1276092000.5,
         "cfba1ce0.80000000 2010-06-09T14:00:00.500Z"),
        ]

    def test_statustoa(self):
        self.assertEqual(
            "leap_add_sec, sync_22, 7 events, no_sys_peer",
            ntp.ntpc.statustoa(ntp.ntpc.TYPE_SYS, 0x12345678))
        self.assertEqual(
            "authenb, reach, sel_sys.peer, 7 events, access_denied",
            ntp.ntpc.statustoa(ntp.ntpc.TYPE_PEER, 0x12345678))
        self.assertEqual(
            "7 events, clk_8",
            ntp.ntpc.statustoa(ntp.ntpc.TYPE_CLOCK, 0x12345678))

    def test_lfp(self):
        for (in_string, to_float, to_string) in self.lfp_set:
            self.assertEqual(ntp.ntpc.prettydate(in_string), to_string)
            self.assertAlmostEqual(ntp.ntpc.lfptofloat(in_string), to_float)

    def test_nul_trunc(self):
        k_type = "aes-128"
        key = ntp.util.hexstr2octets(
            "0fd2287c1e97a50cb9d3cb9f80debcb6")
        sample = ntp.util.hexstr2octets(
            "e3000000000000000000000000000000" +
            "00000000000000000000000000000000" +
            "0000000000000000e6136a712d8c4800" +
            "00000064" +
            "01c167a4df28f25a4794bf00c8b35469")
        mac2 = ntp.ntpc.mac(ntp.poly.polybytes(sample[:48]),
                            ntp.poly.polybytes(key), k_type)
        mac1 = ntp.poly.polybytes(sample[52:])
        self.assertEqual([len(mac1), mac1], [len(mac2), mac2], msg="nul trunc")


if __name__ == '__main__':
    unittest.main()
