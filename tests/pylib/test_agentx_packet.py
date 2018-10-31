#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.agentx_packet as AX

from ntp.agentx_packet import slicedata, decode_pduheader, makeflags

extraData = b"Would you kindly ignore this?"

maximumOIDsubs = tuple(range(1, 129))
maximumOIDstr = b"""\x80\x00\x00\x00\
\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04\
\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x07\x00\x00\x00\x08\
\x00\x00\x00\x09\x00\x00\x00\x0A\x00\x00\x00\x0B\x00\x00\x00\x0C\
\x00\x00\x00\x0D\x00\x00\x00\x0E\x00\x00\x00\x0F\x00\x00\x00\x10\
\
\x00\x00\x00\x11\x00\x00\x00\x12\x00\x00\x00\x13\x00\x00\x00\x14\
\x00\x00\x00\x15\x00\x00\x00\x16\x00\x00\x00\x17\x00\x00\x00\x18\
\x00\x00\x00\x19\x00\x00\x00\x1A\x00\x00\x00\x1B\x00\x00\x00\x1C\
\x00\x00\x00\x1D\x00\x00\x00\x1E\x00\x00\x00\x1F\x00\x00\x00\x20\
\
\x00\x00\x00\x21\x00\x00\x00\x22\x00\x00\x00\x23\x00\x00\x00\x24\
\x00\x00\x00\x25\x00\x00\x00\x26\x00\x00\x00\x27\x00\x00\x00\x28\
\x00\x00\x00\x29\x00\x00\x00\x2A\x00\x00\x00\x2B\x00\x00\x00\x2C\
\x00\x00\x00\x2D\x00\x00\x00\x2E\x00\x00\x00\x2F\x00\x00\x00\x30\
\
\x00\x00\x00\x31\x00\x00\x00\x32\x00\x00\x00\x33\x00\x00\x00\x34\
\x00\x00\x00\x35\x00\x00\x00\x36\x00\x00\x00\x37\x00\x00\x00\x38\
\x00\x00\x00\x39\x00\x00\x00\x3A\x00\x00\x00\x3B\x00\x00\x00\x3C\
\x00\x00\x00\x3D\x00\x00\x00\x3E\x00\x00\x00\x3F\x00\x00\x00\x40\
\
\x00\x00\x00\x41\x00\x00\x00\x42\x00\x00\x00\x43\x00\x00\x00\x44\
\x00\x00\x00\x45\x00\x00\x00\x46\x00\x00\x00\x47\x00\x00\x00\x48\
\x00\x00\x00\x49\x00\x00\x00\x4A\x00\x00\x00\x4B\x00\x00\x00\x4C\
\x00\x00\x00\x4D\x00\x00\x00\x4E\x00\x00\x00\x4F\x00\x00\x00\x50\
\
\x00\x00\x00\x51\x00\x00\x00\x52\x00\x00\x00\x53\x00\x00\x00\x54\
\x00\x00\x00\x55\x00\x00\x00\x56\x00\x00\x00\x57\x00\x00\x00\x58\
\x00\x00\x00\x59\x00\x00\x00\x5A\x00\x00\x00\x5B\x00\x00\x00\x5C\
\x00\x00\x00\x5D\x00\x00\x00\x5E\x00\x00\x00\x5F\x00\x00\x00\x60\
\
\x00\x00\x00\x61\x00\x00\x00\x62\x00\x00\x00\x63\x00\x00\x00\x64\
\x00\x00\x00\x65\x00\x00\x00\x66\x00\x00\x00\x67\x00\x00\x00\x68\
\x00\x00\x00\x69\x00\x00\x00\x6A\x00\x00\x00\x6B\x00\x00\x00\x6C\
\x00\x00\x00\x6D\x00\x00\x00\x6E\x00\x00\x00\x6F\x00\x00\x00\x70\
\
\x00\x00\x00\x71\x00\x00\x00\x72\x00\x00\x00\x73\x00\x00\x00\x74\
\x00\x00\x00\x75\x00\x00\x00\x76\x00\x00\x00\x77\x00\x00\x00\x78\
\x00\x00\x00\x79\x00\x00\x00\x7A\x00\x00\x00\x7B\x00\x00\x00\x7C\
\x00\x00\x00\x7D\x00\x00\x00\x7E\x00\x00\x00\x7F\x00\x00\x00\x80\
"""

# The most commonly used flag setups, some tests use custom flags
standardFlags_bare = makeflags(False, False, False, False, True)
standardFlags = {"flags": standardFlags_bare}
lilEndianFlags = {"flags": makeflags(False, False, False, False, False)}
contextFlags = {"flags": makeflags(False, False, False, True, True)}


def test_pducore(tester, pdu, pduType, endian, sID, tactID, pktID):
    tester.assertEqual(pdu.pduType, pduType)
    tester.assertEqual(pdu.bigEndian, endian)
    tester.assertEqual(pdu.sessionID, sID)
    tester.assertEqual(pdu.transactionID, tactID)
    tester.assertEqual(pdu.packetID, pktID)


class TestNtpclientsNtpsnmpd(unittest.TestCase):
    #
    # PDU tests
    #
    def test_AgentXPDU(self):
        cls = AX.AgentXPDU
        # Test these so we don't need a bunch of redundant tests
        # Test basic, without context
        test = cls(0, True, 1, 2, 3, context=extraData)
        self.assertEqual(repr(test),
                         "AgentXPDU(bigEndian=True, packetID=3, "
                         "pduType=0, sessionID=1, transactionID=2)")
        # Test basic, with context
        test = cls(0, True, 1, 2, 3, hascontext=True)
        self.assertEqual(repr(test),
                         "AgentXPDU(bigEndian=True, context=None, packetID=3, "
                         "pduType=0, sessionID=1, transactionID=2)")
        # Test with added variables
        test.foo = 42
        test._wyk = extraData  # this should be ignored
        test.context = "jabber jabber jabber"
        self.assertEqual(repr(test),
                         "AgentXPDU(bigEndian=True, "
                         "context='jabber jabber jabber', "
                         "foo=42, packetID=3, "
                         "pduType=0, sessionID=1, transactionID=2)")
        # Test __eq__
        a = cls(0, True, 1, 2, 3)
        b = cls(0, True, 1, 2, 3)
        # Test all equal
        self.assertEqual(a == b, True)
        # Test same class, false
        b = "blah blah"
        self.assertEqual(a == b, False)
        # Test different pdu
        b = cls(1, True, 1, 2, 3)
        self.assertEqual(a == b, False)
        # Test different endianness
        b = cls(0, False, 1, 2, 3)
        self.assertEqual(a == b, False)
        # Test different session ID
        b = cls(0, True, 42, 2, 3)
        self.assertEqual(a == b, False)
        # Test different transaction ID
        b = cls(0, True, 1, 23, 3)
        self.assertEqual(a == b, False)
        # Test different packet ID
        b = cls(0, True, 1, 2, 13)
        self.assertEqual(a == b, False)
        # Test different _hascontext
        b = cls(0, True, 1, 2, 3, hascontext=True)
        self.assertEqual(a == b, False)
        # Test with context, equal
        a = cls(0, True, 1, 2, 3, True, "foo")
        b = cls(0, True, 1, 2, 3, True, "foo")
        self.assertEqual(a == b, True)
        # Test with context, not equal
        b = cls(0, True, 1, 2, 3, True, "bar")
        self.assertEqual(a == b, False)

    def test_OpenPDU(self):
        dec = AX.decode_OpenPDU
        cls = AX.OpenPDU

        # Test PDU init, null packet
        nullPkt = cls(True, 1, 2, 3, 4, (), "")
        test_pducore(self, nullPkt, AX.PDU_OPEN, True, 1, 2, 3)
        self.assertEqual(nullPkt.timeout, 4)
        self.assertEqual(nullPkt.oid, AX.OID((), False))
        self.assertEqual(nullPkt.description, "")
        # Test PDU init, basic packet
        basicPkt = cls(False, 1, 2, 3, 4, (1, 2, 3, 4), "foo")
        test_pducore(self, basicPkt, AX.PDU_OPEN, False, 1, 2, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.oid, AX.OID((1, 2, 3, 4), False))
        self.assertEqual(basicPkt.description, "foo")
        # Test encoding, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         b"\x01\x01\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x0C"
                         b"\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
        # Test encoding, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         b"\x01\x01\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x20\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00foo\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        test_pducore(self, nullPkt_new, AX.PDU_OPEN, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.timeout, 4)
        self.assertEqual(nullPkt_new.oid, AX.OID((), False))
        self.assertEqual(nullPkt_new.description, "")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        test_pducore(self, basicPkt_new, AX.PDU_OPEN, False, 1, 2, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.oid, AX.OID((1, 2, 3, 4), False))
        self.assertEqual(basicPkt_new.description, "foo")
        # Test packetVars
        self.assertEqual(basicPkt_new.packetVars(),
                         {"pduType": 1,
                          "bigEndian": False,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "timeout": 4,
                          "oid": AX.OID((1, 2, 3, 4), False),
                          "description": "foo"})

    def test_ClosePDU(self):
        dec = AX.decode_ClosePDU
        cls = AX.ClosePDU

        # Test init
        pkt = cls(True, 1, 2, 3, AX.RSN_OTHER)
        test_pducore(self, pkt, AX.PDU_CLOSE, True, 1, 2, 3)
        self.assertEqual(pkt.reason, AX.RSN_OTHER)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, AX.RSN_OTHER)
        test_pducore(self, pkt_LE, AX.PDU_CLOSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE.reason, AX.RSN_OTHER)
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x02\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x01\x00\x00\x00")
        # Test encoding, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x02\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x01\x00\x00\x00")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_CLOSE, True, 1, 2, 3)
        self.assertEqual(pkt_new.reason, AX.RSN_OTHER)
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_CLOSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.reason, AX.RSN_OTHER)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 2,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "reason": AX.RSN_OTHER})

    def test_RegisterPDU(self):
        dec = AX.decode_xRegisterPDU
        cls = AX.RegisterPDU

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3))
        test_pducore(self, basicPkt, AX.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, basic packet, little endian
        basicPkt_LE = cls(False, 1, 2, 3, 4, 5, (1, 2, 3))
        test_pducore(self, basicPkt_LE, AX.PDU_REGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE.timeout, 4)
        self.assertEqual(basicPkt_LE.priority, 5)
        self.assertEqual(basicPkt_LE.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE.rangeSubid, 0)
        self.assertEqual(basicPkt_LE.upperBound, None)
        self.assertEqual(basicPkt_LE.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        test_pducore(self, fancyPkt, AX.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt.timeout, 4)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(fancyPkt.rangeSubid, 5)
        self.assertEqual(fancyPkt.upperBound, 23)
        self.assertEqual(fancyPkt.context, "blah")
        # Test encode, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         b"\x01\x03\x11\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x14"
                         b"\x04\x05\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test encode, basic packet, little endian
        basicPkt_LE_str = basicPkt_LE.encode()
        self.assertEqual(basicPkt_LE_str,
                         b"\x01\x03\x01\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x14\x00\x00\x00"
                         b"\x04\x05\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00")
        # Test encode, fancy packet
        fancyPkt_str = fancyPkt.encode()
        self.assertEqual(fancyPkt_str,
                         b"\x01\x03\x19\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x20"
                         b"\x00\x00\x00\x04blah"
                         b"\x04\x05\x05\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x17")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        test_pducore(self, basicPkt_new, AX.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, basic packet, little endian
        header, body = slicedata(basicPkt_LE_str, 20)
        header = decode_pduheader(header)
        basicPkt_LE_new = dec(body, header)
        test_pducore(self, basicPkt_LE_new, AX.PDU_REGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE_new.timeout, 4)
        self.assertEqual(basicPkt_LE_new.priority, 5)
        self.assertEqual(basicPkt_LE_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE_new.rangeSubid, 0)
        self.assertEqual(basicPkt_LE_new.upperBound, None)
        self.assertEqual(basicPkt_LE_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        test_pducore(self, fancyPkt_new, AX.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt_new.timeout, 4)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(fancyPkt_new.rangeSubid, 5)
        self.assertEqual(fancyPkt_new.upperBound, 23)
        self.assertEqual(fancyPkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(basicPkt_new.packetVars(),
                         {"pduType": 3,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "timeout": 4,
                          "priority": 5,
                          "subtree": AX.OID((1, 2, 3), False),
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})
        # Test __eq__ gap, equal
        a = cls(True, 1, 2, 3, 4, 5, ())
        b = cls(True, 1, 2, 3, 4, 5, ())
        self.assertEqual(a == b, True)
        # Test __eq__ gap, unequal
        b = cls(False, 1, 2, 3, 4, 5, ())
        self.assertEqual(a == b, False)

    def test_UnregisterPDU(self):
        dec = AX.decode_xRegisterPDU
        cls = AX.UnregisterPDU

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 5, (1, 2, 3))
        test_pducore(self, basicPkt, AX.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, basic packet, little endian
        basicPkt_LE = cls(False, 1, 2, 3, 5, (1, 2, 3))
        test_pducore(self, basicPkt_LE, AX.PDU_UNREGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE.priority, 5)
        self.assertEqual(basicPkt_LE.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE.rangeSubid, 0)
        self.assertEqual(basicPkt_LE.upperBound, None)
        self.assertEqual(basicPkt_LE.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        test_pducore(self, fancyPkt, AX.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(fancyPkt.rangeSubid, 5)
        self.assertEqual(fancyPkt.upperBound, 23)
        self.assertEqual(fancyPkt.context, "blah")
        # Test encode, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         b"\x01\x04\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x14"
                         b"\x00\x05\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test encode, basic packet, little endian
        basicPkt_LE_str = basicPkt_LE.encode()
        self.assertEqual(basicPkt_LE_str,
                         b"\x01\x04\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x14\x00\x00\x00"
                         b"\x00\x05\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00")
        # Test encode, fancy packet
        fancyPkt_str = fancyPkt.encode()
        self.assertEqual(fancyPkt_str,
                         b"\x01\x04\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x20"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x05\x05\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x17")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        test_pducore(self, basicPkt, AX.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, basic packet, little endian
        header, body = slicedata(basicPkt_LE_str, 20)
        header = decode_pduheader(header)
        basicPkt_LE_new = dec(body, header)
        test_pducore(self, basicPkt_LE, AX.PDU_UNREGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE_new.priority, 5)
        self.assertEqual(basicPkt_LE_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE_new.rangeSubid, 0)
        self.assertEqual(basicPkt_LE_new.upperBound, None)
        self.assertEqual(basicPkt_LE_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        test_pducore(self, fancyPkt_new, AX.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, AX.OID((1, 2, 3), False))
        self.assertEqual(fancyPkt_new.rangeSubid, 5)
        self.assertEqual(fancyPkt_new.upperBound, 23)
        self.assertEqual(fancyPkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(basicPkt_new.packetVars(),
                         {"pduType": 4,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "priority": 5,
                          "subtree": AX.OID((1, 2, 3), False),
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})

    def test_GetPDU(self):
        dec = AX.decode_xGetPDU
        cls = AX.GetPDU
        srch = AX.SearchRange

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        test_pducore(self, nullPkt, AX.PDU_GET, True, 1, 2, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (srch((1, 2, 3), (1, 2, 5), False),
                       srch((10, 20), (30, 40), True)),
                      context="blah")
        test_pducore(self, fullPkt, AX.PDU_GET, True, 1, 2, 3)
        self.assertEqual(fullPkt.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test init, full packet, little endian
        fullPkt_LE = cls(False, 1, 2, 3,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)),
                         context="blah")
        test_pducore(self, fullPkt_LE, AX.PDU_GET, False, 1, 2, 3)
        self.assertEqual(fullPkt_LE.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_LE.context, "blah")
        # Test encode, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         b"\x01\x05\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, full packet
        fullPkt_str = fullPkt.encode()
        self.assertEqual(fullPkt_str,
                         b"\x01\x05\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x40"
                         b"\x00\x00\x00\x04blah"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x05"
                         b"\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         b"\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28")
        # Test encode, full packet, little endian
        fullPkt_LE_str = fullPkt_LE.encode()
        self.assertEqual(fullPkt_LE_str,
                         b"\x01\x05\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x40\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x05\x00\x00\x00"
                         b"\x02\x00\x01\x00\x0A\x00\x00\x00\x14\x00\x00\x00"
                         b"\x02\x00\x00\x00\x1E\x00\x00\x00\x28\x00\x00\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        test_pducore(self, nullPkt_new, AX.PDU_GET, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        test_pducore(self, fullPkt_new, AX.PDU_GET, True, 1, 2, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_new.context, "blah")
        # Test decoding, full packet, little endian
        header, body = slicedata(fullPkt_LE_str, 20)
        header = decode_pduheader(header)
        fullPkt_LE_new = dec(body, header)
        test_pducore(self, fullPkt_LE_new, AX.PDU_GET, False, 1, 2, 3)
        self.assertEqual(fullPkt_LE_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(nullPkt_new.packetVars(),
                         {"pduType": 5,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oidranges": (),
                          "context": None})

    def test_GetNextPDU(self):
        dec = AX.decode_xGetPDU
        cls = AX.GetNextPDU
        srch = AX.SearchRange

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        test_pducore(self, nullPkt, AX.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (srch((1, 2, 3), (1, 2, 5), False),
                       srch((10, 20), (30, 40), True)),
                      context="blah")
        test_pducore(self, fullPkt, AX.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(fullPkt.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test init, full packet, little endian
        fullPkt_LE = cls(False, 1, 2, 3,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)),
                         context="blah")
        test_pducore(self, fullPkt_LE, AX.PDU_GET_NEXT, False, 1, 2, 3)
        self.assertEqual(fullPkt_LE.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_LE.context, "blah")
        # Test encode, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         b"\x01\x06\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, full packet
        fullPkt_str = fullPkt.encode()
        self.assertEqual(fullPkt_str,
                         b"\x01\x06\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x40"
                         b"\x00\x00\x00\x04blah"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x05"
                         b"\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         b"\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28")
        # Test encode, full packet, little endian
        fullPkt_LE_str = fullPkt_LE.encode()
        self.assertEqual(fullPkt_LE_str,
                         b"\x01\x06\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x40\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x05\x00\x00\x00"
                         b"\x02\x00\x01\x00\x0A\x00\x00\x00\x14\x00\x00\x00"
                         b"\x02\x00\x00\x00\x1E\x00\x00\x00\x28\x00\x00\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        test_pducore(self, nullPkt_new, AX.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        test_pducore(self, fullPkt_new, AX.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_new.context, "blah")
        # Test decoding, full packet, little endian
        header, body = slicedata(fullPkt_LE_str, 20)
        header = decode_pduheader(header)
        fullPkt_LE_new = dec(body, header)
        test_pducore(self, fullPkt_LE_new, AX.PDU_GET_NEXT, False, 1, 2, 3)
        self.assertEqual(fullPkt_LE_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(nullPkt_new.packetVars(),
                         {"pduType": 6,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oidranges": (),
                          "context": None})

    def test_GetBulkPDU(self):
        dec = AX.decode_GetBulkPDU
        cls = AX.GetBulkPDU
        srch = AX.SearchRange

        # Test init
        pkt = cls(True, 1, 2, 3, 1, 5,
                  (srch((1, 2), (3, 4), False),
                   srch((6, 7), (8, 9), True)),
                  context="blah")
        test_pducore(self, pkt, AX.PDU_GET_BULK, True, 1, 2, 3)
        self.assertEqual(pkt.nonReps, 1)
        self.assertEqual(pkt.maxReps, 5)
        self.assertEqual(pkt.oidranges,
                         (srch((1, 2), (3, 4), False),
                          srch((6, 7), (8, 9), True)))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, 1, 5,
                     (srch((1, 2), (3, 4), False),
                      srch((6, 7), (8, 9), True)),
                     context="blah")
        test_pducore(self, pkt_LE, AX.PDU_GET_BULK, False, 1, 2, 3)
        self.assertEqual(pkt_LE.nonReps, 1)
        self.assertEqual(pkt_LE.maxReps, 5)
        self.assertEqual(pkt_LE.oidranges,
                         (srch((1, 2), (3, 4), False),
                          srch((6, 7), (8, 9), True)))
        self.assertEqual(pkt_LE.context, "blah")
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x07\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x3C"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x01\x00\x05"
                         b"\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                         b"\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09")
        # Test encoding, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x07\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x3C\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x01\x00\x05\x00"
                         b"\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x02\x00\x01\x00\x06\x00\x00\x00\x07\x00\x00\x00"
                         b"\x02\x00\x00\x00\x08\x00\x00\x00\x09\x00\x00\x00")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_GET_BULK, True, 1, 2, 3)
        self.assertEqual(pkt_new.nonReps, 1)
        self.assertEqual(pkt_new.maxReps, 5)
        self.assertEqual(pkt_new.oidranges,
                         (srch((1, 2), (3, 4), False),
                          srch((6, 7), (8, 9), True)))
        self.assertEqual(pkt_new.context, "blah")
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_GET_BULK, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.nonReps, 1)
        self.assertEqual(pkt_LE_new.maxReps, 5)
        self.assertEqual(pkt_LE_new.oidranges,
                         (srch((1, 2), (3, 4), False),
                          srch((6, 7), (8, 9), True)))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 7,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "nonReps": 1,
                          "maxReps": 5,
                          "oidranges": (srch((1, 2), (3, 4), False),
                                        srch((6, 7), (8, 9), True)),
                          "context": "blah"})

    def test_TestSetPDU(self):
        dec = AX.decode_TestSetPDU
        cls = AX.TestSetPDU

        # Test init
        pkt = cls(True, 1, 2, 3,
                  (AX.Varbind(AX.VALUE_OID, (1, 2, 3), AX.OID((4, 5, 6), False)),
                   AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, AX.PDU_TEST_SET, True, 1, 2, 3)
        self.assertEqual(pkt.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3,
                     (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                 AX.OID((4, 5, 6), False)),
                      AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, AX.PDU_TEST_SET, False, 1, 2, 3)
        self.assertEqual(pkt_LE.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE.context, "blah")
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x08\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x48"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x06\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x04\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x04"
                         b"\x00\x00\x00\x04blah")
        # Test encoding, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x08\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x48\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x06\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_TEST_SET, True, 1, 2, 3)
        self.assertEqual(pkt_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID,
                                     AX.OID((1, 2, 3), False),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR,
                                     AX.OID((1, 2, 4), False),
                                     "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_TEST_SET, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID,
                                     AX.OID((1, 2, 3), False),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR,
                                     AX.OID((1, 2, 4), False),
                                     "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 8,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": (AX.Varbind(AX.VALUE_OID,
                                                  AX.OID((1, 2, 3), False),
                                                  AX.OID((4, 5, 6), False)),
                                       AX.Varbind(AX.VALUE_OCTET_STR,
                                                  AX.OID((1, 2, 4), False),
                                                  "blah")),
                          "context": "blah"})

    def test_CommitSetPDU(self):
        dec = AX.decode_CommitSetPDU
        cls = AX.CommitSetPDU

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, AX.PDU_COMMIT_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, AX.PDU_COMMIT_SET, False, 1, 2, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x09\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x09\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_COMMIT_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_COMMIT_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 9,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_UndoSetPDU(self):
        dec = AX.decode_UndoSetPDU
        cls = AX.UndoSetPDU

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, AX.PDU_UNDO_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, AX.PDU_UNDO_SET, False, 1, 2, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0A\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0A\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_UNDO_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_UNDO_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 10,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_CleanupSetPDU(self):
        dec = AX.decode_CleanupSetPDU
        cls = AX.CleanupSetPDU

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, AX.PDU_CLEANUP_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, AX.PDU_CLEANUP_SET, False, 1, 2, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0B\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0B\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_CLEANUP_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_CLEANUP_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 11,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_PingPDU(self):
        dec = AX.decode_PingPDU
        cls = AX.PingPDU

        # Test init
        pkt = cls(True, 1, 2, 3, "blah")
        test_pducore(self, pkt, AX.PDU_PING, True, 1, 2, 3)
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, "blah")
        test_pducore(self, pkt_LE, AX.PDU_PING, False, 1, 2, 3)
        self.assertEqual(pkt_LE.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0D\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x08"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0D\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x08\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_PING, True, 1, 2, 3)
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_PING, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 13,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "context": "blah"})

    def test_NotifyPDU(self):
        dec = AX.decode_NotifyPDU
        cls = AX.NotifyPDU

        # Test init
        pkt = cls(True, 1, 2, 3,
                  (AX.Varbind(AX.VALUE_OID, (1, 2, 3), AX.OID((4, 5, 6), False)),
                   AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, AX.PDU_NOTIFY, True, 1, 2, 3)
        self.assertEqual(pkt.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3,
                     (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                 AX.OID((4, 5, 6), False)),
                      AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, AX.PDU_NOTIFY, False, 1, 2, 3)
        self.assertEqual(pkt_LE.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0C\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x48"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x06\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x04\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x04"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0C\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x48\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x06\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_NOTIFY, True, 1, 2, 3)
        self.assertEqual(pkt_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_NOTIFY, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 12,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                                  AX.OID((4, 5, 6), False)),
                                       AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4),
                                                  "blah")),
                          "context": "blah"})

    def test_IndexAllocPDU(self):
        dec = AX.decode_xIndexAllocPDU
        cls = AX.IndexAllocPDU

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  (AX.Varbind(AX.VALUE_OID, (1, 2, 3), AX.OID((4, 5, 6), False)),
                   AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, AX.PDU_INDEX_ALLOC, True, 1, 2, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, True, True,
                     (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                 AX.OID((4, 5, 6), False)),
                      AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, AX.PDU_INDEX_ALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE.newIndex, True)
        self.assertEqual(pkt_LE.anyIndex, True)
        self.assertEqual(pkt_LE.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0E\x1E\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x48"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x06\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x04\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x04"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0E\x0E\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x48\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x06\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_INDEX_ALLOC, True, 1, 2, 3)
        self.assertEqual(pkt_new.newIndex, True)
        self.assertEqual(pkt_new.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_INDEX_ALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.newIndex, True)
        self.assertEqual(pkt_LE_new.anyIndex, True)
        self.assertEqual(pkt_LE_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 14,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "newIndex": True,
                          "anyIndex": True,
                          "varbinds": (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                                  AX.OID((4, 5, 6), False)),
                                       AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4),
                                                  "blah")),
                          "context": "blah"})

    def test_IndexDeallocPDU(self):
        dec = AX.decode_xIndexAllocPDU
        cls = AX.IndexDeallocPDU

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  (AX.Varbind(AX.VALUE_OID, (1, 2, 3), AX.OID((4, 5, 6), False)),
                   AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, AX.PDU_INDEX_DEALLOC, True, 1, 2, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, True, True,
                     (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                 AX.OID((4, 5, 6), False)),
                      AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, AX.PDU_INDEX_DEALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE.newIndex, True)
        self.assertEqual(pkt_LE.anyIndex, True)
        self.assertEqual(pkt_LE.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x0F\x1E\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x48"
                         b"\x00\x00\x00\x04blah"
                         b"\x00\x06\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x04\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x04"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x0F\x0E\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x48\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x06\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_INDEX_DEALLOC, True, 1, 2, 3)
        self.assertEqual(pkt_new.newIndex, True)
        self.assertEqual(pkt_new.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_INDEX_DEALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.newIndex, True)
        self.assertEqual(pkt_LE_new.anyIndex, True)
        self.assertEqual(pkt_LE_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 15,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "newIndex": True,
                          "anyIndex": True,
                          "varbinds": (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                                  AX.OID((4, 5, 6), False)),
                                       AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4),
                                                  "blah")),
                          "context": "blah"})

    def test_AddAgentCapsPDU(self):
        dec = AX.decode_AddAgentCapsPDU
        cls = AX.AddAgentCapsPDU

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), "blah", context="bluh")
        test_pducore(self, pkt, AX.PDU_ADD_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt.description, "blah")
        self.assertEqual(pkt.context, "bluh")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, (4, 5, 6), "blah", context="bluh")
        test_pducore(self, pkt_LE, AX.PDU_ADD_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE.description, "blah")
        self.assertEqual(pkt_LE.context, "bluh")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x10\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x20"
                         b"\x00\x00\x00\x04bluh"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x10\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x20\x00\x00\x00"
                         b"\x04\x00\x00\x00bluh"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_ADD_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt_new.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_new.description, "blah")
        self.assertEqual(pkt_new.context, "bluh")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_ADD_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE_new.description, "blah")
        self.assertEqual(pkt_LE_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 16,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": AX.OID((4, 5, 6), False),
                          "description": "blah",
                          "context": "bluh"})

    def test_RMAgentCapsPDU(self):
        dec = AX.decode_RMAgentCapsPDU
        cls = AX.RMAgentCapsPDU

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), context="bluh")
        test_pducore(self, pkt, AX.PDU_RM_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt.context, "bluh")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, (4, 5, 6), context="bluh")
        test_pducore(self, pkt_LE, AX.PDU_RM_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE.context, "bluh")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x11\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x18"
                         b"\x00\x00\x00\x04bluh"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x11\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x18\x00\x00\x00"
                         b"\x04\x00\x00\x00bluh"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_RM_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt_new.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_new.context, "bluh")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_RM_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.oid, AX.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 17,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": AX.OID((4, 5, 6), False),
                          "context": "bluh"})

    def test_ResponsePDU(self):
        dec = AX.decode_ResponsePDU
        cls = AX.ResponsePDU

        # Test init
        pkt = cls(True, 1, 2, 3, 4, 5, 6,
                  (AX.Varbind(AX.VALUE_OID, (1, 2, 3), AX.OID((4, 5, 6), False)),
                   AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        test_pducore(self, pkt, AX.PDU_RESPONSE, True, 1, 2, 3)
        self.assertEqual(pkt.sysUptime, 4)
        self.assertEqual(pkt.resError, 5)
        self.assertEqual(pkt.resIndex, 6)
        self.assertEqual(pkt.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, 4, 5, 6,
                     (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                 AX.OID((4, 5, 6), False)),
                      AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        test_pducore(self, pkt_LE, AX.PDU_RESPONSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE.sysUptime, 4)
        self.assertEqual(pkt_LE.resError, 5)
        self.assertEqual(pkt_LE.resIndex, 6)
        self.assertEqual(pkt_LE.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         b"\x01\x12\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x48"
                         b"\x00\x00\x00\x04\x00\x05\x00\x06"
                         b"\x00\x06\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x04\x00\x00"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x04"
                         b"\x00\x00\x00\x04blah")
        # Test encode, little endian
        pkt_LE_str = pkt_LE.encode()
        self.assertEqual(pkt_LE_str,
                         b"\x01\x12\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x48\x00\x00\x00"
                         b"\x04\x00\x00\x00\x05\x00\x06\x00"
                         b"\x06\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        test_pducore(self, pkt_new, AX.PDU_RESPONSE, True, 1, 2, 3)
        self.assertEqual(pkt_new.sysUptime, 4)
        self.assertEqual(pkt_new.resError, 5)
        self.assertEqual(pkt_new.resIndex, 6)
        self.assertEqual(pkt_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, AX.PDU_RESPONSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.sysUptime, 4)
        self.assertEqual(pkt_LE_new.resError, 5)
        self.assertEqual(pkt_LE_new.resIndex, 6)
        self.assertEqual(pkt_LE_new.varbinds,
                         (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                     AX.OID((4, 5, 6), False)),
                          AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 18,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "sysUptime": 4,
                          "resError": 5,
                          "resIndex": 6,
                          "varbinds": (AX.Varbind(AX.VALUE_OID, (1, 2, 3),
                                                  AX.OID((4, 5, 6), False)),
                                       AX.Varbind(AX.VALUE_OCTET_STR, (1, 2, 4),
                                                  "blah"))})

    #
    # Data type tests
    #
    def test_integer32(self):
        enc = AX.encode_integer32
        dec = AX.decode_integer32

        # Encode
        self.assertEqual(enc(True, 42), b"\x00\x00\x00\x2A")
        # Encode, little endian
        self.assertEqual(enc(False, 42), b"\x2A\x00\x00\x00")
        # Decode
        self.assertEqual(dec(b"\x00\x00\x00\x2A" + extraData, standardFlags),
                         (42, extraData))
        # Decode, little endian
        self.assertEqual(dec(b"\x2A\x00\x00\x00" + extraData, lilEndianFlags),
                         (42, extraData))

    def test_nullvalue(self):
        enc = AX.encode_nullvalue
        dec = AX.decode_nullvalue

        # Encode
        self.assertEqual(enc(True, "this is ignored"), b"")
        # Decode
        self.assertEqual(dec(extraData, standardFlags), (None, extraData))

    def test_integer64(self):
        enc = AX.encode_integer64
        dec = AX.decode_integer64

        # Encode
        self.assertEqual(enc(True, 42), b"\x00\x00\x00\x00\x00\x00\x00\x2A")
        # Encode, little endian
        self.assertEqual(enc(False, 42), b"\x2A\x00\x00\x00\x00\x00\x00\x00")
        # Decode
        self.assertEqual(dec(b"\x00\x00\x00\x00\x00\x00\x00\x2A" + extraData,
                             standardFlags),
                         (42, extraData))
        # Decode, little endian
        self.assertEqual(dec(b"\x2A\x00\x00\x00\x00\x00\x00\x00" + extraData,
                             lilEndianFlags),
                         (42, extraData))

    def test_ipaddr(self):
        enc = AX.encode_ipaddr
        dec = AX.decode_ipaddr

        # Encode correct
        self.assertEqual(enc(True, (1, 2, 3, 4)),
                         b"\x00\x00\x00\x04\x01\x02\x03\x04")
        # Encode correct, little endian
        self.assertEqual(enc(False, (1, 2, 3, 4)),
                         b"\x04\x00\x00\x00\x01\x02\x03\x04")
        # Encode incorrect
        try:
            enc(True, (1, 2, 3, 4, 5))
            errored = False
        except ValueError:
            errored = True
        self.assertEqual(errored, True)
        # Decode
        self.assertEqual(dec(b"\x00\x00\x00\x04\x01\x02\x03\x04" + extraData,
                             standardFlags),
                         ((1, 2, 3, 4), extraData))
        # Decode, little endian
        self.assertEqual(dec(b"\x04\x00\x00\x00\x01\x02\x03\x04" + extraData,
                             lilEndianFlags),
                         ((1, 2, 3, 4), extraData))

    def test_OID(self):
        target = AX.OID
        dec = AX.decode_OID

        # Encode empty OID
        cls = target((), False)
        self.assertEqual(cls.encode(True), b"\x00\x00\x00\x00")
        # Encode basic OID
        cls = target((1, 2, 3, 4, 5), False)
        self.assertEqual(cls.encode(True),
                         b"\x05\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05")
        # Encode basic OID, little endian
        self.assertEqual(cls.encode(False),
                         b"\x05\x00\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00")
        # Encode prefixed OID
        cls = target((1, 3, 6, 1, 23, 1, 2, 3), False)
        self.assertEqual(cls.encode(True),
                         b"\x03\x17\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03")
        # Encode include
        cls = target((1, 2), True)
        self.assertEqual(cls.encode(True),
                         b"\x02\x00\x01\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02")
        # Encode together
        cls = target((1, 3, 6, 1, 1, 3, 4, 5, 6), True)
        self.assertEqual(cls.encode(True),
                         b"\x04\x01\x01\x00"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06")
        # Encode maximum size
        cls = target(maximumOIDsubs, False)
        self.assertEqual(cls.encode(True), maximumOIDstr)
        # Encode over maximum size
        try:
            cls = target(maximumOIDsubs + (42,), False)
            cls.encode(True)
            errored = False
        except ValueError:
            errored = True
        self.assertEqual(errored, True)

        # Decode empty OID, extra data
        cls, xtr = dec(b"\x00\x00\x00\x00" + extraData, standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, ())
        self.assertEqual(cls.include, False)
        self.assertEqual(xtr, extraData)
        # Decode basic OID, extra data
        cls, xtr = dec(b"\x05\x00\x00\x00\x00\x00\x00\x01"
                       b"\x00\x00\x00\x02\x00\x00\x00\x03"
                       b"\x00\x00\x00\x04\x00\x00\x00\x05" + extraData,
                       standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, (1, 2, 3, 4, 5))
        self.assertEqual(cls.include, False)
        self.assertEqual(xtr, extraData)
        # Decode basic OID, little endian
        cls, xtr = dec(b"\x05\x00\x00\x00\x01\x00\x00\x00"
                       b"\x02\x00\x00\x00\x03\x00\x00\x00"
                       b"\x04\x00\x00\x00\x05\x00\x00\x00",
                       lilEndianFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, (1, 2, 3, 4, 5))
        self.assertEqual(cls.include, False)
        self.assertEqual(xtr, b"")
        # Decode prefixed OID
        cls, xtr = dec(b"\x03\x17\x00\x00\x00\x00\x00\x01"
                       b"\x00\x00\x00\x02\x00\x00\x00\x03", standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, (1, 3, 6, 1, 23, 1, 2, 3))
        self.assertEqual(cls.include, False)
        self.assertEqual(xtr, b"")
        # Decode include
        cls, xtr = dec(b"\x02\x00\x05\x00\x00\x00\x00\x01\x00\x00\x00\x02",
                       standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, (1, 2))
        self.assertEqual(cls.include, True)
        self.assertEqual(xtr, b"")
        # Decode together
        cls, xtr = dec(b"\x04\x01\x02\x00\x00\x00\x00\x03"
                       b"\x00\x00\x00\x04\x00\x00\x00\x05\x00\x00\x00\x06",
                       standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, (1, 3, 6, 1, 1, 3, 4, 5, 6))
        self.assertEqual(cls.include, True)
        self.assertEqual(xtr, b"")
        # Decode maximum size
        cls, xtr = dec(maximumOIDstr, standardFlags)
        self.assertEqual(isinstance(cls, target), True)
        self.assertEqual(cls.subids, maximumOIDsubs)
        self.assertEqual(cls.include, False)
        self.assertEqual(xtr, b"")
        # Decode over maximum size
        # Need to replace the hardcoded n_subid=128 with 129
        fatOID = b"\x81" + maximumOIDstr[1:] + b"\xDE\xAD\xBE\xEF"
        try:
            cls, xtr = dec(fatOID, standardFlags)
            errored = False
        except ValueError:
            errored = True
        self.assertEqual(errored, True)
        # Test compareOID
        # Test equal
        a = target((1, 2, 3, 4))
        b = target((1, 2, 3, 4))
        self.assertEqual(a.compareOID(b), 0)

        # Test equal length, one < two
        b = target((1, 2, 3, 5))
        self.assertEqual(a.compareOID(b), -1)
        # Test equal length, one > two
        b = target((1, 2, 3, 0))
        self.assertEqual(a.compareOID(b), 1)
        # Test one shorter, less than two, equal for length
        a = target((1, 2, 3))
        b = target((1, 2, 3, 4))
        self.assertEqual(a.compareOID(b), -1)
        # Test one shorter, less than two
        b = target((1, 2, 4, 5))
        self.assertEqual(a.compareOID(b), -1)
        # Test one shorter, greater than two
        b = target((1, 2, 2, 4))
        self.assertEqual(a.compareOID(b), 1)
        # Test two shorter, less than one, equal for length
        a = target((1, 2, 3, 4))
        b = target((1, 2, 3))
        self.assertEqual(a.compareOID(b), 1)
        # Test two shorter, less than one
        a = target((1, 2, 4, 5))
        self.assertEqual(a.compareOID(b), 1)
        # Test two shorter, greater than one
        a = target((1, 2, 2, 4))
        b = target((1, 2, 3))
        self.assertEqual(a.compareOID(b), -1)
        # Test direct comparisons
        # Test ==
        self.assertEqual(target((1, 2, 3)) == target((1, 2, 3)), True)
        # Test !=
        self.assertEqual(target((1, 2, 3)) != target((1, 2)), True)
        # Test <
        self.assertEqual(target((1, 2)) < target((1, 2, 3)), True)
        # Test >
        self.assertEqual(target((1, 2, 3)) > target((1, 2)), True)
        # Test <=
        self.assertEqual(target((1, 2)) <= target((1, 2, 3)), True)
        self.assertEqual(target((1, 2, 3)) <= target((1, 2, 3)), True)
        # Test >=
        self.assertEqual(target((1, 2, 3)) >= target((1, 2)), True)
        self.assertEqual(target((1, 2, 3)) >= target((1, 2, 3)), True)
        # Test insane subids type
        try:
            errored = target("foo")
        except TypeError:
            errored = True
        self.assertEqual(errored, True)
        # Test isNull
        self.assertEqual(target((1, 2)).isNull(), False)
        self.assertEqual(target(()).isNull(), True)

    def test_searchrange(self):
        target = AX.SearchRange
        dec = AX.decode_SearchRange
        oid = AX.OID

        # Test init
        # Basic
        cls = target(oid((1, 2), True), oid((3, 4), False))
        self.assertEqual(cls.start.subids, (1, 2))
        self.assertEqual(cls.start.include, True)
        self.assertEqual(cls.end.subids, (3, 4))
        self.assertEqual(cls.end.include, False)
        # Override
        cls = target(oid((1, 2), True), oid((3, 4), True), False)
        self.assertEqual(cls.start.subids, (1, 2))
        self.assertEqual(cls.start.include, False)
        self.assertEqual(cls.end.subids, (3, 4))
        self.assertEqual(cls.end.include, False)
        # Turn tuples into OIDs
        cls = target((1, 2), (3, 4), True)
        self.assertEqual(cls.start.subids, (1, 2))
        self.assertEqual(cls.start.include, True)
        self.assertEqual(cls.end.subids, (3, 4))
        self.assertEqual(cls.end.include, False)
        # Test encoding
        # Encode minimum size
        cls = target((), (), False)
        self.assertEqual(cls.encode(True),
                         b"\x00\x00\x00\x00\x00\x00\x00\x00")
        # Encode inclusive
        cls = target((1, 2, 3, 4), (5, 6, 7, 8), True)
        self.assertEqual(cls.encode(True),
                         b"\x04\x00\x01\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x04\x00\x00\x00"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x00\x00\x07\x00\x00\x00\x08")
        # Encode exclusive
        cls = target((1, 2, 3, 4), (5, 6, 7, 8), False)
        self.assertEqual(cls.encode(True),
                         b"\x04\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x04\x00\x00\x00"
                         b"\x00\x00\x00\x05\x00\x00\x00\x06"
                         b"\x00\x00\x00\x07\x00\x00\x00\x08")
        # Encode exclusive, little endian
        self.assertEqual(cls.encode(False),
                         b"\x04\x00\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x04\x00\x00\x00"
                         b"\x05\x00\x00\x00\x06\x00\x00\x00"
                         b"\x07\x00\x00\x00\x08\x00\x00\x00")
        # Test decode
        # Decode minimum size, extra data
        self.assertEqual(dec(b"\x00\x00\x00\x00\x00\x00\x00\x00" + extraData,
                             standardFlags),
                         (target((), (), False), extraData))
        # Decode inclusive
        self.assertEqual(dec(b"\x04\x00\x01\x00"
                             b"\x00\x00\x00\x01\x00\x00\x00\x02"
                             b"\x00\x00\x00\x03\x00\x00\x00\x04"
                             b"\x04\x00\x00\x00"
                             b"\x00\x00\x00\x05\x00\x00\x00\x06"
                             b"\x00\x00\x00\x07\x00\x00\x00\x08",
                             standardFlags),
                         (target((1, 2, 3, 4), (5, 6, 7, 8), True), b""))
        # Decode exclusive
        self.assertEqual(dec(b"\x04\x00\x00\x00"
                             b"\x00\x00\x00\x01\x00\x00\x00\x02"
                             b"\x00\x00\x00\x03\x00\x00\x00\x04"
                             b"\x04\x00\x00\x00"
                             b"\x00\x00\x00\x05\x00\x00\x00\x06"
                             b"\x00\x00\x00\x07\x00\x00\x00\x08",
                             standardFlags),
                         (target((1, 2, 3, 4), (5, 6, 7, 8), False), b""))
        # Decode little endian
        self.assertEqual(dec(b"\x04\x00\x01\x00"
                             b"\x01\x00\x00\x00\x02\x00\x00\x00"
                             b"\x03\x00\x00\x00\x04\x00\x00\x00"
                             b"\x04\x00\x00\x00"
                             b"\x05\x00\x00\x00\x06\x00\x00\x00"
                             b"\x07\x00\x00\x00\x08\x00\x00\x00",
                             lilEndianFlags),
                         (target((1, 2, 3, 4), (5, 6, 7, 8), True), b""))
        # Test __eq__
        # Test equal
        a = target((1, 2, 3), (1, 2, 3))
        b = target((1, 2, 3), (1, 2, 3))
        self.assertEqual(a == b, True)
        # Test start unequal
        b = target((1, 2, 3), (1, 2, 3), True)
        self.assertEqual(a == b, False)
        # Test end unequal
        b = target((1, 2, 3), (1, 2, 3, 4))
        self.assertEqual(a == b, False)
        # Test __ne__
        # Test equal
        a = target((1, 2, 3), (1, 2, 3))
        b = target((1, 2, 3), (1, 2, 3))
        self.assertEqual(a != b, False)
        # Test start unequal
        b = target((1, 2, 3), (1, 2, 3), True)
        self.assertEqual(a != b, True)
        # Test end unequal
        b = target((1, 2, 3), (1, 2, 3, 4))
        self.assertEqual(a != b, True)
        # Test __repr__
        self.assertEqual(repr(target((1, 2), (1, 3))),
                         "SearchRange(OID((1, 2), False), OID((1, 3), False))")

    def test_encode_searchrange_list(self):
        enc = AX.encode_searchrange_list
        srch = AX.SearchRange

        # Encode
        self.assertEqual(enc(True, (srch((1, 2), (1, 2), True),
                                    srch((2, 3), (3, 4)))),
                         b"\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04")
        # Encode, little endian
        self.assertEqual(enc(False, (srch((1, 2), (1, 2), True),
                                     srch((2, 3), (3, 4)))),
                         b"\x02\x00\x01\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x02\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00")

    def test_decode_searchrange_list(self):
        dec = AX.decode_searchrange_list
        srch = AX.SearchRange

        # Decode
        self.assertEqual(dec(b"\x02\x00\x01\x00"
                             b"\x00\x00\x00\x01\x00\x00\x00\x02"
                             b"\x02\x00\x00\x00"
                             b"\x00\x00\x00\x01\x00\x00\x00\x02"
                             b"\x02\x00\x00\x00"
                             b"\x00\x00\x00\x02\x00\x00\x00\x03"
                             b"\x02\x00\x00\x00"
                             b"\x00\x00\x00\x03\x00\x00\x00\x04",
                             standardFlags),
                         (srch((1, 2), (1, 2), True),
                          srch((2, 3), (3, 4), False)))
        # Test, little endian
        self.assertEqual(dec(b"\x02\x00\x01\x00"
                             b"\x01\x00\x00\x00\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x01\x00\x00\x00\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00\x03\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x03\x00\x00\x00\x04\x00\x00\x00",
                             lilEndianFlags),
                         (srch((1, 2), (1, 2), True),
                          srch((2, 3), (3, 4), False)))

    def test_xcode_octetstr(self):
        enc = AX.encode_octetstr
        dec = AX.decode_octetstr
        san = AX.sanity_octetstr

        # Encode empty
        self.assertEqual(enc(True, ()), b"\x00\x00\x00\x00")
        # Encode word multiple
        self.assertEqual(enc(True, (1, 2, 3, 4)),
                         b"\x00\x00\x00\x04\x01\x02\x03\x04")
        # Encode non word multiple
        self.assertEqual(enc(True, (1, 2, 3, 4, 5)),
                         b"\x00\x00\x00\x05\x01\x02\x03\x04\x05\x00\x00\x00")
        # Encode string
        self.assertEqual(enc(True, "blah"), b"\x00\x00\x00\x04blah")
        # Encode string, little endian
        self.assertEqual(enc(False, "blah"), b"\x04\x00\x00\x00blah")
        # Decode empty
        self.assertEqual(dec(b"\x00\x00\x00\x00", standardFlags), ("", b""))
        # Decode word multiple, extra data
        self.assertEqual(dec(b"\x00\x00\x00\x04blah" + extraData,
                             standardFlags),
                         ("blah", extraData))
        # Decode word multiple, little endian
        self.assertEqual(dec(b"\x04\x00\x00\x00blah", lilEndianFlags),
                         ("blah", b""))
        # Decode non word multiple, extra data
        self.assertEqual(dec(b"\x00\x00\x00\x05"
                             b"blarg\x00\x00\x00" + extraData,
                             standardFlags),
                         ("blarg", extraData))
        # Test sanity
        # Test str
        try:
            errored = san("foo")  # str is always sane
        except Exception as e:
            errored = e
        self.assertEqual(errored, None)
        # Test sane list
        try:
            errored = san([1, 2, 3])
        except Exception as e:
            errored = e
        self.assertEqual(errored, None)
        # Test sane tuple
        try:
            errored = san((1, 2, 3))
        except Exception as e:
            errored = e
        self.assertEqual(errored, None)
        # Test insane list
        try:
            errored = san([23, 300, 42])
        except ValueError:
            errored = True
        self.assertEqual(errored, True)
        # Test insane tuple
        try:
            errored = san((23, 300, 42))
        except ValueError:
            errored = True
        self.assertEqual(errored, True)
        # Test insane type
        try:
            errored = san(42.23)
        except TypeError:
            errored = True
        self.assertEqual(errored, True)

    def test_Varbind(self):
        target = AX.Varbind

        # Test init
        cls = target(AX.VALUE_INTEGER, (1, 2, 3), 42)
        self.assertEqual(cls.valueType, AX.VALUE_INTEGER)
        self.assertEqual(cls.oid, AX.OID((1, 2, 3), False))
        self.assertEqual(cls.payload, 42)
        # Test repr
        self.assertEqual(repr(cls),
                         "Varbind(vtype=2, oid=OID((1, 2, 3), False), "
                         "payload=42)")
        # Test payloadless types
        cls = target(AX.VALUE_NULL, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x05\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(AX.VALUE_NO_SUCH_OBJECT, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x80\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(AX.VALUE_NO_SUCH_INSTANCE, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x81\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(AX.VALUE_END_OF_MIB_VIEW, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x82\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test octet based types
        cls = target(AX.VALUE_OCTET_STR, (1, 2, 3), (1, 2, 3, 4, 5))
        self.assertEqual(cls.encode(True),
                         b"\x00\x04\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x05"
                         b"\x01\x02\x03\x04\x05\x00\x00\x00")
        cls = target(AX.VALUE_IP_ADDR, (1, 2, 3), (16, 32, 48, 64))
        self.assertEqual(cls.encode(True),
                         b"\x00\x40\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x04\x10\x20\x30\x40")
        # Test integer32 type
        cls = target(AX.VALUE_INTEGER, (1, 2, 3), -42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x02\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\xFF\xFF\xFF\xD6")
        # Test unsigned32 types
        cls = target(AX.VALUE_COUNTER32, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x41\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        cls = target(AX.VALUE_GAUGE32, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x42\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        cls = target(AX.VALUE_TIME_TICKS, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x43\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        # Test integer64 type
        cls = target(AX.VALUE_COUNTER64, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x46\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x00\x00\x00\x00\x2A")
        # Test oid type
        cls = target(AX.VALUE_OID, (1, 2, 3), AX.OID((16, 42, 256), False))
        self.assertEqual(cls.encode(True),
                         b"\x00\x06\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x10"
                         b"\x00\x00\x00\x2A\x00\x00\x01\x00")
        # Test oid type, little endian
        cls = target(AX.VALUE_OID, (1, 2, 3), AX.OID((16, 42, 256), False))
        self.assertEqual(cls.encode(False),
                         b"\x06\x00\x00\x00\x03\x00\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x10\x00\x00\x00"
                         b"\x2A\x00\x00\x00\x00\x01\x00\x00")
        # Test __eq__
        one = target(AX.VALUE_INTEGER, (1, 2, 3), 1)
        two = target(AX.VALUE_INTEGER, (1, 2, 3), 1)
        # Test equal
        self.assertEqual(one == two, True)
        # Test different type
        two = target(AX.VALUE_GAUGE32, (1, 2, 3), 1)
        self.assertEqual(one == two, False)
        # Test different OID
        two = target(AX.VALUE_INTEGER, (1, 2, 3, 4), 1)
        self.assertEqual(one == two, False)
        # Test different payload
        two = target(AX.VALUE_INTEGER, (1, 2, 3), 2)
        self.assertEqual(one == two, False)

    def test_decode_varbind(self):
        f = AX.decode_Varbind

        # Test payloadless types
        self.assertEqual(f(b"\x00\x05\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (AX.Varbind(AX.VALUE_NULL, AX.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x80\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (AX.Varbind(AX.VALUE_NO_SUCH_OBJECT,
                                     AX.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x81\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (AX.Varbind(AX.VALUE_NO_SUCH_INSTANCE,
                                     AX.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x82\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (AX.Varbind(AX.VALUE_END_OF_MIB_VIEW,
                                     AX.OID((1, 2, 3), False)),
                          b""))
        # Test octet based types
        self.assertEqual(f(b"\x00\x04\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x0512345\x00\x00\x00",
                           standardFlags),
                         (AX.Varbind(AX.VALUE_OCTET_STR,
                                     AX.OID((1, 2, 3), False),
                                     "12345"),
                          b""))
        self.assertEqual(f(b"\x00\x40\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x04\x10\x20\x30\x40", standardFlags),
                         (AX.Varbind(AX.VALUE_IP_ADDR,
                                     AX.OID((1, 2, 3), False),
                                     (16, 32, 48, 64)),
                          b""))
        # Test integer32 type
        self.assertEqual(f(b"\x00\x02\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\xFF\xFF\xFF\xD6", standardFlags),
                         (AX.Varbind(AX.VALUE_INTEGER,
                                     AX.OID((1, 2, 3), False),
                                     -42),
                          b""))
        self.assertEqual(f(b"\x00\x41\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (AX.Varbind(AX.VALUE_COUNTER32,
                                     AX.OID((1, 2, 3), False),
                                     42),
                          b""))
        # Test unsigned32 types
        self.assertEqual(f(b"\x00\x42\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (AX.Varbind(AX.VALUE_GAUGE32,
                                     AX.OID((1, 2, 3), False),
                                     42),
                          b""))
        self.assertEqual(f(b"\x00\x43\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (AX.Varbind(AX.VALUE_TIME_TICKS,
                                     AX.OID((1, 2, 3), False),
                                     42),
                          b""))
        # Test integer64 type
        self.assertEqual(f(b"\x00\x46\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x00\x00\x00\x00\x2A", standardFlags),
                         (AX.Varbind(AX.VALUE_COUNTER64,
                                     AX.OID((1, 2, 3), False),
                                     42),
                          b""))
        # Test oid type
        self.assertEqual(f(b"\x00\x06\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x10"
                           b"\x00\x00\x00\x2A\x00\x00\x01\x00", standardFlags),
                         (AX.Varbind(AX.VALUE_OID,
                                     AX.OID((1, 2, 3), False),
                                     AX.OID((16, 42, 256), False)),
                          b""))
        # Test integer32 with little endian
        self.assertEqual(f(b"\x43\x00\x00\x00\x03\x00\x00\x00"
                           b"\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                           b"\x2A\x00\x00\x00", lilEndianFlags),
                         (AX.Varbind(AX.VALUE_TIME_TICKS,
                                     AX.OID((1, 2, 3), False),
                                     42),
                          b""))

    def test_xcode_varbindlist(self):
        enc = AX.encode_varbindlist
        dec = AX.decode_varbindlist
        vb = AX.Varbind

        # Test encode empty
        self.assertEqual(enc(True, []), b"")
        # Test encode big endian
        big = enc(True,
                  [vb(AX.VALUE_INTEGER, (1, 2), 1),
                   vb(AX.VALUE_INTEGER, (3, 4), 2)])
        self.assertEqual(big, b"\x00\x02\x00\x00"
                         b"\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x01"
                         b"\x00\x02\x00\x00"
                         b"\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x00\x00\x00\x02")
        # Test encode little endian
        little = enc(False,
                     [vb(AX.VALUE_INTEGER, (1, 2), 1),
                      vb(AX.VALUE_INTEGER, (3, 4), 2)])
        self.assertEqual(little, b"\x02\x00\x00\x00"
                         b"\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00"
                         b"\x02\x00\x00\x00")
        # Test decode empty
        self.assertEqual(dec(b"", standardFlags), None)
        # Test decode big endian
        self.assertEqual(dec(big, standardFlags),
                         (vb(AX.VALUE_INTEGER, (1, 2), 1),
                          vb(AX.VALUE_INTEGER, (3, 4), 2)))
        # Test decode little endian
        self.assertEqual(dec(little, lilEndianFlags),
                         (vb(AX.VALUE_INTEGER, (1, 2), 1),
                          vb(AX.VALUE_INTEGER, (3, 4), 2)))

    def test_encode_flagbyte(self):
        f = AX.encode_flagbyte
        self.assertEqual(f(makeflags(False, True, False, True, False)), 0x0A)
        self.assertEqual(f(makeflags(True, False, True, False, True)), 0x15)

    def test_decode_flagbyte(self):
        f = AX.decode_flagbyte
        self.assertEqual(f(0x0A), makeflags(False, True, False, True, False))
        self.assertEqual(f(0x15), makeflags(True, False, True, False, True))

    #
    # Misc tests
    #
    def test_makeflags(self):
        f = AX.makeflags
        self.assertEqual(f(True, False, True, False, True),
                         {"instReg": True,
                          "newIndex": False,
                          "anyIndex":  True,
                          "contextP": False,
                          "bigEndian": True})

    def test_getendian(self):
        f = AX.getendian

        # Test big endian
        self.assertEqual(f(True), ">")
        # Test little endian
        self.assertEqual(f(False), "<")

    def test_encode_pduheader(self):
        f = AX.encode_pduheader

        # Test "empty" header
        self.assertEqual(f(AX.PDU_OPEN,
                           False, False, False, False, False,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         b"\x01\x01\x00\x00"
                         b"\xEF\xBE\xAD\xDE\xBE\xBA\xFE\xCA"
                         b"\x0D\xF0\xCE\xFA\x00\x00\x00\x00")
        # Test flags
        self.assertEqual(f(AX.PDU_OPEN,
                           True, True, True, True, True,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         b"\x01\x01\x1F\x00"
                         b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                         b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00")

    def test_decode_pduheader(self):
        f = AX.decode_pduheader

        # Test "empty" header
        self.assertEqual(f(b"\x01\x01\x10\x00"
                           b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00"),
                         {"version": 1,
                          "type": AX.PDU_OPEN,
                          "flags": {"instReg": False,
                                    "newIndex": False,
                                    "anyIndex": False,
                                    "contextP": False,
                                    "bigEndian": True},
                          "session_id": 0xDEADBEEF,
                          "transaction_id": 0xCAFEBABE,
                          "packet_id": 0xFACEF00D,
                          "length": 0})
        # Test "empty" header, little endian
        self.assertEqual(f(b"\x01\x01\x00\x00"
                           b"\xEF\xBE\xAD\xDE\xBE\xBA\xFE\xCA"
                           b"\x0D\xF0\xCE\xFA\x00\x00\x00\x00"),
                         {"version": 1,
                          "type": AX.PDU_OPEN,
                          "flags": {"instReg": False,
                                    "newIndex": False,
                                    "anyIndex": False,
                                    "contextP": False,
                                    "bigEndian": False},
                          "session_id": 0xDEADBEEF,
                          "transaction_id": 0xCAFEBABE,
                          "packet_id": 0xFACEF00D,
                          "length": 0})
        # Test "empty" header, extra data
        self.assertEqual(f(b"\x01\x01\x10\x00"
                           b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00" + extraData),
                         {"version": 1,
                          "type": AX.PDU_OPEN,
                          "flags": {"instReg": False,
                                    "newIndex": False,
                                    "anyIndex": False,
                                    "contextP": False,
                                    "bigEndian": True},
                          "session_id": 0xDEADBEEF,
                          "transaction_id": 0xCAFEBABE,
                          "packet_id": 0xFACEF00D,
                          "length": 0})
        # Test flags
        self.assertEqual(f(b"\x01\x01\x1F\x00"
                           b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00"),
                         {"version": 1,
                          "type": AX.PDU_OPEN,
                          "flags": {"instReg": True,
                                    "newIndex": True,
                                    "anyIndex": True,
                                    "contextP": True,
                                    "bigEndian": True},
                          "session_id": 0xDEADBEEF,
                          "transaction_id": 0xCAFEBABE,
                          "packet_id": 0xFACEF00D,
                          "length": 0})

    def test_decode_packet(self):
        f = AX.decode_packet
        srch = AX.SearchRange
        # Not testing all the variants of each packet type, that is
        # the job of the other tests.
        self.maxDiff = None

        # Test open
        self.assertEqual(f(b"\x01\x01\x10\x00"
                           b"\x00\x00\x00\x0C\x00\x00\x00\x22"
                           b"\x00\x00\x00\x38\x00\x00\x00\x20"
                           b"\x4E\x00\x00\x00"
                           b"\x04\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x00\x00\x00\x03foo\x00"),
                         (AX.OpenPDU(True, 12, 34, 56, 78,
                                     AX.OID((1, 2, 3, 4), False),
                                     "foo"),
                          True,
                          b""))
        # Test open, extraData
        self.assertEqual(f(b"\x01\x01\x10\x00"
                           b"\x00\x00\x00\x0C\x00\x00\x00\x22"
                           b"\x00\x00\x00\x38\x00\x00\x00\x20"
                           b"\x4E\x00\x00\x00"
                           b"\x04\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x00\x00\x00\x03foo\x00" + extraData),
                         (AX.OpenPDU(True, 12, 34, 56, 78,
                                     AX.OID((1, 2, 3, 4), False),
                                     "foo"),
                          True,
                          extraData))
        # Test close
        self.assertEqual(f(b"\x01\x02\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x01\x00\x00\x00"),
                         (AX.ClosePDU(True, 1, 2, 3, AX.RSN_OTHER),
                          True,
                          b""))
        # Test register
        self.assertEqual(f(b"\x01\x03\x11\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x14"
                           b"\x04\x05\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (AX.RegisterPDU(True, 1, 2, 3, 4, 5,
                                         AX.OID((1, 2, 3), False),
                                         0, None, None),
                          True,
                          b""))
        # Test unregister
        self.assertEqual(f(b"\x01\x04\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x14"
                           b"\x00\x05\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (AX.UnregisterPDU(True, 1, 2, 3, 5,
                                           AX.OID((1, 2, 3), False),
                                           0, None, None),
                          True,
                          b""))
        # Test get
        self.assertEqual(f(b"\x01\x05\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.GetPDU(True, 1, 2, 3, ()),
                          True,
                          b""))
        # Test get next
        self.assertEqual(f(b"\x01\x06\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.GetNextPDU(True, 1, 2, 3, ()),
                          True,
                          b""))
        # Test get bulk
        self.assertEqual(f(b"\x01\x07\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x34"
                           b"\x00\x01\x00\x05"
                           b"\x02\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x02\x00\x00\x00"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x02\x00\x01\x00"
                           b"\x00\x00\x00\x06\x00\x00\x00\x07"
                           b"\x02\x00\x00\x00"
                           b"\x00\x00\x00\x08\x00\x00\x00\x09"),
                         (AX.GetBulkPDU(True, 1, 2, 3, 1, 5,
                                        (srch((1, 2), (3, 4), False),
                                         srch((6, 7), (8, 9), True))),
                          True,
                          b""))
        # Test test set
        self.assertEqual(f(b"\x01\x08\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x40"
                           b"\x00\x06\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x04\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x04"
                           b"\x00\x00\x00\x04blah"),
                         (AX.TestSetPDU(True, 1, 2, 3,
                                        (AX.Varbind(AX.VALUE_OID,
                                                    AX.OID((1, 2, 3), False),
                                                    AX.OID((4, 5, 6), False)),
                                         AX.Varbind(AX.VALUE_OCTET_STR,
                                                    AX.OID((1, 2, 4), False),
                                                    "blah"))),
                          True,
                          b""))
        # Test commit set
        self.assertEqual(f(b"\x01\x09\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.CommitSetPDU(True, 1, 2, 3),
                          True,
                          b""))
        # Test undo set
        self.assertEqual(f(b"\x01\x0A\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.UndoSetPDU(True, 1, 2, 3),
                          True,
                          b""))
        # Test cleanup set
        self.assertEqual(f(b"\x01\x0B\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.CleanupSetPDU(True, 1, 2, 3),
                          True,
                          b""))
        # Test notify
        self.assertEqual(f(b"\x01\x0C\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x40"
                           b"\x00\x06\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x04\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x04"
                           b"\x00\x00\x00\x04blah"),
                         (AX.NotifyPDU(True, 1, 2, 3,
                                       (AX.Varbind(AX.VALUE_OID,
                                                   AX.OID((1, 2, 3), False),
                                                   AX.OID((4, 5, 6), False)),
                                        AX.Varbind(AX.VALUE_OCTET_STR,
                                                   AX.OID((1, 2, 4), False),
                                                   "blah"))),
                          True,
                          b""))
        # Test ping
        self.assertEqual(f(b"\x01\x0D\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (AX.PingPDU(True, 1, 2, 3),
                          True,
                          b""))
        # Test index alloc
        self.assertEqual(f(b"\x01\x0E\x16\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x40"
                           b"\x00\x06\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x04\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x04"
                           b"\x00\x00\x00\x04blah"),
                         (AX.IndexAllocPDU(True, 1, 2, 3, True, True,
                                           (AX.Varbind(AX.VALUE_OID,
                                                       AX.OID((1, 2, 3), False),
                                                       AX.OID((4, 5, 6), False)),
                                            AX.Varbind(AX.VALUE_OCTET_STR,
                                                       AX.OID((1, 2, 4), False),
                                                       "blah"))),
                          True,
                          b""))
        # Test index dealloc
        self.assertEqual(f(b"\x01\x0F\x16\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x40"
                           b"\x00\x06\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x04\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x04"
                           b"\x00\x00\x00\x04blah"),
                         (AX.IndexDeallocPDU(True, 1, 2, 3, True, True,
                                             (AX.Varbind(AX.VALUE_OID,
                                                         AX.OID((1, 2, 3),
                                                                False),
                                                         AX.OID((4, 5, 6),
                                                                False)),
                                              AX.Varbind(AX.VALUE_OCTET_STR,
                                                         AX.OID((1, 2, 4),
                                                                False),
                                                         "blah"))),
                          True,
                          b""))
        # Test add agent caps
        self.assertEqual(f(b"\x01\x10\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x18"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x00\x00\x04blah"),
                         (AX.AddAgentCapsPDU(True, 1, 2, 3,
                                             AX.OID((4, 5, 6), False),
                                             "blah"),
                          True,
                          b""))
        # Test rm agent caps
        self.assertEqual(f(b"\x01\x11\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x10"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"),
                         (AX.RMAgentCapsPDU(True, 1, 2, 3,
                                            AX.OID((4, 5, 6), False)),
                          True,
                          b""))
        # Test response
        self.assertEqual(f(b"\x01\x12\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x08"
                           b"\x00\x00\x00\x04\x00\x05\x00\x06"),
                         (AX.ResponsePDU(True, 1, 2, 3, 4, 5, 6),
                          True,
                          b""))
        # Test errors
        # Test insufficient data for header
        try:
            self.assertEqual(f(b""), None)
            errored = False
        except AX.ParseError as e:
            errored = e
        self.assertEqual(errored.message, "Data too short for header")
        # Test insufficient data for packet
        self.assertEqual(f(b"\x01\x11\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x10"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"),
                         (None, False,
                          b"\x01\x11\x10\x00"
                          b"\x00\x00\x00\x01\x00\x00\x00\x02"
                          b"\x00\x00\x00\x03\x00\x00\x00\x10"
                          b"\x03\x00\x00\x00\x00\x00\x00\x04"))
        # Test wrong version
        try:
            f(b"\x02\x11\x10\x00"
              b"\x00\x00\x00\x00\x00\x00\x00\x00"
              b"\x00\x00\x00\x00\x00\x00\x00\x08"
              b"blahblahjabber")
            errored = False
        except AX.ParseError as e:
            errored = e
        self.assertEqual(errored.message, "Unknown packet version 2")
        self.assertEqual(errored.packetData, b"blahblah")
        self.assertEqual(errored.remainingData, b"jabber")
        # Test unrecognized packet type
        try:
            f(b"\x01\xFF\x10\x00"
              b"\x00\x00\x00\x00\x00\x00\x00\x00"
              b"\x00\x00\x00\x00\x00\x00\x00\x08"
              b"blahblah")
            errored = False
        except AX.ParseError as e:
            errored = e
        self.assertEqual(errored.message, "PDU type 255 not in defined types")

    def test_bits2Bools(self):
        bits2bool = AX.bits2Bools

        # Test empty
        self.assertEqual(bits2bool(""), [])
        # Test round bytes
        self.assertEqual(bits2bool("\xFA\xCE"),
                         [True, True, True, True,
                          True, False, True, False,
                          True, True, False, False,
                          True, True, True, False])
        # Test partial bytes
        self.assertEqual(bits2bool("\xFF\xE1", 12),  # The extra bit is to
                         [True, True, True, True,    # confirm crop
                          True, True, True, True,
                          True, True, True, False])

    def test_bools2bits(self):
        bool2bits = AX.bools2Bits

        # Test empty
        self.assertEqual(bool2bits([]), "")
        # Test round bytes
        self.assertEqual(bool2bits([True, True, True, True,
                                    True, False, True, False,
                                    True, True, False, False,
                                    True, True, True, False]),
                         "\xFA\xCE")
        # Test partial bytes
        self.assertEqual(bool2bits([True, True, True, True,
                                    True, True, True, True,
                                    True, True, True, False]),
                         "\xFF\xE0")


if __name__ == "__main__":
    unittest.main()
