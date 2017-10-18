#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.agentx

from ntp.agentx import slicedata, decode_pduheader, makeflags

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
        # Test these so we don't need a bunch of redundant tests
        # Test basic, without context
        test = ntp.agentx.AgentXPDU(0, True, 1, 2, 3, context=extraData)
        self.assertEqual(repr(test),
                         "AgentXPDU(bigEndian=True, packetID=3, "
                         "pduType=0, sessionID=1, transactionID=2)")
        # Test basic, with context
        test = ntp.agentx.AgentXPDU(0, True, 1, 2, 3, hascontext=True)
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

    def test_OpenPDU(self):
        dec = ntp.agentx.decode_OpenPDU
        cls = ntp.agentx.OpenPDU
        x = ntp.agentx

        # Test PDU init, null packet
        nullPkt = cls(True, 1, 2, 3, 4, (), "")
        test_pducore(self, nullPkt, x.PDU_OPEN, True, 1, 2, 3)
        self.assertEqual(nullPkt.timeout, 4)
        self.assertEqual(nullPkt.oid, x.OID((), False))
        self.assertEqual(nullPkt.description, "")
        # Test PDU init, basic packet
        basicPkt = cls(False, 1, 2, 3, 4, (1, 2, 3, 4), "foo")
        test_pducore(self, basicPkt, x.PDU_OPEN, False, 1, 2, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.oid, x.OID((1, 2, 3, 4), False))
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
        test_pducore(self, nullPkt_new, x.PDU_OPEN, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.timeout, 4)
        self.assertEqual(nullPkt_new.oid, x.OID((), False))
        self.assertEqual(nullPkt_new.description, "")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        test_pducore(self, basicPkt_new, x.PDU_OPEN, False, 1, 2, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.oid, x.OID((1, 2, 3, 4), False))
        self.assertEqual(basicPkt_new.description, "foo")
        # Test packetVars
        self.assertEqual(basicPkt_new.packetVars(),
                         {"pduType": 1,
                          "bigEndian": False,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "timeout": 4,
                          "oid": x.OID((1, 2, 3, 4), False),
                          "description": "foo"})

    def test_ClosePDU(self):
        dec = ntp.agentx.decode_ClosePDU
        cls = ntp.agentx.ClosePDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, x.RSN_OTHER)
        test_pducore(self, pkt, x.PDU_CLOSE, True, 1, 2, 3)
        self.assertEqual(pkt.reason, x.RSN_OTHER)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, x.RSN_OTHER)
        test_pducore(self, pkt_LE, x.PDU_CLOSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE.reason, x.RSN_OTHER)
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
        test_pducore(self, pkt_new, x.PDU_CLOSE, True, 1, 2, 3)
        self.assertEqual(pkt_new.reason, x.RSN_OTHER)
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_CLOSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.reason, x.RSN_OTHER)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 2,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "reason": x.RSN_OTHER})

    def test_RegisterPDU(self):
        dec = ntp.agentx.decode_xRegisterPDU
        cls = ntp.agentx.RegisterPDU
        x = ntp.agentx

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3))
        test_pducore(self, basicPkt, x.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, basic packet, little endian
        basicPkt_LE = cls(False, 1, 2, 3, 4, 5, (1, 2, 3))
        test_pducore(self, basicPkt_LE, x.PDU_REGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE.timeout, 4)
        self.assertEqual(basicPkt_LE.priority, 5)
        self.assertEqual(basicPkt_LE.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE.rangeSubid, 0)
        self.assertEqual(basicPkt_LE.upperBound, None)
        self.assertEqual(basicPkt_LE.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        test_pducore(self, fancyPkt, x.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt.timeout, 4)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, x.OID((1, 2, 3), False))
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
        test_pducore(self, basicPkt_new, x.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, basic packet, little endian
        header, body = slicedata(basicPkt_LE_str, 20)
        header = decode_pduheader(header)
        basicPkt_LE_new = dec(body, header)
        test_pducore(self, basicPkt_LE_new, x.PDU_REGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE_new.timeout, 4)
        self.assertEqual(basicPkt_LE_new.priority, 5)
        self.assertEqual(basicPkt_LE_new.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE_new.rangeSubid, 0)
        self.assertEqual(basicPkt_LE_new.upperBound, None)
        self.assertEqual(basicPkt_LE_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        test_pducore(self, fancyPkt_new, x.PDU_REGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt_new.timeout, 4)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, x.OID((1, 2, 3), False))
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
                          "subtree": x.OID((1, 2, 3), False),
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})

    def test_UnregisterPDU(self):
        dec = ntp.agentx.decode_xRegisterPDU
        cls = ntp.agentx.UnregisterPDU
        x = ntp.agentx

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 5, (1, 2, 3))
        test_pducore(self, basicPkt, x.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, basic packet, little endian
        basicPkt_LE = cls(False, 1, 2, 3, 5, (1, 2, 3))
        test_pducore(self, basicPkt_LE, x.PDU_UNREGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE.priority, 5)
        self.assertEqual(basicPkt_LE.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE.rangeSubid, 0)
        self.assertEqual(basicPkt_LE.upperBound, None)
        self.assertEqual(basicPkt_LE.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        test_pducore(self, fancyPkt, x.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, x.OID((1, 2, 3), False))
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
        test_pducore(self, basicPkt, x.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, basic packet, little endian
        header, body = slicedata(basicPkt_LE_str, 20)
        header = decode_pduheader(header)
        basicPkt_LE_new = dec(body, header)
        test_pducore(self, basicPkt_LE, x.PDU_UNREGISTER, False, 1, 2, 3)
        self.assertEqual(basicPkt_LE_new.priority, 5)
        self.assertEqual(basicPkt_LE_new.subtree, x.OID((1, 2, 3), False))
        self.assertEqual(basicPkt_LE_new.rangeSubid, 0)
        self.assertEqual(basicPkt_LE_new.upperBound, None)
        self.assertEqual(basicPkt_LE_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        test_pducore(self, fancyPkt_new, x.PDU_UNREGISTER, True, 1, 2, 3)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, x.OID((1, 2, 3), False))
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
                          "subtree": x.OID((1, 2, 3), False),
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})

    def test_GetPDU(self):
        dec = ntp.agentx.decode_xGetPDU
        cls = ntp.agentx.GetPDU
        x = ntp.agentx
        srch = x.SearchRange

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        test_pducore(self, nullPkt, x.PDU_GET, True, 1, 2, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (srch((1, 2, 3), (1, 2, 5), False),
                       srch((10, 20), (30, 40), True)),
                      context="blah")
        test_pducore(self, fullPkt, x.PDU_GET, True, 1, 2, 3)
        self.assertEqual(fullPkt.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test init, full packet, little endian
        fullPkt_LE = cls(False, 1, 2, 3,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)),
                         context="blah")
        test_pducore(self, fullPkt_LE, x.PDU_GET, False, 1, 2, 3)
        self.assertEqual(fullPkt_LE.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_LE.context, "blah")
        # Test encode, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         b"\x01\x05\x10\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x00\x00\x00\x00")
        # Test encode, full packet
        fullPkt_str = fullPkt.encode()
        self.assertEqual(fullPkt_str,
                         b"\x01\x05\x18\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x00\x00\x00\x03\x00\x00\x00\x44"
                         b"\x00\x00\x00\x04blah"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x01"
                         b"\x00\x00\x00\x02\x00\x00\x00\x05"
                         b"\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         b"\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                         b"\x00\x00\x00\x00")
        # Test encode, full packet, little endian
        fullPkt_LE_str = fullPkt_LE.encode()
        self.assertEqual(fullPkt_LE_str,
                         b"\x01\x05\x08\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00"
                         b"\x03\x00\x00\x00\x44\x00\x00\x00"
                         b"\x04\x00\x00\x00blah"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x01\x00\x00\x00"
                         b"\x02\x00\x00\x00\x05\x00\x00\x00"
                         b"\x02\x00\x01\x00\x0A\x00\x00\x00\x14\x00\x00\x00"
                         b"\x02\x00\x00\x00\x1E\x00\x00\x00\x28\x00\x00\x00"
                         b"\x00\x00\x00\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        test_pducore(self, nullPkt_new, x.PDU_GET, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        test_pducore(self, fullPkt_new, x.PDU_GET, True, 1, 2, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_new.context, "blah")
        # Test decoding, full packet, little endian
        header, body = slicedata(fullPkt_LE_str, 20)
        header = decode_pduheader(header)
        fullPkt_LE_new = dec(body, header)
        test_pducore(self, fullPkt_LE_new, x.PDU_GET, False, 1, 2, 3)
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
        dec = ntp.agentx.decode_xGetPDU
        cls = ntp.agentx.GetNextPDU
        x = ntp.agentx
        srch = x.SearchRange

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        test_pducore(self, nullPkt, x.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (srch((1, 2, 3), (1, 2, 5), False),
                       srch((10, 20), (30, 40), True)),
                      context="blah")
        test_pducore(self, fullPkt, x.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(fullPkt.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test init, full packet, little endian
        fullPkt_LE = cls(False, 1, 2, 3,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)),
                         context="blah")
        test_pducore(self, fullPkt_LE, x.PDU_GET_NEXT, False, 1, 2, 3)
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
        test_pducore(self, nullPkt_new, x.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        test_pducore(self, fullPkt_new, x.PDU_GET_NEXT, True, 1, 2, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         (srch((1, 2, 3), (1, 2, 5), False),
                          srch((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt_new.context, "blah")
        # Test decoding, full packet, little endian
        header, body = slicedata(fullPkt_LE_str, 20)
        header = decode_pduheader(header)
        fullPkt_LE_new = dec(body, header)
        test_pducore(self, fullPkt_LE_new, x.PDU_GET_NEXT, False, 1, 2, 3)
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
        dec = ntp.agentx.decode_GetBulkPDU
        cls = ntp.agentx.GetBulkPDU
        x = ntp.agentx
        srch = x.SearchRange

        # Test init
        pkt = cls(True, 1, 2, 3, 1, 5,
                  (srch((1, 2), (3, 4), False),
                   srch((6, 7), (8, 9), True)),
                  context="blah")
        test_pducore(self, pkt, x.PDU_GET_BULK, True, 1, 2, 3)
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
        test_pducore(self, pkt_LE, x.PDU_GET_BULK, False, 1, 2, 3)
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
        test_pducore(self, pkt_new, x.PDU_GET_BULK, True, 1, 2, 3)
        self.assertEqual(pkt_new.nonReps, 1)
        self.assertEqual(pkt_new.maxReps, 5)
        self.assertEqual(pkt_new.oidranges,
                         ((srch((1, 2), (3, 4), False),
                           srch((6, 7), (8, 9), True))))
        self.assertEqual(pkt_new.context, "blah")
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_GET_BULK, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.nonReps, 1)
        self.assertEqual(pkt_LE_new.maxReps, 5)
        self.assertEqual(pkt_LE_new.oidranges,
                         ((srch((1, 2), (3, 4), False),
                           srch((6, 7), (8, 9), True))))
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
        dec = ntp.agentx.decode_TestSetPDU
        cls = ntp.agentx.TestSetPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3,
                  (x.Varbind(x.VALUE_OID, (1, 2, 3), x.OID((4, 5, 6), False)),
                   x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, x.PDU_TEST_SET, True, 1, 2, 3)
        self.assertEqual(pkt.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3,
                     (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                x.OID((4, 5, 6), False)),
                      x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, x.PDU_TEST_SET, False, 1, 2, 3)
        self.assertEqual(pkt_LE.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
        test_pducore(self, pkt_new, x.PDU_TEST_SET, True, 1, 2, 3)
        self.assertEqual(pkt_new.varbinds,
                         (x.Varbind(x.VALUE_OID,
                                    x.OID((1, 2, 3), False),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR,
                                    x.OID((1, 2, 4), False),
                                    "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decoding, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_TEST_SET, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.varbinds,
                         (x.Varbind(x.VALUE_OID,
                                    x.OID((1, 2, 3), False),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR,
                                    x.OID((1, 2, 4), False),
                                    "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 8,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": (x.Varbind(x.VALUE_OID,
                                                 x.OID((1, 2, 3), False),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR,
                                                 x.OID((1, 2, 4), False),
                                                 "blah")),
                          "context": "blah"})

    def test_CommitSetPDU(self):
        dec = ntp.agentx.decode_CommitSetPDU
        cls = ntp.agentx.CommitSetPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, x.PDU_COMMIT_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, x.PDU_COMMIT_SET, False, 1, 2, 3)
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
        test_pducore(self, pkt_new, x.PDU_COMMIT_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_COMMIT_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 9,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_UndoSetPDU(self):
        dec = ntp.agentx.decode_UndoSetPDU
        cls = ntp.agentx.UndoSetPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, x.PDU_UNDO_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, x.PDU_UNDO_SET, False, 1, 2, 3)
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
        test_pducore(self, pkt_new, x.PDU_UNDO_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_UNDO_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 10,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_CleanupSetPDU(self):
        dec = ntp.agentx.decode_CleanupSetPDU
        cls = ntp.agentx.CleanupSetPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3)
        test_pducore(self, pkt, x.PDU_CLEANUP_SET, True, 1, 2, 3)
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3)
        test_pducore(self, pkt_LE, x.PDU_CLEANUP_SET, False, 1, 2, 3)
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
        test_pducore(self, pkt_new, x.PDU_CLEANUP_SET, True, 1, 2, 3)
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_CLEANUP_SET, False, 1, 2, 3)
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 11,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3})

    def test_PingPDU(self):
        dec = ntp.agentx.decode_PingPDU
        cls = ntp.agentx.PingPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, "blah")
        test_pducore(self, pkt, x.PDU_PING, True, 1, 2, 3)
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, "blah")
        test_pducore(self, pkt_LE, x.PDU_PING, False, 1, 2, 3)
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
        test_pducore(self, pkt_new, x.PDU_PING, True, 1, 2, 3)
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_PING, False, 1, 2, 3)
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
        dec = ntp.agentx.decode_NotifyPDU
        cls = ntp.agentx.NotifyPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3,
                  (x.Varbind(x.VALUE_OID, (1, 2, 3), x.OID((4, 5, 6), False)),
                   x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, x.PDU_NOTIFY, True, 1, 2, 3)
        self.assertEqual(pkt.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3,
                     (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                x.OID((4, 5, 6), False)),
                      x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, x.PDU_NOTIFY, False, 1, 2, 3)
        self.assertEqual(pkt_LE.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
        test_pducore(self, pkt_new, x.PDU_NOTIFY, True, 1, 2, 3)
        self.assertEqual(pkt_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_NOTIFY, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_LE_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 12,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4),
                                                 "blah")),
                          "context": "blah"})

    def test_IndexAllocPDU(self):
        dec = ntp.agentx.decode_xIndexAllocPDU
        cls = ntp.agentx.IndexAllocPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  (x.Varbind(x.VALUE_OID, (1, 2, 3), x.OID((4, 5, 6), False)),
                   x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, x.PDU_INDEX_ALLOC, True, 1, 2, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, True, True,
                     (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                x.OID((4, 5, 6), False)),
                      x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, x.PDU_INDEX_ALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE.newIndex, True)
        self.assertEqual(pkt_LE.anyIndex, True)
        self.assertEqual(pkt_LE.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
        test_pducore(self, pkt_new, x.PDU_INDEX_ALLOC, True, 1, 2, 3)
        self.assertEqual(pkt_new.newIndex, True)
        self.assertEqual(pkt_new.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_INDEX_ALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.newIndex, True)
        self.assertEqual(pkt_LE_new.anyIndex, True)
        self.assertEqual(pkt_LE_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
                          "varbinds": (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4),
                                                 "blah")),
                          "context": "blah"})

    def test_IndexDeallocPDU(self):
        dec = ntp.agentx.decode_xIndexAllocPDU
        cls = ntp.agentx.IndexDeallocPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  (x.Varbind(x.VALUE_OID, (1, 2, 3), x.OID((4, 5, 6), False)),
                   x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        test_pducore(self, pkt, x.PDU_INDEX_DEALLOC, True, 1, 2, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, True, True,
                     (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                x.OID((4, 5, 6), False)),
                      x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")),
                     context="blah")
        test_pducore(self, pkt_LE, x.PDU_INDEX_DEALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE.newIndex, True)
        self.assertEqual(pkt_LE.anyIndex, True)
        self.assertEqual(pkt_LE.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
        test_pducore(self, pkt_new, x.PDU_INDEX_DEALLOC, True, 1, 2, 3)
        self.assertEqual(pkt_new.newIndex, True)
        self.assertEqual(pkt_new.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt_new.context, "blah")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_INDEX_DEALLOC, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.newIndex, True)
        self.assertEqual(pkt_LE_new.anyIndex, True)
        self.assertEqual(pkt_LE_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
                          "varbinds": (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4),
                                                 "blah")),
                          "context": "blah"})

    def test_AddAgentCapsPDU(self):
        dec = ntp.agentx.decode_AddAgentCapsPDU
        cls = ntp.agentx.AddAgentCapsPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), "blah", context="bluh")
        test_pducore(self, pkt, x.PDU_ADD_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt.description, "blah")
        self.assertEqual(pkt.context, "bluh")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, (4, 5, 6), "blah", context="bluh")
        test_pducore(self, pkt_LE, x.PDU_ADD_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE.oid, x.OID((4, 5, 6), False))
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
        test_pducore(self, pkt_new, x.PDU_ADD_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt_new.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt_new.description, "blah")
        self.assertEqual(pkt_new.context, "bluh")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_ADD_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE_new.description, "blah")
        self.assertEqual(pkt_LE_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 16,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": x.OID((4, 5, 6), False),
                          "description": "blah",
                          "context": "bluh"})

    def test_RMAgentCapsPDU(self):
        dec = ntp.agentx.decode_RMAgentCapsPDU
        cls = ntp.agentx.RMAgentCapsPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), context="bluh")
        test_pducore(self, pkt, x.PDU_RM_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt.context, "bluh")
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, (4, 5, 6), context="bluh")
        test_pducore(self, pkt_LE, x.PDU_RM_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE.oid, x.OID((4, 5, 6), False))
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
        test_pducore(self, pkt_new, x.PDU_RM_AGENT_CAPS, True, 1, 2, 3)
        self.assertEqual(pkt_new.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt_new.context, "bluh")
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_RM_AGENT_CAPS, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.oid, x.OID((4, 5, 6), False))
        self.assertEqual(pkt_LE_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 17,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": x.OID((4, 5, 6), False),
                          "context": "bluh"})

    def test_ResponsePDU(self):
        dec = ntp.agentx.decode_ResponsePDU
        cls = ntp.agentx.ResponsePDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, 4, 5, 6,
                  (x.Varbind(x.VALUE_OID, (1, 2, 3), x.OID((4, 5, 6), False)),
                   x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        test_pducore(self, pkt, x.PDU_RESPONSE, True, 1, 2, 3)
        self.assertEqual(pkt.sysUptime, 4)
        self.assertEqual(pkt.resError, 5)
        self.assertEqual(pkt.resIndex, 6)
        self.assertEqual(pkt.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test init, little endian
        pkt_LE = cls(False, 1, 2, 3, 4, 5, 6,
                     (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                x.OID((4, 5, 6), False)),
                      x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        test_pducore(self, pkt_LE, x.PDU_RESPONSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE.sysUptime, 4)
        self.assertEqual(pkt_LE.resError, 5)
        self.assertEqual(pkt_LE.resIndex, 6)
        self.assertEqual(pkt_LE.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
        test_pducore(self, pkt_new, x.PDU_RESPONSE, True, 1, 2, 3)
        self.assertEqual(pkt_new.sysUptime, 4)
        self.assertEqual(pkt_new.resError, 5)
        self.assertEqual(pkt_new.resIndex, 6)
        self.assertEqual(pkt_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
        # Test decode, little endian
        header, body = slicedata(pkt_LE_str, 20)
        header = decode_pduheader(header)
        pkt_LE_new = dec(body, header)
        test_pducore(self, pkt_LE_new, x.PDU_RESPONSE, False, 1, 2, 3)
        self.assertEqual(pkt_LE_new.sysUptime, 4)
        self.assertEqual(pkt_LE_new.resError, 5)
        self.assertEqual(pkt_LE_new.resIndex, 6)
        self.assertEqual(pkt_LE_new.varbinds,
                         (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                    x.OID((4, 5, 6), False)),
                          x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4), "blah")))
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
                          "varbinds": (x.Varbind(x.VALUE_OID, (1, 2, 3),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR, (1, 2, 4),
                                                 "blah"))})

    #
    # Data type tests
    #
    def test_integer32(self):
        enc = ntp.agentx.encode_integer32
        dec = ntp.agentx.decode_integer32

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
        enc = ntp.agentx.encode_nullvalue
        dec = ntp.agentx.decode_nullvalue

        # Encode
        self.assertEqual(enc(True, "this is ignored"), b"")
        # Decode
        self.assertEqual(dec(extraData, standardFlags), (None, extraData))

    def test_integer64(self):
        enc = ntp.agentx.encode_integer64
        dec = ntp.agentx.decode_integer64

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
        enc = ntp.agentx.encode_ipaddr
        dec = ntp.agentx.decode_ipaddr

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
        target = ntp.agentx.OID
        dec = ntp.agentx.decode_OID

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

    def test_searchrange(self):
        target = ntp.agentx.SearchRange
        dec = ntp.agentx.decode_SearchRange
        oid = ntp.agentx.OID

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

    def test_encode_searchrange_list(self):
        enc = ntp.agentx.encode_searchrange_list
        srch = ntp.agentx.SearchRange

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
        # Test, null terminated
        self.assertEqual(enc(True, (srch((1, 2), (1, 2), True),
                                    srch((2, 3), (3, 4))),
                         nullTerminate=True),
                         b"\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         b"\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         b"\x00\x00\x00\x00")

    def test_decode_searchrange_list(self):
        dec = ntp.agentx.decode_searchrange_list
        srch = ntp.agentx.SearchRange

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

    def test_decode_searchrange_list_nullterm(self):
        dec = ntp.agentx.decode_searchrange_list_nullterm
        srch = ntp.agentx.SearchRange

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
                         ((srch((1, 2), (1, 2), True),
                           srch((2, 3), (3, 4), False)),
                          b""))
        # Test, little endian
        self.assertEqual(dec(b"\x02\x00\x01\x00"
                             b"\x01\x00\x00\x00\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x01\x00\x00\x00\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x02\x00\x00\x00\x03\x00\x00\x00"
                             b"\x02\x00\x00\x00"
                             b"\x03\x00\x00\x00\x04\x00\x00\x00"
                             b"\x00\x00\x00\x00" + extraData, lilEndianFlags),
                         ((srch((1, 2), (1, 2), True),
                           srch((2, 3), (3, 4), False)),
                          extraData))

    def test_encode_octetstr(self):
        enc = ntp.agentx.encode_octetstr
        dec = ntp.agentx.decode_octetstr

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

    def test_Varbind(self):
        target = ntp.agentx.Varbind
        a = ntp.agentx

        # Test init
        cls = target(a.VALUE_INTEGER, (1, 2, 3), 42)
        self.assertEqual(cls.valueType, a.VALUE_INTEGER)
        self.assertEqual(cls.oid, a.OID((1, 2, 3), False))
        self.assertEqual(cls.payload, 42)
        # Test repr
        self.assertEqual(repr(cls),
                         "Varbind(vtype=2, oid=OID((1, 2, 3), False), "
                         "payload=42)")
        # Test payloadless types
        cls = target(a.VALUE_NULL, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x05\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(a.VALUE_NO_SUCH_OBJECT, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x80\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(a.VALUE_NO_SUCH_INSTANCE, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x81\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        cls = target(a.VALUE_END_OF_MIB_VIEW, (1, 2, 3))
        self.assertEqual(cls.encode(True),
                         b"\x00\x82\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test octet based types
        cls = target(a.VALUE_OCTET_STR, (1, 2, 3), (1, 2, 3, 4, 5))
        self.assertEqual(cls.encode(True),
                         b"\x00\x04\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x05"
                         b"\x01\x02\x03\x04\x05\x00\x00\x00")
        cls = target(a.VALUE_IP_ADDR, (1, 2, 3), (16, 32, 48, 64))
        self.assertEqual(cls.encode(True),
                         b"\x00\x40\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x04\x10\x20\x30\x40")
        # Test integer32 types
        cls = target(a.VALUE_INTEGER, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x02\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        cls = target(a.VALUE_COUNTER32, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x41\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        cls = target(a.VALUE_GAUGE32, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x42\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        cls = target(a.VALUE_TIME_TICKS, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x43\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x2A")
        # Test integer64 type
        cls = target(a.VALUE_COUNTER64, (1, 2, 3), 42)
        self.assertEqual(cls.encode(True),
                         b"\x00\x46\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x00\x00\x00\x00\x00\x00\x00\x2A")
        # Test oid type
        cls = target(a.VALUE_OID, (1, 2, 3), a.OID((16, 42, 256), False))
        self.assertEqual(cls.encode(True),
                         b"\x00\x06\x00\x00\x03\x00\x00\x00"
                         b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         b"\x03\x00\x00\x00\x00\x00\x00\x10"
                         b"\x00\x00\x00\x2A\x00\x00\x01\x00")
        # Test oid type, little endian
        cls = target(a.VALUE_OID, (1, 2, 3), a.OID((16, 42, 256), False))
        self.assertEqual(cls.encode(False),
                         b"\x06\x00\x00\x00\x03\x00\x00\x00"
                         b"\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                         b"\x03\x00\x00\x00\x10\x00\x00\x00"
                         b"\x2A\x00\x00\x00\x00\x01\x00\x00")

    def test_decode_varbind(self):
        f = ntp.agentx.decode_Varbind
        a = ntp.agentx

        # Test payloadless types
        self.assertEqual(f(b"\x00\x05\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (a.Varbind(a.VALUE_NULL, a.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x80\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (a.Varbind(a.VALUE_NO_SUCH_OBJECT,
                                    a.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x81\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (a.Varbind(a.VALUE_NO_SUCH_INSTANCE,
                                    a.OID((1, 2, 3), False)),
                          b""))
        self.assertEqual(f(b"\x00\x82\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         (a.Varbind(a.VALUE_END_OF_MIB_VIEW,
                                    a.OID((1, 2, 3), False)),
                          b""))
        # Test octet based types
        self.assertEqual(f(b"\x00\x04\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x0512345\x00\x00\x00",
                           standardFlags),
                         (a.Varbind(a.VALUE_OCTET_STR,
                                    a.OID((1, 2, 3), False),
                                    "12345"),
                          b""))
        self.assertEqual(f(b"\x00\x40\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x04\x10\x20\x30\x40", standardFlags),
                         (a.Varbind(a.VALUE_IP_ADDR,
                                    a.OID((1, 2, 3), False),
                                    (16, 32, 48, 64)),
                          b""))
        # Test integer32 types
        self.assertEqual(f(b"\x00\x02\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (a.Varbind(a.VALUE_INTEGER,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))
        self.assertEqual(f(b"\x00\x41\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (a.Varbind(a.VALUE_COUNTER32,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))
        self.assertEqual(f(b"\x00\x42\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (a.Varbind(a.VALUE_GAUGE32,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))
        self.assertEqual(f(b"\x00\x43\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x2A", standardFlags),
                         (a.Varbind(a.VALUE_TIME_TICKS,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))
        # Test integer64 type
        self.assertEqual(f(b"\x00\x46\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x00\x00\x00\x00\x00\x00\x00\x2A", standardFlags),
                         (a.Varbind(a.VALUE_COUNTER64,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))
        # Test oid type
        self.assertEqual(f(b"\x00\x06\x00\x00\x03\x00\x00\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           b"\x03\x00\x00\x00\x00\x00\x00\x10"
                           b"\x00\x00\x00\x2A\x00\x00\x01\x00", standardFlags),
                         (a.Varbind(a.VALUE_OID,
                                    a.OID((1, 2, 3), False),
                                    a.OID((16, 42, 256), False)),
                          b""))
        # Test integer32 with little endian
        self.assertEqual(f(b"\x43\x00\x00\x00\x03\x00\x00\x00"
                           b"\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                           b"\x2A\x00\x00\x00", lilEndianFlags),
                         (a.Varbind(a.VALUE_TIME_TICKS,
                                    a.OID((1, 2, 3), False),
                                    42),
                          b""))

    def test_encode_flagbyte(self):
        f = ntp.agentx.encode_flagbyte
        self.assertEqual(f(makeflags(False, True, False, True, False)), 0x0A)
        self.assertEqual(f(makeflags(True, False, True, False, True)), 0x15)

    def test_decode_flagbyte(self):
        f = ntp.agentx.decode_flagbyte
        self.assertEqual(f(0x0A), makeflags(False, True, False, True, False))
        self.assertEqual(f(0x15), makeflags(True, False, True, False, True))

    #
    # Misc tests
    #
    def test_makeflags(self):
        f = ntp.agentx.makeflags
        self.assertEqual(f(True, False, True, False, True),
                         {"instReg": True,
                          "newIndex": False,
                          "anyIndex":  True,
                          "contextP": False,
                          "bigEndian": True})

    def test_getendian(self):
        f = ntp.agentx.getendian

        # Test big endian
        self.assertEqual(f(True), ">")
        # Test little endian
        self.assertEqual(f(False), "<")

    def test_encode_pduheader(self):
        f = ntp.agentx.encode_pduheader
        a = ntp.agentx

        # Test "empty" header
        self.assertEqual(f(a.PDU_OPEN,
                           False, False, False, False, False,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         b"\x01\x01\x00\x00"
                         b"\xEF\xBE\xAD\xDE\xBE\xBA\xFE\xCA"
                         b"\x0D\xF0\xCE\xFA\x00\x00\x00\x00")
        # Test flags
        self.assertEqual(f(a.PDU_OPEN,
                           True, True, True, True, True,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         b"\x01\x01\x1F\x00"
                         b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                         b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00")

    def test_decode_pduheader(self):
        f = ntp.agentx.decode_pduheader
        a = ntp.agentx

        # Test "empty" header
        self.assertEqual(f(b"\x01\x01\x10\x00"
                           b"\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           b"\xFA\xCE\xF0\x0D\x00\x00\x00\x00"),
                         {"version": 1,
                          "type": a.PDU_OPEN,
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
                          "type": a.PDU_OPEN,
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
                          "type": a.PDU_OPEN,
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
                          "type": a.PDU_OPEN,
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
        f = ntp.agentx.decode_packet
        x = ntp.agentx
        srch = x.SearchRange
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
                         (x.OpenPDU(True, 12, 34, 56, 78,
                                    x.OID((1, 2, 3, 4), False),
                                    "foo"),
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
                         (x.OpenPDU(True, 12, 34, 56, 78,
                                    x.OID((1, 2, 3, 4), False),
                                    "foo"),
                          extraData))
        # Test close
        self.assertEqual(f(b"\x01\x02\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x01\x00\x00\x00"),
                         (x.ClosePDU(True, 1, 2, 3, x.RSN_OTHER),
                          b""))
        # Test register
        self.assertEqual(f(b"\x01\x03\x11\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x14"
                           b"\x04\x05\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (x.RegisterPDU(True, 1, 2, 3, 4, 5,
                                        x.OID((1, 2, 3), False),
                                        0, None, None),
                          b""))
        # Test unregister
        self.assertEqual(f(b"\x01\x04\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x14"
                           b"\x00\x05\x00\x00"
                           b"\x03\x00\x00\x00\x00\x00\x00\x01"
                           b"\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (x.UnregisterPDU(True, 1, 2, 3, 5,
                                          x.OID((1, 2, 3), False),
                                          0, None, None),
                          b""))
        # Test get
        self.assertEqual(f(b"\x01\x05\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x00\x00\x00\x00"),
                         (x.GetPDU(True, 1, 2, 3, ()),
                          b""))
        # Test get next
        self.assertEqual(f(b"\x01\x06\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.GetNextPDU(True, 1, 2, 3, ()),
                          b""))
        # Test get bulk
        self.assertEqual(f(b"\x01\x07\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x34"
                           b"\x00\x01\x00\x05"
                           b"\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           b"\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                           b"\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"),
                         (x.GetBulkPDU(True, 1, 2, 3, 1, 5,
                                       (srch((1, 2), (3, 4), False),
                                        srch((6, 7), (8, 9), True))),
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
                         (x.TestSetPDU(True, 1, 2, 3,
                                       (x.Varbind(x.VALUE_OID,
                                                  x.OID((1, 2, 3), False),
                                                  x.OID((4, 5, 6), False)),
                                        x.Varbind(x.VALUE_OCTET_STR,
                                                  x.OID((1, 2, 4), False),
                                                  "blah"))),
                          b""))
        # Test commit set
        self.assertEqual(f(b"\x01\x09\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.CommitSetPDU(True, 1, 2, 3),
                          b""))
        # Test undo set
        self.assertEqual(f(b"\x01\x0A\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.UndoSetPDU(True, 1, 2, 3),
                          b""))
        # Test cleanup set
        self.assertEqual(f(b"\x01\x0B\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.CleanupSetPDU(True, 1, 2, 3),
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
                         (x.NotifyPDU(True, 1, 2, 3,
                                      (x.Varbind(x.VALUE_OID,
                                                 x.OID((1, 2, 3), False),
                                                 x.OID((4, 5, 6), False)),
                                       x.Varbind(x.VALUE_OCTET_STR,
                                                 x.OID((1, 2, 4), False),
                                                 "blah"))),
                          b""))
        # Test ping
        self.assertEqual(f(b"\x01\x0D\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.PingPDU(True, 1, 2, 3),
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
                         (x.IndexAllocPDU(True, 1, 2, 3, True, True,
                                          (x.Varbind(x.VALUE_OID,
                                                     x.OID((1, 2, 3), False),
                                                     x.OID((4, 5, 6), False)),
                                           x.Varbind(x.VALUE_OCTET_STR,
                                                     x.OID((1, 2, 4), False),
                                                     "blah"))),
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
                         (x.IndexDeallocPDU(True, 1, 2, 3, True, True,
                                            (x.Varbind(x.VALUE_OID,
                                                       x.OID((1, 2, 3),
                                                             False),
                                                       x.OID((4, 5, 6),
                                                             False)),
                                             x.Varbind(x.VALUE_OCTET_STR,
                                                       x.OID((1, 2, 4),
                                                             False),
                                                       "blah"))),
                          b""))
        # Test add agent caps
        self.assertEqual(f(b"\x01\x10\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x18"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"
                           b"\x00\x00\x00\x04blah"),
                         (x.AddAgentCapsPDU(True, 1, 2, 3,
                                            x.OID((4, 5, 6), False),
                                            "blah"),
                          b""))
        # Test rm agent caps
        self.assertEqual(f(b"\x01\x11\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x10"
                           b"\x03\x00\x00\x00\x00\x00\x00\x04"
                           b"\x00\x00\x00\x05\x00\x00\x00\x06"),
                         (x.RMAgentCapsPDU(True, 1, 2, 3,
                                           x.OID((4, 5, 6), False)),
                          b""))
        # Test response
        self.assertEqual(f(b"\x01\x12\x10\x00"
                           b"\x00\x00\x00\x01\x00\x00\x00\x02"
                           b"\x00\x00\x00\x03\x00\x00\x00\x08"
                           b"\x00\x00\x00\x04\x00\x05\x00\x06"),
                         (x.ResponsePDU(True, 1, 2, 3, 4, 5, 6),
                          b""))
        # Test insufficient data
        try:
            f(b"\x01\x11\x10\x00"
              b"\x00\x00\x00\x01\x00\x00\x00\x02"
              b"\x00\x00\x00\x03\x00\x00\x00\x10"
              b"\x03\x00\x00\x00\x00\x00\x00\x04")
            fail = False
        except IndexError:
            fail = True
        self.assertEqual(fail, True)

    def test_mibTree2List(self):
        f = ntp.agentx.mibTree2List

        # Test empty tree
        self.assertEqual(f({}), ())
        # Test flat tree
        self.assertEqual(f({0: None, 1: None, 3: None, 4: None}),
                         ((0,), (1,), (3,), (4,)))
        # Test nested tree
        self.assertEqual(f({0: None,
                            2: {0: None, 1: None},
                            3: {5: {0: None, 2: None}, 6: None},
                            4: None}),
                         ((0,),
                          (2,), (2, 0), (2, 1),
                          (3,), (3, 5), (3, 5, 0), (3, 5, 2), (3, 6),
                          (4,)))

    def test_mibList2Tree(self):
        f = ntp.agentx.mibList2Tree

        # Test empty tree
        self.assertEqual(f(tuple()), {})
        # Test flat tree
        self.assertEqual(f(((0,), (1,), (3,), (4,))),
                         {0: None, 1: None, 3: None, 4:None})
        # Test nested tree
        self.assertEqual(f(((0,),
                            (2,), (2, 0), (2, 1),
                            (3,), (3, 5), (3, 5, 0), (3, 5, 2), (3, 6),
                            (4,))),
                         {0: None,
                          2: {0: None, 1: None},
                          3: {5: {0: None, 2: None}, 6: None},
                          4: None})


if __name__ == "__main__":
    unittest.main()
