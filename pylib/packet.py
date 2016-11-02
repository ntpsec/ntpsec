#
# packet.py - definitions and classes for Python querying of NTP
#
# Freely translated from the old C ntpq code by ESR, with comments
# preserved.  The idea was to cleanly separate ntpq-that-was into a
# thin front-end layer handling mainly command interpretation and a
# back-end that presents the take from ntpd as objects that can be
# re-used by other front ends. Other reusable pieces live in util.py.
#
# This code should be Python2-vs-Python-3 agnostic.  Keep it that way!
#
# SPDX-License-Identifier: BSD-2-clause
from __future__ import print_function, division
import sys, socket, select, struct, curses.ascii, collections
import getpass, hashlib, time
from ntpc import lfptofloat

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
        # line_buffering=True ensures that interactive command sessions work as expected
        return io.TextIOWrapper(stream.buffer, encoding=master_encoding, newline="\n", line_buffering=True)

from ntp_magic import *
from ntp_control import *

# From ntpq.h:

# Limit on packets in a single response.  Increasing this value to
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
MAXFRAGS        = 32

MRU_REPORT_SECS	= 5

class Packet:
    "Encapsulate an NTP fragment"
    # The following two methods are copied from macros in includes/control.h
    @staticmethod
    def VN_MODE(v, m):          return ((((v) & 7) << 3) | ((m) & 0x7))

    @staticmethod
    def PKT_LI_VN_MODE(l, v, m):  return ((((l) & 3) << 6) | Packet.VN_MODE((v), (m)))

    def __init__(self, session, version, mode):
        self.session = session  # Where to get session context
        self.li_vn_mode = 0     # leap, version, mode (uint8_t)
        self.r_e_m_op = 0       # response, error, more, opcode (uint8_t)
        # Subclasses have four uint16_t fields here
        self.count = 0          # octet count of extension data
        self.extension = ''     # extension data
        self.li_vn_mode = Packet.PKT_LI_VN_MODE(0, version, mode)
    format = "!BBHHHHH"
    HEADER_LEN = 12

    def flatten(self, payload1, payload2, payload3, payload4):
        "Flatten the packet into an octet sequence."
        body = struct.pack(Packet.format,
                             self.li_vn_mode,
                             self.r_e_m_op,
                             payload1, payload2, payload3, payload4,
                             self.count)
        return polybytes(body + self.extension)

    def analyze(self, rawdata):
        (self.li_vn_mode,
         self.r_e_m_op,
         payload1, payload2, payload3, payload4,
         self.count) = struct.unpack(Packet.format, rawdata[:Packet.HEADER_LEN])
        self.data = rawdata[Packet.HEADER_LEN:]
        return (payload1, payload2, payload3, payload4)

    def mode(self):
        return self.li_vn_mode & 0x7

    def end(self):
        return self.count + self.offset

    def version(self):
        return (self.li_vn_mode >> 3) & 0x7

class Mode6Packet(Packet):
    "Mode 6 request/response "

    def __init__(self, session, opcode=0, associd=0, qdata=''):
        Packet.__init__(self, session, session.pktversion, MODE_CONTROL)
        self.r_e_m_op = opcode  # ntpq operation code
        self.sequence = 1       # sequence number of request (uint16_t)
        self.status = 0         # status word for association (uint16_t)
        self.associd = associd  # association ID (uint16_t)
        self.offset = 0         # offset of this batch of data (uint16_t)
        self.extension = qdata  # Data for this packet
        self.count = len(qdata)	# length of data

    def is_response(self):
        return self.r_e_m_op & 0x80

    def is_error(self):
        return self.r_e_m_op & 0x40

    def more(self):
        return self.r_e_m_op & 0x20

    def opcode(self):
        return self.r_e_m_op & 0x1F

    def errcode(self):
        return (self.status >> 8) & 0xff

    def stats(self, idx):
        "Return statistics on a fragment."
        return "%5d %5d\t%3d octets\n" % (self.offset, self.end(), self.count)

    def flatten(self):
        return Packet.flatten(self,
                              self.sequence,
                              self.status,
                              self.associd,
                              self.offset)

    def send(self):
        self.session.sequence += 1
        self.sequence = self.session.sequence
        self.session.sendpkt(self.flatten())

    def analyze(self, data):
        (self.sequence,
         self.status,
         self.associd,
         self.offset) = Packet.analyze(self, data)

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
SERR_NOHOST = "***No host open, use `host' command\n"
SERR_BADLENGTH = "***Response length should have been a multiple of 4"
SERR_BADKEY = "***Invalid key identifier"
SERR_INVPASS = "***Invalid password"
SERR_NOKEY = "***Key not found"
SERR_BADNONCE = "***Unexpected nonce response format"
SERR_BADPARM = "***Unknown parameter %s"
SERR_NOCRED = "***No credentials"
SERR_SERVER = "***Server error code"
SERR_STALL = "***No response, probably high-traffic server with low MRU limit"
SERR_BADTAG = "***Bad MRU tag %s"
SERR_BADSORT = "***Sort order %s is not implemented"

def dump_hex_printable(xdata):
    "Dump a packet in hex, in a familiar hex format"
    llen = len(xdata)
    i = 0
    while llen > 0:
        rowlen = min(16, llen)
        restart = i
        for idx in range(16):
            if idx < llen:
                sys.stdout.write("%02x " % ord(xdata[i]))
                i += 1
            else:
                sys.stdout.write("   ")
        i = restart
        for idx in range(rowlen):
            if curses.ascii.isprint(xdata[i]):
                sys.stdout.write(xdata[i])
            else:
                sys.stdout.write('.')
            i += 1
        sys.stdout.write("\n")
        llen -= rowlen

class MRUEntry:
    "A traffic entry for an MRU list."
    def __init__(self):
        self.addr = None	# text of IPv4 or IPv6 address and port
        self.last = None	# timestamp of last receipt
        self.first = None	# timestamp of first receipt
        self.ct = 0		# count of packets received
        self.mv = None		# mode and version
        self.rs = None		# restriction mask (RES_* bits)
    def avgint(self):
        return (e.last - e.first) / e.ct
    def __repr__(self):
        return "<MRUentry: " + repr(self.__dict__)[1:-1] + ">"

class MRUList:
    "A sequence of address-timespan pairs returned by ntpd in one response."
    def __init__(self):
        self.entries = []	# A list of MRUEntry objects
        self.now = None		# server timestamp marking end of operation
    def is_complete(self):
        "Is the server done shipping entries for this span?"
        return self.now is not None
    def __repr__(self):
        return "<MRUList: entries=%s now=%s>" % (self.entries, self.now)

class Mode6Exception(BaseException):
    def __init__(self, message, errorcode=0):
        self.message = message
        self.errorcode = errorcode

class Mode6Session:
    "A session to a host"
    MRU_ROW_LIMIT	= 256

    def __init__(self):
        self.debug = 0
        self.ai_family = socket.AF_UNSPEC
        self.primary_timeout = 5000     # Timeout for first select on receive
        self.secondary_timeout = 3000   # Timeout for later selects
        self.pktversion = NTP_OLDVERSION + 1    # Packet version number we use
        self.always_auth       = False  # Always send authenticated requests
        self.keytype = "MD5"
        self.keyid = None
        self.passwd = None
        self.hostname = None
        self.isnum = False
        self.sock = None
        self.port = 0
        self.sequence = 0
        self.response = ""
        self.rstatus = 0
        self.mruspans = []
        self.ntpd_row_limit = Mode6Session.MRU_ROW_LIMIT

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
        # do a fullblown (dns) lookup. That way we only use the dns
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
            if self.debug > 2:
                sys.stderr.write("ntpq: numeric-mode lookup of %s failed, %s\n" % (hname, e.strerror))
        try:
            return hinted_lookup(port="ntp", hints=0)
        except socket.gaierror as e:
            sys.stderr.write("ntpq: standard-mode lookup of %s failed, %s\n" % (hname, e.strerror))
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
            if e.errno in (socket.EAI_NONAME, socket.EAI_NODATA):
                try:
                    return hinted_lookup(port="ndp", hints=0)
                except socket.gaierror as e:
                    sys.stderr.write("ntpq: ndp lookup failed, %s\n" % e.strerror)
        except AttributeError:
            sys.stderr.write("ntpq: API error, missing socket attributes\n")
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
        if self.debug > 2:
            sys.stdout.write("Opening host %s\n" % self.hostname)
        self.port = sockaddr[1]
        try:
            self.sock = socket.socket(family, socktype, protocol)
        except socket.error as msg:
            sys.stderr.write(msg)
            return False
        try:
            self.sock.connect(sockaddr)
        except socket.error as msg:
            sys.stderr.write(msg)
            return False
        return True

    def password(self):
	"Get a keyid and the password if we don't have one."
        if self.keyid is None:
            try:
                key_id = int(input("Keyid: "))
                # FIXME: Magic number, yuck
                if key_id == 0 or key_id > 65535:
                    raise Mode6Exception(SERR_BADKEY)
            except ValueError:
                raise Mode6Exception(SERR_BADKEY)
            self.keyid = key_id

        if self.passwd is None:
            passwd = getpass.getpass("%s Password: " % self.keytype)
            if passwd is None:
                raise Mode6Exception(SERR_INVPASS)
            self.passwd = passwd

    def sendpkt(self, xdata):
        "Send a packet to the host."
        while len(xdata) % 4:
            xdata += b"\x00"
        if self.debug >= 3:
                print("Sending %d octets\n" % len(xdata))
        try:
            self.sock.sendall(polybytes(xdata))
        except socket.error:
            # On failure, we don't know how much data was actually received
            sys.stderr.write("Write to %s failed\n" % self.hostname)
            return -1
        if self.debug >= 4:
            sys.stdout.write("Request packet:\n")
            dump_hex_printable(xdata)
        return 0

    def sendrequest(self, opcode, associd, qdata, auth=False):
        "Ship an ntpq request packet to a server."
        # Check to make sure the data will fit in one packet
        if len(qdata) > CTL_MAX_DATA_LEN:
            sys.stderr/write(stderr,
                             "***Internal error! Data too long\n",
                             len(qdata))
            return -1

        # Assemble the packet
        pkt = Mode6Packet(self, opcode, associd, qdata)

        # If we have data, pad it out to a 32-bit boundary.
        # Do not include these in the payload count.
        if pkt.extension:
            pkt.extension = polybytes(pkt.extension)
            while ((Packet.HEADER_LEN + len(pkt.extension)) & 3):
                pkt.extension += b"\x00"
            pkt.extension = polystr(pkt.extension)

        # If it isn't authenticated we can just send it.  Otherwise
        # we're going to have to think about it a little.
        if not auth and not self.always_auth:
            return pkt.send()

        if self.keyid is None or self.passwd is None:
            raise Mode6Exception(SERR_NOCRED)

	# Pad out packet to a multiple of 8 octets to be sure
	# receiver can handle it. Note: these pad bytes should
        # *not* be counted in the header count field.
        pkt.extension = polybytes(pkt.extension)
        while ((Packet.HEADER_LEN + len(pkt.extension)) & 7):
            pkt.extension += b"\x00"
        pkt.extension = polystr(pkt.extension)

        # Do the encryption.
        hasher = hashlib.new(self.keytype)
        hasher.update(self.passwd)
	hasher.update(pkt.flatten());
        if hasher.digest_size == 0:
            raise Mode6Exception(SERR_NOKEY)
        else:
            prefix = polystr(struct.pack("!I", self.keyid))
            mac = polystr(hasher.digest())
            pkt.extension += prefix
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
        warn = sys.stderr.write

        # Loop until we have an error or a complete response.  Nearly all
        # code paths to loop again use continue.
        while True:
            # Discarding various invalid packets can cause us to
            #  loop more than MAXFRAGS times, but enforce a sane bound
            #  on how long we're willing to spend here.
            bail += 1
            if bail >= (2*MAXFRAGS):
                raise Mode6Exception(SERR_TOOMUCH)

            if len(fragments) == 0:
                tvo = self.primary_timeout / 1000
            else:
                tvo = self.secondary_timeout / 1000

            try:
                (rd, _, _) = select.select([self.sock], [], [], tvo)
            except select.error as msg:
                raise Mode6Exception(SERR_SELECT)

            if not rd:
                # Timed out.  Return what we have
                if len(fragments) == 0:
                    if timeo:
                        raise Mode6Exception(SERR_TIMEOUT)
                if timeo:
                    if self.debug:
                        sys.stderr.write("ERR_INCOMPLETE: Received fragments:\n")
                        for (i, frag) in enumerate(fragments):
                            sys.stderr.write("%d: %s" % (i+1, frag.stats()))
                        sys.stderr.write("last fragment %sreceived\n",
                                    ("not " "", )[seenlastfrag])
                    raise Mode6Exception(SERR_INCOMPLETE)

            rawdata = polystr(self.sock.recv(4096))
            if self.debug:
                warn("Received %d octets\n" % len(rawdata))
            rpkt = Mode6Packet(self)
            try:
                rpkt.analyze(rawdata)
            except struct.error as reason:
                raise Mode6Exception(SERR_UNSPEC)

            if rpkt.version() > NTP_VERSION or rpkt.version() < NTP_OLDVERSION:
                if self.debug:
                    warn("Packet received with version %d\n" % rpkt.version())
                continue
            if rpkt.mode() != MODE_CONTROL:
                if self.debug:
                    warn("Packet received with mode %d\n" % rpkt.mode())
                continue
            if not rpkt.is_response():
                if self.debug:
                    warn("Received request, wanted response\n")
                #continue

            # Check opcode and sequence number for a match.
            # Could be old data getting to us.
            if rpkt.sequence != self.sequence:
                if self.debug:
                    warn("Received sequence number %d, wanted %d\n" %
                         (rpkt.sequence, self.sequence))
                    continue
            if rpkt.opcode() != opcode:
                if self.debug:
                    warn("Received opcode %d, wanted %d\n" %
                         (rpkt.opcode(), opcode))
                    continue

            # Check the error code.  If non-zero, return it.
            if rpkt.is_error():
                if rpkt.more():
                    warn("Error %d received on non-final packet\n" %
                         rpkt.errcode())
                raise Mode6Exception(SERR_SERVER, rpkt.errcode())

            # Check the association ID to make sure it matches what we expect
            if rpkt.associd != associd:
                warn("Association ID %d doesn't match expected %d\n"
                     % (rpkt.associd, associd))

            # Some debugging involving authentication checks is here in the
            # C version, it's omitted until we support authentication.

            if rpkt.count == 0 and rpkt.more():
                warn("Received count of 0 in non-final fragment\n")
                continue

            if seenlastfrag and rpkt.more():
                warn("Received second last fragment packet\n")
                continue

            # So far, so good.  Record this fragment
            # overlap anything.
            if self.debug >= 2:
                warn("Packet okay\n")

            # Find the most recent fragment with a
            not_earlier = [frag for frag in fragments \
                           if frag.offset >= rpkt.offset]
            if len(not_earlier):
                not_earlier = earlier[0]
                if not_earlier.offset == rpkt.offset:
                    warn("duplicate %d octets at %d ignored, prior %d at %d\n" % (rpkt.count, rpkt.offset, not_earlier.count, not_earlier.offset))
                    continue

            if len(fragments) > 0:
                last = fragments[-1]
                if last.end() > rpkt.offset:
                    warn("received frag at %d overlaps with %d octet frag at %d\n" % (rpkt.offset, last.count, last.offset))
                    continue

            if not_earlier and rpkt.end() > not_earlier.offset:
                warn("received %d octet frag at %d overlaps with frag at %d\n" % (rpkt.count, rpkt.offset, not_earlier.offset))
                continue

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
                        break
                else:
                    #warn("%d packets reassembled\n" % len(fragments))
                    self.response = "".join([frag.data for frag in fragments])
                    if self.debug >= 4:
                        sys.stdout.write("Response packet:\n")
                        dump_hex_printable(self.response)
                    return None
                break

    def doquery(self, opcode, associd=0, qdata="", auth=False):
        "send a request and save the response"
        if not self.havehost():
            raise Mode6Exception(SERR_NOHOST)
        retry = True
        while True:
            # Ship the request
            self.sendrequest(opcode, associd, qdata, auth)
            # Get the response.
            try:
                res = self.getresponse(opcode, associd, not retry)
            except Mode6Exception as e:
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
        self.doquery(opcode=CTL_OP_READSTAT, associd=associd)
        if len(self.response) % 4:
            raise Mode6Exception(SERR_BADLENGTH)
        idlist = []
        if associd == 0:
            for i in range(len(self.response)//4):
                data = self.response[4*i:4*i+4]
                (associd, status) = struct.unpack("!HH", data)
                idlist.append(Peer(self, associd, status))
        idlist.sort(key=lambda a: a.associd)
        return idlist

    def __parse_varlist(self):
        "Parse a response as a textual varlist."
        # Trim trailing NULs from the text
        response = self.response
        while response.endswith(b"\x00"):
            response = response[:-1]
        response = response.rstrip()
        items = []
        if response:
            for pair in response.split(","):
                try:
                    eq = pair.index("=")
                    var = pair[:eq].strip()
                    val = pair[eq+1:].strip()
                    try:
                        val = int(val, 0)
                    except ValueError:
                        try:
                            val = float(val)
                        except ValueError:
                            if val[0] == '"' and val[-1] == '"':
                                val = val[1:-1]
                    items.append((var, val))
                except ValueError:
                    # Yes, ntpd really does emit bare tags for empty
                    # string-valued variables.
                    items.append((pair, ""))
        return collections.OrderedDict(items)

    def readvar(self, associd=0, varlist=None, opcode=CTL_OP_READVAR):
        "Read system vars from the host as a dict, or throw an exception."
        if varlist == None:
            qdata = ""
        else:
            qdata = ",".join(varlist)
        self.doquery(opcode, associd=associd, qdata=qdata)
        return self.__parse_varlist()

    def config(self, configtext):
        "Send configuration text to the daemon. Return True if accepted."
        self.doquery(opcode=CTL_OP_CONFIGURE, qdata=configtext, auth=True)
        # Copes with an implementation error - ntpd uses putdata without
        # setting the size correctly.
        if not self.response:
            raise Mode6Exception(SERR_PERMISSION)
        elif b"\x00" in self.response:
            self.response = self.response[:self.response.index(b"\x00")]
        self.response = self.response.rstrip()
        return self.response == "Config Succeeded"

    def fetch_nonce(self):
        self.doquery(opcode=CTL_OP_REQ_NONCE)
        if not self.response.startswith("nonce="):
            raise Mode6Exception(SERR_BADNONCE)
        return self.response.strip()

    def mrulist(self, variables, rawhook=None):
        "Retrieve MRU list data"
	nonce_uses = 0
	restarted_count = 0
	cap_frags = True
        warn = sys.stderr.write
        sorter = None

        if variables:
            if "sort" in variables:
                sorter = variables["sort"]
                del variables["sort"]
                # FIXME: implement sorting by address, in case anyone cares
                sortdict = {
                "lstint" : lambda e: e.last,		# lstint ascending
                "-lstint" : lambda e: -e.last,		# lstint descending
                "avgint" : lambda e: e.avgint(),	# avgint ascending
                "-avgint" : lambda e: -e.avgint(),	# avgint descending
                "addr" : None,			# IPv4 asc. then IPv6 asc.
                "-addr" : None,			# IPv6 desc. then IPv4 desc.
                "count" : lambda e: e.ct,	# hit count ascending
                "-count": lambda e: -e.ct,	# hit count descending
                }
                if sorter == "listint":
                    sorter = None
                if sorter is not None:
                    sorter = sortdict.getkey(key)
                    if sorter == None:
                        raise Mode6Exception(BAD_SORT % key)
            for k in list(variables.keys()):
                if k in ("mincount", "resall", "resany",
                         "maxlstint", "laddr", "sort"):
                    continue
                else:
                    raise Mode6Exception(SERR_BADPARAM % k)

        # FIXME: Do the reslist parameter mappings from the C version

        nonce = self.fetch_nonce()

        span = MRUList()
        try:
            # Form the initial request
            #next_report = time.time() + MRU_REPORT_SECS
            limit = min(3 * MAXFRAGS, self.ntpd_row_limit)
            frags = MAXFRAGS;
            req_buf = "%s, frags=%d" % (nonce, frags)
            if variables:
                parms = ", " + ",".join([("%s=%s" % it) for it in list(variables.items())])
            else:
                parms = ""
            req_buf += parms

            while True:
                # Request additions to the MRU list
                try:
                    self.doquery(opcode=CTL_OP_READ_MRU, qdata=req_buf)
                    recoverable_read_errors = False
                except Mode6Exception as e:
                    recoverable_read_errors = True
                    if e.errorcode is None:
                        raise e
                    elif e.errorcode == CERR_UNKNOWNVAR:
                        # None of the supplied prior entries match, so
                        # toss them from our list and try again.
                        if self.debug:
                            warn("no overlap between %d prior entries and server MRU list\n" % len(self.mrustats))
                        self.mrustats = []
                        restarted_count += 1
                        if restarted_count > 8:
                            raise Mode6Exception(SERR_STALL)
                        if self.debug:
                            warn("--->   Restarting from the beginning, retry #%u\n" % restarted_count)
                    elif e.errorcode == CERR_UNKNOWNVAR:
                        e.message = "CERR_UNKNOWNVAR from ntpd but no priors given."
                        raise e
                    elif e.errorcode == CERR_BADVALUE:
                        if cap_frags:
                            cap_frags = False;
                            if self.debug:
                                warn("Reverted to row limit from fragments limit.\n");
			else:
                            # ntpd has lower cap on row limit
                            self.ntpd_row_limit -= 1
                            limit = min(limit, ntpd_row_limit)
                            if self.debug:
                                warn("Row limit reduced to %d following CERR_BADVALUE.\n" % limit)
                    elif e.errorcode in (ERR_INCOMPLETE, ERR_TIMEOUT):
			 # Reduce the number of rows/frags requested by
			 # half to recover from lost response fragments.
			if cap_frags:
                            frags = max(2, frags / 2)
                            if self.debug:
                                warn("Frag limit reduced to %d following incomplete response.\n"% frags)
			else:
                            limit = max(2, limit / 2);
                            if self.debug:
                                warn("Row limit reduced to %d following incomplete response.\n" % limit)
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
                last_older = None
                addr_older = None
                highwater = len(span.entries)
                for (tag, val) in variables.items():
                    if tag =="now":
                        span.now = lfptofloat(val)
                        continue
                    elif tag == "last.newest":
                        continue
                    for prefix in ("addr", "last", "first", "ct", "mv", "rs"):
                        if tag.startswith(prefix + "."):
                            (member, idx) = tag.split(".")
                            try:
                                idx = int(idx)
                            except ValueError:
                                raise Mode6Exception(SERR_BADTAG % tag)
                            if idx >= len(span.entries):
                                span.entries.append(MRUEntry())
                            if type(val) != type(0) and val.startswith("0x"):
                                val = lfptofloat(val)
                            setattr(span.entries[-1], prefix, val)

                # If we've seen the end sentinel on the span, break out
                if span.is_complete():
                    break

		# Snooze for a bit between queries to let ntpd catch
		# up with other duties.
                time.sleep(0.05)

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
                                    ntpd_row_limit,
                                    max(limit + 1,
                                        limit * 33 / 32))

		# prepare next query with as many address and last-seen
		# timestamps as will fit in a single packet.
		req_buf = "%s, %s=%d%s" % \
                          (nonce,
                           "frags" if cap_frags else "limit",
                           frags if cap_frags else limit,
                           parms)
		nonce_uses += 1
                if nonce_uses >= 4:
                    nonce = fetch_nonce()
                    nonce_uses = 0
                for i in range(len(span.entries)):
                    e = self.entries[len(span.entries) - i - 1]
                    incr += ", addr.%d=%s, last.%d=%s" % (i, e.addr, i, e.last)
                    if len(req_buf) + len(incr) >= CTL_MAX_DATA_LEN:
                        break
                    else:
                        req_buf += incr
        except KeyboardInterrupt:
            pass	# We can test for interruption with is_complete()

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

        return span

# end
