# Common utility functions

from __future__ import print_function

import socket
import sys
import subprocess
import time
import ntp.ntpc

from ntp.packet import *

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

def canonicalize_dns(hostname):
    (hostname, portsuffix) = portsplit(hostname)
    try:
        ai = socket.getaddrinfo(hostname, None, 0, 0, 0, socket.AI_CANONNAME)
    except socket.gaierror as (s, _e):
        print('getaddrinfo failed: %s' % s, file=sys.stderr)
        raise SystemExit(1)
    (family, socktype, proto, canonname, sockaddr) = ai[0]
    try:
        name = socket.getnameinfo(sockaddr, socket.NI_NAMEREQD)
    except socket.gaierror:
        return canonname.lower() + portsuffix
    return name[0].lower() + portsuffix

def termsize():
    "Return the current terminal size."
    # Should work under Linux and Solaris at least.
    # Alternatives at http://stackoverflow.com/questions/566746/how-to-get-console-window-width-in-python
    import shlex, subprocess, re
    try:
        output = subprocess.check_output(shlex.split('/bin/stty -a'))
    except OSError:
        return (24, 80)
    for pattern in ('rows\D+(\d+); columns\D+(\d+);', '\s+(\d+)\D+rows;\s+(\d+)\D+columns;'):
        m = re.search(pattern, output)
        if m:
            return int(m.group(1)), int(m.group(2))
    return (24, 80)

class PeerSummary:
    "Reusable report generator for peer statistics"
    def __init__(self, displaymode, pktversion, showhostnames, wideremote):
        self.displaymode = displaymode		# peers/apeers.opeers
        self.pktversion = pktversion		# interpretation of flash bits
        self.showhostnames = showhostnames	# If false, display numeric IPs
        self.wideremote = wideremote		# show wide remote names?
        # By default, the peer spreadsheet layout is designed so lines just
        # fit in 80 characters. This tells us how much extra horizontal space
        # we have available on a wider terminal emulator
        self.horizontal_slack = termsize()[1] - 80
        # Peer spreadsheet column widths
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
                    hpoll = NTP_MINPOLL
            elif name == "ppoll":
                ppoll = value
                if ppoll < 0:
                    ppoll = NTP_MINPOLL
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
        if hmode == MODE_BCLIENT:
            # broadcastclient or multicastclient
            ptype = 'b'
        elif hmode == MODE_BROADCAST:
            # broadcast or multicast server
            if srcadr.startswith("224."):	# IANA multicast address prefix
                ptype = 'M'
            else:
                ptype = 'B'
        elif hmode == MODE_CLIENT:
            if srchost and '(' in srchost:
                ptype = 'l'	# local refclock
            elif dstadr_refid == "POOL":
                ptype = 'p'	# pool
            elif srcadr.startswith("224."):
                ptype = 'a'	# manycastclient
            else:
                ptype = 'u'	# unicast
        elif hmode == MODE_ACTIVE:
            ptype = 's'		# symmetric active
        elif hmode == MODE_PASSIVE:
            ptype = 'S'		# symmetric passive

        #
        # Got everything, format the line
        #
        line = ""
        poll_sec = 1 << min(ppoll, hpoll)
        if self.pktversion > NTP_OLDVERSION:
            c = " x.-+#*o"[CTL_PEER_STATVAL(rstatus) & 0x7]
        else:
            c = " .+*"[CTL_PEER_STATVAL(rstatus) & 0x3]
        # Source host or clockname
        if srchost != None:
            clock_name = srchost
        elif self.showhostnames:
            clock_name = canonicalize_dns(srcadr)
        else:
            clock_name = srcadr
        if self.wideremote and len(clock_name) > self.namewidth:
            line += ("%c%s\n" % (c, clock_name))
            line + (" " * (self.namewidth + 2))
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
        line += (
            " %2ld %c %4.4s %4.4s  %3lo  %7.3f %8.3f %s\n" % \
            (variables.get("stratum", 0),
             ptype,
             PeerSummary.prettyinterval(now if last_sync is None else int(now - ntp.ntpc.lfptofloat(last_sync))),
             PeerSummary.prettyinterval(poll_sec),
             reach, estdelay, estoffset,
             jd))
        return line

class MRUSummary:
    "Reusable class for MRU entry summary generation."
    def __init__(self, showhostnames):
        self.showhostnames = showhostnames	# If false, display numeric IPs
        self.now = time.time()

    header = "lstint avgint rstr r m v  count rport remote address"

    width = 78

    def summary(self, entry):
        lstint = int(self.now - entry.last + 0.5)
        active = int(entry.last - entry.first + 0.5)
        favgint = active / entry.ct
        avgint = int(favgint + 0.5)
        stats = "%6d" % lstint
        if 5 < avgint or 1 == entry.ct:
            stats += " %6d" % avgint
        else:
            stats += " %6.2f" % favgint
        if entry.rs & RES_KOD:
            rscode = 'K'
        elif entry.rs & RES_LIMITED:
            rscode = 'L'
        else:
            rscode = '.'
        (dns, port) = portsplit(entry.addr)
        if self.showhostnames:
            dns = canonicalize_dns(dns)
        stats += " %4hx %c %d %d %6d %5s %s" % \
                 (entry.rs, rscode, PKT_MODE(entry.mv), PKT_VERSION(entry.mv),
                  entry.ct, port[1:], dns)
        return stats

# end
