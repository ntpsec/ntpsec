#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import ntp.agentx

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
standardFlags = {"instReg": False,
                 "newIndex": False,
                 "anyIndex": False,
                 "contextP": False,
                 "bigEndian": True}
lilEndianFlags = {"instReg": False,
                  "newIndex": False,
                  "anyIndex": False,
                  "contextP": False,
                  "bigEndian": False}
contextFlags = {"instReg": False,
                "newIndex": False,
                "anyIndex": False,
                "contextP": True,
                "bigEndian": True}


def makeFlags(iR, nI, aI, cP, bE):
    return {"instReg": iR,
            "newIndex": nI,
            "anyIndex": aI,
            "contextP": cP,
            "bigEndian": bE}


class TestNtpclientsNtpsnmpd(unittest.TestCase):
    #
    # PDU tests
    #
    def test_encode_openpdu(self):
        f = ntp.agentx.encode_openpdu

        # Test null packet
        self.assertEqual(f(0, 0, 0, 0, (), ""),
                         "\x01\x01\x10\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x0C"
                         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
        # Test basic packet
        self.assertEqual(f(12, 34, 56, 78, (1, 2, 3, 4), "foo"),
                         "\x01\x01\x10\x00"
                         "\x00\x00\x00\x0C\x00\x00\x00\x22"
                         "\x00\x00\x00\x38\x00\x00\x00\x20"
                         "\x4E\x00\x00\x00"
                         "\x04\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x03foo\x00")

    def test_decode_openpdu(self):
        f = ntp.agentx.decode_openpdu

        # Test null packet
        self.assertEqual(f("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
                           standardFlags),
                         {"timeout": 0,
                          "oid": {"subids": (), "include": False},
                          "description": ""})
        # Test basic packet
        self.assertEqual(f("\x4E\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x03foo\x00", standardFlags),
                         {"timeout": 78,
                          "oid": {"subids": (1, 2, 3, 4), "include": False},
                          "description": "foo"})
        # Test basic packet, little endian
        self.assertEqual(f("\x4E\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x03\x00\x00\x00foo\x00", lilEndianFlags),
                         {"timeout": 78,
                          "oid": {"subids": (1, 2, 3, 4), "include": False},
                          "description": "foo"})

    def test_encode_closepdu(self):
        f = ntp.agentx.encode_closepdu
        a = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3, a.RSN_OTHER),
                         "\x01\x02\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x01\x00\x00\x00")

    def test_decode_closepdu(self):
        f = ntp.agentx.decode_closepdu

        # Test
        self.assertEqual(f("\x01\x00\x00\x00", standardFlags),
                         {"reason": 1})

    def test_encode_registerpdu(self):
        f = ntp.agentx.encode_registerpdu

        # Test basic packet
        self.assertEqual(f(1, 2, 3, 4, 5, (1, 2, 3)),
                         "\x01\x03\x11\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x14"
                         "\x04\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test with context
        self.assertEqual(f(1, 2, 3, 4, 5, (1, 2, 3), context="blah"),
                         "\x01\x03\x19\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x1C"
                         "\x00\x00\x00\x04blah"
                         "\x04\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test with range
        self.assertEqual(f(1, 2, 3, 4, 5, (1, 2, 3),
                           rangeSubid=5, upperBound=23),
                         "\x01\x03\x11\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x18"
                         "\x04\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")
        # Test with context and range
        self.assertEqual(f(1, 2, 3, 4, 5, (1, 2, 3), context="blah",
                           rangeSubid=5, upperBound=23),
                         "\x01\x03\x19\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04blah"
                         "\x04\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")

    def test_decode_registerpdu(self):
        f = ntp.agentx.decode_registerpdu

        # Test basic packet
        self.assertEqual(f("\x04\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03", standardFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "timeout": 4, "priority": 5, "context": None,
                          "range_subid": 0, "upper_bound": None})
        # Test basic, little endian
        self.assertEqual(f("\x04\x05\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00", lilEndianFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "timeout": 4, "priority": 5, "context": None,
                          "range_subid": 0, "upper_bound": None})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah\x04\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03", contextFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "timeout": 4, "priority": 5, "context": "blah",
                          "range_subid": 0, "upper_bound": None})
        # Test with range
        self.assertEqual(f("\x04\x05\x05\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x17", standardFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "timeout": 4, "priority": 5, "context": None,
                          "range_subid": 5, "upper_bound": 23})
        # Test with context and range
        self.assertEqual(f("\x00\x00\x00\x04blah\x04\x05\x05\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x17", contextFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "timeout": 4, "priority": 5, "context": "blah",
                          "range_subid": 5, "upper_bound": 23})

    def test_encode_unregisterpdu(self):
        f = ntp.agentx.encode_unregisterpdu

        # Test basic packet
        self.assertEqual(f(1, 2, 3, 5, (1, 2, 3)),
                         "\x01\x04\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x14"
                         "\x00\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test with context
        self.assertEqual(f(1, 2, 3, 5, (1, 2, 3), context="blah"),
                         "\x01\x04\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x1C"
                         "\x00\x00\x00\x04blah"
                         "\x00\x05\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test with range
        self.assertEqual(f(1, 2, 3, 5, (1, 2, 3),
                           rangeSubid=5, upperBound=23),
                         "\x01\x04\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x18"
                         "\x00\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")
        # Test with context and range
        self.assertEqual(f(1, 2, 3, 5, (1, 2, 3), context="blah",
                           rangeSubid=5, upperBound=23),
                         "\x01\x04\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04blah"
                         "\x00\x05\x05\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x17")

    def test_decode_unregisterpdu(self):
        f = ntp.agentx.decode_unregisterpdu

        # Test basic packet
        self.assertEqual(f("\x00\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03", standardFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "priority": 5, "context": None,
                          "range_subid": 0, "upper_bound": None})
        # Test basic, little endian
        self.assertEqual(f("\x00\x05\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00", lilEndianFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "priority": 5, "context": None,
                          "range_subid": 0, "upper_bound": None})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah\x00\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03", contextFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "priority": 5, "context": "blah",
                          "range_subid": 0, "upper_bound": None})
        # Test with range
        self.assertEqual(f("\x04\x05\x05\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x17", standardFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "priority": 5, "context": None,
                          "range_subid": 5, "upper_bound": 23})
        # Test with context and range
        self.assertEqual(f("\x00\x00\x00\x04blah\x04\x05\x05\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x00\x00\x00\x17", contextFlags),
                         {"oid": {"subids": (1, 2, 3), "include": False},
                          "priority": 5, "context": "blah",
                          "range_subid": 5, "upper_bound": 23})

    def test_encode_getpdu(self):
        f = ntp.agentx.encode_getpdu

        # Test null
        self.assertEqual(f(1, 2, 3, ()),
                         "\x01\x05\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x00")
        # Test basic
        self.assertEqual(f(1, 2, 3,
                           (((1, 2, 3), (1, 2, 5), False),
                            ((10, 20), (30, 40), True))),
                         "\x01\x05\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x3C"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x05"
                         "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                         "\x00\x00\x00\x00")
        # Test with context
        self.assertEqual(f(1, 2, 3,
                           (((1, 2, 3), (1, 2, 5), False),
                            ((10, 20), (30, 40), True)),
                           context="blah"),
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

    def test_decode_getpdu(self):
        f = ntp.agentx.decode_getpdu

        # Test null
        self.assertEqual(f("\x00\x00\x00\x00", standardFlags),
                         {"context": None,
                          "oidranges": ()})
        # Test basic
        self.assertEqual(f(
            "\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
            "\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x05"
            "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
            "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
            "\x00\x00\x00\x00", standardFlags),
                         {"context": None,
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})
        # Test basic, little endian
        self.assertEqual(f(
            "\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
            "\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00"
            "\x02\x00\x01\x00\x0A\x00\x00\x00\x14\x00\x00\x00"
            "\x02\x00\x00\x00\x1E\x00\x00\x00\x28\x00\x00\x00"
            "\x00\x00\x00\x00", lilEndianFlags),
                         {"context": None,
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x05"
                           "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                           "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                           "\x00\x00\x00\x00", contextFlags),
                         {"context": "blah",
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})

    def test_encode_getnextpdu(self):
        f = ntp.agentx.encode_getnextpdu

        # Test null
        self.assertEqual(f(1, 2, 3, ()),
                         "\x01\x06\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x00")
        # Test basic
        self.assertEqual(f(1, 2, 3,
                           (((1, 2, 3), (1, 2, 5), False),
                            ((10, 20), (30, 40), True))),
                         "\x01\x06\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x3C"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x03\x00\x00\x00\x00\x00\x00\x01"
                         "\x00\x00\x00\x02\x00\x00\x00\x05"
                         "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                         "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                         "\x00\x00\x00\x00")
        # Test with context
        self.assertEqual(f(1, 2, 3,
                           (((1, 2, 3), (1, 2, 5), False),
                            ((10, 20), (30, 40), True)),
                           context="blah"),
                         "\x01\x06\x18\x00"
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

    def test_decode_getnextpdu(self):
        f = ntp.agentx.decode_getnextpdu

        # Test null
        self.assertEqual(f("\x00\x00\x00\x00", standardFlags),
                         {"context": None,
                          "oidranges": ()})
        # Test basic
        self.assertEqual(f(
            "\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
            "\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x05"
            "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
            "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
            "\x00\x00\x00\x00", standardFlags),
                         {"context": None,
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})
        # Test basic, little endian
        self.assertEqual(f(
            "\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
            "\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00"
            "\x02\x00\x01\x00\x0A\x00\x00\x00\x14\x00\x00\x00"
            "\x02\x00\x00\x00\x1E\x00\x00\x00\x28\x00\x00\x00"
            "\x00\x00\x00\x00", lilEndianFlags),
                         {"context": None,
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x05"
                           "\x02\x00\x01\x00\x00\x00\x00\x0A\x00\x00\x00\x14"
                           "\x02\x00\x00\x00\x00\x00\x00\x1E\x00\x00\x00\x28"
                           "\x00\x00\x00\x00", contextFlags),
                         {"context": "blah",
                          "oidranges": ({"start": {"subids": (1, 2, 3),
                                                   "include": False},
                                         "end": {"subids": (1, 2, 5),
                                                 "include": False}},
                                        {"start": {"subids": (10, 20),
                                                   "include": True},
                                         "end": {"subids": (30, 40),
                                                 "include": False}})})

    def test_encode_getbulkpdu(self):
        f = ntp.agentx.encode_getbulkpdu

        # Test basic
        self.assertEqual(f(1, 2, 3, 1, 5,
                           (((1, 2), (3, 4), False),
                            ((6, 7), (8, 9), True))),
                         "\x01\x07\x10\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x38"
                         "\x00\x01\x00\x05"
                         "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                         "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"
                         "\x00\x00\x00\x00")
        # Test with context
        self.assertEqual(f(1, 2, 3, 1, 5,
                           (((1, 2), (3, 4), False),
                            ((6, 7), (8, 9), True)),
                           context="blah"),
                         "\x01\x07\x18\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x40"
                         "\x00\x00\x00\x04blah"
                         "\x00\x01\x00\x05"
                         "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                         "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"
                         "\x00\x00\x00\x00")

    def test_decode_getbulkpdu(self):
        f = ntp.agentx.decode_getbulkpdu

        # Test basic
        self.assertEqual(f("\x00\x01\x00\x05"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                           "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"
                           "\x00\x00\x00\x00", standardFlags),
                         {"context": None, "non_reps": 1, "max_reps": 5,
                          "oidranges": ({"start": {"subids": (1, 2),
                                                   "include": False},
                                         "end": {"subids": (3, 4),
                                                 "include": False}},
                                        {"start": {"subids": (6, 7),
                                                   "include": True},
                                         "end": {"subids": (8, 9),
                                                 "include": False}})})
        # Test basic, little endian
        self.assertEqual(f("\x01\x00\x05\x00"
                           "\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x02\x00\x01\x00\x06\x00\x00\x00\x07\x00\x00\x00"
                           "\x02\x00\x00\x00\x08\x00\x00\x00\x09\x00\x00\x00"
                           "\x00\x00\x00\x00", lilEndianFlags),
                         {"context": None, "non_reps": 1, "max_reps": 5,
                          "oidranges": ({"start": {"subids": (1, 2),
                                                   "include": False},
                                         "end": {"subids": (3, 4),
                                                 "include": False}},
                                        {"start": {"subids": (6, 7),
                                                   "include": True},
                                         "end": {"subids": (8, 9),
                                                 "include": False}})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x00\x01\x00\x05"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                           "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"
                           "\x00\x00\x00\x00", contextFlags),
                         {"context": "blah", "non_reps": 1, "max_reps": 5,
                          "oidranges": ({"start": {"subids": (1, 2),
                                                   "include": False},
                                         "end": {"subids": (3, 4),
                                                 "include": False}},
                                        {"start": {"subids": (6, 7),
                                                   "include": True},
                                         "end": {"subids": (8, 9),
                                                 "include": False}})})

    def test_encode_testsetpdu(self):
        f = ntp.agentx.encode_testsetpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah"))),
                         "\x01\x08\x10\x00"
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
                         "\x00\x00\x00\x04blah")
        # Test with context
        self.assertEqual(f(1, 2, 3,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah")), context="blah"),
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

    def test_decode_testsetpdu(self):
        f = ntp.agentx.decode_testsetpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f("\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test, little endian
        self.assertEqual(f("\x06\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x04\x00\x00\x00"
                           "\x04\x00\x00\x00blah", lilEndianFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", contextFlags),
                         {"context": "blah",
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})

    def test_encode_commitsetpdu(self):
        f = ntp.agentx.encode_commitsetpdu

        # Test
        self.assertEqual(f(1, 2, 3), "\x01\x09\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")

    def test_encode_undosetpdu(self):
        f = ntp.agentx.encode_undosetpdu

        # Test
        self.assertEqual(f(1, 2, 3), "\x01\x0A\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")

    def test_encode_cleanupsetpdu(self):
        f = ntp.agentx.encode_cleanupsetpdu

        # Test
        self.assertEqual(f(1, 2, 3), "\x01\x0B\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")

    def test_encode_pingpdu(self):
        f = ntp.agentx.encode_pingpdu

        # Test
        self.assertEqual(f(1, 2, 3), "\x01\x0D\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x00")
        # Test with context
        self.assertEqual(f(1, 2, 3, "blah"), "\x01\x0D\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x08"
                         "\x00\x00\x00\x04blah")

    def test_decode_pingpdu(self):
        f = ntp.agentx.decode_pingpdu

        # Test
        self.assertEqual(f("", standardFlags),
                         {"context": None})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah", contextFlags),
                         {"context": "blah"})

    def test_encode_notifypdu(self):
        f = ntp.agentx.encode_notifypdu
        x = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah"))),
                         "\x01\x0C\x10\x00"
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
                         "\x00\x00\x00\x04blah")
        # Test with context
        self.assertEqual(f(1, 2, 3,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah")), context="blah"),
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

    def test_decode_notifypdu(self):
        f = ntp.agentx.decode_notifypdu
        x = ntp.agentx

        # Test
        self.assertEqual(f("\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test, little endian
        self.assertEqual(f("\x06\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x04\x00\x00\x00"
                           "\x04\x00\x00\x00blah", lilEndianFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", contextFlags),
                         {"context": "blah",
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})

    def test_encode_indexallocpdu(self):
        f = ntp.agentx.encode_indexallocpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3, True, True,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah"))),
                         "\x01\x0E\x16\x00"
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
                         "\x00\x00\x00\x04blah")
        # Test with context
        self.assertEqual(f(1, 2, 3, True, True,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah")), context="blah"),
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

    def test_decode_indexallocpdu(self):
        f = ntp.agentx.decode_indexallocpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f("\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test, little endian
        self.assertEqual(f("\x06\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x04\x00\x00\x00"
                           "\x04\x00\x00\x00blah", lilEndianFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", contextFlags),
                         {"context": "blah",
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})

    def test_encode_indexdeallocpdu(self):
        f = ntp.agentx.encode_indexdeallocpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3, True, True,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah"))),
                         "\x01\x0F\x16\x00"
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
                         "\x00\x00\x00\x04blah")
        # Test with context
        self.assertEqual(f(1, 2, 3, True, True,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah")), context="blah"),
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

    def test_decode_indexdeallocpdu(self):
        f = ntp.agentx.decode_indexdeallocpdu
        x = ntp.agentx

        # Test
        self.assertEqual(f("\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test, little endian
        self.assertEqual(f("\x06\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x03\x00\x00\x00"
                           "\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x04\x00\x00\x00"
                           "\x03\x00\x00\x00\x01\x00\x00\x00"
                           "\x02\x00\x00\x00\x04\x00\x00\x00"
                           "\x04\x00\x00\x00blah", lilEndianFlags),
                         {"context": None,
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04blah"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", contextFlags),
                         {"context": "blah",
                          "varbinds": ({"type": x.OID,
                                        "name": {"subids": (1, 2, 3),
                                                 "include": False},
                                        "data": {"subids": (4, 5, 6),
                                                 "include": False}},
                                       {"type": x.OCTET_STR,
                                        "name": {"subids": (1, 2, 4),
                                                 "include": False},
                                        "data": "blah"})})

    def test_encode_addagentcapspdu(self):
        f = ntp.agentx.encode_addagentcapspdu

        # Test
        self.assertEqual(f(1, 2, 3, (4, 5, 6), "blah"),
                         "\x01\x10\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x18"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x04blah")
        # Test with context
        self.assertEqual(f(1, 2, 3, (4, 5, 6), "blah", context="bluh"),
                         "\x01\x10\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x20"
                         "\x00\x00\x00\x04bluh"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x04blah")

    def test_decode_addagentcapspdu(self):
        f = ntp.agentx.decode_addagentcapspdu

        # Test
        self.assertEqual(f("\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"context": None,
                          "oid": {"subids": (4, 5, 6), "include": False},
                          "description": "blah"})
        # Test, little endian
        self.assertEqual(f("\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00"
                           "\x04\x00\x00\x00blah", lilEndianFlags),
                         {"context": None,
                          "oid": {"subids": (4, 5, 6), "include": False},
                          "description": "blah"})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04bluh"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x04blah", contextFlags),
                         {"context": "bluh",
                          "oid": {"subids": (4, 5, 6), "include": False},
                          "description": "blah"})

    def test_encode_rmagentcapspdu(self):
        f = ntp.agentx.encode_rmagentcapspdu

        # Test
        self.assertEqual(f(1, 2, 3, (4, 5, 6)),
                         "\x01\x11\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x10"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06")
        # Test with context
        self.assertEqual(f(1, 2, 3, (4, 5, 6), context="bluh"),
                         "\x01\x11\x18\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x18"
                         "\x00\x00\x00\x04bluh"
                         "\x03\x00\x00\x00\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06")

    def test_decode_rmagentcapspdu(self):
        f = ntp.agentx.decode_rmagentcapspdu

        # Test
        self.assertEqual(f("\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06", standardFlags),
                         {"context": None,
                          "oid": {"subids": (4, 5, 6), "include": False}})
        # Test, little endian
        self.assertEqual(f("\x03\x00\x00\x00\x04\x00\x00\x00"
                           "\x05\x00\x00\x00\x06\x00\x00\x00", lilEndianFlags),
                         {"context": None,
                          "oid": {"subids": (4, 5, 6), "include": False}})
        # Test with context
        self.assertEqual(f("\x00\x00\x00\x04bluh"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06", contextFlags),
                         {"context": "bluh",
                          "oid": {"subids": (4, 5, 6), "include": False}})

    def test_encode_responsepdu(self):
        f = ntp.agentx.encode_responsepdu
        x = ntp.agentx

        # Test
        self.assertEqual(f(1, 2, 3, 4, 5, 6),
                         "\x01\x12\x10\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x08"
                         "\x00\x00\x00\x04\x00\x05\x00\x06")
        # Test with varbinds
        self.assertEqual(f(1, 2, 3, 4, 5, 6,
                           ((x.OID, (1, 2, 3), (4, 5, 6), False),
                            (x.OCTET_STR, (1, 2, 4), "blah"))),
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

    def test_decode_responsepdu(self):
        f = ntp.agentx.decode_responsepdu
        x = ntp.agentx

        # Test
        self.assertEqual(f("\x00\x00\x00\x04\x00\x05\x00\x06", standardFlags),
                         {"sys_uptime": 4, "res_err": 5,
                          "res_index": 6, "varbinds": None})
        # Test, little endian
        self.assertEqual(f("\x04\x00\x00\x00\x05\x00\x06\x00", lilEndianFlags),
                         {"sys_uptime": 4, "res_err": 5,
                          "res_index": 6, "varbinds": None})
        # Test with varbinds
        self.assertEqual(f("\x00\x00\x00\x04\x00\x05\x00\x06"
                           "\x00\x06\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x04\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x04"
                           "\x00\x00\x00\x04blah", standardFlags),
                         {"sys_uptime": 4, "res_err": 5, "res_index": 6,
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
        self.assertEqual(f(42), "\x00\x00\x00\x2A")

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
        self.assertEqual(f(), "")

    def test_decode_nullvalue(self):
        f = ntp.agentx.decode_nullvalue

        # Test
        self.assertEqual(f(extraData, standardFlags), (None, extraData))

    def test_encode_integer64(self):
        f = ntp.agentx.encode_integer64

        # Test
        self.assertEqual(f(42), "\x00\x00\x00\x00\x00\x00\x00\x2A")

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
        self.assertEqual(f((1, 2, 3, 4)),
                         "\x00\x00\x00\x04\x01\x02\x03\x04")
        # Test incorrect
        try:
            f((1, 2, 3, 4, 5))
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
        self.assertEqual(f((), False), "\x00\x00\x00\x00")
        # Test basic OID
        self.assertEqual(f((1, 2, 3, 4, 5), False),
                         "\x05\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x05")
        # Test prefixed OID
        self.assertEqual(f((1, 3, 6, 1, 23, 1, 2, 3), False),
                         "\x03\x17\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03")
        # Test include
        self.assertEqual(f((1, 2), True),
                         "\x02\x00\x01\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02")
        # Test together
        self.assertEqual(f((1, 3, 6, 1, 1, 3, 4, 5, 6), True),
                         "\x04\x01\x01\x00"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x00\x00\x00\x05\x00\x00\x00\x06")

        # Test maximum size
        self.assertEqual(f(maximumOIDsubs, False), maximumOIDstr)
        # Test over maximum size
        try:
            f(maximumOIDsubs + (42,), False)
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
        self.assertEqual(f((), (), False), "\x00\x00\x00\x00\x00\x00\x00\x00")
        # Test inclusive
        self.assertEqual(f((1, 2, 3, 4), (5, 6, 7, 8), True),
                         "\x04\x00\x01\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02"
                         "\x00\x00\x00\x03\x00\x00\x00\x04"
                         "\x04\x00\x00\x00"
                         "\x00\x00\x00\x05\x00\x00\x00\x06"
                         "\x00\x00\x00\x07\x00\x00\x00\x08")
        # Test exclusive
        self.assertEqual(f((1, 2, 3, 4), (5, 6, 7, 8), False),
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
        self.assertEqual(f((((1, 2), (1, 2), True),
                            ((2, 3), (3, 4), False))),
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
        self.assertEqual(f(()), "\x00\x00\x00\x00")
        # Test word multiple
        self.assertEqual(f((1, 2, 3, 4)), "\x00\x00\x00\x04\x01\x02\x03\x04")
        # Test non word multiple
        self.assertEqual(f((1, 2, 3, 4, 5)),
                         "\x00\x00\x00\x05\x01\x02\x03\x04\x05\x00\x00\x00")
        # Test string
        self.assertEqual(f("blah"), "\x00\x00\x00\x04blah")

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
        self.assertEqual(f(a.NULL, (1, 2, 3)),
                         "\x00\x05\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(a.NO_SUCH_OBJECT, (1, 2, 3)),
                         "\x00\x80\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(a.NO_SUCH_INSTANCE, (1, 2, 3)),
                         "\x00\x81\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        self.assertEqual(f(a.END_OF_MIB_VIEW, (1, 2, 3)),
                         "\x00\x82\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03")
        # Test octet based types
        self.assertEqual(f(a.OCTET_STR, (1, 2, 3), (1, 2, 3, 4, 5)),
                         "\x00\x04\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x05"
                         "\x01\x02\x03\x04\x05\x00\x00\x00")
        self.assertEqual(f(a.IP_ADDR, (1, 2, 3), (16, 32, 48, 64)),
                         "\x00\x40\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x04\x10\x20\x30\x40")
        # Test integer32 types
        self.assertEqual(f(a.INTEGER, (1, 2, 3), 42),
                         "\x00\x02\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(a.COUNTER32, (1, 2, 3), 42),
                         "\x00\x41\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(a.GAUGE32, (1, 2, 3), 42),
                         "\x00\x42\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        self.assertEqual(f(a.TIME_TICKS, (1, 2, 3), 42),
                         "\x00\x43\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x2A")
        # Test integer64 type
        self.assertEqual(f(a.COUNTER64, (1, 2, 3), 42),
                         "\x00\x46\x00\x00\x03\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03"
                         "\x00\x00\x00\x00\x00\x00\x00\x2A")
        # Text oid type
        self.assertEqual(f(a.OID, (1, 2, 3), (16, 42, 256), False),
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
        self.assertEqual(f(standardFlags), ">")
        # Test little endian
        self.assertEqual(f(lilEndianFlags), "<")

    def test_slicedata(self):
        f = ntp.agentx.slicedata

        # Test
        self.assertEqual(f("foobaz", 2), ("fo", "obaz"))

    def test_encode_pduheader(self):
        f = ntp.agentx.encode_pduheader
        a = ntp.agentx

        # Test "empty" header
        self.assertEqual(f(a.PDU_OPEN,
                           False, False, False, False,
                           0xDEADBEEF, 0xCAFEBABE, 0xFACEF00D, 0),
                         "\x01\x01\x10\x00"
                         "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
                         "\xFA\xCE\xF0\x0D\x00\x00\x00\x00")
        # Test flags
        self.assertEqual(f(a.PDU_OPEN,
                           True, True, True, True,
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
                          "flags": {"instReg ": False,
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
                         ({"header": {"version": 1, "type": x.PDU_OPEN,
                                      "flags": standardFlags, "session_id": 12,
                                      "transaction_id": 34, "packet_id": 56,
                                      "length": 32},
                           "body": {"timeout": 78,
                                    "oid": {"subids": (1, 2, 3, 4),
                                            "include": False},
                                    "description": "foo"}},
                          ""))
        # Test close
        self.assertEqual(f("\x01\x02\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x01\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_CLOSE,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 4},
                           "body": {"reason": x.RSN_OTHER}},
                          ""))
        # Test register
        self.assertEqual(f("\x01\x03\x11\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x14"
                           "\x04\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"),
                         ({"header": {"version": 1, "type": x.PDU_REGISTER,
                                      "flags": makeFlags(True, False, False,
                                                         False, True),
                                      "session_id": 1, "transaction_id": 2,
                                      "packet_id": 3, "length": 20},
                           "body": {"oid": {"subids": (1, 2, 3),
                                            "include": False},
                                    "timeout": 4, "priority": 5,
                                    "context": None, "range_subid": 0,
                                    "upper_bound": None}},
                          ""))
        # Test unregister
        self.assertEqual(f("\x01\x04\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x14"
                           "\x00\x05\x00\x00"
                           "\x03\x00\x00\x00\x00\x00\x00\x01"
                           "\x00\x00\x00\x02\x00\x00\x00\x03"),
                         ({"header": {"version": 1, "type": x.PDU_UNREGISTER,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 20},
                           "body": {"oid": {"subids": (1, 2, 3),
                                            "include": False},
                                    "priority": 5, "context": None,
                                    "range_subid": 0, "upper_bound": None}},
                          ""))
        # Test get
        self.assertEqual(f("\x01\x05\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_GET,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 4},
                           "body": {"context": None, "oidranges": ()}},
                          ""))
        # Test get next
        self.assertEqual(f("\x01\x06\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_GET_NEXT,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 4},
                           "body": {"context": None, "oidranges": ()}},
                          ""))
        # Test get bulk
        self.assertEqual(f("\x01\x07\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x38"
                           "\x00\x01\x00\x05"
                           "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x02\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04"
                           "\x02\x00\x01\x00\x00\x00\x00\x06\x00\x00\x00\x07"
                           "\x02\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x09"
                           "\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_GET_BULK,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 56},
                           "body": {"context": None, "non_reps": 1,
                                    "max_reps": 5,
                                    "oidranges": ({"start": {"subids": (1, 2),
                                                             "include": False},
                                                   "end": {"subids": (3, 4),
                                                           "include": False}},
                                                  {"start": {"subids": (6, 7),
                                                             "include": True},
                                                   "end": {"subids": (8, 9),
                                                           "include": False}}
                                    )}}, ""))
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
                         ({"header": {"version": 1, "type": x.PDU_TEST_SET,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 64},
                           "body": {"context": None,
                                    "varbinds": ({"type": x.OID,
                                                  "name": {"subids": (1, 2, 3),
                                                           "include": False},
                                                  "data": {"subids": (4, 5, 6),
                                                           "include": False}},
                                                 {"type": x.OCTET_STR,
                                                  "name": {"subids": (1, 2, 4),
                                                           "include": False},
                                                  "data": "blah"})}},
                          ""))
        # Test commit set
        self.assertEqual(f("\x01\x09\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_COMMIT_SET,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 0},
                           "body": None},
                          ""))
        # Test undo set
        self.assertEqual(f("\x01\x0A\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_UNDO_SET,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 0},
                           "body": None},
                          ""))
        # Test cleanup set
        self.assertEqual(f("\x01\x0B\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_CLEANUP_SET,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 0},
                           "body": None},
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
                         ({"header": {"version": 1, "type": x.PDU_NOTIFY,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 64},
                           "body": {"context": None,
                                    "varbinds": ({"type": x.OID,
                                                  "name": {"subids": (1, 2, 3),
                                                           "include": False},
                                                  "data": {"subids": (4, 5, 6),
                                                           "include": False}},
                                                 {"type": x.OCTET_STR,
                                                  "name": {"subids": (1, 2, 4),
                                                           "include": False},
                                                  "data": "blah"})}},
                          ""))
        # Test ping
        self.assertEqual(f("\x01\x0D\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x00"),
                         ({"header": {"version": 1, "type": x.PDU_PING,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 0},
                           "body": {"context": None}},
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
                         ({"header": {"version": 1, "type": x.PDU_INDEX_ALLOC,
                                      "flags": makeFlags(False, True, True,
                                                         False, True),
                                      "session_id": 1, "transaction_id": 2,
                                      "packet_id": 3, "length": 64},
                           "body": {"context": None,
                                    "varbinds": ({"type": x.OID,
                                                  "name": {"subids": (1, 2, 3),
                                                           "include": False},
                                                  "data": {"subids": (4, 5, 6),
                                                           "include": False}},
                                                 {"type": x.OCTET_STR,
                                                  "name": {"subids": (1, 2, 4),
                                                           "include": False},
                                                  "data": "blah"})}},
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
                         ({"header": {"version": 1,
                                      "type": x.PDU_INDEX_DEALLOC,
                                      "flags": makeFlags(False, True, True,
                                                         False, True),
                                      "session_id": 1, "transaction_id": 2,
                                      "packet_id": 3, "length": 64},
                           "body": {"context": None,
                                    "varbinds": ({"type": x.OID,
                                                  "name": {"subids": (1, 2, 3),
                                                           "include": False},
                                                  "data": {"subids": (4, 5, 6),
                                                           "include": False}},
                                                 {"type": x.OCTET_STR,
                                                  "name": {"subids": (1, 2, 4),
                                                           "include": False},
                                                  "data": "blah"})}},
                          ""))
        # Test add agent caps
        self.assertEqual(f("\x01\x10\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x18"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"
                           "\x00\x00\x00\x04blah"),
                         ({"header": {"version": 1,
                                      "type": x.PDU_ADD_AGENT_CAPS,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 24},
                           "body": {"context": None,
                                    "oid": {"subids": (4, 5, 6),
                                            "include": False},
                                    "description": "blah"}},
                          ""))
        # Test rm agent caps
        self.assertEqual(f("\x01\x11\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x10"
                           "\x03\x00\x00\x00\x00\x00\x00\x04"
                           "\x00\x00\x00\x05\x00\x00\x00\x06"),
                         ({"header": {"version": 1,
                                      "type": x.PDU_RM_AGENT_CAPS,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 16},
                           "body": {"context": None,
                                    "oid": {"subids": (4, 5, 6),
                                            "include": False}}},
                          ""))
        # Test response
        self.assertEqual(f("\x01\x12\x10\x00"
                           "\x00\x00\x00\x01\x00\x00\x00\x02"
                           "\x00\x00\x00\x03\x00\x00\x00\x08"
                           "\x00\x00\x00\x04\x00\x05\x00\x06"),
                         ({"header": {"version": 1, "type": x.PDU_RESPONSE,
                                      "flags": standardFlags, "session_id": 1,
                                      "transaction_id": 2, "packet_id": 3,
                                      "length": 8},
                           "body": {"sys_uptime": 4, "res_err": 5,
                                    "res_index": 6, "varbinds": None}},
                          ""))


if __name__ == "__main__":
    unittest.main()
