#
# packet.py - definitions and classes for ntp-style querying of NTP
#
# This code should be Python2-vs-Python-3 agnostic.  Keep it that way!
#
# SPDX-License-Identifier: BSD-2-clause
from __future__ import print_function, division
import os, sys, socket, select, struct, curses.ascii

CTL_MAX_DATA_LEN	= 468	# Max data in a control packet

# Modes from ntp.h
MODE_CONTROL = 6

# Opcodes from ntp_control.c
CTL_OP_UNSPEC		= 0	# unspecified
CTL_OP_READSTAT		= 1	# read status
CTL_OP_READVAR		= 2	# read variables
CTL_OP_WRITEVAR		= 3	# write variables
CTL_OP_READCLOCK	= 4	# read clock variables
CTL_OP_WRITECLOCK	= 5	# write clock variables
CTL_OP_SETTRAP		= 6	# set trap address
CTL_OP_ASYNCMSG		= 7	# asynchronous message
CTL_OP_CONFIGURE	= 8	# runtime configuration
CTL_OP_READ_MRU		= 10	# retrieve MRU (mrulist)
CTL_OP_READ_ORDLIST_A	= 11	# ordered list req. auth.
CTL_OP_REQ_NONCE	= 12	# request a client nonce
CTL_OP_UNSETTRAP	= 31	# unset trap

# NTP Status codes
NTP_STATUS_INVALID      = 0
NTP_STATUS_FALSETICKER  = 1
NTP_STATUS_EXCESS       = 2
NTP_STATUS_OUTLYER      = 3
NTP_STATUS_CANDIDATE    = 4
NTP_STATUS_SELECTED     = 5
NTP_STATUS_SYSPEER      = 6
NTP_STATUS_PPSPEER      = 7

# NTP association type identifier
NTP_CLOCKTYPE_UNKNOWN   = '-'
NTP_CLOCKTYPE_BROADCAST = 'b'
NTP_CLOCKTYPE_LOCAL     = 'l'
NTP_CLOCKTYPE_UNICAST   = 'u'
NTP_CLOCKTYPE_MULTICAST = 'm'

NERR_UNSPEC	= 0
NERR_PERMISSION	= 1
NERR_BADFMT	= 2
NERR_BADOP	= 3
NERR_BADASSOC	= 4
NERR_UNKNOWNVAR	= 5
NERR_BADVALUE	= 6
NERR_RESTRICT	= 7

NERR_NORESOURCE	= NERR_PERMISSION	# wish there was a different code

# Variable Sets
PEERVARS  = CTL_OP_READVAR
#CLOCKVARS = CTL_OP_CLOCKVAR

LEAP_NOWARNING	= 0x0	# leap_none: normal, no leap second warning
LEAP_ADDSECOND	= 0x1	# leap_add_sec: last minute of day has 61 seconds
LEAP_DELSECOND	= 0x2	# leap_del_sec: last minute of day has 59 seconds
LEAP_NOTINSYNC	= 0x3	# leap_alarm: overload, clock is free running

NTP_OLDVERSION	= 1	# C code said "oldest credible version"
NTP_VERSION	= 4	# Current version

# Limit on packets in a single response.  Increasing this value to
# 96 will marginally speed "mrulist" operation on lossless networks
# but it has been observed to cause loss on WiFi networks and with
# an IPv6 go6.net tunnel over UDP.  That loss causes the request
# row limit to be cut in half, and it grows back very slowly to
# ensure forward progress is made and loss isn't triggered too quickly
# afterward.  While the lossless case gains only marginally with
# MAXFRAGS == 96, the lossy case is a lot slower due to the repeated
# timeouts.  Empirally, MAXFRAGS == 32 avoids most of the routine loss
# on both the WiFi and UDP v6 tunnel tests and seems a good compromise.
# This suggests some device in the path has a limit of 32 ~512 byte UDP
# packets in queue.
# Lowering MAXFRAGS may help with particularly lossy networks, but some
# ntpq commands may rely on the longtime value of 24 implicitly,
# assuming a single multipacket response will be large enough for any
# needs.  In contrast, the "mrulist" command is implemented as a series
# of requests and multipacket responses to each.
MAXFRAGS	= 32

class ntp_packet:
    "Encapsulate an NTP fragment"
    # The following two methods are copied from macros in includes/control.h
    @staticmethod
    def VN_MODE(v, m):		return ((((v) & 7) << 3) | ((m) & 0x7))
    @staticmethod
    def PKT_LI_VN_MODE(l,v,m):	return ((((l) & 3) << 6) | ntp_packet.VN_MODE((v), (m)))
    def __init__(self, session, version, mode):
        self.session = session	# Where to get session context
	self.li_vn_mode = 0	# leap, version, mode (uint8_t)
	self.r_m_e_op = 0	# response, more, error, opcode (uint8_t)
        # Subclasses have four uint16_t fields here
        self.count = 0		# octet count of extension data
	self.extension = ''	# extension data
        self.li_vn_mode = ntp_packet.PKT_LI_VN_MODE(0, version, mode)
    format = "!BBHHHHH"
    def send(self, payload1, payload2, payload3, payload4):
        "Send the packet and its payload in association with a session"
        self.count = len(self.extension)
        body = struct.pack(ntp_packet.format,
                             self.li_vn_mode,
                             self.r_m_e_op,
                             payload1, payload2, payload3, payload4,
                             self.count)
        self.session.sendpkt(body + self.extension)
    def analyze(self, rawdata):
        (self.li_vn_mode,
         self.r_m_e_op,
         payload1, payload2, payload3, payload4,
         self.count) = struct.unpack(ntp_packet.format, rawdata[:12])
        self.data = rawdata[12:]
        return (payload1, payload2, payload3, payload4)
    def mode(self):
        return self.li_vn_mode & 0x7
    def end(self):
        return self.count + self.offset
    def version(self):
        return (self.li_vn_mode >> 3) & 0x7

class control_frag(ntp_packet):
    "ntpq request/response "
    def __init__(self, session, opcode=0, associd=0, qdata=''):
        ntp_packet.__init__(self, session, session.pktversion, MODE_CONTROL)
        self.r_m_e_op = opcode	# ntpq operation code
	self.sequence = 0	# sequence number of request (uint16_t)
	self.status = 0		# status word for association (uint16_t)
	self.associd = associd	# association ID (uint16_t)
	self.offset = 0		# offset of this batch of data (uint16_t)
        self.extension = qdata	# Data for this packet
    def is_response(self):
        return self.r_m_e_op & 0x80
    def opcode(self):
        return self.r_m_e_op & 0x1F
    def is_error(self):
        return self.r_m_e_op & 0x40
    def errcode(self):
        return (self.status >> 8) & 0xff
    def more(self):
        return self.r_m_e_op & 0x20
    def stats(self, idx):
        "Return statistics on a fragment."
        return "%5d %5d\t%3d octets\n" % (self.offset, self.end(), self.count)
    def send(self):
        self.session.sequence += 1
        self.sequence = self.session.sequence
        ntp_packet.send(self,
                        self.sequence, self.status, self.associd, self.offset) 
    def analyze(self, data):
        (self.sequence,
         self.status,
         self.associd,
         self.offset) = ntp_packet.analyze(self, data)

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

def dump_hex_printable(xdata):
    "Dump a packet in hex, in a familiar hex format"
    llen = len(xdata)
    i = 0;
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

class ntpq_session:
    "A session to a host"
    def __init__(self):
        self.debug = 0
        self.ai_family = socket.AF_UNSPEC
        self.primary_timeout = 5000	# Timeout for first select on receive
        self.secondary_timeout = 3000	# Timeout for later selects
        self.pktversion = NTP_OLDVERSION + 1	# Packet version number we use 
        self.always_auth       = False	# Always send authenticated requests 
        self.keyid = None
        self.password = None
        self.name = None
        self.isnum = False
        self.sock = None
        self.port = 0
        self.sequence = 0
        self.response = ""
        self.rstatus = 0
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
        except socket.gaierror as (errno, string):
            if self.debug > 2:
                sys.stderr.write("ntpq: numeric-mode lookup of %s failed, %s\n" % (hname, string))
        try:
            return hinted_lookup(port="ntp", hints=0)
        except socket.gaierror as (errno, string):
            sys.stderr.write("ntpq: standard-mode lookup of %s failed, %s\n" % (hname, string))
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
            if errno in (socket.EAI_NONAME, socket.EAI_NODATA):
                try:
                    return hinted_lookup(port="ndp", hints=0)
                except socket.gaierror as (errno, string):
                    sys.stderr.write("ntpq: ndp lookup failed, %s\n" % string)
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
            self.name = sockaddr.inet_ntop(sockaddr[0], family)
            self.isnum = True
        else:
            self.name = canonname or hname
            self.isnum = False
        if self.debug > 2:
            sys.stdout.write("Opening host %s\n" % temphost)
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
    def sendpkt(self, xdata):
	"Send a packet to the host."
	if self.debug >= 3:
		print("Sending %d octets\n" % len(xdata))
        try:
            self.sock.sendall(xdata)
        except socket.error:
            # On failure, we don't know how much data was actually received
            sys.stderr.write("Write to %s failed\n" % self.name)
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
        pkt = control_frag(self, opcode, associd, qdata)

        # If it isn't authenticated we can just send it.  Otherwise
        # we're going to have to think about it a little.
	if not auth and not self.always_auth:
            return pkt.send();
        else:
            sys.stderr.write("Authenticated send is not yet implemented\n");
            return -1
    def getresponse(self, opcode, associd, timeo):
        "Get a response expected to match a given opcode and associd."
	 # This is pretty tricky.  We may get between 1 and MAXFRAG packets
	 # back in response to the request.  We peel the data out of
	 # each packet and collect it in one long block.  When the last
	 # packet in the sequence is received we'll know how much data we
	 # should have had.  Note we use one long time out, should reconsider.
	fragments = []
        self.response = ''
	seenlastfrag = False;
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
                warn("too many packets in response; bailing out\n")
                return SERR_TOOMUCH

            if len(fragments) == 0:
                tvo = self.primary_timeout / 1000
            else:
                tvo = self.secondary_timeout / 1000

            try:
                (rd, _, _) = select.select([self.sock], [], [], tvo)
            except select.error as msg:
                warn("select failed: %s\n" % msg[1])
                return SERR_SELECT

            if not rd:
                # Timed out.  Return what we have
                if len(fragments) == 0:
                    if timeo:
                        warn("%s: timed out, nothing received\n" % self.name)
                        return SERR_TIMEOUT
                if timeo:
                    warn("%s: timed out with incomplete data\n" % self.name);
                    if self.debug:
                        sys.stderr.write("ERR_INCOMPLETE: Received fragments:\n")
                        for (i, frag) in enumerae(fragments):
                            sys.stderr.write("%d: %s" % (i+1, frag.stats()))
                        sys.stderr.write("last fragment %sreceived\n",
                                    ("not " "", )[seenlastfrag])
                    return SERR_INCOMPLETE

            rawdata = self.sock.recv(4096);
            if self.debug:
                warn("Received %d octets\n" % len(rawdata))
            rpkt = control_frag(self)
            try:
                rpkt.analyze(rawdata)
            except struct.error as reason:
                warn("packet analysis failed: %s\n" % reason)
                return SERR_UNSPEC

            if rpkt.version() > NTP_VERSION or rpkt.version() < NTP_OLDVERSION:
                if self.debug:
                    warn("Packet received with version %d\n" % rpkt.version())
                continue
            if rpkt.mode() != MODE_CONTROL:
                if self.debug:
                    warn("Packet received with mode %d\n" % pkt.mode())
                continue
            if not rpkt.is_response():
                if self.debug:
                    warn("Received request, wanted response\n");
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
                return rpkt.errcode()

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
                if last.end() > rkpt.offset:
                    warn("received frag at %d overlaps with %d octet frag at %d\n" % (rpkt.offset, last.count, last.offset));
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
                    if fragments[f-1].endpoint() != fragments[f].offset:
                        break
                else:
                    #warn("%d packets reassembled\n" % len(fragments))
                    self.response = "".join([frag.data for frag in fragments])
                    if self.debug >= 4:
                        sys.stdout.write("Response packet:\n")
                        dump_hex_printable(self.response)
                    return None
    def doquery(self, opcode, associd=0, qdata="", auth=False, quiet=False):
        "send a request and save the response"
        if not self.havehost():
            return SERR_NOHOST
	done = False;
        while True:
            # Ship the request
            res = self.sendrequest(opcode, associd, qdata, auth);
            if res is not None:
                return res
            # Get the response.
            res = self.getresponse(opcode, associd, done)
            if res:
		if not quiet:
                    if type(res) == type(0):
                        sys.stderr.write("***Packet error %d" % res)
                    else:
                        sys.stderr.write(res.format(associd))
                if not done and res in (SERR_TIMEOUT,SERR_INCOMPLETE):
                    done = True
                    continue
            break
        # Return None on success, otherwise an error string
	return res;

# end
