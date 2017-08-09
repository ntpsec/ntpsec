# -*- coding: utf-8 -*-

"""
packet.py - definitions and classes for Python querying of NTP

Freely translated from the old C ntpq code by ESR, with comments
preserved.  The idea was to cleanly separate ntpq-that-was into a
thin front-end layer handling mainly command interpretation and a
back-end that presents the take from ntpd as objects that can be
re-used by other front ends. Other reusable pieces live in util.py.

This code should be Python2-vs-Python-3 agnostic.  Keep it that way!

Here are some pictures to help make sense of this code. First, from RFC 5905,
the general structure of an NTP packet (Figure 8):

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |LI | VN  |Mode |    Stratum     |     Poll      |  Precision   |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Root Delay                            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Root Dispersion                       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                          Reference ID                         |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                     Reference Timestamp (64)                  +
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                      Origin Timestamp (64)                    +
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                      Receive Timestamp (64)                   +
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                      Transmit Timestamp (64)                  +
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                    Extension Field 1 (variable)               .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                    Extension Field 2 (variable)               .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                          Key Identifier                       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                            dgst (128)                         |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

The fixed header is 48 bytes long.  The simplest possible case of an
NTP packet is the minimal SNTP request, a mode 3 packet with the
Stratum and all following fields zeroed out to byte 47.

How to interpret these fields:

t_1, the origin timestamp, is the time according to the client at
which the request was sent.

t_2, the receive timestamp, is the time according to the server at
which the request was received.

t_3, the transmit timestamp, is the time according to the server at
which the reply was sent.

You also need t_4, the destination timestamp, is the time according to
the client at which the reply was received.  This is not in the reply packet,
it's the packet receipt time collected by the client.

The 'Reference timestamp' is an unused historical relic.  It's supposed to be
copied unchanged from upstream in the strutum hierarchy. Normal practice
has been for Stratum 1 servers to fill it in with the raw timestamp from the
most recent reference-clock.

Theta is the thing we want to estimate: the offset between the server
clock and the client clock. The sign convention is that theta is
positive iff the server is ahead of the client.

Theta is estimated by [(t_2-t_1)+(t_3-t_4)]/2. The accuracy of this
estimate is predicated upon network latency being symmetrical.

Delta is the network round trip time, i.e. (t_4-t_1)-(t_3-t_2). Here's
how the terms work: (t_4-t_1) is the total time that the request was
in flight, and (t_3-t_2) is time that the server spent processing it;
when you subtract that out you're left with just network delays.

Lambda nominally represents the maximum amount by which theta could be
off. It's computed as delta/2 + epsilon. The delta/2 term usually
dominates and represents the maximum amount by which network asymmetry
could be throwing off the calculation. Epsilon is the sum of three
other sources of error:

rho_r: the (im)precision field from response packet, representing the
server's inherent error in clock measurement.

rho_s: the client's own (im)precision.

PHI*(t_4-t_1): The amount by which the client's clock may plausibly
have drifted while the packet was in flight. PHI is taken to be a
constant of 15ppm.

rho_r and rho_s are estimated by making back-to-back calls to
clock_gettime() (or similar) and taking their difference. They're
encoded on the wire as an eight-bit two's complement integer
representing, to the nearest integer, log_2 of the value in seconds.

If you look at the raw data, there are 3 unknowns:
   * transit time client to server
   * transit time server to client
   * clock offset
>but there are only two equations, so you can't solve it.

NTP gets a 3rd equation by assuming the transit times are equal.  That lets
it solve for the clock offset.

If you assume that both clocks are accurate which is reasonable if you have
GPS at both ends, then you can easily solve for the transit times in each
direction.

The RFC 5905 diagram is slightly out of date in that the digest header assumes
a 128-bit (16-octet) MD5 hash, but it is also possible for the field to be a
160-bit (20-octet) SHA1 hash.

An extension field consists of a 16-bit network-order type field
length, followed by a 16-bit network-order payload length in octets,
followed by the payload (which must be padded to a 4-octet boundary).

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |         Type field             |      Payload length          |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                        Payload (variable)                     |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Here's what a Mode 6 packet looks like:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |LI | VN  | 6   |R|E|M|  Opcode  |          Sequence            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |               Status           |       Association ID         |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |               Offset           |            Count             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      .                                                               .
      .                        Payload (variable)                     .
      .                                                               .
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                          Key Identifier                       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                            dgst (128)                         |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

In this case the fixed header is 24 bytes long.

R = Response bit
E = Error bit
M = More bit.

A Mode 6 packet cannot have extension fields.

"""
# SPDX-License-Identifier: BSD-2-clause
from __future__ import print_function, division
import getpass
import hashlib
import select
import socket
import string
import struct
import sys
import time
import ntp.control
import ntp.magic
import ntp.ntpc
import ntp.util

# General notes on Python 2/3 compatibility:
#
# This code uses the following strategy to allow it to run on both Python 2
# and Python 3:
#
# - Use binary I/O to read/write data from/to files and subprocesses;
#   where the exact bytes are important (such as in checking for
#   modified files), use the binary data directly
#
# - Use latin-1 encoding to transform binary data to/from Unicode when
#   necessary for operations where Python 3 expects Unicode; the
#   polystr and polybytes functions are used to do this so that
#   when running on Python 2, the byte string data is used unchanged;
#   also, the make_wrapper function constructs a text stream that can
#   wrap a file opened in binary mode for cases where a file object
#   that can be passed around from function to function is needed
#
# - Construct custom stdin, stdout, and stderr streams when running
#   on Python 3 that force latin-1 encoding, and wrap them around the
#   underlying binary buffers (in Python 2, the streams are binary
#   and are used unchanged); this ensures that the same transformation
#   is done on data from/to the standard streams, as is done on binary
#   data from/to files and subprocesses; the make_std_wrapper function
#   does this

master_encoding = 'latin-1'

if str is bytes:  # Python 2
    polystr = str
    polybytes = bytes
    polyord = ord
    polychr = str
    input = raw_input

    def string_escape(s):
        return s.decode('string_escape')

    def make_wrapper(fp):
        return fp

else:  # Python 3
    import io

    def polystr(o):
        "Polymorphic string factory function"
        if isinstance(o, str):
            return o
        if not isinstance(o, bytes):
            return str(o)
        return str(o, encoding=master_encoding)

    def polybytes(s):
        "Polymorphic string encoding function"
        if isinstance(s, bytes):
            return s
        if not isinstance(s, str):
            return bytes(s)
        return bytes(s, encoding=master_encoding)

    def polyord(c):
        "Polymorphic ord() function"
        if isinstance(c, str):
            return ord(c)
        else:
            return c

    def polychr(c):
        "Polymorphic chr() function"
        if isinstance(c, int):
            return chr(c)
        else:
            return c

    def string_escape(s):
        "Polymorphic string_escape/unicode_escape"
        # This hack is necessary because Unicode strings in Python 3 don't
        # have a decode method, so there's no simple way to ask it for the
        # equivalent of decode('string_escape') in Python 2. This function
        # assumes that it will be called with a Python 3 'str' instance
        return s.encode(master_encoding).decode('unicode_escape')

    def make_wrapper(fp):
        "Wrapper factory function to enforce master encoding"
        # This can be used to wrap normally binary streams for API
        # compatibility with functions that need a text stream in
        # Python 3; it ensures that the binary bytes are decoded using
        # the master encoding we use to turn bytes to Unicode in
        # polystr above
        # newline="\n" ensures that Python 3 won't mangle line breaks
        return io.TextIOWrapper(fp, encoding=master_encoding, newline="\n")

    def make_std_wrapper(stream):
        "Standard input/output wrapper factory function"
        # This ensures that the encoding of standard output and standard
        # error on Python 3 matches the master encoding we use to turn
        # bytes to Unicode in polystr above
        # line_buffering=True ensures that interactive
        # command sessions work as expected
        return io.TextIOWrapper(stream.buffer,
                                encoding=master_encoding, newline="\n",
                                line_buffering=True)

# Limit on packets in a single Mode 6 response.  Increasing this value to
# 96 will marginally speed "mrulist" operation on lossless networks
# but it has been observed to cause loss on WiFi networks and with
# an IPv6 go6.net tunnel over UDP.  That loss causes the request
# row limit to be cut in half, and it grows back very slowly to
# ensure forward progress is made and loss isn't triggered too quickly
# afterward.  While the lossless case gains only marginally with
# MAXFRAGS == 96, the lossy case is a lot slower due to the repeated
# timeouts.  Empirically, MAXFRAGS == 32 avoids most of the routine loss
# on both the WiFi and UDP v6 tunnel tests and seems a good compromise.
# This suggests some device in the path has a limit of 32 ~512 byte UDP
# packets in queue.
# Lowering MAXFRAGS may help with particularly lossy networks, but some
# ntpq commands may rely on the longtime value of 24 implicitly,
# assuming a single multipacket response will be large enough for any
# needs.  In contrast, the "mrulist" command is implemented as a series
# of requests and multipacket responses to each.
MAXFRAGS = 32

# Requests are automatically retried once, so total timeout with no
# response is a bit over 2 * DEFTIMEOUT, or 10 seconds.  At the other
# extreme, a request eliciting 32 packets of responses each for some
# reason nearly DEFSTIMEOUT seconds after the prior in that series,
# with a single packet dropped, would take around 32 * DEFSTIMEOUT, or
# 93 seconds to fail each of two times, or 186 seconds.
# Some commands involve a series of requests, such as "peers" and
# "mrulist", so the cumulative timeouts are even longer for those.
DEFTIMEOUT = 5000
DEFSTIMEOUT = 3000


class Packet:
    "Encapsulate an NTP fragment"
    # The following two methods are copied from macros in includes/control.h
    @staticmethod
    def VN_MODE(v, m):
        return ((((v) & 7) << 3) | ((m) & 0x7))

    @staticmethod
    def PKT_LI_VN_MODE(l, v, m):
        return ((((l) & 3) << 6) | Packet.VN_MODE((v), (m)))

    def __init__(self, mode=ntp.magic.MODE_CLIENT,
                 version=ntp.magic.NTP_VERSION, session=None):
        self.session = session  # Where to get session context
        self.li_vn_mode = 0     # leap, version, mode (uint8_t)
        # Subclasses have variable fields here
        self.extension = b''     # extension data
        self.li_vn_mode = Packet.PKT_LI_VN_MODE(3, version, mode)

    # These decorators will allow us to assign the extension Python 3 strings
    @property
    def extension(self):
        return self.__extension

    @extension.setter
    def extension(self, x):
        self.__extension = polybytes(x)

    def leap(self):
        return ("no-leap", "add-leap", "del-leap",
                "unsync")[ntp.magic.PKT_LEAP(self.li_vn_mode)]

    def version(self):
        return (self.li_vn_mode >> 3) & 0x7

    def mode(self):
        return self.li_vn_mode & 0x7


class SyncException(BaseException):

    def __init__(self, message, errorcode=0):
        self.message = message
        self.errorcode = errorcode

    def __str__(self):
        return self.message


class SyncPacket(Packet):
    "Mode 1-5 time-synchronization packet, including SNTP."
    format = "!BBBbIIIQQQQ"
    HEADER_LEN = 48
    UNIX_EPOCH = 2208988800     # Midnight 1 Jan 1970 in secs since NTP epoch
    PHI = 15 * 1e-6             # 15ppm

    def __init__(self, data=''):
        Packet.__init__(self)
        self.status = 0         # status word for association (uint16_t)
        self.stratum = 0
        self.poll = 0
        self.precision = 0
        self.rootdelay = 0
        self.rootdispersion = 0
        self.refid = 0
        self.reference_timestamp = 0
        self.origin_timestamp = 0
        self.receive_timestamp = 0
        self.transmit_timestamp = 0
        self.data = polybytes(data)
        self.extension = ''
        self.extfields = []
        self.mac = ''
        self.hostname = None
        self.resolved = None
        self.received = SyncPacket.posix_to_ntp(time.time())
        self.trusted = True
        self.rescaled = False
        if self.data:
            self.analyze()

    def analyze(self):
        if len(self.data) < SyncPacket.HEADER_LEN or (len(self.data) & 3) != 0:
            raise SyncException("impossible packet length")
        (self.li_vn_mode,
         self.stratum,
         self.poll,
         self.precision,
         self.root_delay,
         self.root_dispersion,
         self.refid,
         self.reference_timestamp,
         self.origin_timestamp,
         self.receive_timestamp,
         self.transmit_timestamp) = struct.unpack(
            SyncPacket.format, self.data[:SyncPacket.HEADER_LEN])
        self.extension = self.data[SyncPacket.HEADER_LEN:]
        # Parse the extension field if present. We figure out whether
        # an extension field is present by measuring the MAC size. If
        # the number of 4-octet words following the packet header is
        # 0, no MAC is present and the packet is not authenticated. If
        # 1, the packet is a crypto-NAK; if 3, the packet is
        # authenticated with DES; if 5, the packet is authenticated
        # with MD5; if 6, the packet is authenticated with SHA1. If 2
        # or 4, the packet is a runt and discarded forthwith. If
        # greater than 6, an extension field is present, so we
        # subtract the length of the field and go around again.
        while len(self.extension) > 24:
            (ftype, flen) = struct.unpack("!II", self.extension[:8])
            self.extfields.append((ftype, self.extension[8:8+flen]))
            self.extension = self.extension[8+flen:]
        if len(self.extension) == 4:    # Crypto-NAK
            self.mac = self.extension
        if len(self.extension) == 12:   # DES
            raise SyncException("Unsupported DES authentication")
        elif len(self.extension) in (8, 16):
            raise SyncException("Packet is a runt")
        elif len(self.extension) in (20, 24):   # MD5 or SHA1
            self.mac = self.extension

    @staticmethod
    def ntp_to_posix(t):
        "Scale from NTP time to POSIX time"
        # Note: assumes we're in the same NTP era as the transmitter...
        return (t * 2**-32) - SyncPacket.UNIX_EPOCH

    @staticmethod
    def posix_to_ntp(t):
        "Scale from POSIX time to NTP time"
        # Note: assumes we're in the same NTP era as the transmitter...
        return int((t + SyncPacket.UNIX_EPOCH) * 2**32)

    def posixize(self):
        "Rescale all timestamps to POSIX time."
        if not self.rescaled:
            self.rescaled = True
            self.root_delay *= 2**-16
            self.root_dispersion *= 2**-16
            self.reference_timestamp = SyncPacket.ntp_to_posix(
                self.reference_timestamp)
            self.origin_timestamp = SyncPacket.ntp_to_posix(
                self.origin_timestamp)
            self.receive_timestamp = SyncPacket.ntp_to_posix(
                self.receive_timestamp)
            self.transmit_timestamp = SyncPacket.ntp_to_posix(
                self.transmit_timestamp)
            self.received = SyncPacket.ntp_to_posix(self.received)

    def t1(self):
        return self.origin_timestamp

    def t2(self):
        return self.receive_timestamp

    def t3(self):
        return self.transmit_timestamp

    def t4(self):
        return self.received

    def delta(self):
        "Packet flight time"
        return (self.t4() - self.t1()) - (self.t3() - self.t2())

    def epsilon(self):
        "Residual error due to clock imprecision."
        # FIXME: Include client imprecision.
        return SyncPacket.PHI * (self.t4() - self.t1()) + 2**self.precision

    def synchd(self):
        "Synchronization distance, estimates worst-case error in seconds"
        # This is "lambda" in NTP-speak, but that's a Python keyword
        return abs(self.delta()/2 + self.epsilon())

    def adjust(self):
        "Adjustment implied by this packet - 'theta' in NTP-speak."
        return ((self.t2()-self.t1())+(self.t3()-self.t4()))/2

    def leap(self):
        return ("no-leap", "add-leap", "del-leap",
                "unsync")[((self.li_vn_mode) >> 6) & 0x3]

    def flatten(self):
        "Flatten the packet into an octet sequence."
        body = struct.pack(SyncPacket.format,
                           self.li_vn_mode,
                           self.stratum,
                           self.poll,
                           self.precision,
                           self.rootdelay,
                           self.rootdispersion,
                           self.refid,
                           self.reference_timestamp,
                           self.origin_timestamp,
                           self.receive_timestamp,
                           self.transmit_timestamp)
        return body + self.extension

    def refid_octets(self):
        "Analyze refid into octets."
        return ((self.refid >> 24) & 0xff,
                (self.refid >> 16) & 0xff,
                (self.refid >> 8) & 0xff,
                self.refid & 0xff)

    def refid_as_string(self):
        "Sometimes it's a clock name or KOD type"
        return polystr(struct.pack(*(("BBBB",) + self.refid_octets())))

    def refid_as_address(self):
        "Sometimes it's an IPV4 address."
        return polystr("%d.%d.%d.%d" % self.refid_octets())

    def is_crypto_nak(self):
        return len(self.mac) == 4

    def has_MD5(self):
        return len(self.mac) == 20

    def has_SHA1(self):
        return len(self.mac) == 24

    def __repr__(self):
        "Represent a posixized sync packet in an eyeball-friendly format."
        r = "<NTP:%s:%d:%d" % (self.leap(), self.version(), self.mode())
        r += "%f:%f" % (self.root_delay, self.root_dispersion)
        rs = self.refid_as_string()
        if not all(c in string.printable for c in rs):
            rs = self.refid_as_address()
        r += ":" + rs
        r += ":" + ntp.util.rfc3339(
            SyncPacket.ntp_to_posix(self.reference_timestamp))
        r += ":" + ntp.util.rfc3339(
            SyncPacket.ntp_to_posix(self.origin_timestamp))
        r += ":" + ntp.util.rfc3339(
            SyncPacket.ntp_to_posix(self.receive_timestamp))
        r += ":" + ntp.util.rfc3339(
            SyncPacket.ntp_to_posix(self.transmit_timestamp))
        if self.extfields:
            r += ":" + repr(self.extfields)
        if self.mac:
            r += ":" + repr(self.mac)[1:-1]
        r += ">"
        return r


class ControlPacket(Packet):
    "Mode 6 request/response."

    def __init__(self, session, opcode=0, associd=0, qdata=''):
        Packet.__init__(self, mode=ntp.magic.MODE_CONTROL,
                        version=session.pktversion,
                        session=session)
        self.r_e_m_op = opcode    # ntpq operation code
        self.sequence = 1         # sequence number of request (uint16_t)
        self.status = 0           # status word for association (uint16_t)
        self.associd = associd    # association ID (uint16_t)
        self.offset = 0           # offset of this batch of data (uint16_t)
        self.extension = qdata    # Data for this packet
        self.count = len(qdata)   # length of data
    format = "!BBHHHHH"
    HEADER_LEN = 12

    def is_response(self):
        return True if self.r_e_m_op & 0x80 else False

    def is_error(self):
        return True if self.r_e_m_op & 0x40 else False

    def more(self):
        return True if self.r_e_m_op & 0x20 else False

    def opcode(self):
        return self.r_e_m_op & 0x1F

    def errcode(self):
        return (self.status >> 8) & 0xff

    def end(self):
        return self.count + self.offset

    def stats(self, idx):
        "Return statistics on a fragment."
        return "%5d %5d\t%3d octets\n" % (self.offset, self.end(), self.count)

    def analyze(self, rawdata):
        rawdata = polybytes(rawdata)
        (self.li_vn_mode,
         self.r_e_m_op,
         self.sequence,
         self.status,
         self.associd,
         self.offset,
         self.count) = struct.unpack(ControlPacket.format,
                                     rawdata[:ControlPacket.HEADER_LEN])
        self.data = rawdata[ControlPacket.HEADER_LEN:]
        return (self.sequence, self.status, self.associd, self.offset)

    def flatten(self):
        "Flatten the packet into an octet sequence."
        body = struct.pack(ControlPacket.format,
                           self.li_vn_mode,
                           self.r_e_m_op,
                           self.sequence,
                           self.status,
                           self.associd,
                           self.offset,
                           self.count)
        return body + self.extension

    def send(self):
        self.session.sendpkt(self.flatten())


class Peer:
    "The information we have about an NTP peer."

    def __init__(self, session, associd, status):
        self.session = session
        self.associd = associd
        self.status = status
        self.variables = {}

    def readvars(self):
        self.variables = self.session.readvars()

    def __str__(self):
        return "<Peer: associd=%s status=%0x>" % (self.associd, self.status)
    __repr__ = __str__

SERR_BADFMT = "***Server reports a bad format request packet\n"
SERR_PERMISSION = "***Server disallowed request (authentication?)\n"
SERR_BADOP = "***Server reports a bad opcode in request\n"
SERR_BADASSOC = "***Association ID {0} unknown to server\n"
SERR_UNKNOWNVAR = "***A request variable unknown to the server\n"
SERR_BADVALUE = "***Server indicates a request variable was bad\n"
SERR_UNSPEC = "***Server returned an unspecified error\n"
SERR_TIMEOUT = "***Request timed out\n"
SERR_INCOMPLETE = "***Response from server was incomplete\n"
SERR_TOOMUCH = "***Buffer size exceeded for returned data\n"
SERR_SELECT = "***Select call failed\n"
SERR_NOHOST = "***No host open\n"
SERR_BADLENGTH = "***Response length should have been a multiple of 4"
SERR_BADKEY = "***Invalid key identifier"
SERR_INVPASS = "***Invalid password"
SERR_NOKEY = "***Key not found"
SERR_BADNONCE = "***Unexpected nonce response format"
SERR_BADPARAM = "***Unknown parameter '%s'"
SERR_NOCRED = "***No credentials"
SERR_SERVER = "***Server error code %s"
SERR_STALL = "***No response, probably high-traffic server with low MRU limit"
SERR_BADTAG = "***Bad MRU tag %s"
SERR_BADSORT = "***Sort order %s is not implemented"
SERR_NOTRUST = "***No trusted keys have been declared"


def dump_hex_printable(xdata, outfp=sys.stdout):
    "Dump a packet in hex, in a familiar hex format"
    llen = len(xdata)
    i = 0
    while llen > 0:
        rowlen = min(16, llen)
        restart = i
        for idx in range(16):
            if idx < llen:
                outfp.write("%02x " % polyord(xdata[i]))
                i += 1
            else:
                outfp.write("   ")
        i = restart
        for idx in range(rowlen):
            # Do not use curses.isprint(), netbsd base doesn't install curses
            if polyord(xdata[i]) >= 32 and polyord(xdata[i]) < 127:
                outfp.write(polychr(xdata[i]))
            else:
                outfp.write('.')
            i += 1
        outfp.write("\n")
        llen -= rowlen


class MRUEntry:
    "A traffic entry for an MRU list."

    def __init__(self):
        self.addr = None        # text of IPv4 or IPv6 address and port
        self.last = None        # timestamp of last receipt
        self.first = None       # timestamp of first receipt
        self.ct = 0             # count of packets received
        self.mv = None          # mode and version
        self.rs = None          # restriction mask (RES_* bits)

    def avgint(self):
        last = ntp.ntpc.lfptofloat(self.last)
        first = ntp.ntpc.lfptofloat(self.first)
        return (last - first) / self.ct

    def sortaddr(self):
        addr = self.addr
        if addr[0] == '[':
            # IPv6   [n:n:n::n:n]:sock
            # or     [n:n:n::n:n%x]:sock
            addr = addr[1:addr.find(']')]
            pct = addr.find('%')
            if pct > 0:
                # <addr>%<n> for local IPv6 address on interface n
                addr = addr[:pct]
            return socket.inet_pton(socket.AF_INET6, addr)
        else:
            # IPv4   a.b.c.d:sock
            addr = addr[:addr.find(':')]
            # prefix with 0s so IPv6 sorts after IPv4
            # Need 16 rather than 12 to catch ::1
            return b'\0'*16 + socket.inet_pton(socket.AF_INET, addr)

    def __repr__(self):
        return "<MRUentry: " + repr(self.__dict__)[1:-1] + ">"


class MRUList:
    "A sequence of address-timespan pairs returned by ntpd in one response."

    def __init__(self):
        self.entries = []       # A list of MRUEntry objects
        self.now = None         # server timestamp marking end of operation

    def is_complete(self):
        "Is the server done shipping entries for this span?"
        return self.now is not None

    def __repr__(self):
        return "<MRUList: entries=%s now=%s>" % (self.entries, self.now)


class ControlException(BaseException):

    def __init__(self, message, errorcode=0):
        self.message = message
        self.errorcode = errorcode

    def __str__(self):
        return self.message


class ControlSession:
    "A session to a host"
    MRU_ROW_LIMIT = 256
    server_errors = {
        ntp.control.CERR_UNSPEC: "UNSPEC",
        ntp.control.CERR_PERMISSION: "PERMISSION",
        ntp.control.CERR_BADFMT: "BADFMT",
        ntp.control.CERR_BADOP: "BADOP",
        ntp.control.CERR_BADASSOC: "BADASSOC",
        ntp.control.CERR_UNKNOWNVAR: "UNKNOWNVAR",
        ntp.control.CERR_BADVALUE: "BADVALUE",
        ntp.control.CERR_RESTRICT: "RESTRICT",
        }

    def __init__(self):
        self.debug = 0
        self.ai_family = socket.AF_UNSPEC
        self.primary_timeout = DEFTIMEOUT       # Timeout for first select
        self.secondary_timeout = DEFSTIMEOUT    # Timeout for later selects
        # Packet version number we use
        self.pktversion = ntp.magic.NTP_OLDVERSION + 1
        self.always_auth = False  # Always send authenticated requests
        self.keytype = "MD5"
        self.keyid = None
        self.passwd = None
        self.auth = None
        self.hostname = None
        self.isnum = False
        self.sock = None
        self.port = 0
        self.sequence = 0
        self.response = ""
        self.rstatus = 0
        self.ntpd_row_limit = ControlSession.MRU_ROW_LIMIT
        self.logfp = sys.stdout
        self.nonce_xmit = 0

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def havehost(self):
        "Is the session connected to a host?"
        return self.sock is not None

    def __lookuphost(self, hname, fam):
        "Try different ways to interpret an address and family"
        if hname.startswith("["):
            hname = hname[1:-1]
        # First try to resolve it as an ip address and if that fails,
        # do a fullblown (DNS) lookup. That way we only use the dns
        # when it is needed and work around some implementations that
        # will return an "IPv4-mapped IPv6 address" address if you
        # give it an IPv4 address to lookup.

        def hinted_lookup(port, hints):
            return socket.getaddrinfo(hname, port, self.ai_family,
                                      socket.SOCK_DGRAM,
                                      socket.IPPROTO_UDP,
                                      hints)
        try:
            return hinted_lookup(port="ntp", hints=socket.AI_NUMERICHOST)
        except socket.gaierror as e:
            ntp.util.dolog(self.logfp,
                           "ntpq: numeric-mode lookup of %s failed, %s\n"
                           % (hname, e.strerror), self.debug, 3)
        try:
            return hinted_lookup(port="ntp", hints=0)
        except socket.gaierror as e1:
            self.logfp.write("ntpq: standard-mode lookup of %s failed, %s\n"
                             % (hname, e1.strerror))
            # EAI_NODATA and AI_CANONNAME should both exist - they're in the
            # POSIX API.  If this code throws AttributeErrors there is
            # probably a very old and broken socket layer in your Python
            # build.  The C implementation had a second fallback mode that
            # removed AI_ADDRCONFIG if the first fallback raised BADFLAGS.
            fallback_hints = socket.AI_CANONNAME
            try:
                fallback_hints |= socket.AI_ADDRCONFIG
            except AttributeError:
                pass
            try:
                if e1.errno in (socket.EAI_NONAME, socket.EAI_NODATA):
                    try:
                        return hinted_lookup(port="ntp", hints=0)
                    except socket.gaierror as e2:
                        self.logfp.write("ntpq: ndp lookup failed, %s\n"
                                         % e2.strerror)
            except AttributeError:
                self.logfp.write(
                    "ntpq: API error, missing socket attributes\n")
        return None

    def openhost(self, hname, fam=socket.AF_UNSPEC):
        "openhost - open a socket to a host"
        res = self.__lookuphost(hname, fam)
        if res is None:
            return False
        # C implementation didn't use multiple responses, so we don't either
        (family, socktype, protocol, canonname, sockaddr) = res[0]
        if canonname is None:
            self.hostname = sockaddr.inet_ntop(sockaddr[0], family)
            self.isnum = True
        else:
            self.hostname = canonname or hname
            self.isnum = False
        ntp.util.dolog(self.logfp, "Opening host %s\n" % self.hostname,
                       self.debug, 3)
        self.port = sockaddr[1]
        try:
            self.sock = socket.socket(family, socktype, protocol)
        except socket.error as e:
            raise ControlException("Error opening %s: %s [%d]"
                                   % (hname, e.strerror, e.errno))
        try:
            self.sock.connect(sockaddr)
        except socket.error as e:
            raise ControlException("Error connecting to %s: %s [%d]"
                                   % (hname, e.strerror, e.errno))
        return True

    def password(self):
        "Get a keyid and the password if we don't have one."
        if self.keyid is None:
            if self.auth is None:
                try:
                    self.auth = Authenticator()
                except (OSError, IOError):
                    pass
            if self.auth and self.hostname == "localhost":
                try:
                    (self.keyid, self.keytype, self.passwd) \
                        = self.auth.control()
                    return
                except ValueError:
                    # There are no trusted keys.  Barf.
                    raise ControlException(SERR_NOTRUST)
            try:
                key_id = int(input("Keyid: "))
                # FIXME: Magic number, yuck
                if key_id == 0 or key_id > 65535:
                    raise ControlException(SERR_BADKEY)
            except (SyntaxError, ValueError):
                raise ControlException(SERR_BADKEY)
            self.keyid = key_id

        if self.passwd is None:
            try:
                passwd = self.auth[self.keyid][1]
            except (IndexError, TypeError):
                passwd = getpass.getpass("%s Password: " % self.keytype)
                if passwd is None:
                    raise ControlException(SERR_INVPASS)
            self.passwd = passwd

    def sendpkt(self, xdata):
        "Send a packet to the host."
        while len(xdata) % 4:
            xdata += b"\x00"
        ntp.util.dolog(self.logfp,
                       "Sending %d octets.  seq=%d\n"
                       % (len(xdata), self.sequence), self.debug, 3)
        try:
            self.sock.sendall(polybytes(xdata))
        except socket.error:
            # On failure, we don't know how much data was actually received
            self.logfp.write("Write to %s failed\n" % self.hostname)
            return -1
        if self.debug >= 5:  # special, not replacing with dolog()
            self.logfp.write("Request packet:\n")
            dump_hex_printable(xdata, self.logfp)
        return 0

    def sendrequest(self, opcode, associd, qdata, auth=False):
        "Ship an ntpq request packet to a server."
        if self.debug:  # special, not replacing with dolog()
            if self.debug >= 3:
                self.logfp.write("\n")   # extra space to help find clumps
            self.logfp.write("sendrequest: opcode=%d, associd=%d, qdata=%s\n"
                             % (opcode, associd, qdata))

        # Check to make sure the data will fit in one packet
        if len(qdata) > ntp.control.CTL_MAX_DATA_LEN:
            self.logfp.write("***Internal error! Data too large (%d)\n" %
                             len(qdata))
            return -1

        # Assemble the packet
        pkt = ControlPacket(self, opcode, associd, qdata)

        self.sequence += 1
        self.sequence %= 65536  # Has to fit in a struct H field
        pkt.sequence = self.sequence

        # If we have data, pad it out to a 32-bit boundary.
        # Do not include these in the payload count.
        if pkt.extension:
            while ((ControlPacket.HEADER_LEN + len(pkt.extension)) & 3):
                pkt.extension += b"\x00"

        # If it isn't authenticated we can just send it.  Otherwise
        # we're going to have to think about it a little.
        if not auth and not self.always_auth:
            return pkt.send()

        if self.keyid is None or self.passwd is None:
            raise ControlException(SERR_NOCRED)

        # Pad out packet to a multiple of 8 octets to be sure
        # receiver can handle it. Note: these pad bytes should
        # *not* be counted in the header count field.
        while ((ControlPacket.HEADER_LEN + len(pkt.extension)) & 7):
            pkt.extension += b"\x00"

        # Do the MAC compuation.
        mac = Authenticator.compute_mac(pkt.flatten(),
                                        self.keyid, self.keytype, self.passwd)
        if mac is None:
            raise ControlException(SERR_NOKEY)
        else:
            pkt.extension += mac
        return pkt.send()

    def getresponse(self, opcode, associd, timeo):
        "Get a response expected to match a given opcode and associd."
        # This is pretty tricky.  We may get between 1 and MAXFRAG packets
        # back in response to the request.  We peel the data out of
        # each packet and collect it in one long block.  When the last
        # packet in the sequence is received we'll know how much data we
        # should have had.  Note we use one long time out, should reconsider.
        fragments = []
        self.response = ''
        seenlastfrag = False
        bail = 0
        warn = self.logfp.write
        warndbg = (lambda txt, th: ntp.util.dolog(self.logfp, txt,
                                                  self.debug, th))

        warndbg("Fragment collection begins\n", 1)
        # Loop until we have an error or a complete response.  Nearly all
        # code paths to loop again use continue.
        while True:
            # Discarding various invalid packets can cause us to
            #  loop more than MAXFRAGS times, but enforce a sane bound
            #  on how long we're willing to spend here.
            bail += 1
            if bail >= (2*MAXFRAGS):
                raise ControlException(SERR_TOOMUCH)

            if len(fragments) == 0:
                tvo = self.primary_timeout / 1000
            else:
                tvo = self.secondary_timeout / 1000

            warndbg("At %s, select with timeout %d begins\n"
                    % (time.asctime(), tvo), 5)
            try:
                (rd, _, _) = select.select([self.sock], [], [], tvo)
            except select.error:
                raise ControlException(SERR_SELECT)
            warndbg("At %s, select with timeout %d ends\n"
                    % (time.asctime(), tvo), 5)

            if not rd:
                # Timed out.  Return what we have
                if len(fragments) == 0:
                    if timeo:
                        raise ControlException(SERR_TIMEOUT)
                if timeo:
                    if self.debug:  # special, not replacing with dolog()
                        self.logfp.write(
                            "ERR_INCOMPLETE: Received fragments:\n")
                        for (i, frag) in enumerate(fragments):
                            self.logfp.write("%d: %s" % (i+1, frag.stats(i)))
                        self.logfp.write("last fragment %sreceived\n"
                                         % ("not ", "")[seenlastfrag])
                raise ControlException(SERR_INCOMPLETE)

            warndbg("At %s, socket read begins\n" % time.asctime(), 4)
            rawdata = polybytes(self.sock.recv(4096))
            warndbg("Received %d octets\n" % len(rawdata), 3)
            rpkt = ControlPacket(self)
            try:
                rpkt.analyze(rawdata)
            except struct.error:
                raise ControlException(SERR_UNSPEC)

            if ((rpkt.version() > ntp.magic.NTP_VERSION or
                    rpkt.version() < ntp.magic.NTP_OLDVERSION)):
                warndbg("Fragment received with version %d\n"
                        % rpkt.version(), 1)
                continue
            if rpkt.mode() != ntp.magic.MODE_CONTROL:
                warndbg("Fragment received with mode %d\n" % rpkt.mode(), 1)
                continue
            if not rpkt.is_response():
                warndbg("Received request, wanted response\n", 1)
                # continue

            # Check opcode and sequence number for a match.
            # Could be old data getting to us.
            # =======
            # These had the continues inside a if debug block. Probably
            # shouldn't have been there, but if there is a problem move
            # them back.
            if rpkt.sequence != self.sequence:
                warndbg("Received sequence number %d, wanted %d\n" %
                        (rpkt.sequence, self.sequence), 1)
                continue
            if rpkt.opcode() != opcode:
                warndbg("Received opcode %d, wanted %d\n" %
                        (rpkt.opcode(), opcode), 1)
                continue

            # Check the error code.  If non-zero, return it.
            if rpkt.is_error():
                if rpkt.more():
                    warn("Error %d received on non-final fragment\n"
                         % rpkt.errcode())
                self.keyid = self.passwd = None
                raise ControlException(
                    SERR_SERVER
                    % ControlSession.server_errors[rpkt.errcode()],
                    rpkt.errcode())

            # Check the association ID to make sure it matches what we expect
            if rpkt.associd != associd:
                warn("Association ID %d doesn't match expected %d\n"
                     % (rpkt.associd, associd))

            # validate received payload size is padded to next 32-bit
            # boundary and no smaller than claimed by rpkt.count
            if len(rawdata) & 0x3:
                warn("Response fragment not padded, size = %d\n"
                     % len(rawdata))
                continue

            shouldbesize = (ControlPacket.HEADER_LEN + rpkt.count + 3) & ~3
            if len(rawdata) < shouldbesize:
                warn("Response fragment claims %u octets payload, "
                     "above %d received\n"
                     % (rpkt.count, len(rawdata) - ControlPacket.HEADER_LEN))
                raise ControlException(SERR_INCOMPLETE)

            if rpkt.count > (len(rawdata) - ControlPacket.HEADER_LEN):
                    warn("Received count of %u octets, data in "
                         " fragment is %ld\n"
                         % (rpkt.count,
                            len(rawdata) - ControlPacket.HEADER_LEN))
                    continue

            # Someday, perhaps, check authentication here

            # Clip off the MAC, if any
            rpkt.data = rpkt.data[:rpkt.count]

            if rpkt.count == 0 and rpkt.more():
                warn("Received count of 0 in non-final fragment\n")
                continue

            if seenlastfrag and rpkt.more():
                warn("Received second last fragment\n")
                continue

            # Find the most recent fragment with a
            not_earlier = [frag for frag in fragments
                           if frag.offset >= rpkt.offset]
            if len(not_earlier):
                not_earlier = not_earlier[0]
                if not_earlier.offset == rpkt.offset:
                    warn("duplicate %d octets at %d ignored, prior "
                         " %d at %d\n"
                         % (rpkt.count, rpkt.offset,
                            not_earlier.count, not_earlier.offset))
                    continue

            if len(fragments) > 0:
                last = fragments[-1]
                if last.end() > rpkt.offset:
                    warn("received frag at %d overlaps with %d octet "
                         "frag at %d\n"
                         % (rpkt.offset, last.count, last.offset))
                    continue

            if not_earlier and rpkt.end() > not_earlier.offset:
                warn("received %d octet frag at %d overlaps with "
                     "frag at %d\n"
                     % (rpkt.count, rpkt.offset, not_earlier.offset))
                continue

            warndbg("Recording fragment %d, size = %d offset = %d, "
                    " end = %d, more=%s\n"
                    % (len(fragments)+1, rpkt.count,
                       rpkt.offset, rpkt.end(), rpkt.more()), 3)

            # Passed all tests, insert it into the frag list.
            fragments.append(rpkt)
            fragments.sort(key=lambda frag: frag.offset)

            # Figure out if this was the last.
            # Record status info out of the last packet.
            if not rpkt.more():
                seenlastfrag = True
                self.rstatus = rpkt.status

            # If we've seen the last fragment, look for holes in the sequence.
            # If there aren't any, we're done.
            if seenlastfrag and fragments[0].offset == 0:
                for f in range(1, len(fragments)):
                    if fragments[f-1].end() != fragments[f].offset:
                        warndbg("Hole in fragment sequence, %d of %d\n"
                                % (f, len(fragments)), 1)
                        break
                else:
                    self.response = polybytes(
                        "".join([polystr(frag.data) for frag in fragments]))
                    warndbg("Fragment collection ends. %d bytes "
                            " in %d fragments\n"
                            % (len(self.response), len(fragments)), 1)
                    if self.debug >= 5:  # special, not replacing with dolog()
                        warn("Response packet:\n")
                        dump_hex_printable(self.response, self.logfp)
                    elif self.debug >= 3:
                        # FIXME: Garbage when retrieving assoc list (binary)
                        warn("Response packet:\n%s\n" % self.response)
                    elif self.debug >= 2:
                        # FIXME: Garbage when retrieving assoc list (binary)
                        eol = self.response.find("\n")
                        firstline = self.response[:eol]
                        warn("First line:\n%s\n" % firstline)
                    return None
                break

    def doquery(self, opcode, associd=0, qdata="", auth=False):
        "send a request and save the response"
        if not self.havehost():
            raise ControlException(SERR_NOHOST)
        retry = True
        while True:
            # Ship the request
            self.sendrequest(opcode, associd, qdata, auth)
            # Get the response.
            try:
                res = self.getresponse(opcode, associd, not retry)
            except ControlException as e:
                if retry and e.message in (SERR_TIMEOUT, SERR_INCOMPLETE):
                    retry = False
                    continue
                else:
                    raise e
            break
        # Return data on success
        return res

    def readstat(self, associd=0):
        "Read peer status, or throw an exception."
        self.doquery(opcode=ntp.control.CTL_OP_READSTAT, associd=associd)
        if len(self.response) % 4:
            raise ControlException(SERR_BADLENGTH)
        idlist = []
        if associd == 0:
            for i in range(len(self.response)//4):
                data = self.response[4*i:4*i+4]
                (associd, status) = struct.unpack("!HH", data)
                idlist.append(Peer(self, associd, status))
        idlist.sort(key=lambda a: a.associd)
        return idlist

    def __parse_varlist(self, raw=False):
        "Parse a response as a textual varlist."
        # Strip out NULs and binary garbage from text;
        # ntpd seems prone to generate these, especially
        # in reslist responses.
        instring = False
        response = ""
        for c in self.response:
            if c == '"':
                response = response + c
                instring = not instring
            elif instring and c == ',':
                response = response + "\xae"
            elif polyord(c) > 0 and polyord(c) < 127:
                response = response + polychr(c)
        response = response.rstrip()
        items = []
        if response:
            for pair in response.split(","):
                try:
                    pair = pair.strip().replace("\xae", ",")
                    eq = pair.index("=")
                    var = pair[:eq].strip()
                    val = pair[eq+1:].strip()
                    casted = False
                    try:
                        if raw is True:
                            val = (int(val, 0), val)
                        else:
                            val = int(val, 0)
                        casted = True
                    except ValueError:
                        try:
                            if raw is True:
                                valf = (float(val), val)
                            else:
                                valf = float(val)
                            if (var == "delay") and (raw is False):
                                # raw==false, if true str returned anyway
                                # hack to pass string version
                                # so printout can handle .3f vs .6f
                                items.append(("delay-s", val))
                            val = valf
                            casted = True
                        except ValueError:
                            if val[0] == '"' and val[-1] == '"':
                                val = val[1:-1]
                                if raw is True:
                                    val = (val, val)
                                casted = True
                    if (raw is True) and (casted is False):
                        val = (val, val)
                    items.append((var, val))
                except ValueError:
                    # Yes, ntpd really does emit bare tags for empty
                    # string-valued variables.
                    items.append((pair, ""))
        return ntp.util.OrderedDict(items)

    def readvar(self, associd=0, varlist=None,
                opcode=ntp.control.CTL_OP_READVAR, raw=False):
        "Read system vars from the host as a dict, or throw an exception."
        if varlist is None:
            qdata = ""
        else:
            qdata = ",".join(varlist)
        self.doquery(opcode, associd=associd, qdata=qdata)
        return self.__parse_varlist(raw)

    def config(self, configtext):
        "Send configuration text to the daemon. Return True if accepted."
        self.doquery(opcode=ntp.control.CTL_OP_CONFIGURE,
                     qdata=configtext, auth=True)
        # Copes with an implementation error - ntpd uses putdata without
        # setting the size correctly.
        if not self.response:
            raise ControlException(SERR_PERMISSION)
        elif b"\x00" in self.response:
            self.response = self.response[:self.response.index(b"\x00")]
        self.response = self.response.rstrip()
        return self.response == "Config Succeeded"

    def fetch_nonce(self):
        """
Receive a nonce that can be replayed - combats source address spoofing
"""
        self.doquery(opcode=ntp.control.CTL_OP_REQ_NONCE)
        self.nonce_xmit = time.time()
        if not self.response.startswith(polybytes("nonce=")):
            print("## Nonce expected: %s" % self.response)
            raise ControlException(SERR_BADNONCE)
        return polystr(self.response.strip())

    def mrulist(self, variables=None, rawhook=None, direct=None):
        "Retrieve MRU list data"
        restarted_count = 0
        cap_frags = True
        warndbg = (lambda txt, th: ntp.util.dolog(self.logfp, txt,
                                                  self.debug, th))
        sorter = None
        frags = MAXFRAGS
        self.slots = 0
        if variables is None:
            variables = {}

        if variables:
            if "sort" in variables:
                sortkey = variables["sort"]
                del variables["sort"]
                # Slots are retrieved oldest first.
                # Slots are printed in reverse so the normal/no-sort
                #  case prints youngest first.
                # That means sort functions are backwards.
                # Note lstint is backwards again (aka normal/forward)
                #  since we really want to sort on now-last rather than last.
                sortdict = {
                    # lstint ascending
                    "lstint": lambda e: ntp.ntpc.lfptofloat(e.last),
                    # lstint descending
                    "-lstint": lambda e: -ntp.ntpc.lfptofloat(e.last),
                    # avgint ascending
                    "avgint": lambda e: -e.avgint(),
                    # avgint descending
                    "-avgint": lambda e: e.avgint(),
                    # IPv4 asc. then IPv6 asc.
                    "addr": lambda e: e.sortaddr(),
                    # IPv6 desc. then IPv4 desc.
                    "-addr": lambda e: e.sortaddr(),
                    # hit count ascending
                    "count": lambda e: -e.ct,
                    # hit count descending
                    "-count": lambda e: e.ct,
                }
                if sortkey == "lstint":
                    sortkey = None   # normal/default case, no need to sort
                if sortkey is not None:
                    sorter = sortdict.get(sortkey)
                    if sorter is None:
                        raise ControlException(SERR_BADSORT % sortkey)
            for k in list(variables.keys()):
                if k in ("mincount", "resall", "resany", "kod", "limited",
                         "maxlstint", "laddr", "recent", "sort",
                         "frags", "limit"):
                    continue
                else:
                    raise ControlException(SERR_BADPARAM % k)
            if 'frags' in variables:
                frags = int(variables.get('frags'))
                del variables['frags']
            if 'kod' in variables:
                variables['resany'] = variables.get('resany', 0) \
                    | ntp.magic.RES_KOD
                del variables['kod']
            if 'limited' in variables:
                variables['resany'] = variables.get('resany', 0) \
                    | ntp.magic.RES_LIMITED
                del variables['limited']

        nonce = self.fetch_nonce()

        span = MRUList()
        try:
            # Form the initial request
            limit = min(3 * MAXFRAGS, self.ntpd_row_limit)
            req_buf = "%s, frags=%d" % (nonce, frags)
            if variables:
                if 'resall' in variables:
                    variables['resall'] = hex(variables['resall'])
                if 'resany' in variables:
                    variables['resany'] = hex(variables['resany'])
                parms = ", " + ",".join([("%s=%s" % it)
                                         for it in list(variables.items())])
            else:
                parms = ""
            req_buf += parms
            first_time_only = "recent=%s" % variables.get("recent")

            while True:
                # Request additions to the MRU list
                try:
                    self.doquery(opcode=ntp.control.CTL_OP_READ_MRU,
                                 qdata=req_buf)
                    recoverable_read_errors = False
                except ControlException as e:
                    recoverable_read_errors = True
                    if e.errorcode is None:
                        raise e
                    elif e.errorcode == ntp.control.CERR_UNKNOWNVAR:
                        # None of the supplied prior entries match, so
                        # toss them from our list and try again.
                        warndbg("no overlap between prior entries and "
                                "server MRU list\n", 1)
                        restarted_count += 1
                        if restarted_count > 8:
                            raise ControlException(SERR_STALL)
                        warndbg("--->   Restarting from the beginning, "
                                "retry #%u\n" % restarted_count, 1)
                    elif e.errorcode == ntp.control.CERR_UNKNOWNVAR:
                        e.message = ("CERR_UNKNOWNVAR from ntpd but "
                                     "no priors given.")
                        raise e
                    elif e.errorcode == ntp.control.CERR_BADVALUE:
                        if cap_frags:
                            cap_frags = False
                            warndbg("Reverted to row limit from "
                                    "fragments limit.\n", 1)
                        else:
                            # ntpd has lower cap on row limit
                            self.ntpd_row_limit -= 1
                            limit = min(limit, self.ntpd_row_limit)
                            warndbg("Row limit reduced to %d following "
                                    "CERR_BADVALUE.\n" % limit, 1)
                    elif e.errorcode in (SERR_INCOMPLETE, SERR_TIMEOUT):
                        # Reduce the number of rows/frags requested by
                        # half to recover from lost response fragments.
                        if cap_frags:
                            frags = max(2, frags / 2)
                            warndbg("Frag limit reduced to %d following "
                                    "incomplete response.\n" % frags, 1)
                        else:
                            limit = max(2, limit / 2)
                            warndbg("Row limit reduced to %d following "
                                    " incomplete response.\n" % limit, 1)
                    elif e.errorcode:
                        raise e

                # Parse the response
                variables = self.__parse_varlist()

                # Comment from the C code:
                # This is a cheap cop-out implementation of rawmode
                # output for mrulist.  A better approach would be to
                # dump similar output after the list is collected by
                # ntpq with a continuous sequence of indexes.  This
                # cheap approach has indexes resetting to zero for
                # each query/response, and duplicates are not
                # coalesced.
                if rawhook:
                    rawhook(variables)

                # Analyze the contents of this response into a span structure
                curidx = -1
                mru = None
                for (tag, val) in variables.items():
                    warndbg("tag=%s, val=%s\n" % (tag, val), 4)
                    if tag == "nonce":
                        nonce = "%s=%s" % (tag, val)
                    elif tag == "last.older":
                        continue
                    elif tag == "addr.older":
                        continue
                    if tag == "now":
                        # finished marker
                        span.now = ntp.ntpc.lfptofloat(val)
                        continue
                    elif tag == "last.newest":
                        # more finished
                        continue
                    for prefix in ("addr", "last", "first", "ct", "mv", "rs"):
                        if tag.startswith(prefix + "."):
                            (member, idx) = tag.split(".")
                            try:
                                idx = int(idx)
                            except ValueError:
                                raise ControlException(SERR_BADTAG % tag)
                            if idx != curidx:
                                # This makes duplicates
                                curidx = idx
                                if mru:
                                    # Can't have partial slots on list
                                    # or printing crashes after ^C
                                    # Append full slot now
                                    span.entries.append(mru)
                                mru = MRUEntry()
                                self.slots += 1
                            setattr(mru, prefix, val)
                if mru:
                    span.entries.append(mru)
                if direct is not None:
                    direct(span.entries)

                # If we've seen the end sentinel on the span, break out
                if span.is_complete():
                    break

                # The C version of ntpq used to snooze for a bit
                # between MRU queries to let ntpd catch up with other
                # duties.  It turns out this is quite a bad idea.  Above
                # a certain traffic threshold, servers accumulate MRU records
                # faster than this protocol loop can capture them such that
                # you never get a complete span.  The last thing you want to
                # do when trying to keep up with a high-traffic server is stall
                # in the read loop.
                # time.sleep(0.05)

                # If there were no errors, increase the number of rows
                # to a maximum of 3 * MAXFRAGS (the most packets ntpq
                # can handle in one response), on the assumption that
                # no less than 3 rows fit in each packet, capped at
                # our best guess at the server's row limit.
                if not recoverable_read_errors:
                    if cap_frags:
                        frags = min(MAXFRAGS, frags + 1)
                    else:
                        limit = min(3 * MAXFRAGS,
                                    self.ntpd_row_limit,
                                    max(limit + 1,
                                        limit * 33 / 32))

                # Only ship 'recent' on the first request
                parms = parms.replace(first_time_only, "")

                # Prepare next query with as many address and last-seen
                # timestamps as will fit in a single packet.  A new nonce
                # might be required.
                if time.time() - self.nonce_xmit >= ntp.control.NONCE_TIMEOUT:
                    nonce = self.fetch_nonce()
                req_buf = "%s, %s=%d%s" % \
                          (nonce,
                           "frags" if cap_frags else "limit",
                           frags if cap_frags else limit,
                           parms)
                for i in range(len(span.entries)):
                    e = span.entries[len(span.entries) - i - 1]
                    incr = ", addr.%d=%s, last.%d=%s" % (i, e.addr, i, e.last)
                    if ((len(req_buf) + len(incr) >=
                            ntp.control.CTL_MAX_DATA_LEN)):
                        break
                    else:
                        req_buf += incr
                if direct is not None:
                    span.entries = []
        except KeyboardInterrupt:
            pass        # We can test for interruption with is_complete()

        # C ntpq's code for stitching together spans was absurdly
        # overelaborate - all that dancing with last.older and
        # addr.older was, as far as I can tell, just pointless.
        # Much simpler to just run through the final list throwing
        # out every entry with an IP address that is duplicated
        # with a later most-recent-transmission time.
        addrdict = {}
        deletia = []
        for (i, entry) in enumerate(span.entries):
            if entry.addr not in addrdict:
                addrdict[entry.addr] = []
            addrdict[entry.addr].append((i, entry.last))
        for addr in addrdict:
            deletia += sorted(addrdict[addr], key=lambda x: x[1])[:-1]
        deletia = [x[0] for x in deletia]
        deletia.sort(reverse=True)
        for i in deletia:
            span.entries.pop(i)

        # Sort for presentation
        if sorter:
            span.entries.sort(key=sorter)
            if sortkey == "addr":
                # I don't know how to feed a minus sign to text sort
                span.entries.reverse()

        return span

    def __ordlist(self, listtype):
        "Retrieve ordered-list data."
        self.doquery(opcode=ntp.control.CTL_OP_READ_ORDLIST_A,
                     qdata=listtype, auth=True)
        stanzas = []
        for (key, value) in self.__parse_varlist().items():
            if key[-1].isdigit() and '.' in key:
                (stem, stanza) = key.split(".")
                stanza = int(stanza)
                if stanza > len(stanzas) - 1:
                    for i in range(len(stanzas), stanza + 1):
                        stanzas.append(ntp.util.OrderedDict())
                stanzas[stanza][stem] = value
        return stanzas

    def reslist(self):
        "Retrieve reslist data."
        return self.__ordlist("addr_restrictions")

    def ifstats(self):
        "Retrieve ifstats data."
        return self.__ordlist("ifstats")


class Authenticator:
    "MAC authentication manager for NTP packets."

    def __init__(self, keyfile=None):
        # We allow I/O and permission errors upward deliberately
        self.passwords = {}
        if keyfile is not None:
            for line in open(keyfile):
                if '#' in line:
                    line = line[:line.index("#")]
                line = line.strip()
                if not line:
                    continue
                (keyid, keytype, passwd) = line.split()
                self.passwords[int(keyid)] = (keytype, passwd)

    def __len__(self):
        return len(self.passwords)

    def __getitem__(self, keyid):
        return self.passwords.get(keyid)

    def control(self, keyid=None):
        "Get a keyid/passwd pair that is trusted on localhost"
        if keyid is not None:
            if keyid in self.passwords:
                return (keyid,) + self.passwords[keyid]
            else:
                return (keyid, None, None)
        for line in open("/etc/ntp.conf"):
            if line.startswith("control"):
                keyid = int(line.split()[1])
                (keytype, passwd) = self.passwords[keyid]
                if passwd is None:
                    raise ValueError
                return (keyid, keytype, passwd)
        else:
            raise ValueError

    @staticmethod
    def compute_mac(payload, keyid, keytype, passwd):
        hasher = hashlib.new(keytype)
        hasher.update(polybytes(passwd))
        hasher.update(payload)
        if hasher.digest_size == 0:
            return None
        else:
            return struct.pack("!I", keyid) + hasher.digest()

    @staticmethod
    def have_mac(packet):
        "Does this packet have a MAC?"
        # According to RFC 5909 7.5 the MAC is always present when an extension
        # field is present. Note: this crude test will fail on Mode 6 packets.
        # On those you have to go in and look at the count.
        return len(packet) > ntp.magic.LEN_PKT_NOMAC

    def verify_mac(self, packet):
        "Does the MAC on this packet verify according to credentials we have?"
        # FIXME: Someday, figure out how to handle SHA1?
        HASHLEN = 16    # Length of MD5 hash.
        payload = packet[:-HASHLEN-4]
        keyid = packet[-HASHLEN-4:-HASHLEN]
        mac = packet[-HASHLEN:]
        (keyid,) = struct.unpack("!I", keyid)
        if keyid not in self.passwords:
            return False
        (keytype, passwd) = self.passwords[keyid]
        hasher = hashlib.new(keytype)
        hasher.update(passwd)
        hasher.update(payload)
        return hasher.digest() == mac

# end
