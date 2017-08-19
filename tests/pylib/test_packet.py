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

odict = ntp.util.OrderedDict


class FileJig:
    def __init__(self):
        self.data = []
        self.flushed = False
        self.readline_return = [""]

    def __enter__(self):
        return self

    def __exit__(self, *args):
        return False

    def __iter__(self):
        return self.readline_return.__iter__()

    def write(self, data):
        self.data.append(data)
        self.flushed = False

    def flush(self):
        self.flushed = True

    def readline(self):
        if len(self.readline_return) > 0:
            return self.readline_return.pop(0)
        return ""


class SocketJig:
    def __init__(self):
        self.data = []
        self.return_data = []
        self.closed = False
        self.connected = None
        self.fail_connect = False

    def sendall(self, data):
        self.data.append(data)

    def close(self):
        self.closed = True

    def connect(self, addr):
        if self.fail_connect is True:
            err = socket.error()
            err.strerror = "socket!"
            err.errno = 16
            raise err
        self.connected = addr

    def recv(self, bytecount):
        if len(self.return_data) > 0:
            current = self.return_data.pop(0)
            if len(current) > bytecount:
                ret = current[:bytecount]
                current = current[bytecount:]
                self.return_data.insert(0, current)  # push unwanted data
                return ret
            else:
                return current
        return None


class SessionJig:
    def __init__(self):
        self.readvars_calls = 0

    def readvars(self):
        self.readvars_calls += 1
        return {"foo": 23, "bar": 42}


class ControlPacketJig:
    HEADER_LEN = ntp.packet.ControlPacket.HEADER_LEN

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


class HasherJig:
    def __init__(self):
        self.update_calls = []
        self.digest_size = 0

    def update(self, data):
        self.update_calls.append(data)
        if (data is not None) and (data != "None"):
            self.digest_size += 1

    def digest(self):
        return "blah" * 4  # 16 byte hash


class SocketModuleJig:
    error = socket.error
    gaierror = socket._socket.gaierror
    SOCK_DGRAM = socket.SOCK_DGRAM
    IPPROTO_UDP = socket.IPPROTO_UDP
    AF_UNSPEC = socket.AF_UNSPEC
    AI_NUMERICHOST = socket.AI_NUMERICHOST
    AI_CANONNAME = socket.AI_CANONNAME
    EAI_NONAME = socket.EAI_NONAME
    EAI_NODATA = socket.EAI_NODATA

    def __init__(self):
        self.gai_calls = []
        self.gai_error_count = 0
        self.socket_calls = []
        self.socket_fail = False
        self.socket_fail_connect = False
        self.socketsReturned = []
        self.inet_ntop_calls = []

    def getaddrinfo(self, host, port, family=None, socktype=None,
                    proto=None, flags=None):
        self.gai_calls.append((host, port, family, socktype, proto, flags))
        if self.gai_error_count > 0:
            self.gai_error_count -= 1
            err = self.gaierror("blah")
            err.errno = socket.EAI_NONAME
            raise err
        return 42

    def socket(self, family, socktype, protocol):
        self.socket_calls.append((family, socktype, protocol))
        if self.socket_fail is True:
            err = self.error()
            err.strerror = "error!"
            err.errno = 23
            raise err
        sock = SocketJig()
        if self.socket_fail_connect is True:
            sock.fail_connect = True
        self.socketsReturned.append(sock)
        return sock

    def inet_ntop(self, addr, family):
        self.inet_ntop_calls.append((addr, family))
        return "canon.com"


class GetpassModuleJig:
    def __init__(self):
        self.getpass_calls = []

    def getpass(self, prompt, stream=None):
        self.getpass_calls.append((prompt, stream))
        return "xyzzy"


class HashlibModuleJig:
    def __init__(self):
        self.new_calls = []
        self.hashers_returned = []

    def new(self, name):
        self.new_calls.append(name)
        h = HasherJig()
        self.hashers_returned.append(h)
        return h


class SelectModuleJig:
    error = select.error

    def __init__(self):
        self.select_calls = []

    def select(self, ins, outs, excepts, timeout=0):
        self.select_calls.append((ins, outs, excepts, timeout))
        return (ins, [], [])


class AuthenticatorJig:
    compute_mac_calls = []

    def __init__(self):
        self.control_call_count = 0
        self.fail_getitem = False
        self.compute_mac_calls = []

    def __getitem__(self, key):
        if self.fail_getitem is True:
            raise IndexError
        return ("passtype", "pass")

    def control(self):
        self.control_call_count += 1
        return (23, "keytype", "miranda")

    @staticmethod
    def compute_mac(flatpkt, keyid, keytype, passwd):
        AuthenticatorJig.compute_mac_calls.append((flatpkt, keyid,
                                                   keytype, passwd))
        return "mac"


class TestMisc(unittest.TestCase):
    def test_Peer(self):
        session = SessionJig()
        # Test init
        cls = ntp.packet.Peer(session, 2, 3)
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
        f = ntp.packet.dump_hex_printable
        fp = FileJig()
        data = "\x00\x01\x02\x03\x04\x05\x06\x07" \
               "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
        # Test a single line
        f(data, fp)
        total = "".join(fp.data)  # easier than 300 million seperate strings
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
        cls = ntp.packet.MRUEntry()
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
        # Test sortaddr, ipv6
        cls.addr = "[11:22:33::44:55]:42"
        self.assertEqual(cls.sortaddr(),
                         "\x00\x11\x00\x22\x00\x33\x00\x00"
                         "\x00\x00\x00\x00\x00\x44\x00\x55")
        # Test sortaddr, ipv6, local
        cls.addr = "[11:22:33::44:55%8]:42"
        self.assertEqual(cls.sortaddr(),
                         "\x00\x11\x00\x22\x00\x33\x00\x00"
                         "\x00\x00\x00\x00\x00\x44\x00\x55")
        # Test sortaddr, ipv4
        cls.addr = "11.22.33.44:23"
        self.assertEqual(cls.sortaddr(), (("\0" * 16) + "\x0b\x16\x21\x2c"))
        # Test __repr__
        self.assertEqual(cls.__repr__(),
                         "<MRUEntry: "
                         "'last': '0x00000200.00000000', "
                         "'addr': '11.22.33.44:23', 'rs': None, 'mv': None, "
                         "'first': '0x00000100.00000000', 'ct': 4>")

    def test_MRUList(self):
        # Test init
        cls = ntp.packet.MRUList()
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


class TestControlSession(unittest.TestCase):
    target = ntp.packet.ControlSession

    def test___init__(self):
        # Test
        cls = self.target()
        self.assertEqual(cls.debug, 0)
        self.assertEqual(cls.ai_family, socket.AF_UNSPEC)
        self.assertEqual(cls.primary_timeout, ntp.packet.DEFTIMEOUT)
        self.assertEqual(cls.secondary_timeout, ntp.packet.DEFSTIMEOUT)
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
        sockjig = SocketJig()
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
        logjig = FileJig()
        try:
            fakesockmod = SocketModuleJig()
            ntp.packet.socket = fakesockmod
            # Init
            cls = self.target()
            cls.debug = 3
            cls.logfp = logjig
            # Test first type
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, 42)
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST)])
            self.assertEqual(logjig.data, [])
            # Test second type
            logjig.__init__()  # reset
            fakesockmod.__init__()
            fakesockmod.gai_error_count = 1
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, 42)
            self.assertEqual(logjig.data,
                             ["ntpq: numeric-mode lookup of blah.com "
                              "failed, None\n"])
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0)])
            # Test third type
            logjig.__init__()  # reset
            fakesockmod.__init__()
            fakesockmod.gai_error_count = 2
            result = cls._ControlSession__lookuphost("blah.com", "family")
            self.assertEqual(result, 42)
            self.assertEqual(logjig.data,
                             ["ntpq: numeric-mode lookup of blah.com "
                              "failed, None\n",
                              "ntpq: standard-mode lookup of blah.com "
                              "failed, None\n"])
            self.assertEqual(fakesockmod.gai_calls,
                             [("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP,
                               socket.AI_NUMERICHOST),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0),
                              ("blah.com", "ntp", cls.ai_family,
                               socket.SOCK_DGRAM, socket.IPPROTO_UDP, 0)])
        finally:
            ntp.packet.socket = socket

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
        logjig = FileJig()
        try:
            fakesockmod = SocketModuleJig()
            ntp.packet.socket = fakesockmod
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
            self.assertEqual(logjig.data, ["Opening host canon\n"])
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
            self.assertEqual(logjig.data, ["Opening host canon.com\n"])
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
            except ntp.packet.ControlException as e:
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
            except ntp.packet.ControlException as e:
                errored = e
            self.assertEqual(errored.message,
                             "Error connecting to foo.org: socket! [16]")
        finally:
            ntp.packet.socket = socket

    def test_password(self):
        iojig = FileJig()
        fakegetpmod = GetpassModuleJig()
        # Init
        cls = self.target()
        try:
            tempauth = ntp.packet.Authenticator()
            ntp.packet.Authenticator = AuthenticatorJig
            ntp.packet.getpass = fakegetpmod
            tempstdin = sys.stdin
            sys.stdin = iojig
            tempstdout = sys.stdout
            sys.stdout = iojig
            # Test, with nothing
            iojig.readline_return = ["1\n"] * 10
            cls.password()
            self.assertEqual(isinstance(cls.auth, AuthenticatorJig), True)
            self.assertEqual(cls.keyid, 1)
            self.assertEqual(cls.passwd, "pass")
            # Test, with auth and localhost
            cls.keyid = None
            cls.passwd = None
            cls.hostname = "localhost"
            cls.password()
            self.assertEqual(cls.keyid, 23)
            self.assertEqual(cls.keytype, "keytype")
            self.assertEqual(cls.passwd, "miranda")
            # Test, with all but password
            cls.passwd = None
            cls.auth.fail_getitem = True
            cls.password()
            self.assertEqual(fakegetpmod.getpass_calls,
                             [("keytype Password: ", None)])
            self.assertEqual(cls.passwd, "xyzzy")
        finally:
            ntp.packet.Authenticator = tempauth
            ntp.packet.getpass = getpass
            sys.stdin = tempstdin
            sys.stdout = tempstdout

    def test_sendpkt(self):
        logjig = FileJig()
        sockjig = SocketJig()

        # Init
        cls = self.target()
        cls.logfp = logjig
        cls.sock = sockjig
        cls.debug = 3
        # Test
        res = cls.sendpkt("blahfoo")
        self.assertEqual(res, 0)
        self.assertEqual(len(logjig.data), 1)
        self.assertEqual(logjig.data[0], "Sending 8 octets.  seq=0\n")
        self.assertEqual(len(sockjig.data), 1)
        self.assertEqual(sockjig.data[0], "blahfoo\x00")

    def test_sendrequest(self):
        logjig = FileJig()
        try:
            tempcpkt = ntp.packet.ControlPacket
            ntp.packet.ControlPacket = ControlPacketJig
            tempauth = ntp.packet.Authenticator
            ntp.packet.Authenticator = AuthenticatorJig
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
            result = cls.sendrequest(1, 2, "foo")
            self.assertEqual(result.sequence, 1)
            self.assertEqual(result.extension, "foo\x00")
            # Test with auth
            cls.keyid = 1
            cls.passwd = "qwerty"
            result = cls.sendrequest(1, 2, "foo", True)
            self.assertEqual(result.sequence, 2)
            self.assertEqual(result.extension, "foo\x00mac")
            # Test with auth keyid / password failure
            cls.keyid = None
            try:
                cls.sendrequest(1, 2, "foo", True)
                errored = False
            except ntp.packet.ControlException:
                errored = True
            self.assertEqual(errored, True)
        finally:
            ntp.packet.ControlPacket = tempcpkt
            ntp.packet.Authenticator = tempauth

    def test_getresponse(self):
        logjig = FileJig()
        sockjig = SocketJig()
        fakeselectmod = SelectModuleJig()
        # Init
        cls = self.target()
        cls.debug = 3
        cls.logfp = logjig
        cls.sock = sockjig
        try:
            ntp.packet.select = fakeselectmod
            # Test empty
            sockjig.return_data = [
                "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"]
            cls.getresponse(1, 2, True)
            self.assertEqual(cls.response, "")
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
            self.assertEqual(cls.response, "foo=4223,blah=248,x=23,quux=1")
        finally:
            ntp.packet.select = select

    def test___validate_packet(self):
        logjig = FileJig()
        # Init
        cls = self.target()
        cls.debug = 5
        cls.logfp = logjig
        # Test good packet, empty data
        raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         True)
        self.assertEqual(logjig.data, [])
        # Test good packet, with data
        logjig.data = []
        raw = "\x0E\xA1\x00\x01\x00\x02\x00\x03\x00\x00\x00\x09" \
              "foo=4223,\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        cls.sequence = 1
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 3),
                         True)
        self.assertEqual(logjig.data, [])
        # Test bad packet, bad version
        # Need to fix version logic 0x46 can be ver == 5, or 0
        cls.sequence = 0
        logjig.data = []
        raw = "\x46\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data, ["Fragment received with version 0\n"])
        # Test bad packet, bad mode
        logjig.data = []
        raw = "\x0D\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data, ["Fragment received with mode 5\n"])
        # Test bad packet, bad response bit
        logjig.data = []
        raw = "\x0E\x01\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data, ["Received request, wanted response\n"])
        # Test bad packet, bad sequence
        logjig.data = []
        raw = "\x0E\x81\x00\x01\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data,
                         ["Received sequence number 1, wanted 0\n"])
        # Test bad packet, bad opcode
        logjig.data = []
        raw = "\x0E\x80\x00\x00\x00\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data,
                         ["Received opcode 0, wanted 1\n"])
        # Test error packet
        logjig.data = []
        raw = "\x0E\xC1\x00\x00" + \
              chr(ntp.control.CERR_BADVALUE) + \
              "\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        try:
            cls._ControlSession__validate_packet(pkt, raw, 1, 2)
            self.assertEqual(False, True)  # it should have errored here
        except ntp.packet.ControlException as e:
            self.assertEqual(e.errorcode, ntp.control.CERR_BADVALUE)
        self.assertEqual(logjig.data, [])
        # Test error packet, with more bit
        logjig.data = []
        errcs = chr(ntp.control.CERR_BADVALUE)
        raw = "\x0E\xE1\x00\x00" + errcs + "\x03\x00\x02\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        try:
            cls._ControlSession__validate_packet(pkt, raw, 1, 2)
            self.assertEqual(False, True)  # it should have errored here
        except ntp.packet.ControlException as e:
            self.assertEqual(e.errorcode, ntp.control.CERR_BADVALUE)
        errstr = "Error " + str(ntp.control.CERR_BADVALUE) + \
                 " received on non-final fragment\n"
        self.assertEqual(logjig.data, [errstr])
        # Test ok-ish packet, bad associd
        logjig.data = []
        raw = "\x0E\x81\x00\x00\x00\x03\x00\xFF\x00\x00\x00\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         True)
        self.assertEqual(logjig.data,
                         ["Association ID 255 doesn't match expected 2\n"])
        # Test bad data padding
        logjig.data = []
        raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x01@"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        self.assertEqual(cls._ControlSession__validate_packet(pkt, raw, 1, 2),
                         False)
        self.assertEqual(logjig.data,
                         ["Response fragment not padded, size = 13\n"])
        # Test too little data
        logjig.data = []
        raw = "\x0E\x81\x00\x00\x00\x03\x00\x02\x00\x00\x00\x10foo\x00"
        pkt = ntp.packet.ControlPacket(cls)
        pkt.analyze(raw)
        try:
            cls._ControlSession__validate_packet(pkt, raw, 1, 2)
            self.assertEqual(True, False)  # should have errored here
        except ntp.packet.ControlException as e:
            self.assertEqual(e.message, ntp.packet.SERR_INCOMPLETE)
        self.assertEqual(logjig.data,
                         ["Response fragment claims 16 octets payload, "
                          "above 4 received\n"])

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
                raise ntp.packet.ControlException(ntp.packet.SERR_TIMEOUT)
            return "flax!"
        # Init
        cls = self.target()
        cls.sendrequest = sendrequest_jig
        cls.getresponse = getresponse_jig
        cls.sock = True  # to fool havehost()
        # Test no retry
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
        cls.response = "\xDE\xAD\xF0\x0D"
        idlist = cls.readstat()
        self.assertEqual(len(idlist), 1)
        self.assertEqual(isinstance(idlist[0], ntp.packet.Peer), True)
        self.assertEqual(idlist[0].associd, 0xDEAD)
        self.assertEqual(idlist[0].status, 0xF00D)
        self.assertEqual(queries, [(ntp.control.CTL_OP_READSTAT,
                                    0, "", False)])
        # Test incorrect response
        cls.response = "foo"
        try:
            cls.readstat()
            errored = False
        except ntp.packet.ControlException:
            errored = True
        self.assertEqual(errored, True)

    def test___parse_varlist(self):
        # Init
        cls = self.target()
        cls.response = 'srcadr=0.0.0.0, srcport=0, srchost="0.ubuntu.pool.ntp.org",\r\ndstadr=0.0.0.0, dstport=0, leap=3, stratum=16, precision=-22,\r\nrootdelay=0.000, rootdisp=0.000, refid=POOL,\r\nreftime=0x00000000.00000000, rec=0x00000000.00000000, reach=0x0,\r\nunreach=0, hmode=3, pmode=0, hpoll=6, ppoll=10, headway=0, flash=0x1600,\r\nkeyid=0, offset=0.000, delay=0.000, dispersion=16000.000, jitter=0.000,\r\nfiltdelay= 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00,\r\nfiltoffset= 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00,\r\nfiltdisp= 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00 16000.00\r\n'
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
                                 "16000.00 16000.00 16000.00 16000.00"))))
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
                                  "16000.00 16000.00 16000.00 16000.00")))))

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
        cls.response = "Config Succeeded    \n \x00 blah blah"
        # Test success
        result = cls.config("Boo!")
        self.assertEqual(result, True)
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_CONFIGURE, 0, "Boo!", True)])
        # Test failure
        queries = []
        cls.response = "whatever man..."
        result = cls.config("Boo!")
        self.assertEqual(result, False)
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_CONFIGURE, 0, "Boo!", True)])

    def test_fetch_nonce(self):
        queries = []

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
        # Init
        filefp = FileJig()
        cls = self.target()
        cls.doquery = doquery_jig
        # Test success
        cls.response = "nonce=blah blah  "
        result = cls.fetch_nonce()
        self.assertEqual(result, "nonce=blah blah")
        self.assertEqual(queries,
                         [(ntp.control.CTL_OP_REQ_NONCE, 0, "", False)])
        # Test failure
        queries = []
        cls.logfp = filefp
        cls.response = "blah blah"
        try:
            result = cls.fetch_nonce()
            errored = False
        except ntp.packet.ControlException:
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

        def doquery_jig(opcode, associd=0, qdata="", auth=False):
            queries.append((opcode, associd, qdata, auth))
            if len(query_results) > 0:
                setresponse(query_results.pop(0))
        logjig = FileJig()
        # Init
        cls = self.target()
        cls.fetch_nonce = fetch_nonce_jig
        cls.doquery = doquery_jig
        cls.logfp = logjig
        # Test empty varlist
        result = cls.mrulist()
        self.assertEqual(nonce_fetch_count, [4])
        self.assertEqual(queries,
                         [(10, 0, "nonce=foo, frags=32", False),
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
        self.assertEqual(isinstance(result, ntp.packet.MRUList), True)
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
        result = cls.mrulist(variables={"sort": "addr", "frags": 24})
        self.assertEqual(nonce_fetch_count, [4])
        self.assertEqual(queries,
                         [(10, 0, "nonce=foo, frags=24", False),
                          (10, 0,
                           "nonce=foo, frags=25, addr.0=1.2.3.4:23, last.0=40",
                           False),
                          (10, 0,
                           "nonce=foo, frags=26, addr.0=1.2.3.4:23, "
                           "last.0=41, addr.1=1.2.3.4:23, last.1=40", False),
                          (10, 0,
                           "nonce=foo, frags=27, addr.0=10.20.30.40:23, "
                           "last.0=42, addr.1=1.2.3.4:23, last.1=41, "
                           "addr.2=1.2.3.4:23, last.2=40", False)])
        self.assertEqual(isinstance(result, ntp.packet.MRUList), True)
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
    target = ntp.packet.Authenticator
    open_calls = []
    open_files = []
    open_data = []

    def openjig(self, filename):
        self.open_calls.append(filename)
        fd = FileJig()
        fd.readline_return = self.open_data
        self.open_files.append(fd)
        return fd

    def test___init__(self):
        try:
            ntp.packet.open = self.openjig
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
            ntp.packet.open = open
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
            ntp.packet.open = self.openjig
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
            ntp.packet.open = open

    def test_compute_mac(self):
        f = self.target.compute_mac
        try:
            temphash = ntp.packet.hashlib
            fakehashlibmod = HashlibModuleJig()
            ntp.packet.hashlib = fakehashlibmod
            # Test no digest
            self.assertEqual(f(None, None, None, None), None)
            # Test with digest
            self.assertEqual(f("foo", 0x42, "bar", "quux"),
                             "\x00\x00\x00\x42blahblahblahblah")
        finally:
            ntp.packet.hashlib = temphash

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
            temphash = ntp.packet.hashlib
            fakehashlibmod = HashlibModuleJig()
            ntp.packet.hashlib = fakehashlibmod
            # Test good
            self.assertEqual(cls.verify_mac(good_pkt), True)
            # Test bad
            self.assertEqual(cls.verify_mac(bad_pkt), False)
        finally:
            ntp.packet.hashlib = temphash


if __name__ == "__main__":
    unittest.main()
