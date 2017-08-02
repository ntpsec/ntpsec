#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.agentx

from ntp.agentx import slicedata, decode_pduheader

extraData = "Would you kindly ignore this?"

maximumOIDsubs = tuple(range(1, 129))
maximumOIDstr = "\x80\x00\x00\x00\
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
"

# The most commonly used flag setups, some tests use custom flags
standardFlags = {"flags": {"instReg": False,
                           "newIndex": False,
                           "anyIndex": False,
                           "contextP": False,
                           "bigEndian": True}}
standardFlags_bare = standardFlags["flags"]
lilEndianFlags = {"flags": {"instReg": False,
                            "newIndex": False,
                            "anyIndex": False,
                            "contextP": False,
                            "bigEndian": False}}
contextFlags = {"flags": {"instReg": False,
                          "newIndex": False,
                          "anyIndex": False,
                          "contextP": True,
                          "bigEndian": True}}


def makeFlags(iR, nI, aI, cP, bE):
    return {"flags": {"instReg": iR,
                      "newIndex": nI,
                      "anyIndex": aI,
                      "contextP": cP,
                      "bigEndian": bE}}


class TestNtpclientsNtpsnmpd(unittest.TestCase):
    #
    # PDU tests
    #
    def test_OpenPDU(self):
        dec = ntp.agentx.decode_OpenPDU
        cls = ntp.agentx.OpenPDU

        # Test PDU init, null packet
        nullPkt = cls(True, 1, 2, 3, 4, (), "")
        self.assertEqual(nullPkt.bigEndian, True)
        self.assertEqual(nullPkt.sessionID, 1)
        self.assertEqual(nullPkt.transactionID, 2)
        self.assertEqual(nullPkt.packetID, 3)
        self.assertEqual(nullPkt.timeout, 4)
        self.assertEqual(nullPkt.oid, ())
        self.assertEqual(nullPkt.description, "")
        # Test PDU init, basic packet
        basicPkt = cls(False, 1, 2, 3, 4, (1, 2, 3, 4), "foo")
        self.assertEqual(basicPkt.bigEndian, False)
        self.assertEqual(basicPkt.sessionID, 1)
        self.assertEqual(basicPkt.transactionID, 2)
        self.assertEqual(basicPkt.packetID, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.oid, (1, 2, 3, 4))
        self.assertEqual(basicPkt.description, "foo")
        # Test encoding, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         "\x01\x01\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x0C"
                         "\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
        # Test encoding, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         "\x01\x01\x00\x00"
                         "\x01\x00\x00\x00\x02\x00\x00\x00"
                         "\x03\x00\x00\x00\x20\x00\x00\x00"
                         "\x04\x00\x00\x00"
                         "\x04\x00\x00\x00"
                         "\x01\x00\x00\x00\x02\x00\x00\x00"
                         "\x03\x00\x00\x00\x04\x00\x00\x00"
                         "\x03\x00\x00\x00foo\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        self.assertEqual(nullPkt_new.bigEndian, True)
        self.assertEqual(nullPkt_new.sessionID, 1)
        self.assertEqual(nullPkt_new.transactionID, 2)
        self.assertEqual(nullPkt_new.packetID, 3)
        self.assertEqual(nullPkt_new.timeout, 4)
        self.assertEqual(nullPkt_new.oid, {"subids": (), "include": False})
        self.assertEqual(nullPkt_new.description, "")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        self.assertEqual(basicPkt_new.bigEndian, False)
        self.assertEqual(basicPkt_new.sessionID, 1)
        self.assertEqual(basicPkt_new.transactionID, 2)
        self.assertEqual(basicPkt_new.packetID, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.oid, {"subids": (1, 2, 3, 4),
                                            "include": False})
        self.assertEqual(basicPkt_new.description, "foo")
        # Test packetVars
        self.assertEqual(basicPkt_new.packetVars(),
                         {"pduType": 1,
                          "bigEndian": False,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "timeout": 4,
                          "oid": {"subids": (1, 2, 3, 4), "include": False},
                          "description": "foo"})

    def test_ClosePDU(self):
        dec = ntp.agentx.decode_ClosePDU
        cls = ntp.agentx.ClosePDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, x.RSN_OTHER)
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.reason, x.RSN_OTHER)
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x02\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x01\x00\x00\x00")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.reason, x.RSN_OTHER)
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

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3))
        self.assertEqual(basicPkt.bigEndian, True)
        self.assertEqual(basicPkt.sessionID, 1)
        self.assertEqual(basicPkt.transactionID, 2)
        self.assertEqual(basicPkt.packetID, 3)
        self.assertEqual(basicPkt.timeout, 4)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, (1, 2, 3))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 4, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        self.assertEqual(fancyPkt.bigEndian, True)
        self.assertEqual(fancyPkt.sessionID, 1)
        self.assertEqual(fancyPkt.transactionID, 2)
        self.assertEqual(fancyPkt.packetID, 3)
        self.assertEqual(fancyPkt.timeout, 4)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, (1, 2, 3))
        self.assertEqual(fancyPkt.rangeSubid, 5)
        self.assertEqual(fancyPkt.upperBound, 23)
        self.assertEqual(fancyPkt.context, "blah")
        # Test encode, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         "\x01\x03\x11\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x14"
                         "\x04\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test encode, fancy packet
        fancyPkt_str = fancyPkt.encode()
        self.assertEqual(fancyPkt_str,
                         "\x01\x03\x19\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04blah"
                         "\x04\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        self.assertEqual(basicPkt_new.bigEndian, True)
        self.assertEqual(basicPkt_new.sessionID, 1)
        self.assertEqual(basicPkt_new.transactionID, 2)
        self.assertEqual(basicPkt_new.packetID, 3)
        self.assertEqual(basicPkt_new.timeout, 4)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, {"subids": (1, 2, 3),
                                                "include": False})
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        self.assertEqual(fancyPkt_new.bigEndian, True)
        self.assertEqual(fancyPkt_new.sessionID, 1)
        self.assertEqual(fancyPkt_new.transactionID, 2)
        self.assertEqual(fancyPkt_new.packetID, 3)
        self.assertEqual(fancyPkt_new.timeout, 4)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, {"subids": (1, 2, 3),
                                                "include": False})
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
                          "subtree": {"subids": (1, 2, 3), "include": False},
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})

    def test_UnregisterPDU(self):
        dec = ntp.agentx.decode_xRegisterPDU
        cls = ntp.agentx.UnregisterPDU

        # Test init, basic packet
        basicPkt = cls(True, 1, 2, 3, 5, (1, 2, 3))
        self.assertEqual(basicPkt.bigEndian, True)
        self.assertEqual(basicPkt.sessionID, 1)
        self.assertEqual(basicPkt.transactionID, 2)
        self.assertEqual(basicPkt.packetID, 3)
        self.assertEqual(basicPkt.priority, 5)
        self.assertEqual(basicPkt.subtree, (1, 2, 3))
        self.assertEqual(basicPkt.rangeSubid, 0)
        self.assertEqual(basicPkt.upperBound, None)
        self.assertEqual(basicPkt.context, None)
        # Test init, fancy packet
        fancyPkt = cls(True, 1, 2, 3, 5, (1, 2, 3),
                       rangeSubid=5, upperBound=23, context="blah")
        self.assertEqual(fancyPkt.bigEndian, True)
        self.assertEqual(fancyPkt.sessionID, 1)
        self.assertEqual(fancyPkt.transactionID, 2)
        self.assertEqual(fancyPkt.packetID, 3)
        self.assertEqual(fancyPkt.priority, 5)
        self.assertEqual(fancyPkt.subtree, (1, 2, 3))
        self.assertEqual(fancyPkt.rangeSubid, 5)
        self.assertEqual(fancyPkt.upperBound, 23)
        self.assertEqual(fancyPkt.context, "blah")
        # Test encode, basic packet
        basicPkt_str = basicPkt.encode()
        self.assertEqual(basicPkt_str,
                         "\x01\x04\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x14"
                         "\x00\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test encode, fancy packet
        fancyPkt_str = fancyPkt.encode()
        self.assertEqual(fancyPkt_str,
                         "\x01\x04\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04blah"
                         "\x00\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")
        # Test decoding, basic packet
        header, body = slicedata(basicPkt_str, 20)
        header = decode_pduheader(header)
        basicPkt_new = dec(body, header)
        self.assertEqual(basicPkt_new.bigEndian, True)
        self.assertEqual(basicPkt_new.sessionID, 1)
        self.assertEqual(basicPkt_new.transactionID, 2)
        self.assertEqual(basicPkt_new.packetID, 3)
        self.assertEqual(basicPkt_new.priority, 5)
        self.assertEqual(basicPkt_new.subtree, {"subids": (1, 2, 3),
                                                "include": False})
        self.assertEqual(basicPkt_new.rangeSubid, 0)
        self.assertEqual(basicPkt_new.upperBound, None)
        self.assertEqual(basicPkt_new.context, None)
        # Test decoding, fancy packet
        header, body = slicedata(fancyPkt_str, 20)
        header = decode_pduheader(header)
        fancyPkt_new = dec(body, header)
        self.assertEqual(fancyPkt_new.bigEndian, True)
        self.assertEqual(fancyPkt_new.sessionID, 1)
        self.assertEqual(fancyPkt_new.transactionID, 2)
        self.assertEqual(fancyPkt_new.packetID, 3)
        self.assertEqual(fancyPkt_new.priority, 5)
        self.assertEqual(fancyPkt_new.subtree, {"subids": (1, 2, 3),
                                                "include": False})
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
                          "subtree": {"subids": (1, 2, 3), "include": False},
                          "rangeSubid": 0,
                          "upperBound": None,
                          "context": None})

    def test_GetPDU(self):
        dec = ntp.agentx.decode_xGetPDU
        cls = ntp.agentx.GetPDU

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        self.assertEqual(nullPkt.bigEndian, True)
        self.assertEqual(nullPkt.sessionID, 1)
        self.assertEqual(nullPkt.transactionID, 2)
        self.assertEqual(nullPkt.packetID, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (((1, 2, 3), (1, 2, 5), False),
                       ((10, 20), (30, 40), True)),
                      context="blah")
        self.assertEqual(fullPkt.bigEndian, True)
        self.assertEqual(fullPkt.sessionID, 1)
        self.assertEqual(fullPkt.transactionID, 2)
        self.assertEqual(fullPkt.packetID, 3)
        self.assertEqual(fullPkt.oidranges, (((1, 2, 3), (1, 2, 5), False),
                                             ((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test encode, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         "\x01\x05\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x00")
        # Test encode, full packet
        fullPkt_str = fullPkt.encode()
        self.assertEqual(fullPkt_str,
                         "\x01\x05\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x44"
                         "\x00\x00\x00\x04blah"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x05"
                         "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                         "\x00\x00\x00\x00")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        self.assertEqual(nullPkt_new.bigEndian, True)
        self.assertEqual(nullPkt_new.sessionID, 1)
        self.assertEqual(nullPkt_new.transactionID, 2)
        self.assertEqual(nullPkt_new.packetID, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        self.assertEqual(fullPkt_new.bigEndian, True)
        self.assertEqual(fullPkt_new.sessionID, 1)
        self.assertEqual(fullPkt_new.transactionID, 2)
        self.assertEqual(fullPkt_new.packetID, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         ({"start": {"subids": (1, 2, 3), "include": False},
                           "end": {"subids": (1, 2, 5), "include": False}},
                          {"start": {"subids": (10, 20), "include": True},
                           "end": {"subids": (30, 40), "include": False}}))
        self.assertEqual(fullPkt_new.context, "blah")
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

        # Test init, null packet
        nullPkt = cls(True, 1, 2, 3, ())
        self.assertEqual(nullPkt.bigEndian, True)
        self.assertEqual(nullPkt.sessionID, 1)
        self.assertEqual(nullPkt.transactionID, 2)
        self.assertEqual(nullPkt.packetID, 3)
        self.assertEqual(nullPkt.oidranges, ())
        self.assertEqual(nullPkt.context, None)
        # Test init, full packet
        fullPkt = cls(True, 1, 2, 3,
                      (((1, 2, 3), (1, 2, 5), False),
                       ((10, 20), (30, 40), True)),
                      context="blah")
        self.assertEqual(fullPkt.bigEndian, True)
        self.assertEqual(fullPkt.sessionID, 1)
        self.assertEqual(fullPkt.transactionID, 2)
        self.assertEqual(fullPkt.packetID, 3)
        self.assertEqual(fullPkt.oidranges, (((1, 2, 3), (1, 2, 5), False),
                                             ((10, 20), (30, 40), True)))
        self.assertEqual(fullPkt.context, "blah")
        # Test encode, null packet
        nullPkt_str = nullPkt.encode()
        self.assertEqual(nullPkt_str,
                         "\x01\x06\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test encode, full packet
        fullPkt_str = fullPkt.encode()
        self.assertEqual(fullPkt_str,
                         "\x01\x06\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x40"
                         "\x00\x00\x00\x04blah"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x05"
                         "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28")
        # Test decoding, null packet
        header, body = slicedata(nullPkt_str, 20)
        header = decode_pduheader(header)
        nullPkt_new = dec(body, header)
        self.assertEqual(nullPkt_new.bigEndian, True)
        self.assertEqual(nullPkt_new.sessionID, 1)
        self.assertEqual(nullPkt_new.transactionID, 2)
        self.assertEqual(nullPkt_new.packetID, 3)
        self.assertEqual(nullPkt_new.oidranges, ())
        self.assertEqual(nullPkt_new.context, None)
        # Test decoding, full packet
        header, body = slicedata(fullPkt_str, 20)
        header = decode_pduheader(header)
        fullPkt_new = dec(body, header)
        self.assertEqual(fullPkt_new.bigEndian, True)
        self.assertEqual(fullPkt_new.sessionID, 1)
        self.assertEqual(fullPkt_new.transactionID, 2)
        self.assertEqual(fullPkt_new.packetID, 3)
        self.assertEqual(fullPkt_new.oidranges,
                         ({"start": {"subids": (1, 2, 3), "include": False},
                           "end": {"subids": (1, 2, 5), "include": False}},
                          {"start": {"subids": (10, 20), "include": True},
                           "end": {"subids": (30, 40), "include": False}}))
        self.assertEqual(fullPkt_new.context, "blah")
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

        # Test init
        pkt = cls(True, 1, 2, 3, 1, 5,
                  (((1, 2), (3, 4), False),
                   ((6, 7), (8, 9), True)),
                  context="blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.nonReps, 1)
        self.assertEqual(pkt.maxReps, 5)
        self.assertEqual(pkt.oidranges,
                         (((1, 2), (3, 4), False),
                          ((6, 7), (8, 9), True)))
        self.assertEqual(pkt.context, "blah")
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x07\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x3C"
                         "\x00\x00\x00\x04blah"
                         "\x00\x01\x00\x05"
                         "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                         "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.nonReps, 1)
        self.assertEqual(pkt_new.maxReps, 5)
        self.assertEqual(pkt_new.oidranges,
                         ({"start": {"subids": (1, 2), "include": False},
                           "end": {"subids": (3, 4), "include": False}},
                          {"start": {"subids": (6, 7), "include": True},
                           "end": {"subids": (8, 9), "include": False}}))
        self.assertEqual(pkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 7,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "nonReps": 1,
                          "maxReps": 5,
                          "oidranges": ({"start": {"subids": (1, 2),
                                                   "include": False},
                                         "end": {"subids": (3, 4),
                                                 "include": False}},
                                        {"start": {"subids": (6, 7),
                                                   "include": True},
                                         "end": {"subids": (8, 9),
                                                 "include": False}}),
                          "context": "blah"})

    def test_TestSetPDU(self):
        dec = ntp.agentx.decode_TestSetPDU
        cls = ntp.agentx.TestSetPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3,
                  ((x.OID, (1, 2, 3), (4, 5, 6), False),
                   (x.OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.varbinds,
                         ((x.OID, (1, 2, 3), (4, 5, 6), False),
                          (x.OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test encoding
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x08\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x48"
                         "\x00\x00\x00\x04blah"
                         "\x00\x06\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x04\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x04"
                         "\x00\x00\x00\x04blah")
        # Test decoding
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.varbinds,
                         ({"type": x.OID,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": {"subids": (4, 5, 6), "include": False}},
                          {"type": x.OCTET_STR,
                           "name": {"subids": (1, 2, 4), "include": False},
                           "data": "blah"}))
        self.assertEqual(pkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 8,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"}),
                         "context": "blah"})

    def test_CommitSetPDU(self):
        dec = ntp.agentx.decode_CommitSetPDU
        cls = ntp.agentx.CommitSetPDU

        # Test init
        pkt = cls(True, 1, 2, 3)
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x09\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
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

        # Test init
        pkt = cls(True, 1, 2, 3)
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0A\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
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

        # Test init
        pkt = cls(True, 1, 2, 3)
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0B\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
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

        # Test init
        pkt = cls(True, 1, 2, 3, "blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0D\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x08"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.context, "blah")
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
                  ((x.OID, (1, 2, 3), (4, 5, 6), False),
                   (x.OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.varbinds,
                         ((6, (1, 2, 3), (4, 5, 6), False),
                          (4, (1, 2, 4), 'blah')))
        self.assertEqual(pkt.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0C\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x48"
                         "\x00\x00\x00\x04blah"
                         "\x00\x06\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x04\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x04"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.varbinds,
                         ({"type": x.OID,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": {"subids": (4, 5, 6), "include": False}},
                          {"type": x.OCTET_STR,
                           "name": {"subids": (1, 2, 4), "include": False},
                           "data": "blah"}))
        self.assertEqual(pkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 12,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"}),
                          "context": "blah"})

    def test_IndexAllocPDU(self):
        dec = ntp.agentx.decode_xIndexAllocPDU
        cls = ntp.agentx.IndexAllocPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  ((x.OID, (1, 2, 3), (4, 5, 6), False),
                   (x.OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         ((x.OID, (1, 2, 3), (4, 5, 6), False),
                          (x.OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0E\x1E\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x48"
                         "\x00\x00\x00\x04blah"
                         "\x00\x06\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x04\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x04"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         ({"type": x.OID,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": {"subids": (4, 5, 6), "include": False}},
                          {"type": x.OCTET_STR,
                           "name": {"subids": (1, 2, 4), "include": False},
                           "data": "blah"}))
        self.assertEqual(pkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 14,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "newIndex": True,
                          "anyIndex": True,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"}),
                          "context": "blah"})

    def test_IndexDeallocPDU(self):
        dec = ntp.agentx.decode_xIndexAllocPDU
        cls = ntp.agentx.IndexDeallocPDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, True, True,
                  ((x.OID, (1, 2, 3), (4, 5, 6), False),
                   (x.OCTET_STR, (1, 2, 4), "blah")),
                  context="blah")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt.varbinds,
                         ((x.OID, (1, 2, 3), (4, 5, 6), False),
                          (x.OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.context, "blah")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x0F\x1E\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x48"
                         "\x00\x00\x00\x04blah"
                         "\x00\x06\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x04\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x04"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt.newIndex, True)
        self.assertEqual(pkt.anyIndex, True)
        self.assertEqual(pkt_new.varbinds,
                         ({"type": x.OID,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": {"subids": (4, 5, 6), "include": False}},
                          {"type": x.OCTET_STR,
                           "name": {"subids": (1, 2, 4), "include": False},
                           "data": "blah"}))
        self.assertEqual(pkt_new.context, "blah")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 15,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "newIndex": True,
                          "anyIndex": True,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"}),
                          "context": "blah"})

    def test_AddAgentCapsPDU(self):
        dec = ntp.agentx.decode_AddAgentCapsPDU
        cls = ntp.agentx.AddAgentCapsPDU

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), "blah", context="bluh")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.oid, (4, 5, 6))
        self.assertEqual(pkt.description, "blah")
        self.assertEqual(pkt.context, "bluh")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x10\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04bluh"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.oid, {"subids": (4, 5, 6),
                                       "include": False})
        self.assertEqual(pkt_new.description, "blah")
        self.assertEqual(pkt_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 16,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": {"subids": (4, 5, 6), "include": False},
                          "description": "blah",
                          "context": "bluh"})

    def test_RMAgentCapsPDU(self):
        dec = ntp.agentx.decode_RMAgentCapsPDU
        cls = ntp.agentx.RMAgentCapsPDU

        # Test init
        pkt = cls(True, 1, 2, 3, (4, 5, 6), context="bluh")
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.oid, (4, 5, 6))
        self.assertEqual(pkt.context, "bluh")
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x11\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x18"
                         "\x00\x00\x00\x04bluh"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.oid, {"subids": (4, 5, 6),
                                       "include": False})
        self.assertEqual(pkt_new.context, "bluh")
        # Test packetVars
        self.assertEqual(pkt_new.packetVars(),
                         {"pduType": 17,
                          "bigEndian": True,
                          "sessionID": 1,
                          "transactionID": 2,
                          "packetID": 3,
                          "oid": {"subids": (4, 5, 6), "include": False},
                          "context": "bluh"})

    def test_ResponsePDU(self):
        dec = ntp.agentx.decode_ResponsePDU
        cls = ntp.agentx.ResponsePDU
        x = ntp.agentx

        # Test init
        pkt = cls(True, 1, 2, 3, 4, 5, 6,
                  ((x.OID, (1, 2, 3), (4, 5, 6), False),
                   (x.OCTET_STR, (1, 2, 4), "blah")))
        self.assertEqual(pkt.bigEndian, True)
        self.assertEqual(pkt.sessionID, 1)
        self.assertEqual(pkt.transactionID, 2)
        self.assertEqual(pkt.packetID, 3)
        self.assertEqual(pkt.sysUptime, 4)
        self.assertEqual(pkt.resError, 5)
        self.assertEqual(pkt.resIndex, 6)
        self.assertEqual(pkt.varbinds,
                         ((6, (1, 2, 3), (4, 5, 6), False),
                          (4, (1, 2, 4), 'blah')))
        # Test encode
        pkt_str = pkt.encode()
        self.assertEqual(pkt_str,
                         "\x01\x12\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x48"
                         "\x00\x00\x00\x04\x00\x05\x00\x06"
                         "\x00\x06\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x04\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x04"
                         "\x00\x00\x00\x04blah")
        # Test decode
        header, body = slicedata(pkt_str, 20)
        header = decode_pduheader(header)
        pkt_new = dec(body, header)
        self.assertEqual(pkt_new.bigEndian, True)
        self.assertEqual(pkt_new.sessionID, 1)
        self.assertEqual(pkt_new.transactionID, 2)
        self.assertEqual(pkt_new.packetID, 3)
        self.assertEqual(pkt_new.sysUptime, 4)
        self.assertEqual(pkt_new.resError, 5)
        self.assertEqual(pkt_new.resIndex, 6)
        self.assertEqual(pkt_new.varbinds,
                         ({"type": x.OID,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": {"subids": (4, 5, 6), "include": False}},
                          {"type": x.OCTET_STR,
                           "name": {"subids": (1, 2, 4), "include": False},
                           "data": "blah"}))
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
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})

    #
    # Data type tests
    #
    def test_encode_integer32(self):
        f = ntp.agentx.encode_integer32

        # Test
        self.assertEqual(f(True, 42), "\x00\x00\x00\x2A")

    def test_decode_integer32(self):
        f = ntp.agentx.decode_integer32

        # Test
        self.assertEqual(f("\x00\x00\x00\x2A" + extraData, standardFlags),
                         (42, extraData))
        # Test little endian
        self.assertEqual(f("\x2A\x00\x00\x00" + extraData, lilEndianFlags),
                         (42, extraData))

    def test_encode_nullvalue(self):
        f = ntp.agentx.encode_nullvalue

        # Test
        self.assertEqual(f(True), "")

    def test_decode_nullvalue(self):
        f = ntp.agentx.decode_nullvalue

        # Test
        self.assertEqual(f(extraData, standardFlags), (None, extraData))

    def test_encode_integer64(self):
        f = ntp.agentx.encode_integer64

        # Test
        self.assertEqual(f(True, 42), "\x00\x00\x00\x00\x00\x00\x00\x2A")

    def test_decode_integer64(self):
        f = ntp.agentx.decode_integer64

        # Test
        self.assertEqual(f("\x00\x00\x00\x00\x00\x00\x00\x2A" + extraData,
                           standardFlags),
                         (42, extraData))
        # Test
        self.assertEqual(f("\x2A\x00\x00\x00\x00\x00\x00\x00" + extraData,
                           lilEndianFlags),
                         (42, extraData))

    def test_encode_ipaddr(self):
        f = ntp.agentx.encode_ipaddr

        # Test correct
        self.assertEqual(f(True, (1, 2, 3, 4)),
                         "\x00\x00\x00\x04\x01\x02\x03\x04")
        # Test incorrect
        try:
            f(True, (1, 2, 3, 4, 5))
            errored = False
        except ValueError:
            errored = True
        self.assertEqual(errored, True)

    def test_decode_ipaddr(self):
        f = ntp.agentx.decode_ipaddr

        # Test
        self.assertEqual(f("\x00\x00\x00\x04\x01\x02\x03\x04" + extraData,
                           standardFlags),
                         ((1, 2, 3, 4), extraData))
        # Test little endian
        self.assertEqual(f("\x04\x00\x00\x00\x01\x02\x03\x04" + extraData,
                           lilEndianFlags),
                         ((1, 2, 3, 4), extraData))

    def test_encode_oid(self):
        f = ntp.agentx.encode_oid

        # Test empty OID
        self.assertEqual(f(True, (), False), "\x00\x00\x00\x00")
        # Test basic OID
        self.assertEqual(f(True, (1, 2, 3, 4, 5), False),
                         "\x05\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x05")
        # Test basic OID, little endian
        self.assertEqual(f(False, (1, 2, 3, 4, 5), False),
                         "\x05\x00\x00\x00"
                         "\x01\x00\x00\x00\x02\x00\x00\x00"
                         "\x03\x00\x00\x00\x04\x00\x00\x00"
                         "\x05\x00\x00\x00")
        # Test prefixed OID
        self.assertEqual(f(True, (1, 3, 6, 1, 23, 1, 2, 3), False),
                         "\x03\x17\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03")
        # Test include
        self.assertEqual(f(True, (1, 2), True),
                         "\x02\x00\x01\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02")
        # Test together
        self.assertEqual(f(True, (1, 3, 6, 1, 1, 3, 4, 5, 6), True),
                         "\x04\x01\x01\x00"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06")

        # Test maximum size
        self.assertEqual(f(True, maximumOIDsubs, False), maximumOIDstr)
        # Test over maximum size
        try:
            f(True, maximumOIDsubs + (42,), False)
            fail = False
        except ValueError:
            fail = True
        self.assertEqual(fail, True)

    def test_decode_oid(self):
        f = ntp.agentx.decode_oid

        # Test empty OID, extra data
        self.assertEqual(f("\x00\x00\x00\x00" + extraData, standardFlags),
                         ({"subids": (), "include": False}, extraData))
        # Test basic OID, extra data
        self.assertEqual(f("\x05\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x04\x00\x00\x00\x05" + extraData,
                           standardFlags),
                         ({"subids": (1, 2, 3, 4, 5), "include": False},
                          extraData))
        # Test basic OID, little endian
        self.assertEqual(f("\x05\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x04\x00\x00\x00\x05\x00\x00\x00", lilEndianFlags),
                         ({"subids": (1, 2, 3, 4, 5), "include": False},
                          ""))
        # Test prefixed OID
        self.assertEqual(f("\x03\x17\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03", standardFlags),
                         ({"subids": (1, 3, 6, 1, 23, 1, 2, 3),
                           "include": False},
                          ""))
        # Test include
        self.assertEqual(f("\x02\x00\x05\x00\x00\x00\x00\x01\x00\x00\x00\x02",
                           standardFlags),
                         ({"subids": (1, 2), "include": True}, ""))
        # Test together
        self.assertEqual(f("\x04\x01\x02\x00\x00\x00\x00\x03"
                           "\x00\x00\x00\x04\x00\x00\x00\x05\x00\x00\x00\x06",
                           standardFlags),
                         ({"subids": (1, 3, 6, 1, 1, 3, 4, 5, 6),
                           "include": True},
                          ""))
        # Test maximum size
        self.assertEqual(f(maximumOIDstr, standardFlags),
                         ({"subids": maximumOIDsubs, "include": False}, ""))
        # Test over maximum size
        # Need to replace the hardcoded n_subid=128 with 129
        fatOID = "\x81" + maximumOIDstr[1:] + "\xDE\xAD\xBE\xEF"
        try:
            f(fatOID, standardFlags)
            fail = False
        except ValueError:
            fail = True
        self.assertEqual(fail, True)

    def test_encode_searchrange(self):
        f = ntp.agentx.encode_searchrange

        # Test minimum size
        self.assertEqual(f(True, (), (), False),
                         "\x00\x00\x00\x00\x00\x00\x00\x00")
        # Test inclusive
        self.assertEqual(f(True, (1, 2, 3, 4), (5, 6, 7, 8), True),
                         "\x04\x00\x01\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x04\x00\x00\x00"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x07\x00\x00\x00\x08")
        # Test exclusive
        self.assertEqual(f(True, (1, 2, 3, 4), (5, 6, 7, 8), False),
                         "\x04\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x04\x00\x00\x00"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x07\x00\x00\x00\x08")

    def test_decode_searchrange(self):
        f = ntp.agentx.decode_searchrange

        # Test minimum size, extra data
        self.assertEqual(f("\x00\x00\x00\x00\x00\x00\x00\x00" + extraData,
                           standardFlags),
                         ({"start": {"subids": (), "include": False},
                           "end": {"subids": (), "include": False}},
                          extraData))
        # Test inclusive
        self.assertEqual(f("\x04\x00\x01\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x04\x00\x00\x00"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x07\x00\x00\x00\x08", standardFlags),
                         ({"start": {"subids": (1, 2, 3, 4), "include": True},
                           "end": {"subids": (5, 6, 7, 8), "include": False}},
                          ""))
        # Test exclusive
        self.assertEqual(f("\x04\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x04\x00\x00\x00"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x07\x00\x00\x00\x08", standardFlags),
                         ({"start": {"subids": (1, 2, 3, 4), "include": False},
                           "end": {"subids": (5, 6, 7, 8), "include": False}},
                          ""))
        # Test little endian
        self.assertEqual(f("\x04\x00\x01\x00"
                           "\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x07\x00\x00\x00\x08\x00\x00\x00", lilEndianFlags),
                         ({"start": {"subids": (1, 2, 3, 4), "include": True},
                           "end": {"subids": (5, 6, 7, 8), "include": False}},
                          ""))

    def test_encode_searchrange_list(self):
        f = ntp.agentx.encode_searchrange_list

        # Test
        self.assertEqual(f(True, (((1, 2), (1, 2), True),
                                  ((2, 3), (3, 4), False))),
                         "\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04")
        # Test, null terminated
        self.assertEqual(f(True,
                           (((1, 2), (1, 2), True),
                            ((2, 3), (3, 4), False)),
                           nullTerminate=True),
                         "\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x00")

    def test_decode_searchrange_list(self):
        f = ntp.agentx.decode_searchrange_list

        # Test
        self.assertEqual(f("\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04",
                           standardFlags),
                         ({"start": {"subids": (1, 2), "include": True},
                           "end": {"subids": (1, 2), "include": False}},
                          {"start": {"subids": (2, 3), "include": False},
                           "end": {"subids": (3, 4), "include": False}}))
        # Test, little endian
        self.assertEqual(f("\x02\x00\x01\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x02\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00",
                           lilEndianFlags),
                         ({"start": {"subids": (1, 2), "include": True},
                           "end": {"subids": (1, 2), "include": False}},
                          {"start": {"subids": (2, 3), "include": False},
                           "end": {"subids": (3, 4), "include": False}}))

    def test_decode_searchrange_list_nullterm(self):
        f = ntp.agentx.decode_searchrange_list_nullterm

        # Test
        self.assertEqual(f("\x02\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x00" + extraData, standardFlags),
                         (({"start": {"subids": (1, 2), "include": True},
                           "end": {"subids": (1, 2), "include": False}},
                          {"start": {"subids": (2, 3), "include": False},
                           "end": {"subids": (3, 4), "include": False}}),
                          extraData))
        # Test, little endian
        self.assertEqual(f("\x02\x00\x01\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x02\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x00\x00\x00\x00" + extraData, lilEndianFlags),
                         (({"start": {"subids": (1, 2), "include": True},
                           "end": {"subids": (1, 2), "include": False}},
                          {"start": {"subids": (2, 3), "include": False},
                           "end": {"subids": (3, 4), "include": False}}),
                          extraData))

    def test_encode_octetstr(self):
        f = ntp.agentx.encode_octetstr

        # Test empty
        self.assertEqual(f(True, ()), "\x00\x00\x00\x00")
        # Test word multiple
        self.assertEqual(f(True, (1, 2, 3, 4)),
                         "\x00\x00\x00\x04\x01\x02\x03\x04")
        # Test non word multiple
        self.assertEqual(f(True, (1, 2, 3, 4, 5)),
                         "\x00\x00\x00\x05\x01\x02\x03\x04\x05\x00\x00\x00")
        # Test string
        self.assertEqual(f(True, "blah"), "\x00\x00\x00\x04blah")

    def test_decode_octetstr(self):
        f = ntp.agentx.decode_octetstr

        # Test empty
        self.assertEqual(f("\x00\x00\x00\x00", standardFlags), ("", ""))
        # Test word multiple, extra data
        self.assertEqual(f("\x00\x00\x00\x04blah" + extraData, standardFlags),
                         ("blah", extraData))
        # Test word multiple, little endian
        self.assertEqual(f("\x04\x00\x00\x00blah", lilEndianFlags),
                         ("blah", ""))
        # Test non word multiple, extra data
        self.assertEqual(f("\x00\x00\x00\x05"
                           "blarg\x00\x00\x00" + extraData, standardFlags),
                         ("blarg", extraData))

    def test_encode_varbind(self):
        f = ntp.agentx.encode_varbind
        a = ntp.agentx

        # Test payloadless types
        self.assertEqual(f(True, a.NULL, (1, 2, 3)),
                         "\x00\x05\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(True, a.NO_SUCH_OBJECT, (1, 2, 3)),
                         "\x00\x80\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(True, a.NO_SUCH_INSTANCE, (1, 2, 3)),
                         "\x00\x81\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(True, a.END_OF_MIB_VIEW, (1, 2, 3)),
                         "\x00\x82\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test octet based types
        self.assertEqual(f(True, a.OCTET_STR, (1, 2, 3), (1, 2, 3, 4, 5)),
                         "\x00\x04\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x05"
                         "\x01\x02\x03\x04\x05\x00\x00\x00")
        self.assertEqual(f(True, a.IP_ADDR, (1, 2, 3), (16, 32, 48, 64)),
                         "\x00\x40\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x04\x10\x20\x30\x40")
        # Test integer32 types
        self.assertEqual(f(True, a.INTEGER, (1, 2, 3), 42),
                         "\x00\x02\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(True, a.COUNTER32, (1, 2, 3), 42),
                         "\x00\x41\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(True, a.GAUGE32, (1, 2, 3), 42),
                         "\x00\x42\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(True, a.TIME_TICKS, (1, 2, 3), 42),
                         "\x00\x43\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        # Test integer64 type
        self.assertEqual(f(True, a.COUNTER64, (1, 2, 3), 42),
                         "\x00\x46\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x00\x00\x00\x00\x2A")
        # Test oid type
        self.assertEqual(f(True, a.OID, (1, 2, 3), (16, 42, 256), False),
                         "\x00\x06\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x10"
                         "\x00\x00\x00\x2A\x00\x00\x01\x00")

    def test_decode_varbind(self):
        f = ntp.agentx.decode_varbind
        a = ntp.agentx

        # Test payloadless types
        self.assertEqual(f("\x00\x05\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         ({"type": a.NULL,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": None},
                          ""))
        self.assertEqual(f("\x00\x80\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         ({"type": a.NO_SUCH_OBJECT,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": None},
                          ""))
        self.assertEqual(f("\x00\x81\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         ({"type": a.NO_SUCH_INSTANCE,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": None},
                          ""))
        self.assertEqual(f("\x00\x82\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03",
                           standardFlags),
                         ({"type": a.END_OF_MIB_VIEW,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": None},
                          ""))
        # Test octet based types
        self.assertEqual(f("\x00\x04\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x0512345\x00\x00\x00",
                           standardFlags),
                         ({"type": a.OCTET_STR,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": "12345"},
                          ""))
        self.assertEqual(f("\x00\x40\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x04\x10\x20\x30\x40", standardFlags),
                         ({"type": a.IP_ADDR,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": (16, 32, 48, 64)},
                          ""))
        # Test integer32 types
        self.assertEqual(f("\x00\x02\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x2A", standardFlags),
                         ({"type": a.INTEGER,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))
        self.assertEqual(f("\x00\x41\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x2A", standardFlags),
                         ({"type": a.COUNTER32,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))
        self.assertEqual(f("\x00\x42\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x2A", standardFlags),
                         ({"type": a.GAUGE32,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))
        self.assertEqual(f("\x00\x43\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x2A", standardFlags),
                         ({"type": a.TIME_TICKS,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))
        # Test integer64 type
        self.assertEqual(f("\x00\x46\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x00\x00\x00\x00\x2A", standardFlags),
                         ({"type": a.COUNTER64,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))
        # Test oid type
        self.assertEqual(f("\x00\x06\x00\x00\x03\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x10"
                           "\x00\x00\x00\x2A\x00\x00\x01\x00", standardFlags),
                         ({"type": a.OID,
                           "name": {"subids": (1, 2, 3),
                                    "include": False},
                           "data": {"subids": (16, 42, 256),
                                    "include": False}},
                          ""))
        # Test integer32 with little endian
        self.assertEqual(f("\x43\x00\x00\x00\x03\x00\x00\x00"
                           "\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x2A\x00\x00\x00", lilEndianFlags),
                         ({"type": a.TIME_TICKS,
                           "name": {"subids": (1, 2, 3), "include": False},
                           "data": 42},
                          ""))

    #
    # Misc tests
    #
    def test_getendian(self):
        f = ntp.agentx.getendian

        # Test big endian
        self.assertEqual(f(True), ">")
        # Test little endian
        self.assertEqual(f(False), "<")

    def test_slicedata(self):
        f = ntp.agentx.slicedata

        # Test
        self.assertEqual(f("foobaz", 2), ("fo", "obaz"))

    def test_encode_pduheader(self):
        f = ntp.agentx.encode_pduheader
        a = ntp.agentx

        # Test "empty" header
        self.assertEqual(f(a.PDU_OPEN,
                           False, False, False, False, False,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         "\x01\x01\x00\x00"
                         "\xEF\xBE\xAD\xDE\xBE\xBA\xFE\xCA"
                         "\x0D\xF0\xCE\xFA\x00\x00\x00\x00")
        # Test flags
        self.assertEqual(f(a.PDU_OPEN,
                           True, True, True, True, True,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         "\x01\x01\x1F\x00"
                         "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                         "\xFA\xCE\xF0\x0D\x00\x00\x00\x00")

    def test_decode_pduheader(self):
        f = ntp.agentx.decode_pduheader
        a = ntp.agentx

        # Test "empty" header
        self.assertEqual(f("\x01\x01\x10\x00"
                           "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           "\xFA\xCE\xF0\x0D\x00\x00\x00\x00"),
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
        self.assertEqual(f("\x01\x01\x00\x00"
                           "\xEF\xBE\xAD\xDE\xBE\xBA\xFE\xCA"
                           "\x0D\xF0\xCE\xFA\x00\x00\x00\x00"),
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
        self.assertEqual(f("\x01\x01\x10\x00"
                           "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           "\xFA\xCE\xF0\x0D\x00\x00\x00\x00" + extraData),
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
        self.assertEqual(f("\x01\x01\x1F\x00"
                           "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                           "\xFA\xCE\xF0\x0D\x00\x00\x00\x00"),
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
        # Not testing all the variants of each packet type, that is
        # the job of the other tests.
        self.maxDiff = None

        # Test open
        self.assertEqual(f("\x01\x01\x10\x00"
                           "\x00\x00\x00\x0C\x00\x00\x00\x22"
                           "\x00\x00\x00\x38\x00\x00\x00\x20"
                           "\x4E\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x03foo\x00"),
                         (x.OpenPDU(True, 12, 34, 56, 78,
                                    {"subids": (1, 2, 3, 4),
                                     "include": False},
                                    "foo"),
                          ""))
        # Test open, extraData
        self.assertEqual(f("\x01\x01\x10\x00"
                           "\x00\x00\x00\x0C\x00\x00\x00\x22"
                           "\x00\x00\x00\x38\x00\x00\x00\x20"
                           "\x4E\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x03foo\x00" + extraData),
                         (x.OpenPDU(True, 12, 34, 56, 78,
                                    {"subids": (1, 2, 3, 4),
                                     "include": False},
                                    "foo"),
                          extraData))
        # Test close
        self.assertEqual(f("\x01\x02\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x01\x00\x00\x00"),
                         (x.ClosePDU(True, 1, 2, 3, x.RSN_OTHER),
                          ""))
        # Test register
        self.assertEqual(f("\x01\x03\x11\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x14"
                           "\x04\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (x.RegisterPDU(True, 1, 2, 3, 4, 5,
                                        {"subids": (1, 2, 3),
                                         "include": False},
                                        0, None, None),
                          ""))
        # Test unregister
        self.assertEqual(f("\x01\x04\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x14"
                           "\x00\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"),
                         (x.UnregisterPDU(True, 1, 2, 3, 5,
                                          {"subids": (1, 2, 3),
                                           "include": False},
                                          0, None, None),
                          ""))
        # Test get
        self.assertEqual(f("\x01\x05\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x00"),
                         (x.GetPDU(True, 1, 2, 3, ()),
                          ""))
        # Test get next
        self.assertEqual(f("\x01\x06\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.GetNextPDU(True, 1, 2, 3, ()),
                          ""))
        # Test get bulk
        self.assertEqual(f("\x01\x07\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x34"
                           "\x00\x01\x00\x05"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                           "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"),
                         (x.GetBulkPDU(True, 1, 2, 3, 1, 5,
                                       ({"start": {"subids": (1, 2),
                                                   "include": False},
                                         "end": {"subids": (3, 4),
                                                 "include": False}},
                                        {"start": {"subids": (6, 7),
                                                   "include": True},
                                         "end": {"subids": (8, 9),
                                                 "include": False}})),
                          ""))
        # Test test set
        self.assertEqual(f("\x01\x08\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x40"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah"),
                         (x.TestSetPDU(True, 1, 2, 3,
                                       ({"type": x.OID,
                                         "name": {"subids": (1, 2, 3),
                                                  "include": False},
                                         "data": {"subids": (4, 5, 6),
                                                  "include": False}},
                                        {"type": x.OCTET_STR,
                                         "name": {"subids": (1, 2, 4),
                                                  "include": False},
                                         "data": "blah"})),
                          ""))
        # Test commit set
        self.assertEqual(f("\x01\x09\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.CommitSetPDU(True, 1, 2, 3),
                          ""))
        # Test undo set
        self.assertEqual(f("\x01\x0A\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.UndoSetPDU(True, 1, 2, 3),
                          ""))
        # Test cleanup set
        self.assertEqual(f("\x01\x0B\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.CleanupSetPDU(True, 1, 2, 3),
                          ""))
        # Test notify
        self.assertEqual(f("\x01\x0C\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x40"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah"),
                         (x.NotifyPDU(True, 1, 2, 3,
                                      ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})),
                          ""))
        # Test ping
        self.assertEqual(f("\x01\x0D\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         (x.PingPDU(True, 1, 2, 3),
                          ""))
        # Test index alloc
        self.assertEqual(f("\x01\x0E\x16\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x40"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah"),
                         (x.IndexAllocPDU(True, 1, 2, 3, True, True,
                                          ({"type": x.OID,
                                            "name": {"subids": (1, 2, 3),
                                                     "include": False},
                                            "data": {"subids": (4, 5, 6),
                                                     "include": False}},
                                           {"type": x.OCTET_STR,
                                            "name": {"subids": (1, 2, 4),
                                                     "include": False},
                                            "data": "blah"})),
                          ""))
        # Test index dealloc
        self.assertEqual(f("\x01\x0F\x16\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x40"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah"),
                         (x.IndexDeallocPDU(True, 1, 2, 3, True, True,
                                            ({"type": x.OID,
                                              "name": {"subids": (1, 2, 3),
                                                       "include": False},
                                              "data": {"subids": (4, 5, 6),
                                                       "include": False}},
                                             {"type": x.OCTET_STR,
                                              "name": {"subids": (1, 2, 4),
                                                       "include": False},
                                              "data": "blah"})),
                          ""))
        # Test add agent caps
        self.assertEqual(f("\x01\x10\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x18"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x04blah"),
                         (x.AddAgentCapsPDU(True, 1, 2, 3,
                                            {"subids": (4, 5, 6),
                                             "include": False},
                                            "blah"),
                          ""))
        # Test rm agent caps
        self.assertEqual(f("\x01\x11\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x10"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"),
                         (x.RMAgentCapsPDU(True, 1, 2, 3,
                                           {"subids": (4, 5, 6),
                                            "include": False}),
                          ""))
        # Test response
        self.assertEqual(f("\x01\x12\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x08"
                           "\x00\x00\x00\x04\x00\x05\x00\x06"),
                         (x.ResponsePDU(True, 1, 2, 3, 4, 5, 6),
                          ""))
        # Test insufficient data
        try:
            f("\x01\x11\x10\x00"
              "\x00\x00\x00\x01\x00\x00\x00\x02"
              "\x00\x00\x00\x03\x00\x00\x00\x10"
              "\x03\x00\x00\x00\x00\x00\x00\x04")
            fail = False
        except IndexError:
            fail = True
        self.assertEqual(fail, True)


if __name__ == "__main__":
    unittest.main()
