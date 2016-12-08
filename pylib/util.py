# Common utility functions
# SPDX-License-Identifier: BSD-2-clause

from __future__ import print_function

import socket
import sys
import time
import os
import re
import shutil
import collections

import ntp.ntpc
import ntp.version
import ntp.magic
import ntp.control

def stdversion():
    return "%s-%s-%s %s" % (ntp.version.VERSION, ntp.version.VCS_TICK,
                            ntp.version.VCS_BASENAME, ntp.version.VCS_DATE)

def rfc3339(t):
    "RFC 3339 string from Unix time, including fractional second."
    rep = time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(t))
    t = str(t)
    if "." in t:
        subsec = t.split(".", 1)[1]
        if int(subsec) > 0:
            rep += "." + subsec
    rep += "Z"
    return rep

def portsplit(hostname):
    portsuffix = ""
    if hostname.count(":") == 1:		# IPv4 with appended port
        (hostname, portsuffix) = hostname.split(":")
        portsuffix = ":" + portsuffix
    elif ']' in hostname:			# IPv6
        rbrak = hostname.rindex("]")
        if ":" in hostname[rbrak:]:
            portsep =  hostname.rindex(":")
            portsuffix = hostname[portsep:]
            hostname = hostname[:portsep]
            hostname = hostname[1:-1]	# Strip brackets
    return (hostname, portsuffix)

# A hack to avoid repleatedly hammering on DNS when ntpmon runs.
canonicalization_cache = {}

def canonicalize_dns(inhost, family=socket.AF_UNSPEC):
    "Canonicalize a hostname or numeric IP address."
    if inhost in canonicalization_cache:
        return canonicalization_cache[inhost]
    # Catch garbaged hostnames in corrupted Mode 6 responses
    m = re.match("([:.[\]]|\w)*", inhost)
    if not m:
        raise TypeError
    (hostname, portsuffix) = portsplit(inhost)
    try:
        ai = socket.getaddrinfo(hostname, None, family, 0, 0, socket.AI_CANONNAME)
    except socket.gaierror as e:
        print('getaddrinfo failed: %s' % e, file=sys.stderr)
        raise SystemExit(1)
    (family, socktype, proto, canonname, sockaddr) = ai[0]
    try:
        name = socket.getnameinfo(sockaddr, socket.NI_NAMEREQD)
        result = name[0].lower() + portsuffix
    except socket.gaierror:
        # On OS X, canonname is empty for hosts without rDNS.
        # Fall back to the hostname.
        canonicalized = canonname or hostname
        result = canonicalized.lower() + portsuffix
    canonicalization_cache[inhost] = result
    return result

TermSize = collections.namedtuple("TermSize", ["width", "height"])

def termsize():
    "Return the current terminal size."
    # Alternatives at http://stackoverflow.com/questions/566746/how-to-get-console-window-width-in-python
    # The way this is used makes it not a big deal if the default is wrong.
    size = (80, 24)
    if os.isatty(1):
        try:
            (w, h) = shutil.get_terminal_size((80, 24))
            size = (w, h)
        except AttributeError:
            try:
                # OK, Python version < 3.3, cope
                import fcntl, termios, struct
                h, w, hp, wp = struct.unpack('HHHH',
                    fcntl.ioctl(2, termios.TIOCGWINSZ,
                    struct.pack('HHHH', 0, 0, 0, 0)))
                size = (w, h)
            except IOError:
                pass
    return TermSize(*size)

class PeerSummary:
    "Reusable report generator for peer statistics"
    def __init__(self, displaymode, pktversion, showhostnames, wideremote, termwidth=None, debug=0):
        self.displaymode = displaymode		# peers/apeers.opeers
        self.pktversion = pktversion		# interpretation of flash bits
        self.showhostnames = showhostnames	# If false, display numeric IPs
        self.wideremote = wideremote		# show wide remote names?
        self.debug = debug
        self.termwidth = termwidth
        # By default, the peer spreadsheet layout is designed so lines just
        # fit in 80 characters. This tells us how much extra horizontal space
        # we have available on a wider terminal emulator.
        self.horizontal_slack = min((termwidth or 80) - 80, 24)
        # Peer spreadsheet column widths. The reason we cap extra
        # width used at 24 is that on very wide displays, slamming the
        # non-hostname fields all the way to the right produces a huge
        # river that makes the entries difficult to read as wholes.
        # This choice caps the peername field width at that of the longest
        # possible IPV6 numeric address.
        self.namewidth = 15 + self.horizontal_slack
        self.refidwidth = 15
        # Compute peer spreadsheet headers 
        self.__remote = "     remote    ".ljust(self.namewidth)
        self.__common = "st t when poll reach   delay   offset  "
        self.__header = None

    @staticmethod
    def prettyinterval(diff):
        "Print an interval in natural time units."
        if diff <= 0:
            return "-"
        if diff <= 2048:
            return str(diff)
        diff = (diff + 29) / 60
        if diff <= 300:
            return "%dm" % diff
        diff = (diff + 29) / 60
        if diff <= 96:
            return "%dh" % diff
        diff = (diff + 11) / 24
        return "%dd" % diff

    @staticmethod
    def high_truncate(hostname, maxlen):
        "Truncate on the left using leading _ to indicate 'more'."
        # Used for local IPv6 addresses, best distinguished by low bits
        if len(hostname) <= maxlen:
            return hostname
        else:
            return '-' + hostname[-maxlen+1:]

    def header(self):
        "Column headers for peer display"
        if self.displaymode == "apeers":
            self.__header = self.__remote + \
                             "   refid   assid  ".ljust(self.refidwidth) + \
                             self.__common + "jitter"
        elif self.displaymode == "opeers":
            self.__header = self.__remote + \
                             "       local      ".ljust(self.refidwidth) + \
                             self.__common + "  disp"
        else:
            self.__header  = self.__remote + \
                             "       refid      ".ljust(self.refidwidth) + \
                             self.__common + "jitter"
        return self.__header

    def width(self):
        "Width of display"
        return 78 + self.horizontal_slack

    def summary(self, rstatus, variables, associd):
        "Peer status summary line."
        hmode = 0
        srchost = None
        srcport = 0
        srcadr = None
        dstadr_refid = ""
        ppoll = 0
        hpoll = 0
        reach = 0
        ptype = '?'
        have_jitter = False
        clock_name = ''

        now = time.time()

        for (name, value) in variables.items():
            if name in ("srcadr", "peeradr"):
                srcadr = value
            elif name == "srchost":
                srchost = value
            elif name == "dstadr":
                # The C code tried to get a fallback pytpe from this in case
                # the hmode field was not included
                if "local" in self.__header:
                    dstadr_refid = value
            elif name == "hmode":
                hmode = value
            elif name == "refid":
                # The C code for this looked crazily overelaborate.  Best
                # guess is that it was designed to deal with formats that
                # no longer occur in this field.
                if "refid" in self.__header:
                    dstadr_refid = value
            elif name == "hpoll":
                hpoll = value
                if hpoll < 0:
                    hpoll = ntp.magic.NTP_MINPOLL
            elif name == "ppoll":
                ppoll = value
                if ppoll < 0:
                    ppoll = ntp.magic.NTP_MINPOLL
            elif name == "reach":
                # Shipped as hex, displayed in octal
                reach = value
            elif name == "delay":
                estdelay = value
            elif name == "offset":
                estoffset = value
            elif name == "jitter":
                if "jitter" in self.__header:
                    estjitter = value
                    have_jitter = True
            elif name == "rootdisp" or name == "dispersion":
                estdisp = value
            elif name == "rec":
                rec = value	# l_fp timestamp
            elif name == "srcport" or name == "peerport":
                srcport = value
            elif name == "reftime":
                reftime = value	# l_fp timestamp
        if hmode == ntp.magic.MODE_BCLIENT:
            # broadcastclient or multicastclient
            ptype = 'b'
        elif hmode == ntp.magic.MODE_BROADCAST:
            # broadcast or multicast server
            if srcadr.startswith("224."):	# IANA multicast address prefix
                ptype = 'M'
            else:
                ptype = 'B'
        elif hmode == ntp.magic.MODE_CLIENT:
            if srchost and '(' in srchost:
                ptype = 'l'	# local refclock
            elif dstadr_refid == "POOL":
                ptype = 'p'	# pool
            elif srcadr.startswith("224."):
                ptype = 'a'	# manycastclient
            else:
                ptype = 'u'	# unicast
        elif hmode == ntp.magic.MODE_ACTIVE:
            ptype = 's'		# symmetric active
        elif hmode == ntp.magic.MODE_PASSIVE:
            ptype = 'S'		# symmetric passive

        #
        # Got everything, format the line
        #
        line = ""
        poll_sec = 1 << min(ppoll, hpoll)
        if self.pktversion > ntp.magic.NTP_OLDVERSION:
            c = " x.-+#*o"[ntp.control.CTL_PEER_STATVAL(rstatus) & 0x7]
        else:
            c = " .+*"[ntp.control.CTL_PEER_STATVAL(rstatus) & 0x3]
        # Source host or clockname
        if srchost != None:
            clock_name = srchost
        elif self.showhostnames:
            try:
                if self.debug:
                    sys.stderr.write("DNS lookup begins...\n")
                clock_name = canonicalize_dns(srcadr)
                if self.debug:
                    sys.stderr.write("DNS lookup ends.\n")
            except TypeError:
                return ''
        else:
            clock_name = srcadr
        if self.wideremote and len(clock_name) > self.namewidth:
            line += ("%c%s\n" % (c, clock_name))
            line += (" " * (self.namewidth + 2))
        else:
            line += ("%c%-*.*s " % \
                             (c, self.namewidth, self.namewidth, clock_name[:self.namewidth]))
        # Destination address, assoc ID or refid.
        assocwidth = 7 if self.displaymode == "apeers" else 0
        if "." not in dstadr_refid and ":" not in dstadr_refid:
            dstadr_refid = "." + dstadr_refid + "."
        if assocwidth and len(dstadr_refid) >= self.refidwidth - assocwidth:
            visible = "..."
        else:
            visible = dstadr_refid
        line += self.high_truncate(visible, self.refidwidth)
        if self.displaymode == "apeers":
            line += (" " * (self.refidwidth - len(visible) - assocwidth + 1))
            line += ("%-6d" % (associd))
        else:
            line += (" " * (self.refidwidth - len(visible)))
        # The rest of the story
        last_sync = variables.get("rec") or variables.get("reftime")
        jd = estjitter if have_jitter else estdisp
        jd = "      -" if jd >= 999 else ("%7.3f" % jd)
        try:
            line += (
                " %2ld %c %4.4s %4.4s  %3lo  %7.3f %8.3f %s\n" % \
                (variables.get("stratum", 0),
                 ptype,
                 PeerSummary.prettyinterval(now if last_sync is None else int(now - ntp.ntpc.lfptofloat(last_sync))),
                 PeerSummary.prettyinterval(poll_sec),
                 reach, estdelay, estoffset,
                 jd))
            return line
        except TypeError:
            # This can happen when ntpd ships a corrupt varlist
            return ''

class MRUSummary:
    "Reusable class for MRU entry summary generation."
    def __init__(self, showhostnames):
        self.showhostnames = showhostnames	# If false, display numeric IPs
        self.now = time.time()

    header = "lstint avgint rstr r m v  count rport remote address"

    width = 78

    def summary(self, entry):
        lstint = int(self.now - entry.last + 0.5)
        active = float(entry.last - entry.first)
        favgint = active / entry.ct
        avgint = int(favgint + 0.5)
        stats = "%6d" % lstint
        if 5 < avgint or 1 == entry.ct:
            stats += " %6d" % avgint
        else:
            stats += " %6.2f" % favgint
        if entry.rs & ntp.magic.RES_KOD:
            rscode = 'K'
        elif entry.rs & ntp.magic.RES_LIMITED:
            rscode = 'L'
        else:
            rscode = '.'
        (dns, port) = portsplit(entry.addr)
        try:
            if self.showhostnames:
                dns = canonicalize_dns(dns)
            stats += " %4hx %c %d %d %6d %5s %s" % \
                     (entry.rs, rscode,
                      ntp.magic.PKT_MODE(entry.mv),
                      ntp.magic.PKT_VERSION(entry.mv),
                      entry.ct, port[1:], dns)
            return stats
        except TypeError:
            # This can happen when ntpd ships a corrupt varlist
            return ''

class ReslistSummary:
    "Reusable class for reslist entry summary generation."
    header = """\
   hits    addr/prefix or addr mask
           restrictions
"""
    width = 72
    @staticmethod
    def __getPrefix(mask):
        if not mask:
            prefix = ''
        if ':' in mask:
            sep = ':'
            base = 16
        else:
            sep = '.'
            base = 10
        prefix = sum([bin(int(x, base)).count('1') for x in mask.split(sep) if x])
        return '/' + str(prefix)
    def summary(self, variables):
        hits = variables.get("hits", "?")
        address = variables.get("addr", "?")
        mask = variables.get("mask", "?")
        if address == '?' or mask == '?':
            return ''
        address += ReslistSummary.__getPrefix(mask)
        flags = variables.get("flags", "?")
        # reslist responses are often corrupted
        s = "%10s %s\n           %s\n" % (hits, address, flags)
        # Throw away corrupted entries.  This is a shim - we really
        # want to make ntpd stop generating garbage
        for c in s:
            if not c.isalnum() and not c in "/.: \n":
                return ''
        return s

class IfstatsSummary:
    "Reusable class for ifstats entry summary generation."
    header = """\
    interface name                                        send
 #  address/broadcast     drop flag ttl mc received sent failed peers   uptime
 """
    width = 72
    def summary(self, i, variables):
        try:
            s = "%3u %-24.24s %c %4x %3d %2d %6d %6d %6d %5d %8d\n    %s\n" % \
               (i, variables['name'],
	       '.' if variables['en'] else 'D',
		variables.get('flags', '?'),
                variables.get('tl', '?'),
                variables.get('mc', '?'),
		variables.get('rx', '?'),
                variables.get('tx', '?'),
                variables.get('txerr', '?'),
		variables.get('pc', '?'),
                variables.get('up', '?'),
                variables.get('addr', '?'))
            if variables.get("bcast"):
                s += "    %s\n" % variables['bcast']
        except TypeError:
            # Can happen when ntpd ships a corrupted response
            return ''
        for c in s:
            if not c.isalnum() and not c in "/.:[] \n":
                return ''
        return s

try:
    import collections
    OrderedDict = collections.OrderedDict
except ImportError:
    class OrderedDict(dict):
        "A stupid simple implementation in order to be back-portable to 2.6"
        # This can be simple because it doesn't need to be fast.
        # The programs that use it only have to run at human speed,
        # and the collections are small.
        def __init__(self, items=None):
            dict.__init__(self)
            self.__keys = []
            if items:
                for (k, v) in items:
                    self[k] = v
        def __setitem__(self, key, val):
            dict.__setitem__(self, key, val)
            self.__keys.append(key)
        def __delitem__(self, key):
            dict.__delitem__(self, key)
            self.__keys.remove(key)
        def keys(self):
            return self.__keys
        def items(self):
            return tuple([(k, self[k]) for k in self.__keys])
        def __iter__(self):
            for key in self.__keys:
                yield key

# end
