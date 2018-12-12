#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, division

import unittest
import ntp.packet
import ntp.control
import ntp.util
import ntp.magic
import socket
import select
import sys
import getpass
import jigs
import ntp.poly

odict = ntp.util.OrderedDict

ntpp = ntp.packet
ctlerr = ntp.packet.ControlException


class SessionJig:
    def __init__(self):
        self.readvars_calls = 0

    def readvars(self):
        self.readvars_calls += 1
        return {"foo": 23, "bar": 42}


class ControlPacketJig:
    HEADER_LEN = ntpp.ControlPacket.HEADER_LEN

    def __init__(self, session, opcode, associd, data):
        self.session = session
        self.opcode = opcode
        self.associd = associd
        self.extension = data
        self.sequence = None
        self.send_call_count = 0

    def send(self):
        self.send_call_count += 1
        return self

    def flatten(self):
        return self.extension


class AuthenticatorJig:
    compute_mac_calls = []

    def __init__(self):
        self.control_call_count = 0
        self.control_fail = 0
        self.fail_getitem = False
        self.compute_mac_calls = []

    def __getitem__(self, key):
        if self.fail_getitem is True:
            raise IndexError
        return ("passtype", "pass")

    def control(self):
        self.control_call_count += 1
        if self.control_fail > 0:
            self.control_fail -= 1
            raise ValueError
        return (23, "keytype", "miranda")

    @staticmethod
    def compute_mac(flatpkt, keyid, keytype, passwd):
        AuthenticatorJig.compute_mac_calls.append((flatpkt, keyid,
                                                   keytype, passwd))
        return "mac"


# ==========================================================
#  Tests
# ==========================================================


class TestPacket(unittest.TestCase):
    target = ntpp.Packet

    def test_VN_MODE(self):
        f = self.target.VN_MODE
        self.assertEqual(f(0, 0), 0x00)
        self.assertEqual(f(0, 6), 0x06)
        self.assertEqual(f(1, 6), 0x0E)
        self.assertEqual(f(7, 6), 0x3E)
        self.assertEqual(f(9, 6), 0x0E)  # overflow version
        self.assertEqual(f(0, 9), 0x01)  # overflow mode

    def test_PKT_LI_VM_MODE(self):
        f = self.target.PKT_LI_VN_MODE
        self.assertEqual(f(0, 0, 0), 0x00)
        self.assertEqual(f(1, 4, 6), 0x66)
        self.assertEqual(f(3, 4, 6), 0xE6)
        self.assertEqual(f(5, 4, 6), 0x66)  # overflow leap

    def test_Packet(self):
        # Test __init__, basic
        cls = self.target()
        self.assertEqual(cls.session, None)
        self.assertEqual(cls.li_vn_mode, 0xE3)
        self.assertEqual(cls.extension, b'')
        # Test __init__, custom
        cls = self.target(2, 2, "foo")
        self.assertEqual(cls.session, "foo")
        self.assertEqual(cls.li_vn_mode, 0xD2)
        # Test leap
        self.assertEqual(cls.leap(), "unsync")
        cls.li_vn_mode = self.target.PKT_LI_VN_MODE(0, 1, 1)
        self.assertEqual(cls.leap(), "no-leap")
        cls.li_vn_mode = self.target.PKT_LI_VN_MODE(1, 1, 1)
        self.assertEqual(cls.leap(), "add-leap")
        cls.li_vn_mode = self.target.PKT_LI_VN_MODE(2, 3, 4)
        self.assertEqual(cls.leap(), "del-leap")
        # Test version
        self.assertEqual(cls.version(), 3)
        # Test mode
        self.assertEqual(cls.mode(), 4)


class TestSyncPacket(unittest.TestCase):
    target = ntpp.SyncPacket

    def test___init__(self):
        # Test without data (that will be tested via analyze())
        cls = self.target()
        self.assertEqual(cls.status, 0)
        self.assertEqual(cls.stratum, 0)
        self.assertEqual(cls.poll, 0)
        self.assertEqual(cls.precision, 0)
        self.assertEqual(cls.root_delay, 0)
        self.assertEqual(cls.root_dispersion, 0)
        self.assertEqual(cls.refid, 0)
        self.assertEqual(cls.reference_timestamp, 0)
        self.assertEqual(cls.origin_timestamp, 0)
        self.assertEqual(cls.receive_timestamp, 0)
        self.assertEqual(cls.transmit_timestamp, 0)
        self.assertEqual(cls.extension, ntp.poly.polybytes(''))
        self.assertEqual(cls.extfields, [])
        self.assertEqual(cls.mac, '')
        self.assertEqual(cls.hostname, None)
        self.assertEqual(cls.resolved, None)
        # In theory should test cls.received here, but it is assigned to time()
        self.assertEqual(cls.trusted, True)
        self.assertEqual(cls.rescaled, False)

    def test_analyze(self):
        # Test data less than packet
        data = "blah blah"
        try:
            cls = self.target(data)
            errored = False
        except ntpp.SyncException as e:
            errored = e.message
        self.assertEqual(errored, "impossible packet length")
        # Test data not word aligned
        data = "\x5C\x10\x01\xFF" \
               "\x00\x00\x01\x01\x00\x00\x01\x02\x00\x00\x01\x03" \
               "\x00\x01\x02\x03\x04\x05\x06\x07" \
               "\x01\x01\x02\x03\x04\x05\x06\x07" \
               "\x02\x01\x02\x03\x04\x05\x06\x07" \
               "\x03\x01\x02\x03\x04\x05\x06\x07\x0B\xAD"
        try:
            cls = self.target(data)
            errored = False
        except ntpp.SyncException as e:
            errored = e.message
        self.assertEqual(errored, "impossible packet length")
        # Test without extension
        data = "\x5C\x10\x01\xFF" \
               "\x00\x00\x01\x01\x00\x00\x01\x02\x00\x00\x01\x03" \
               "\x00\x01\x02\x03\x04\x05\x06\x07" \
               "\x01\x01\x02\x03\x04\x05\x06\x07" \
               "\x02\x01\x02\x03\x04\x05\x06\x07" \
               "\x03\x01\x02\x03\x04\x05\x06\x07"
        cls = self.target(data)  # This calls analyze
        self.assertEqual(cls.li_vn_mode, 0x5C)
        self.assertEqual(cls.stratum, 16)
        self.assertEqual(cls.poll, 1)
        self.assertEqual(cls.precision, -1)
        self.assertEqual(cls.root_delay, 257)
        self.assertEqual(cls.root_dispersion, 258)
        self.assertEqual(cls.refid, 259)
        self.assertEqual(cls.reference_timestamp, 0x0001020304050607)
        self.assertEqual(cls.origin_timestamp, 0x0101020304050607)
        self.assertEqual(cls.receive_timestamp, 0x0201020304050607)
        self.assertEqual(cls.transmit_timestamp, 0x0301020304050607)
        self.assertEqual(cls.extfields, [])
        # Test with extension, Crypto-NAK
        ext = "\x00\x00\x00\x01\x00\x00\x00\x04blah" \
              "\x00\x00\x00\x02\x00\x00\x00\x0Cjabberjabber" \
              "\x00\x00\x00\x03\x00\x00\x00\x20" \
              "In the end, our choices make us."
        mac = "\x11\x22\x33\x44"
        data2 = data + ext + mac
        cls = self.target(data2)
        self.assertEqual(cls.li_vn_mode, 0x5C)
        self.assertEqual(cls.stratum, 16)
        self.assertEqual(cls.poll, 1)
        self.assertEqual(cls.precision, -1)
        self.assertEqual(cls.root_delay, 257)
        self.assertEqual(cls.root_dispersion, 258)
        self.assertEqual(cls.refid, 259)
        self.assertEqual(cls.reference_timestamp, 0x0001020304050607)
        self.assertEqual(cls.origin_timestamp, 0x0101020304050607)
        self.assertEqual(cls.receive_timestamp, 0x0201020304050607)
        self.assertEqual(cls.transmit_timestamp, 0x0301020304050607)
        self.assertEqual(cls.extfields,
                         [(1, ntp.poly.polybytes("blah")),
                          (2, ntp.poly.polybytes("jabberjabber")),
                          (3, ntp.poly.polybytes("In the end, our choices make us."))])
        self.assertEqual(cls.extension, ntp.poly.polybytes(ext + mac))
        self.assertEqual(cls.mac, ntp.poly.polybytes(mac))
        # Test with extension, DES
        data2 = data + ext + "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC"
        try:
            cls = self.target(data2)
            errored = False
        except ntpp.SyncException as e:
            errored = e.message
        self.assertEqual(errored, "Unsupported DES authentication")
        # Test with extension, runt 8
        data2 = data + ext + "\x11\x22\x33\x44\x55\x66\x77\x88"
        try:
            cls = self.target(data2)
            errored = False
        except ntpp.SyncException as e:
            errored = e.message
        self.assertEqual(errored, "Packet is a runt")
        # Test with extension, runt 16
        data2 = data + ext + "\x00\x11\x22\x33\x44\x55\x66\x77" \
                             "\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
        try:
            cls = self.target(data2)
            errored = False
        except ntpp.SyncException as e:
            errored = e.message
        self.assertEqual(errored, "Packet is a runt")
        # Test with extension, MD5 or SHA1, 20
        mac = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09" \
              "\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13"
        data2 = data + ext + mac
        cls = self.target(data2)
        self.assertEqual(cls.mac, ntp.poly.polybytes(mac))
        self.assertEqual(cls.extension, ntp.poly.polybytes(ext + mac))
        # Test with extension, MD5 or SHA1, 24
        mac += "\x14\x15\x16\x17"
        data2 = data + ext + mac
        cls = self.target(ntp.poly.polybytes(data2))
        self.assertEqual(cls.mac, ntp.poly.polybytes(mac))
        self.assertEqual(cls.extension, ntp.poly.polybytes(ext + mac))

    # ====================================================================
    # The tests referring to the Timeless Void are testing outside of NTP's
    # representation range, they are left in on the off chance that they
    # catch a bug in the future. If they become problematic they can be
    # removed without loss.
    # ====================================================================
    def test_ntp_to_posix(self):
        f = self.target.ntp_to_posix
        # Test the Timeless Void Before Existence
        self.assertAlmostEqual(f(-1), -2208988800.0)
        # Test beginning of NTP epoch
        self.assertAlmostEqual(f(0), -2208988800.0)
        # Test just before the Age of Unix
        self.assertAlmostEqual(f(2208988799 * 2**32), -1.0)
        # Test the beginning of the Age of Unix
        self.assertAlmostEqual(f(2208988800 * 2**32), 0.0)
        # Test the End of Time
        self.assertAlmostEqual(f(0xFFFFFFFF00000000), 2085978495.0)
        # Test the Timeless Void Beyond Existence
        self.assertEqual(f(0x10000000000000000), 2085978496.0)  # Doesn't clip

    def test_posix_to_ntp(self):
        # This function may develop float precision issues, however it
        # returns an integer so testing for that is complicated. For the
        # moment the tests are being left as-is until such issues appear.
        f = self.target.posix_to_ntp
        # Test the Timeless Void Before Existence
        self.assertEqual(f(-2208988801), -0x100000000)  # It doesn't clip
        # Test beginning of NTP epoch
        self.assertEqual(f(-2208988800), 0)
        # Test just before the Age of Unix
        self.assertEqual(f(-1), 2208988799 * 2**32)
        # Test the beginning of the Age of Unix
        self.assertEqual(f(0), 2208988800 * 2**32)
        # Test the End of Time
        self.assertEqual(f(2085978495), 0xFFFFFFFF00000000)
        # Test the Timeless Void Beyond Existence
        self.assertEqual(f(2085978496), 0x10000000000000000)  # It doesn't clip

    def test_posixize(self):
        cls = self.target()
        # Test already scaled
        cls.rescaled = True
        cls.received = 0  # __init__ sets to current time
        cls.posixize()
        self.assertEqual(cls.rescaled, True)
        self.assertEqual(cls.root_delay, 0)
        self.assertEqual(cls.root_dispersion, 0)
        self.assertEqual(cls.reference_timestamp, 0)
        self.assertEqual(cls.origin_timestamp, 0)
        self.assertEqual(cls.receive_timestamp, 0)
        self.assertEqual(cls.transmit_timestamp, 0)
        self.assertEqual(cls.received, 0)
        # Test scaling
        cls.rescaled = False
        cls.root_delay = 131072
        cls.root_dispersion = 131072
        cls.posixize()
        self.assertEqual(cls.rescaled, True)
        self.assertEqual(cls.root_delay, 2)
        self.assertEqual(cls.root_dispersion, 2)
        self.assertAlmostEqual(cls.reference_timestamp, -2208988800.0)
        self.assertAlmostEqual(cls.origin_timestamp, -2208988800.0)
        self.assertAlmostEqual(cls.receive_timestamp, -2208988800.0)
        self.assertAlmostEqual(cls.transmit_timestamp, -2208988800.0)
        self.assertAlmostEqual(cls.received, -2208988800.0)

    def test_t1_4(self):
        cls = self.target()
        cls.origin_timestamp = 1
        cls.receive_timestamp = 2
        cls.transmit_timestamp = 3
        cls.received = 4
        self.assertEqual(cls.t1(), 1)
        self.assertEqual(cls.t2(), 2)
        self.assertEqual(cls.t3(), 3)
        self.assertEqual(cls.t4(), 4)

    def test_delta_epsilon_synchd_adjust(self):
        # Combined because they share setup, and are short tests
        cls = self.target()
        cls.origin_timestamp = 1
        cls.receive_timestamp = 2
        cls.transmit_timestamp = 3
        cls.received = 4
        cls.precision = 4
        # Test delta
        self.assertEqual(cls.delta(), 2)
        # Test epsilon
        self.assertAlmostEqual(cls.epsilon(), 16.000045)
        # Test synchd
        self.assertAlmostEqual(cls.synchd(), 17.000045)
        # Test adjust
        self.assertAlmostEqual(cls.adjust(), 0.0)

    def test_flatten(self):
        data = "\x5C\x10\x01\xFF" \
               "\x00\x00\x01\x01\x00\x00\x01\x02\x00\x00\x01\x03" \
               "\x00\x01\x02\x03\x04\x05\x06\x07" \
               "\x01\x01\x02\x03\x04\x05\x06\x07" \
               "\x02\x01\x02\x03\x04\x05\x06\x07" \
               "\x03\x01\x02\x03\x04\x05\x06\x07"
        ext = "\x00\x00\x00\x01\x00\x00\x00\x04blah" \
              "\x00\x00\x00\x02\x00\x00\x00\x0Cjabberjabber" \
              "\x00\x00\x00\x03\x00\x00\x00\x20" \
              "In the end, our choices make us." \
              "\x11\x22\x33\x44"
        pkt = ntp.poly.polybytes(data + ext)
        cls = self.target(pkt)
        self.assertEqual(cls.flatten(), pkt)

    def test_refid_octets(self):
        cls = self.target()
        cls.refid = 0x12345678
        self.assertEqual(cls.refid_octets(), (0x12, 0x34, 0x56, 0x78))

    def test_refid_as_string(self):
        cls = self.target()
        cls.refid = 0x12345678
        self.assertEqual(cls.refid_as_string(), "\x12\x34\x56\x78")

    def test_refid_as_address(self):
        cls = self.target()
        cls.refid = 0x01020304
        self.assertEqual(cls.refid_as_address(), "1.2.3.4")

    def test_is_crypto_nak(self):
        cls = self.target()
        # Test True
        cls.mac = "blah"
        self.assertEqual(cls.is_crypto_nak(), True)
        # Test too low
        cls.mac = "bla"
        self.assertEqual(cls.is_crypto_nak(), False)
        # Test too high
        cls.mac = "blah!"
        self.assertEqual(cls.is_crypto_nak(), False)

    def test_MD5(self):
        cls = self.target()
        # Test True
        cls.mac = "blah" * 5  # 20 bytes
        self.assertEqual(cls.has_MD5(), True)
        # Test too low
        cls.mac = "bla" * 5  # 15 bytes
        self.assertEqual(cls.has_MD5(), False)
        # Test too high
        cls.mac = "blah!" * 5  # 24 bytes
        self.assertEqual(cls.has_MD5(), False)

    def test_SHA1(self):
        cls = self.target()
        # Test True
        cls.mac = "blah" * 6  # 24 bytes
        self.assertEqual(cls.has_SHA1(), True)
        # Test too low
        cls.mac = "bla" * 6  # 18 bytes
        self.assertEqual(cls.has_SHA1(), False)
        # Test too high
        cls.mac = "blah!" * 6  # 30 bytes
        self.assertEqual(cls.has_SHA1(), False)

    def test___repr__(self):
        cls = self.target()
        cls.reference_timestamp = self.target.posix_to_ntp(0)
        cls.origin_timestamp = self.target.posix_to_ntp(1)
        cls.receive_timestamp = self.target.posix_to_ntp(2)
        cls.transmit_timestamp = self.target.posix_to_ntp(3)
        # Test without extensions
        self.assertEqual(cls.__repr__(),
                         "<NTP:unsync:4:3:0.000000:0.000000:0.0.0.0:"
                         "1970-01-01T00:00:00Z:1970-01-01T00:00:01Z:"
                         "1970-01-01T00:00:02Z:1970-01-01T00:00:03Z>")
        # Test with extensions
        cls.extfields = "foobar"
        cls.mac = "machinations"
        self.assertEqual(cls.__repr__(),
                         "<NTP:unsync:4:3:0.000000:0.000000:0.0.0.0:"
                         "1970-01-01T00:00:00Z:1970-01-01T00:00:01Z:"
                         "1970-01-01T00:00:02Z:1970-01-01T00:00:03Z:"
                         "'foobar':machinations>")


class TestMisc(unittest.TestCase):
    def test_Peer(self):
        session = SessionJig()
        # Test init
        cls = ntpp.Peer(session, 2, 3)
        self.assertEqual(cls.session, session)
        self.assertEqual(cls.associd, 2)
        self.assertEqual(cls.status, 3)
        self.assertEqual(cls.variables, {})
        # Test readvars
        cls.readvars()
        self.assertEqual(cls.variables, {"foo": 23, "bar": 42})
        # Test __str__
        self.assertEqual(str(cls), "<Peer: associd=2 status=3>")
        # Test __repr__
        self.assertEqual(repr(cls), "<Peer: associd=2 status=3>")

    def test_dump_hex_printable(self):
        f = ntpp.dump_hex_printable
        fp = jigs.FileJig()
        data = "\x00\x01\x02\x03\x04\x05\x06\x07" \
               "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
        # Test a single line
        f(data, fp)
        total = "".join(fp.data)  # easier than 300 million separate strings
        fp.data = []
        self.assertEqual(total,
                         "00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f "
                         "................\n")
        # Test >1 lines, partial line
        data += "Would you kindly test this?"
        f(data, fp)
        total = "".join(fp.data)
        fp.data = []
        self.assertEqual(total,
                         "00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f "
                         "................\n"
                         "57 6f 75 6c 64 20 79 6f 75 20 6b 69 6e 64 6c 79 "
                         "Would you kindly\n"
                         "20 74 65 73 74 20 74 68 69 73 3f                "
                         " test this?\n")

    def test_MRUEntry(self):
        # Test init
        cls = ntpp.MRUEntry()
        self.assertEqual(cls.addr, None)
        self.assertEqual(cls.last, None)
        self.assertEqual(cls.first, None)
        self.assertEqual(cls.ct, 0)
        self.assertEqual(cls.mv, None)
        self.assertEqual(cls.rs, None)
        # Test avgint
        cls.last = "0x00000200.00000000"
        cls.first = "0x00000100.00000000"
        cls.ct = 4
        self.assertEqual(cls.avgint(), 64)
        if socket.has_ipv6:
            # Test sortaddr, ipv6
            cls.addr = "[11:22:33::44:55]:42"
            self.assertEqual(cls.sortaddr(),
                             ntp.poly.polybytes("\x00\x11\x00\x22\x00\x33\x00\x00"
                                                "\x00\x00\x00\x00\x00\x44\x00\x55"))
            # Test sortaddr, ipv6, local
            cls.addr = "[11:22:33::44:55%8]:42"
            self.assertEqual(cls.sortaddr(),
                             ntp.poly.polybytes("\x00\x11\x00\x22\x00\x33\x00\x00"
                                                "\x00\x00\x00\x00\x00\x44\x00\x55"))

        # Test sortaddr, ipv4
        cls.addr = "11.22.33.44:23"
        self.assertEqual(cls.sortaddr(),
                         ntp.poly.polybytes((("\0" * 16) + "\x0b\x16\x21\x2c")))
        # Test __repr__
        # Python dicts enumeration order changes with different versions
        if sys.version_info[0] < 3:
            # Python 2
            self.assertEqual(cls.__repr__(),
                             "<MRUEntry: "
                             "'last': '0x00000200.00000000', "
                             "'addr': '11.22.33.44:23', 'rs': None, "
                             "'mv': None, 'first': '0x00000100.00000000', "
                             "'ct': 4>")
        elif sys.version_info[1] >= 6:  # Already know it is 3.something
            # Python 3.6+, dicts enumerate in assignment order
            self.assertEqual(cls.__repr__(),
                             "<MRUEntry: 'addr': '11.22.33.44:23', "
                             "'last': '0x00000200.00000000', "
                             "'first': '0x00000100.00000000', 'ct': 4, "
                             "'mv': None, 'rs': None>")
        else:
            # Python 3.x < 3.6, dicts enumerate randomly
            # I can not test randomness of this type
            pass

    def test_MRUList(self):
        # Test init
        cls = ntpp.MRUList()
        self.assertEqual(cls.entries, [])
        self.assertEqual(cls.now, None)
        # Test is_complete, no
        self.assertEqual(cls.is_complete(), False)
        # Test is_complete, yes
        cls.now = "0x01234567.89ABCDEF"
        self.assertEqual(cls.is_complete(), True)
        # Test __repr__, simplified
        cls.entries = [1, 2, 3, 4]
        self.assertEqual(cls.__repr__(),
                         "<MRUList: entries=[1, 2, 3, 4] "
                         "now=0x01234567.89ABCDEF>")


class TestControlPacket(unittest.TestCase):
    target = ntpp.ControlPacket
    session = ntpp.ControlSession

    def test___init__(self):
        ses = self.session()
        cls = self.target(ses)
        self.assertEqual(cls.r_e_m_op, 0)
        self.assertEqual(cls.sequence, 1)
        self.assertEqual(cls.status, 0)
        self.assertEqual(cls.associd, 0)
        self.assertEqual(cls.offset, 0)
        self.assertEqual(cls.extension, ntp.poly.polybytes(""))
        self.assertEqual(cls.count, 0)

    def test_is_response(self):
        cls = self.target(self.session())
        # Test True
        cls.r_e_m_op = 0x80
        self.assertEqual(cls.is_response(), True)
        # Test False
        cls.r_e_m_op = 0x7F
        self.assertEqual(cls.is_response(), False)

    def test_is_error(self):
        cls = self.target(self.session())
        # Test True
        cls.r_e_m_op = 0x40
        self.assertEqual(cls.is_error(), True)
        # Test False
        cls.r_e_m_op = 0xBF
        self.assertEqual(cls.is_error(), False)

    def test_more(self):
        cls = self.target(self.session())
        # Test True
        cls.r_e_m_op = 0x20
        self.assertEqual(cls.more(), True)
        # Test False
        cls.r_e_m_op = 0xDF
        self.assertEqual(cls.more(), False)

    def test_opcode(self):
        cls = self.target(self.session())
        # Test normal
        cls.r_e_m_op = 0x06
        self.assertEqual(cls.opcode(), 6)
        # Test maximum
        cls.r_e_m_op = 0xFF
        self.assertEqual(cls.opcode(), 31)
        # Test maximum, no bits
        cls.r_e_m_op = 0x1F
        self.assertEqual(cls.opcode(), 31)

    def test_errcode(self):
        cls = self.target(self.session())
        # Test none
        cls.status = 0x00FF
        self.assertEqual(cls.errcode(), 0)
        # Test midling
        cls.status = 0x11FF
        self.assertEqual(cls.errcode(), 17)
        # Test maximum
        cls.status = 0xFF00
        self.assertEqual(cls.errcode(), 255)

    def test_end(self):
        cls = self.target(self.session())
        cls.count = 5
        cls.offset = 10
        self.assertEqual(cls.end(), 15)

    def test_stats(self):
        cls = self.target(self.session())
        cls.count = 25
        cls.offset = 10
        self.assertEqual(cls.stats(), "   10    35\t 25 octets\n")

    def test_analyze_flatten_send(self):
        cls = self.target(self.session())
        header = "\x8A\x3F\x00\x01\x00\x02\x00\x03\x00\x20\x00\x10"
        payload = "\x00\x11\x22\x33\x44\x55\x66\x77" \
                  "\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
        ext = "\x01\x02\x03\x04" \
              "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7" \
              "\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF"
        totaldata = header + payload + ext
        # Test analyze
        result = cls.analyze(totaldata)
        self.assertEqual(result, (1, 2, 3, 32))
        self.assertEqual(cls.li_vn_mode, 0x8A)
        self.assertEqual(cls.r_e_m_op, 0x3F)
        self.assertEqual(cls.sequence, 1)
        self.assertEqual(cls.status, 2)
        self.assertEqual(cls.associd, 3)
        self.assertEqual(cls.offset, 32)
        self.assertEqual(cls.count, 16)
        # Test flatten
        self.assertEqual(cls.flatten(), ntp.poly.polybytes(totaldata))
        # Test send
        send_data = []

        def send_jig(pkt):
            send_data.append(pkt)
        cls.session.sendpkt = send_jig
        cls.send()
        self.assertEqual(send_data, [ntp.poly.polybytes(totaldata)])


class TestControlSession(unittest.TestCase):
    target = ntpp.ControlSession

    def test___init__(self):
        # Test
        cls = self.target()
        self.assertEqual(cls.debug, 0)
        self.assertEqual(cls.ai_family, socket.AF_UNSPEC)
        self.assertEqual(cls.primary_timeout, ntpp.DEFTIMEOUT)
        self.assertEqual(cls.secondary_timeout, ntpp.DEFSTIMEOUT)
        self.assertEqual(cls.pktversion, ntp.magic.NTP_OLDVERSION + 1)
        self.assertEqual(cls.always_auth, False)
        self.assertEqual(cls.keytype, "MD5")
        self.assertEqual(cls.keyid, None)
        self.assertEqual(cls.passwd, None)
        self.assertEqual(cls.auth, None)
        self.assertEqual(cls.hostname, None)
        self.assertEqual(cls.isnum, False)
        self.assertEqual(cls.sock, None)
        self.assertEqual(cls.port, 0)
        self.assertEqual(cls.sequence, 0)
        self.assertEqual(cls.response, "")
        self.assertEqual(cls.rstatus, 0)
        self.assertEqual(cls.ntpd_row_limit, self.target.MRU_ROW_LIMIT)
        self.assertEqual(cls.logfp, sys.stdout)
        self.assertEqual(cls.nonce_xmit, 0)

    def test_close(self):
        # Init
        sockjig = jigs.SocketJig()
        cls = self.target()
        cls.sock = sockjig
        # Test
        cls.close()
        self.assertEqual(sockjig.closed, True)
        self.assertEqual(cls.sock, None)

    def test_havehost(self):
        # Init
        cls = self.target()
        # Test empty
        self.assertEqual(cls.havehost(), False)
        # Test full
        cls.sock = True
        self.assertEqual(cls.havehost(), True)

    def test___lookuphost(self):
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        faketimemod.time_returns = [0] * 10
        try:
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            fakesockmod = jigs.SocketModuleJig()
            ntpp.socket = fakesockmod
            # Init
            cls = self.target()
            cls.debug = 3
            cls.logfp = logjig
            # Test first type
            fakesockmod.gai_returns = [42]
            result = cls._ControlSession__lookuphost("[blah.com]", "family")
            self.assertEqual(result, 42)
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST)])
            self.assertEqual(logjig.data, [])
            # Test second type
            logjig.__init__()  # reset
            fakesockmod.__init__()
            fakesockmod.gai_returns = [42]
            fakesockmod.gai_error_count = 1
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, 42)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z ntpq: numeric-mode lookup "
                              "of blah.com failed, None\n"])
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0)])
            # Test third type
            logjig.__init__()  # reset
            fakesockmod.__init__()
            fakesockmod.gai_returns = [42]
            fakesockmod.gai_error_count = 2
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, 42)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z ntpq: numeric-mode lookup "
                              "of blah.com failed, None\n",
                              "ntpq: standard-mode lookup of blah.com failed, "
                              "None\n"])
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0)])
            # Test all types failed
            logjig.__init__()  # reset
            fakesockmod.__init__()
            fakesockmod.gai_error_count = 3
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, None)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z ntpq: numeric-mode lookup "
                              "of blah.com failed, None\n",
                              "ntpq: standard-mode lookup of blah.com failed, "
                              "None\n",
                              "ntpq: ndp lookup failed, None\n"])
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0)])
        finally:
            ntpp.socket = socket
            ntp.util.time = timetemp

    def test_openhost(self):
        lookups = []
        returnNothing = True
        noCanon = False

        def lookup_jig(hostname, family):
            lookups.append((hostname, family))
            if returnNothing is True:
                return None
            elif noCanon is True:
                return [("family", "socktype", "protocol", None,
                         ("1.2.3.4", 80)), ]
            else:
                return [("family", "socktype", "protocol", "canon",
                         ("1.2.3.4", 80)), ]
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        try:
            fakesockmod = jigs.SocketModuleJig()
            ntpp.socket = fakesockmod
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            faketimemod.time_returns = [0] * 10
            # Init
            cls = self.target()
            cls.debug = 3
            cls.logfp = logjig
            cls._ControlSession__lookuphost = lookup_jig
            # Test, lookup failure
            result = cls.openhost("foo.org")
            self.assertEqual(result, False)
            self.assertEqual(lookups, [("foo.org", socket.AF_UNSPEC)])
            # Test, with canon, and success
            returnNothing = False
            lookups = []
            result = cls.openhost("foo.org")
            self.assertEqual(result, True)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Opening host canon\n"])
            self.assertEqual(lookups, [("foo.org", socket.AF_UNSPEC)])
            self.assertEqual(fakesockmod.socket_calls,
                             [("family", "socktype", "protocol")])
            sock = fakesockmod.socketsReturned[0]
            self.assertEqual(sock.connected, ("1.2.3.4", 80))
            self.assertEqual(cls.hostname, "canon")
            self.assertEqual(cls.isnum, False)
            self.assertEqual(cls.port, 80)
            self.assertEqual(cls.sock, sock)
            # Test, without canon, and success
            noCanon = True
            lookups = []
            logjig.__init__()
            fakesockmod.__init__()
            cls.sock = None
            result = cls.openhost("foo.org")
            self.assertEqual(result, True)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Opening host canon.com\n"])
            self.assertEqual(lookups, [("foo.org", socket.AF_UNSPEC)])
            self.assertEqual(fakesockmod.socket_calls,
                             [("family", "socktype", "protocol")])
            sock = fakesockmod.socketsReturned[0]
            self.assertEqual(sock.connected, ("1.2.3.4", 80))
            self.assertEqual(cls.hostname, "canon.com")
            self.assertEqual(cls.isnum, True)
            self.assertEqual(cls.port, 80)
            self.assertEqual(cls.sock, sock)
            # Test, with canon, and socket creation failure
            noCanon = False
            cls.sock = None
            fakesockmod.socket_fail = True
            try:
                result = cls.openhost("foo.org")
                errored = False
            except ctlerr as e:
                errored = e
            self.assertEqual(errored.message,
                             "Error opening foo.org: error! [23]")
            # Test, with canon, and socket connection failure
            fakesockmod.socket_fail = False
            fakesockmod.socket_fail_connect = True
            cls.sock = None
            try:
                result = cls.openhost("foo.org")
                errored = False
            except ctlerr as e:
                errored = e
            self.assertEqual(errored.message,
                             "Error connecting to foo.org: socket! [16]")
        finally:
            ntpp.socket = socket
            ntp.util.time = timetemp

    def test_password(self):
        iojig = jigs.FileJig()
        fakegetpmod = jigs.GetpassModuleJig()
        fakeosmod = jigs.OSModuleJig()
        # Init
        cls = self.target()
        try:
            tempauth = ntpp.Authenticator
            ntpp.Authenticator = AuthenticatorJig
            ntpp.getpass = fakegetpmod
            tempstdin = sys.stdin
            sys.stdin = iojig
            tempstdout = sys.stdout
            sys.stdout = iojig
            tempos = ntpp.os
            ntpp.os = fakeosmod
            # Test with nothing
            iojig.readline_return = ["1\n"] * 10
            fakeosmod.isatty_returns = [True] * 10
            cls.password()
            self.assertEqual(isinstance(cls.auth, AuthenticatorJig), True)
            self.assertEqual(cls.keyid, 1)
            self.assertEqual(cls.passwd, "pass")
            # Test with auth and localhost, fail
            cls.keyid = None
            cls.passwd = None
            cls.hostname = "localhost"
            cls.auth.control_fail = 1
            try:
                cls.password()
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_NOTRUST)
            # Test with auth and localhost
            cls.keyid = None
            cls.passwd = None
            cls.hostname = "localhost"
            cls.password()
            self.assertEqual(cls.keyid, 23)
            self.assertEqual(cls.keytype, "keytype")
            self.assertEqual(cls.passwd, "miranda")
            # Test with all but password, normal password
            cls.passwd = None
            cls.auth.fail_getitem = True
            fakegetpmod.getpass_returns = ["xyzzy"]
            cls.password()
            self.assertEqual(fakegetpmod.getpass_calls,
                             [("keytype Password: ", None)])
            self.assertEqual(cls.passwd, "xyzzy")
            # Test with all but password, hex password
            fakegetpmod.getpass_calls = []
            cls.passwd = None
            cls.auth.fail_getitem = True
            fakegetpmod.getpass_returns = ["0102030405060708090A"
                                           "0B0C0D0E0F1011121314"]  # 40 char
            cls.password()
            self.assertEqual(fakegetpmod.getpass_calls,
                             [("keytype Password: ", None)])
            self.assertEqual(cls.passwd,
                             "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"
                             "\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14")
        finally:
            ntpp.Authenticator = tempauth
            ntpp.getpass = getpass
            sys.stdin = tempstdin
            sys.stdout = tempstdout
            ntpp.os = tempos

    def test_sendpkt(self):
        logjig = jigs.FileJig()
        sockjig = jigs.SocketJig()
        faketimemod = jigs.TimeModuleJig()
        faketimemod.time_returns = [0] * 10

        # Init
        cls = self.target()
        cls.logfp = logjig
        cls.sock = sockjig
        cls.debug = 3
        try:
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            # Test
            res = cls.sendpkt(ntp.poly.polybytes("blahfoo"))
            self.assertEqual(res, 0)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Sending 8 octets.  "
                              "seq=0\n"])
            self.assertEqual(sockjig.data, [ntp.poly.polybytes("blahfoo\x00")])
            # Test error
            logjig.__init__()
            sockjig.fail_send = 1
            res = cls.sendpkt("blah")
            self.assertEqual(res, -1)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Sending 4 octets.  seq=0\n",
                              "Write to None failed\n"])
        finally:
            ntp.util.time = timetemp

    def test_sendrequest(self):
        logjig = jigs.FileJig()
        try:
            tempcpkt = ntpp.ControlPacket
            ntpp.ControlPacket = ControlPacketJig
            tempauth = ntpp.Authenticator
            ntpp.Authenticator = AuthenticatorJig
            cls = self.target()
            cls.logfp = logjig
            cls.debug = 3
            # Test oversize data
            datalen = ntp.control.CTL_MAX_DATA_LEN + 1
            data = "a" * datalen
            result = cls.sendrequest(1, 2, data)
            self.assertEqual(result, -1)
            self.assertEqual(logjig.data,
                             ["\n",
                              "sendrequest: opcode=1, associd=2, "
                              "qdata=" + data + "\n",
                              "***Internal error! Data too large "
                              "(" + str(len(data)) + ")\n"])
            # Test no auth
            result = cls.sendrequest(1, 2, ntp.poly.polybytes("foo"))
            self.assertEqual(result.sequence, 1)
            self.assertEqual(result.extension, ntp.poly.polybytes("foo\x00"))
            # Test with auth
            cls.keyid = 1
            cls.passwd = "qwerty"
            result = cls.sendrequest(1, 2, ntp.poly.polybytes("foo"), True)
            self.assertEqual(result.sequence, 2)
            self.assertEqual(result.extension, ntp.poly.polybytes("foo\x00mac"))
            # Test with auth keyid / password failure
            cls.keyid = None
            try:
                cls.sendrequest(1, 2, "foo", True)
                errored = False
            except ctlerr:
                errored = True
            self.assertEqual(errored, True)
        finally:
            ntpp.ControlPacket = tempcpkt
            ntpp.Authenticator = tempauth

    def test_getresponse(self):
        logjig = jigs.FileJig()
        sockjig = jigs.SocketJig()
        fakeselectmod = jigs.SelectModuleJig()
        # Init
        cls = self.target()
        cls.debug = 3
        cls.logfp = logjig
        cls.sock = sockjig
        try:
            ntpp.select = fakeselectmod
            # Test empty
            sockjig.return_data = [
                "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"]
            cls.getresponse(1, 2, True)
            self.assertEqual(cls.response, ntp.poly.polybytes(""))
            # Test with data
            sockjig.return_data = [
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09"
                "foo=4223,\x00\x00\x00",
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x09\x00\x0E"
                "blah=248,x=23,\x00\x00",
                "\x0E\x81\x00\x01\x00\x02\x00\x03\x00\x17\x00\x06"
                "quux=1\x00\x00"]
            cls.sequence = 1
            cls.getresponse(1, 3, True)
            self.assertEqual(cls.response,
                             ntp.poly.polybytes("foo=4223,blah=248,x=23,quux=1"))
            # Test with data, duplicate packet
            sockjig.return_data = [
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09"
                "foo=4223,\x00\x00\x00",
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x09\x00\x0E"
                "blah=248,x=23,\x00\x00",
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x09\x00\x0E"
                "blah=248,x=23,\x00\x00",
                "\x0E\x81\x00\x01\x00\x02\x00\x03\x00\x17\x00\x06"
                "quux=1\x00\x00"]
            cls.sequence = 1
            cls.getresponse(1, 3, True)
            self.assertEqual(cls.response,
                             ntp.poly.polybytes("foo=4223,blah=248,x=23,quux=1"))
            # Test MAXFRAGS bail
            maxtemp = ntpp.MAXFRAGS
            ntpp.MAXFRAGS = 1
            sockjig.return_data = [
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09"
                "foo=4223,\x00\x00\x00",
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x09\x00\x0E"
                "blah=248,x=23,\x00\x00",
                "\x0E\x81\x00\x01\x00\x02\x00\x03\x00\x17\x00\x06"
                "quux=1\x00\x00"]
            cls.sequence = 1
            try:
                cls.getresponse(1, 3, True)
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_TOOMUCH)
            ntpp.MAXFRAGS = maxtemp
            # Test select fail
            fakeselectmod.select_fail = 1
            try:
                cls.getresponse(1, 2, True)
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_SELECT)
            # Test no data and timeout
            fakeselectmod.do_return = [False]
            try:
                cls.getresponse(1, 2, True)
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_TIMEOUT)
            # Test partial data and no timeout
            sockjig.return_data = [
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09"
                "foo=4223,\x00\x00\x00",
                "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x09\x00\x0E"
                "blah=248,x=23,\x00\x00",
                "\x0E\x81\x00\x01\x00\x02\x00\x03\x00\x17\x00\x06"
                "quux=1\x00\x00"]
            fakeselectmod.do_return = [True, False]
            try:
                cls.getresponse(1, 2, False)
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_INCOMPLETE)
            # Test header parse fail
            sockjig.return_data = [
                "\x0E\x81\x00\x00\x00\x03"]
            try:
                cls.getresponse(1, 2, True)
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_UNSPEC)
        finally:
            ntpp.select = select

    def test___validate_packet(self):
        logjig = jigs.FileJig()
        faketimemod = jigs.TimeModuleJig()
        # Init
        cls = self.target()
        cls.debug = 5
        cls.logfp = logjig
        try:
            timetemp = ntp.util.time
            ntp.util.time = faketimemod
            faketimemod.time_returns = [0] * 10
            # Test good packet, empty data
            raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             True)
            self.assertEqual(logjig.data, [])
            # Test good packet, with data
            logjig.data = []
            raw = "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09" \
                  "foo=4223,\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            cls.sequence = 1
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 3),
                             True)
            self.assertEqual(logjig.data, [])
            # Test bad packet, bad version
            # Need to fix version logic 0x46 can be ver == 5, or 0
            cls.sequence = 0
            logjig.data = []
            raw = "\x46\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Fragment "
                              "received with version 0\n"])
            # Test bad packet, bad mode
            logjig.data = []
            raw = "\x0D\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Fragment received "
                              "with mode 5\n"])
            # Test bad packet, bad response bit
            logjig.data = []
            raw = "\x0E\x01\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Received request, "
                              "wanted response\n"])
            # Test bad packet, bad sequence
            logjig.data = []
            raw = "\x0E\x81\x00\x01\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Received sequence "
                              "number 1, wanted 0\n"])
            # Test bad packet, bad opcode
            logjig.data = []
            raw = "\x0E\x80\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Received opcode 0, "
                              "wanted 1\n"])
            # Test error packet
            logjig.data = []
            raw = "\x0E\xC1\x00\x00" + \
                  chr(ntp.control.CERR_BADVALUE) + \
                  "\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            try:
                cls._ControlSession__validate_packet(pkt, raw, 1, 2)
                self.assertEqual(False, True)  # it should have errored here
            except ctlerr as e:
                self.assertEqual(e.errorcode, ntp.control.CERR_BADVALUE)
            self.assertEqual(logjig.data, [])
            # Test error packet, with more bit
            logjig.data = []
            errcs = chr(ntp.control.CERR_BADVALUE)
            raw = "\x0E\xE1\x00\x00" + errcs + "\x03\x00\x02\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            try:
                cls._ControlSession__validate_packet(pkt, raw, 1, 2)
                self.assertEqual(False, True)  # it should have errored here
            except ctlerr as e:
                self.assertEqual(e.errorcode, ntp.control.CERR_BADVALUE)
            errstr = "Error " + str(ntp.control.CERR_BADVALUE) + \
                     " received on non-final fragment\n"
            self.assertEqual(logjig.data, [errstr])
            # Test ok-ish packet, bad associd
            logjig.data = []
            raw = "\x0E\x81\x00\x00\x00\x03\x00\xFF\x00\x00\x00\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             True)
            self.assertEqual(logjig.data,
                             ["Association ID 255 doesn't match expected 2\n"])
            # Test bad data padding
            logjig.data = []
            raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x01@"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            self.assertEqual(cls._ControlSession__validate_packet(pkt, raw,
                                                                  1, 2),
                             False)
            self.assertEqual(logjig.data,
                             ["Response fragment not padded, size = 13\n"])
            # Test too little data
            logjig.data = []
            raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x10foo\x00"
            pkt = ntpp.ControlPacket(cls)
            pkt.analyze(raw)
            try:
                cls._ControlSession__validate_packet(pkt, raw, 1, 2)
                self.assertEqual(True, False)  # should have errored here
            except ctlerr as e:
                self.assertEqual(e.message, ntpp.SERR_INCOMPLETE)
            self.assertEqual(logjig.data,
                             ["Response fragment claims 16 octets payload, "
                              "above 4 received\n"])
        finally:
            ntp.util.time = timetemp

    def test_doquery(self):
        sends = []

        def sendrequest_jig(opcode, associd, qdata, auth):
            sends.append((opcode, associd, qdata, auth))
        gets = []
        doerror = [False]

        def getresponse_jig(opcode, associd, retry):
            gets.append((opcode, associd, retry))
            if doerror[0]:
                doerror[0] = False
                raise ctlerr(ntpp.SERR_TIMEOUT)
            return "flax!"
        # Init
        cls = self.target()
        cls.sendrequest = sendrequest_jig
        cls.getresponse = getresponse_jig

        # Test no socket
        try:
            cls.doquery(1, 2, "blah")
            errored = False
        except ctlerr as e:
            errored = e.message
        self.assertEqual(errored, ntpp.SERR_NOHOST)
        # Test no retry
        cls.sock = True  # to fool havehost()
        result = cls.doquery(1, 2, "blah")
        self.assertEqual(result, "flax!")
        self.assertEqual(len(sends), 1)
        self.assertEqual(sends[0], (1, 2, "blah", False))
        self.assertEqual(len(gets), 1)
        self.assertEqual(gets[0], (1, 2, False))
        # Reset
        sends = []
        gets = []
        doerror[0] = True
        # Test retry
        result = cls.doquery(1, 2, "quux")
        self.assertEqual(result, "flax!")
        self.assertEqual(len(sends), 2)
        self.assertEqual(sends, [(1, 2, "quux", False),
                                 (1, 2, "quux", False)])
        self.assertEqual(len(gets), 2)
        self.assertEqual(gets, [(1, 2, False), (1, 2, True)])

    def test_readstat(self):
        # Init
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        cls = self.target()
        cls.doquery = doquery_jig
        # Test empty
        cls.response = ""
        idlist = cls.readstat(42)
        self.assertEqual(idlist, [])
        self.assertEqual(queries, [(ntp.control.CTL_OP_READSTAT,
                                    42, "", False)])
        # Test normal
        queries = []
        cls.response = ntp.poly.polybytes("\xDE\xAD\xF0\x0D")
        idlist = cls.readstat()
        self.assertEqual(len(idlist), 1)
        self.assertEqual(isinstance(idlist[0], ntpp.Peer), True)
        self.assertEqual(idlist[0].associd, 0xDEAD)
        self.assertEqual(idlist[0].status, 0xF00D)
        self.assertEqual(queries, [(ntp.control.CTL_OP_READSTAT,
                                    0, "", False)])
        # Test incorrect response
        cls.response = "foo"
        try:
            cls.readstat()
            errored = False
        except ctlerr:
            errored = True
        self.assertEqual(errored, True)

    def test___parse_varlist(self):
        # Init
        cls = self.target()
        cls.response = 'srcadr=0.0.0.0, srcport=0, srchost="0.ubuntu.pool.ntp.org",\r\ndstadr=0.0.0.0, dstport=0, leap=3, stratum=16, precision=-22,\r\nrootdelay=0.000, rootdisp=0.000, refid=POOL,\r\nreftime=0x00000000.00000000, rec=0x00000000.00000000, reach=0x0,\r\nunreach=0, hmode=3, pmode=0, hpoll=6, ppoll=10, headway=0, flash=0x1600,\r\nkeyid=0, offset=0.000, delay=0.000, dispersion=16000.000, jitter=0.000,\r\nfiltdelay= 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00,\r\nfiltoffset= 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00,\r\nfiltdisp= 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00,\r\nnovalue, blankvalue=, quotedvalue="jabber"'
        # Test with basic packet
        self.assertEqual(cls._ControlSession__parse_varlist(),
                         odict((("srcadr", "0.0.0.0"), ("srcport", 0),
                                ("srchost", "0.ubuntu.pool.ntp.org"),
                                ("dstadr", "0.0.0.0"), ("dstport", 0),
                                ("leap", 3), ("stratum", 16),
                                ("precision", -22), ("rootdelay", 0.0),
                                ("rootdisp", 0.0), ("refid", "POOL"),
                                ("reftime", "0x00000000.00000000"),
                                ("rec", "0x00000000.00000000"),
                                ("reach", 0), ("unreach", 0), ("hmode", 3),
                                ("pmode", 0), ("hpoll", 6), ("ppoll", 10),
                                ("headway", 0), ("flash", 5632), ("keyid", 0),
                                ("offset", 0.0), ("delay-s", "0.000"),
                                ("delay", 0.0), ("dispersion", 16000.0),
                                ("jitter", 0.0),
                                ("filtdelay",
                                 "0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00"),
                                ("filtoffset",
                                 "0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00"),
                                ("filtdisp",
                                 "16000.00 16000.00 16000.00 16000.00 "
                                 "16000.00 16000.00 16000.00 16000.00"),
                                ("novalue", ""), ("blankvalue", ""),
                                ("quotedvalue", "jabber"))))
        # Test with basic packet, raw mode
        self.assertEqual(cls._ControlSession__parse_varlist(raw=True),
                         odict((("srcadr", ("0.0.0.0", "0.0.0.0")),
                                ("srcport", (0, "0")),
                                ("srchost", ("0.ubuntu.pool.ntp.org",
                                             "0.ubuntu.pool.ntp.org")),
                                ("dstadr", ("0.0.0.0", "0.0.0.0")),
                                ("dstport", (0, "0")),
                                ("leap", (3, "3")), ("stratum", (16, "16")),
                                ("precision", (-22, "-22")),
                                ("rootdelay", (0.0, "0.000")),
                                ("rootdisp", (0.0, "0.000")),
                                ("refid", ("POOL", "POOL")),
                                ("reftime", ("0x00000000.00000000",
                                             "0x00000000.00000000")),
                                ("rec", ("0x00000000.00000000",
                                         "0x00000000.00000000")),
                                ("reach", (0, "0x0")), ("unreach", (0, "0")),
                                ("hmode", (3, "3")), ("pmode", (0, "0")),
                                ("hpoll", (6, "6")), ("ppoll", (10, "10")),
                                ("headway", (0, "0")),
                                ("flash", (5632, "0x1600")),
                                ("keyid", (0, "0")),
                                ("offset", (0.0, "0.000")),
                                ("delay", (0.0, "0.000")),
                                ("dispersion", (16000.0, "16000.000")),
                                ("jitter", (0.0, "0.000")),
                                ("filtdelay",
                                 ("0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00",
                                  "0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00")),
                                ("filtoffset",
                                 ("0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00",
                                  "0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00")),
                                ("filtdisp",
                                 ("16000.00 16000.00 16000.00 16000.00 "
                                  "16000.00 16000.00 16000.00 16000.00",
                                  "16000.00 16000.00 16000.00 16000.00 "
                                  "16000.00 16000.00 16000.00 16000.00")),
                                ("novalue", ("", "")),
                                ("blankvalue", ("", "")),
                                ("quotedvalue", ("jabber", "jabber")))))

    def test_readvar(self):
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        # Init
        cls = self.target()
        cls.doquery = doquery_jig
        cls.response = "foo=bar, murphy=42"
        # Test basic
        result = cls.readvar()
        self.assertEqual(result, odict((("foo", "bar"), ("murphy", 42))))
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_READVAR, 0, "", False)])
        # Test raw
        queries = []
        result = cls.readvar(raw=True)
        self.assertEqual(result, odict((("foo", ("bar", "bar")),
                                        ("murphy", (42, "42")))))
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_READVAR, 0, "", False)])
        # Test with varlist
        queries = []
        result = cls.readvar(varlist=("foo", "bar", "quux"))
        self.assertEqual(result, odict((("foo", "bar"), ("murphy", 42))))
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_READVAR,
                           0, "foo,bar,quux", False)])

    def test_config(self):
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        # Init
        cls = self.target()
        cls.doquery = doquery_jig
        cls.response = ntp.poly.polybytes("Config Succeeded    \n \x00 blah blah")
        # Test success
        result = cls.config(ntp.poly.polybytes("Boo!"))
        self.assertEqual(result, True)
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_CONFIGURE, 0,
                           ntp.poly.polybytes("Boo!"), True)])
        # Test failure
        queries = []
        cls.response = ntp.poly.polybytes("whatever man...")
        result = cls.config(ntp.poly.polybytes("Boo!"))
        self.assertEqual(result, False)
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_CONFIGURE, 0,
                           ntp.poly.polybytes("Boo!"), True)])
        # Test no response
        queries = []
        cls.response = ""
        try:
            cls.config(ntp.poly.polybytes("blah"))
            errored = False
        except ctlerr as e:
            errored = e.message
        self.assertEqual(errored, ntpp.SERR_PERMISSION)

    def test_fetch_nonce(self):
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        # Init
        filefp = jigs.FileJig()
        cls = self.target()
        cls.doquery = doquery_jig
        # Test success
        cls.response = ntp.poly.polybytes("nonce=blah blah  ")
        result = cls.fetch_nonce()
        self.assertEqual(result, "nonce=blah blah")
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_REQ_NONCE, 0, "", False)])
        # Test failure
        queries = []
        cls.logfp = filefp
        cls.response = ntp.poly.polybytes("blah blah")
        try:
            result = cls.fetch_nonce()
            errored = False
        except ctlerr:
            errored = True
        self.assertEqual(errored, True)
        self.assertEqual(filefp.data, ["## Nonce expected: blah blah"])
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_REQ_NONCE, 0, "", False),
                          (ntp.control.CTL_OP_REQ_NONCE, 0, "", False),
                          (ntp.control.CTL_OP_REQ_NONCE, 0, "", False),
                          (ntp.control.CTL_OP_REQ_NONCE, 0, "", False)])

    def test_mrulist(self):

        def setresponse(data):  # needed for doquery_jig
            cls.response = data
        nonce_fetch_count = [0]

        def fetch_nonce_jig():
            nonce_fetch_count[0] += 1
            return "nonce=foo"
        queries = []
        qrm = ["addr.1=1.2.3.4:23,last.1=40,first.1=23,ct.1=1,mv.1=2,rs.1=3",
               "addr.2=1.2.3.4:23,last.2=41,first.2=23,ct.2=1,mv.2=2,rs.2=3",
               "addr.1=10.20.30.40:23,last.1=42,first.1=23,ct.1=1,"
               "mv.1=2,rs.1=3",
               "now=0x00000000.00000000"]
        query_results = qrm[:]  # qrm == query results master
        query_fail = [0]
        query_fail_code = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
            if query_fail[0] > 0:
                query_fail[0] -= 1
                code = query_fail_code.pop(0)
                raise ctlerr("foo", errorcode=code)
            if len(query_results) > 0:
                setresponse(query_results.pop(0))
        logjig = jigs.FileJig()
        # Init
        cls = self.target()
        cls.fetch_nonce = fetch_nonce_jig
        cls.doquery = doquery_jig
        cls.logfp = logjig
        faketimemod = jigs.TimeModuleJig()
        faketimemod.time_returns = [0] * 500
        try:
            timetemp = ntp.packet.time
            ntp.util.time = faketimemod
            # Test empty varlist
            result = cls.mrulist()
            self.assertEqual(nonce_fetch_count, [4])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=32", False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=1.2.3.4:23, "
                               "last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=1.2.3.4:23, "
                               "last.0=41, addr.1=1.2.3.4:23, last.1=40",
                               False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(isinstance(result, ntpp.MRUList), True)
            self.assertEqual(len(result.entries), 2)
            mru = result.entries[0]
            self.assertEqual(mru.addr, "1.2.3.4:23")
            self.assertEqual(mru.last, 41)
            self.assertEqual(mru.first, 23)
            self.assertEqual(mru.ct, 1)
            self.assertEqual(mru.mv, 2)
            self.assertEqual(mru.rs, 3)
            mru = result.entries[1]
            self.assertEqual(mru.addr, "10.20.30.40:23")
            self.assertEqual(mru.last, 42)
            self.assertEqual(mru.first, 23)
            self.assertEqual(mru.ct, 1)
            self.assertEqual(mru.mv, 2)
            self.assertEqual(mru.rs, 3)
            # Test with sort and frags (also test frag increment)
            nonce_fetch_count = [0]
            query_results = qrm[:]
            queries = []
            result = cls.mrulist(variables={"sort": "addr",
                                            "frags": 24,
                                            "resall": 5})
            self.assertEqual(nonce_fetch_count, [4])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=24, resall=0x5", False),
                              (10, 0,
                               "nonce=foo, frags=25, resall=0x5, "
                               "addr.0=1.2.3.4:23, last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, frags=26, resall=0x5, "
                               "addr.0=1.2.3.4:23, last.0=41, addr.1=1.2.3.4:23, "
                               "last.1=40", False),
                              (10, 0,
                               "nonce=foo, frags=27, resall=0x5, "
                               "addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(isinstance(result, ntpp.MRUList), True)
            self.assertEqual(len(result.entries), 2)
            mru = result.entries[0]
            self.assertEqual(mru.addr, "10.20.30.40:23")
            self.assertEqual(mru.last, 42)
            self.assertEqual(mru.first, 23)
            self.assertEqual(mru.ct, 1)
            self.assertEqual(mru.mv, 2)
            self.assertEqual(mru.rs, 3)
            mru = result.entries[1]
            self.assertEqual(mru.addr, "1.2.3.4:23")
            self.assertEqual(mru.last, 41)
            self.assertEqual(mru.first, 23)
            self.assertEqual(mru.ct, 1)
            self.assertEqual(mru.mv, 2)
            self.assertEqual(mru.rs, 3)
            # Test sorter error
            nonce_fetch_count = [0]
            try:
                cls.mrulist(variables={"sort": "foo"})
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_BADSORT % "foo")
            # Test varbind error
            nonce_fetch_count = [0]
            try:
                cls.mrulist(variables={"foo": 1})
                errored = False
            except ctlerr as e:
                errored = e.message
            self.assertEqual(errored, ntpp.SERR_BADPARAM % "foo")
            # Test add to request errors
            # Test None error
            nonce_fetch_count = [0]
            queries = []
            query_fail = [1]
            query_fail_code = [None]
            try:
                cls.mrulist()
                errored = False
            except ctlerr as e:
                errored = e.errorcode
            self.assertEqual(errored, None)
            # Test random error
            nonce_fetch_count = [0]
            queries = []
            query_fail = [1]
            query_fail_code = ["therdaglib"]
            try:
                cls.mrulist()
                errored = False
            except ctlerr as e:
                errored = e.errorcode
            self.assertEqual(errored, "therdaglib")
            # Test unknown var error
            nonce_fetch_count = [0]
            query_results = qrm[:]
            queries = []
            query_fail = [1]
            query_fail_code = [ntp.control.CERR_UNKNOWNVAR] * 2
            cls.response = ""
            result = cls.mrulist()
            self.assertEqual(nonce_fetch_count, [5])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=32", False),
                              (10, 0, "nonce=foo, frags=32", False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=1.2.3.4:23, last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=1.2.3.4:23, "
                               "last.0=41, addr.1=1.2.3.4:23, last.1=40", False),
                              (10, 0,
                               "nonce=foo, frags=32, addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(isinstance(result, ntpp.MRUList), True)
            self.assertEqual(len(result.entries), 2)
            # Test bad value error
            nonce_fetch_count = [0]
            query_results = qrm[:]
            queries = []
            query_fail = [2]
            query_fail_code = [ntp.control.CERR_BADVALUE] * 2
            cls.response = ""
            logjig.data = []
            cls.debug = 1
            result = cls.mrulist()
            self.assertEqual(nonce_fetch_count, [6])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=32", False),
                              (10, 0, "nonce=foo, limit=96", False),
                              (10, 0, "nonce=foo, limit=96", False),
                              (10, 0,
                               "nonce=foo, limit=96, addr.0=1.2.3.4:23, last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, limit=96, addr.0=1.2.3.4:23, "
                               "last.0=41, addr.1=1.2.3.4:23, last.1=40", False),
                              (10, 0,
                               "nonce=foo, limit=96, addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(isinstance(result, ntpp.MRUList), True)
            self.assertEqual(len(result.entries), 2)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Reverted to row limit "
                              "from fragments limit.\n",
                              "1970-01-01T00:00:00Z Row limit reduced to 96 "
                              "following CERR_BADVALUE.\n"])
            # Test incomplete error
            nonce_fetch_count = [0]
            query_results = qrm[:]
            queries = []
            query_fail = [3]
            query_fail_code = [ntpp.SERR_INCOMPLETE,
                               ntp.control.CERR_BADVALUE,  # Trigger cap_frags
                               ntpp.SERR_INCOMPLETE]
            cls.response = ""
            logjig.data = []
            cls.debug = 1
            result = cls.mrulist()
            self.assertEqual(nonce_fetch_count, [7])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=32", False),
                              (10, 0, "nonce=foo, frags=16", False),
                              (10, 0, "nonce=foo, limit=96", False),
                              (10, 0, "nonce=foo, limit=48", False),
                              (10, 0,
                               "nonce=foo, limit=49, addr.0=1.2.3.4:23, last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, limit=51, addr.0=1.2.3.4:23, "
                               "last.0=41, addr.1=1.2.3.4:23, last.1=40", False),
                              (10, 0,
                               "nonce=foo, limit=52, addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(len(result.entries), 2)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Frag limit reduced to 16 "
                              "following incomplete response.\n",
                              "1970-01-01T00:00:00Z Reverted to row limit "
                              "from fragments limit.\n",
                              "1970-01-01T00:00:00Z Row limit reduced to 48 "
                              "following  incomplete response.\n"])
            # Test timeout error
            nonce_fetch_count = [0]
            query_results = qrm[:]
            queries = []
            query_fail = [3]
            query_fail_code = [ntpp.SERR_TIMEOUT,
                               ntp.control.CERR_BADVALUE,  # Trigger cap_frags
                               ntpp.SERR_TIMEOUT]
            cls.response = ""
            logjig.data = []
            cls.debug = 1
            result = cls.mrulist()
            self.assertEqual(nonce_fetch_count, [7])
            self.assertEqual(queries,
                             [(10, 0, "nonce=foo, frags=32", False),
                              (10, 0, "nonce=foo, frags=16", False),
                              (10, 0, "nonce=foo, limit=96", False),
                              (10, 0, "nonce=foo, limit=48", False),
                              (10, 0,
                               "nonce=foo, limit=49, addr.0=1.2.3.4:23, last.0=40",
                               False),
                              (10, 0,
                               "nonce=foo, limit=51, addr.0=1.2.3.4:23, "
                               "last.0=41, addr.1=1.2.3.4:23, last.1=40", False),
                              (10, 0,
                               "nonce=foo, limit=52, addr.0=10.20.30.40:23, "
                               "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                               "addr.2=1.2.3.4:23, last.2=40", False)])
            self.assertEqual(len(result.entries), 2)
            self.assertEqual(logjig.data,
                             ["1970-01-01T00:00:00Z Frag limit reduced to 16 "
                              "following incomplete response.\n",
                              "1970-01-01T00:00:00Z Reverted to row limit "
                              "from fragments limit.\n",
                              "1970-01-01T00:00:00Z Row limit reduced to 48 "
                              "following  incomplete response.\n"])
        finally:
            ntp.util.time = timetemp

    def test___ordlist(self):
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        # Init
        cls = self.target()
        cls.doquery = doquery_jig
        # Test
        cls.response = "foo.0=42, bar.2=songbird"
        result = cls._ControlSession__ordlist("blah")
        self.assertEqual(result, [odict((("foo", 42),)), odict(),
                                  odict((("bar", "songbird"),))])
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_READ_ORDLIST_A,
                           0, "blah", True)])

    def test_reslist(self):
        ords = []

        def ordlist_jig(listtype):
            ords.append(listtype)
            return 23
        # Init
        cls = self.target()
        cls._ControlSession__ordlist = ordlist_jig
        # Test
        result = cls.reslist()
        self.assertEqual(result, 23)
        self.assertEqual(ords, ["addr_restrictions"])

    def test_ifstats(self):
        ords = []

        def ordlist_jig(listtype):
            ords.append(listtype)
            return 23
        # Init
        cls = self.target()
        cls._ControlSession__ordlist = ordlist_jig
        # Test
        result = cls.ifstats()
        self.assertEqual(result, 23)
        self.assertEqual(ords, ["ifstats"])


class TestAuthenticator(unittest.TestCase):
    target = ntpp.Authenticator
    open_calls = []
    open_files = []
    open_data = []

    def openjig(self, filename):
        self.open_calls.append(filename)
        fd = jigs.FileJig()
        fd.readline_return = self.open_data
        self.open_files.append(fd)
        return fd

    def test___init__(self):
        try:
            ntpp.open = self.openjig
            # Test without file
            cls = self.target()
            self.assertEqual(cls.passwords, {})
            self.assertEqual(self.open_calls, [])
            self.assertEqual(self.open_files, [])
            # Test with file
            self.open_data = ["# blah blah\n",
                              "0 foo sw0rdf1sh\n",
                              "\n",
                              "1 bar password1 # blargh\n"]
            cls = self.target("file")
            self.assertEqual(cls.passwords, {0: ("foo", "sw0rdf1sh"),
                                             1: ("bar", "password1")})
            self.assertEqual(self.open_calls, ["file"])
            self.assertEqual(len(self.open_files), 1)
        finally:
            ntpp.open = open
            self.open_calls = []
            self.open_files = []
            self.open_data = []

    def test___len__(self):
        cls = self.target()
        cls.passwords = {0: ("a", "z"),
                         2: ("b", "y")}
        self.assertEqual(cls.__len__(), 2)

    def test___getitem__(self):
        cls = self.target()
        cls.passwords = {0: ("a", "z"),
                         2: ("b", "y")}
        self.assertEqual(cls[2], ("b", "y"))

    def test_control(self):
        cls = self.target()
        cls.passwords = {0: ("a", "z"),
                         2: ("b", "y")}
        # Keyid in passwords
        self.assertEqual(cls.control(0), (0, "a", "z"))
        # Keyid not in passwords
        self.assertEqual(cls.control(1), (1, None, None))
        try:
            # Read keyid from /etc/ntp.conf
            ntpp.open = self.openjig
            self.open_data = ["blah blah", "control 2"]
            self.assertEqual(cls.control(), (2, "b", "y"))
            # Fail to read keyid from /etc/ntp.conf
            self.open_data = ["blah blah"]
            try:
                cls.control()
                errored = False
            except ValueError:
                errored = True
            self.assertEqual(errored, True)
        finally:
            ntpp.open = open

    def test_compute_mac(self):
        f = self.target.compute_mac
        try:
            temphash = ntpp.hashlib
            fakehashlibmod = jigs.HashlibModuleJig()
            ntpp.hashlib = fakehashlibmod
            # Test no digest
            self.assertEqual(f("", 0, None, ntp.poly.polybytes("")), None)
            # Test with digest
            self.assertEqual(f("foo", 0x42, "bar", "quux"),
                             ntp.poly.polybytes("\x00\x00\x00\x42blahblahblahblah"))
        finally:
            ntpp.hashlib = temphash

    def test_have_mac(self):
        f = self.target.have_mac
        # Test under limit
        pkt = "@" * ntp.magic.LEN_PKT_NOMAC
        self.assertEqual(f(pkt), False)
        # Test over limit
        pkt += "foo"
        self.assertEqual(f(pkt), True)

    def test_verify_mac(self):
        cls = self.target()
        cls.passwords[0x23] = ("a", "z")
        good_pkt = "foobar\x00\x00\x00\x23blahblahblahblah"
        bad_pkt = "foobar\xDE\xAD\xDE\xAFblahblahblah"
        try:
            temphash = ntpp.hashlib
            fakehashlibmod = jigs.HashlibModuleJig()
            ntpp.hashlib = fakehashlibmod
            # Test good
            self.assertEqual(cls.verify_mac(ntp.poly.polybytes(good_pkt)), True)
            # Test bad
            self.assertEqual(cls.verify_mac(ntp.poly.polybytes(bad_pkt)), False)
        finally:
            ntpp.hashlib = temphash


if __name__ == "__main__":
    unittest.main()
