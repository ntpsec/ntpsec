# -*- coding: utf-8 -*-
"Common utility functions"
# SPDX-License-Identifier: BSD-2-clause

from __future__ import print_function, division


import collections
import os
import re
import shutil
import socket
import string
import sys
import time
import ntp.ntpc
import ntp.magic
import ntp.control


# Old CTL_PST defines for version 2.
OLD_CTL_PST_CONFIG = 0x80
OLD_CTL_PST_AUTHENABLE = 0x40
OLD_CTL_PST_AUTHENTIC = 0x20
OLD_CTL_PST_REACH = 0x10
OLD_CTL_PST_SANE = 0x08
OLD_CTL_PST_DISP = 0x04

OLD_CTL_PST_SEL_REJECT = 0
OLD_CTL_PST_SEL_SELCAND = 1
OLD_CTL_PST_SEL_SYNCCAND = 2
OLD_CTL_PST_SEL_SYSPEER = 3


# Units for formatting
UNIT_NS = "ns"       # nano second
UNIT_US = u"µs"      # micro second
UNIT_MS = "ms"       # milli second
UNIT_S = "s"         # second
UNIT_KS = "ks"       # kilo seconds
UNITS_SEC = [UNIT_NS, UNIT_US, UNIT_MS, UNIT_S, UNIT_KS]
UNIT_PPT = "ppt"     # parts per trillion
UNIT_PPB = "ppb"     # parts per billion
UNIT_PPM = "ppm"     # parts per million
UNIT_PPK = u"‰"      # parts per thousand
UNITS_PPX = [UNIT_PPT, UNIT_PPB, UNIT_PPM, UNIT_PPK]
unitgroups = (UNITS_SEC, UNITS_PPX)


# These two functions are not tested because they will muck up the module
# for everything else, and they are simple.

def check_unicode():  # pragma: no cover
    if "UTF-8" != sys.stdout.encoding:
        deunicode_units()
        return True  # needed by ntpmon
    return False


def deunicode_units():  # pragma: no cover
    """Under certain conditions it is not possible to force unicode output,
    this overwrites units that contain unicode with safe versions"""
    global UNIT_US
    global UNIT_PPK
    # Replacement units
    new_us = "us"
    new_ppk = "ppk"
    # Replace units in unit groups
    UNITS_SEC[UNITS_SEC.index(UNIT_US)] = new_us
    UNITS_PPX[UNITS_PPX.index(UNIT_PPK)] = new_ppk
    # Replace the units themselves
    UNIT_US = new_us
    UNIT_PPK = new_ppk


# Variables that have units
S_VARS = ("tai", "poll")
MS_VARS = ("rootdelay", "rootdisp", "rootdist", "offset", "sys_jitter",
           "clk_jitter", "leapsmearoffset", "authdelay", "koffset", "kmaxerr",
           "kesterr", "kprecis", "kppsjitter", "fuzz", "clk_wander_threshold",
           "tick", "in", "out", "bias", "delay", "jitter", "dispersion",
           "fudgetime1", "fudgetime2")
PPM_VARS = ("frequency", "clk_wander")


def dolog(logfp, text, debug, threshold):
    """debug is the current debug value
    threshold is the trigger for the current log"""
    if logfp is None:
        return  # can turn off logging by supplying a None file descriptior
    text = rfc3339(time.time()) + " " + text + "\n"
    if debug >= threshold:
        logfp.write(text)
        logfp.flush()  # we don't want to lose an important log to a crash


def safeargcast(arg, castfunc, errtext, usage):
    """Attempts to typecast an argument, prints and dies on failure.
    errtext must contain a %s for splicing in the argument, and be
    newline terminated."""
    try:
        casted = castfunc(arg)
    except ValueError:
        sys.stderr.write(errtext % arg)
        sys.stderr.write(usage)
        raise SystemExit(1)
    return casted


def stdversion():
    "Returns the NTPsec version string in a standard format"
    return "ntpsec-%s" % "@NTPSEC_VERSION_EXTENDED@"


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


def deformatNTPTime(txt):
    txt = txt[2:]  # Strip '0x'
    txt = "".join(txt.split("."))  # Strip '.'
    value = ntp.util.hexstr2octets(txt)
    return value


def hexstr2octets(hexstr):
    if (len(hexstr) % 2) != 0:
        hexstr = hexstr[:-1]  # slice off the last char
    values = []
    for index in range(0, len(hexstr), 2):
        values.append(chr(int(hexstr[index:index+2], 16)))
    return "".join(values)


def slicedata(data, slicepoint):
    "Breaks a sequence into two pieces at the slice point"
    return data[:slicepoint], data[slicepoint:]


def portsplit(hostname):
    portsuffix = ""
    if hostname.count(":") == 1:                # IPv4 with appended port
        (hostname, portsuffix) = hostname.split(":")
        portsuffix = ":" + portsuffix
    elif ']' in hostname:                       # IPv6
        rbrak = hostname.rindex("]")
        if ":" in hostname[rbrak:]:
            portsep = hostname.rindex(":")
            portsuffix = hostname[portsep:]
            hostname = hostname[:portsep]
            hostname = hostname[1:-1]   # Strip brackets
    return (hostname, portsuffix)


def parseConf(text):
    inQuote = False
    quoteStarter = ""
    lines = []
    tokens = []
    current = []

    def pushToken():
        token = "".join(current)
        if inQuote is False:  # Attempt type conversion
            try:
                token = int(token)
            except ValueError:
                try:
                    token = float(token)
                except ValueError:
                    pass
        wrapper = (inQuote, token)
        tokens.append(wrapper)
        current[:] = []

    def pushLine():
        if len(current) > 0:
            pushToken()
        if len(tokens) > 0:
            lines.append(tokens[:])
            tokens[:] = []

    i = 0
    tlen = len(text)
    while i < tlen:
        if inQuote is True:
            if text[i] == quoteStarter:  # Ending a text string
                pushToken()
                quoteStarter = ""
                inQuote = False
            elif text[i] == "\\":  # Starting an escape sequence
                i += 1
                if text[i] in "'\"n\\":
                    current.append(eval("\'\\" + text[i] + "\'"))
            else:
                current.append(text[i])
        else:
            if text[i] == "#":  # Comment
                while (i < tlen) and (text[i] != "\n"):
                    i += 1  # Advance to end of line...
                i -= 1  # ...and back up so we don't skip the newline
            elif text[i] in "'\"":  # Starting a text string
                inQuote = True
                quoteStarter = text[i]
                if len(current) > 0:
                    pushToken()
            elif text[i] == "\\":  # Linebreak escape
                i += 1
                if text[i] != "\n":
                    raise SyntaxError
            elif text[i] == "\n":  # EOL: break the lines
                pushLine()
            elif text[i] in string.whitespace:
                if len(current) > 0:
                    pushToken()
            else:
                current.append(text[i])
        i += 1
    pushLine()
    return lines


def stringfilt(data):
    "Pretty print string of space separated numbers"
    parts = data.split()

    cooked = []
    for part in parts:
        # These are expected to fit on a single 80-char line.
        # Accounting for other factors this leaves each number with
        # 7 chars + a space.
        fitted = fitinfield(part, 7)
        cooked.append(fitted)
    rendered = " ".join(cooked)
    return rendered


def stringfiltcooker(data):
    "Cooks a filt* string of space separated numbers, expects milliseconds"
    parts = data.split()
    oomcount = {}
    minscale = -100000  # Keep track of the maxdownscale for each value
    # Find out what the 'natural' unit of each value is
    for part in parts:
        # Only care about OOMs, the real scaling happens later
        value, oom = scalestring(part)
        # Track the highest maxdownscale so we do not invent precision
        ds = maxdownscale(part)
        minscale = max(ds, minscale)
        oomcount[oom] = oomcount.get(oom, 0) + 1
    # Find the most common unit
    mostcommon = 0
    highestcount = 0
    for key in oomcount.keys():
        if key < minscale:
            continue  # skip any scale that would result in making up data
        count = oomcount[key]
        if count > highestcount:
            mostcommon = key
            highestcount = count
    newunit = UNITS_SEC[mostcommon + UNITS_SEC.index(UNIT_MS)]
    # Shift all values to the new unit
    cooked = []
    for part in parts:
        part = rescalestring(part, mostcommon)
        fitted = fitinfield(part, 7)
        cooked.append(fitted)
    rendered = " ".join(cooked) + " " + UNITS_SEC[mostcommon +
                                                  UNITS_SEC.index(UNIT_MS)]
    return rendered


def getunitgroup(unit):
    "Returns the unit group which contains a given unit"
    for group in unitgroups:
        if unit in group:
            return group


def oomsbetweenunits(a, b):
    "Calculates how many orders of magnitude separate two units"
    group = getunitgroup(a)
    if b is None:  # Caller is asking for the distance from the base unit
        return group.index(a) * 3
    elif b in group:
        ia = group.index(a)
        ib = group.index(b)
        return abs((ia - ib) * 3)
    return None


def breaknumberstring(value):
    "Breaks a number string into (aboveDecimal, belowDecimal, isNegative?)"
    if value[0] == "-":
        value = value[1:]
        negative = True
    else:
        negative = False
    if "." in value:
        above, below = value.split(".")
    else:
        above = value
        below = ""
    return (above, below, negative)


def gluenumberstring(above, below, isnegative):
    "Glues together parts of a number string"
    if above == "":
        above = "0"
    if len(below) > 0:
        newvalue = ".".join((above, below))
    else:
        newvalue = above
    if isnegative is True:
        newvalue = "-" + newvalue
    return newvalue


def maxdownscale(value):
    "Maximum units a value can be scaled down without inventing data"
    if "." in value:
        digitcount = len(value.split(".")[1])
        # Return a negative so it can be fed directly to a scaling function
        return -(digitcount // 3)
    else:
        # No decimals, the value is already at the maximum down-scale
        return 0


def rescalestring(value, unitsscaled):
    "Rescale a number string by a given number of units"
    whole, dec, negative = breaknumberstring(value)
    if unitsscaled == 0:
        # This may seem redundant, but glue forces certain formatting details
        value = gluenumberstring(whole, dec, negative)
        return value
    hilen = len(whole)
    lolen = len(dec)
    digitsmoved = abs(unitsscaled * 3)
    if unitsscaled > 0:  # Scale to a larger unit, move decimal left
        if hilen < digitsmoved:
            # Scaling beyond the digits, pad it out. We can pad here
            # without making up digits that don't exist
            padcount = digitsmoved - hilen
            newwhole = ""
            newdec = ("0" * padcount) + whole + dec
        else:  # Scaling in the digits, no need to pad
            choppoint = -digitsmoved
            newdec = whole[choppoint:] + dec
            newwhole = whole[:choppoint]
    elif unitsscaled < 0:  # scale to a smaller unit, move decimal right
        if lolen < digitsmoved:
            # Scaling beyond the digits would force us to make up data
            # that doesn't exist. So fail.
            # The caller should have already caught this with maxdownscale()
            return None
        else:
            newwhole = whole + dec[:digitsmoved]
            newdec = dec[digitsmoved:]
    newwhole = newwhole.lstrip("0")
    newvalue = gluenumberstring(newwhole, newdec, negative)
    return newvalue


def formatzero(value):
    "Scale a zero value for the unit with the highest available precision"
    scale = maxdownscale(value)
    newvalue = rescalestring(value, scale).lstrip("-")
    return (newvalue, scale)


def scalestring(value):
    "Scales a number string to fit in the range 1.0-999.9"
    if isstringzero(value):
        return formatzero(value)
    whole, dec, negative = breaknumberstring(value)
    hilen = len(whole)
    if (hilen == 0) or isstringzero(whole):  # Need to shift to smaller units
        i = 0
        lolen = len(dec)
        while i < lolen:  # need to find the actual digits
            if dec[i] != "0":
                break
            i += 1
        lounits = (i // 3) + 1  # always need to shift one more unit
        movechars = lounits * 3
        if lolen < movechars:
            # Not enough digits to scale all the way down. Inventing
            # digits is unacceptable, so scale down as much as we can.
            lounits = (i // 3)  # "always", unless out of digits
            movechars = lounits * 3
        newwhole = dec[:movechars].lstrip("0")
        newdec = dec[movechars:]
        unitsmoved = -lounits
    else:  # Shift to larger units
        hiunits = hilen // 3  # How many we have, not how many to move
        hidigits = hilen % 3
        if hidigits == 0:  # full unit above the decimal
            hiunits -= 1  # the unit above the decimal doesn't count
            hidigits = 3
        newwhole = whole[:hidigits]
        newdec = whole[hidigits:] + dec
        unitsmoved = hiunits
    newvalue = gluenumberstring(newwhole, newdec, negative)
    return (newvalue, unitsmoved)


def fitinfield(value, fieldsize):
    "Attempt to fit value into a field, preserving as much data as possible"
    vallen = len(value)
    if fieldsize is None:
        newvalue = value
    elif vallen == fieldsize:  # Goldilocks!
        newvalue = value
    elif vallen < fieldsize:  # Extra room, pad it out
        pad = " " * (fieldsize - vallen)
        newvalue = pad + value
    else:  # Insufficient room, round as few digits as possible
        if "." in value:  # Ok, we *do* have decimals to crop
            diff = vallen - fieldsize
            declen = len(value.split(".")[1])  # length of decimals
            croplen = min(declen, diff)  # Never round above the decimal point
            roundlen = declen - croplen  # How many digits we round to
            newvalue = str(round(float(value), roundlen))
            splitted = newvalue.split(".")  # This should never fail
            declen = len(splitted[1])
            if roundlen == 0:  # if rounding all the decimals don't display .0
                # but do display the point, to show that there is more beyond
                newvalue = splitted[0] + "."
            elif roundlen > declen:  # some zeros have been cropped, fix that
                padcount = roundlen - declen
                newvalue = newvalue + ("0" * padcount)
        else:  # No decimals, nothing we can crop
            newvalue = value
    return newvalue


def cropprecision(value, ooms):
    "Crops digits below the maximum precision"
    if "." not in value:  # No decimals, nothing to crop
        return value
    if ooms == 0:  # We are at the baseunit, crop it all
        return value.split(".")[0]
    dstart = value.find(".") + 1
    dsize = len(value) - dstart
    precision = min(ooms, dsize)
    cropcount = dsize - precision
    if cropcount > 0:
        value = value[:-cropcount]
    return value


def isstringzero(value):
    "Detects whether a string is equal to zero"
    for i in value:
        if i not in ("-", ".", "0"):
            return False
    return True


def unitrelativeto(unit, move):
    "Returns a unit at a different scale from the input unit"
    for group in unitgroups:
        if unit in group:
            if move is None:  # asking for the base unit
                return group[0]
            else:
                index = group.index(unit)
                index += move  # index of the new unit
                if 0 <= index < len(group):  # found the new unit
                    return group[index]
                else:  # not in range
                    return None
    return None  # couldn't find anything


def unitifyvar(value, varname, baseunit=None, width=8, unitSpace=False):
    "Call unitify() with the correct units for varname"
    if varname in S_VARS:
        start = UNIT_S
    elif varname in MS_VARS:
        start = UNIT_MS
    elif varname in PPM_VARS:
        start = UNIT_PPM
    else:
        return value
    return unitify(value, start, baseunit, width, unitSpace)


def unitify(value, startingunit, baseunit=None, width=8, unitSpace=False):
    "Formats a numberstring with relevant units. Attemps to fit in width."
    if baseunit is None:
        baseunit = getunitgroup(startingunit)[0]
    ooms = oomsbetweenunits(startingunit, baseunit)
    if isstringzero(value):
        newvalue, unitsmoved = formatzero(value)
    else:
        newvalue = cropprecision(value, ooms)
        newvalue, unitsmoved = scalestring(newvalue)
    unitget = unitrelativeto(startingunit, unitsmoved)
    if unitSpace is True:
        spaceWidthAdjustment = 1
        spacer = " "
    else:
        spaceWidthAdjustment = 0
        spacer = ""
    if unitget is not None:  # We have a unit
        if width is None:
            realwidth = None
        else:
            realwidth = width - (len(unitget) + spaceWidthAdjustment)
        newvalue = fitinfield(newvalue, realwidth) + spacer + unitget
    else:  # don't have a replacement unit, use original
        newvalue = value + spacer + startingunit
    if width is None:
        newvalue = newvalue.strip()
    return newvalue


def f8dot4(f):
    "Scaled floating point formatting to fit in 8 characters"

    if isinstance(f, str):
        # a string? pass it on as a signal
        return "%8s" % f
    if not isinstance(f, (int, float)):
        # huh?
        return "       X"
    if str(float(f)).lower() == 'nan':
        # yes, this is a better test than math.isnan()
        # it also catches None, strings, etc.
        return "     nan"

    fmt = "%8d"          # xxxxxxxx or -xxxxxxx
    if f >= 0:
        if f < 1000.0:
            fmt = "%8.4f"    # xxx.xxxx  normal case
        elif f < 10000.0:
            fmt = "%8.3f"    # xxxx.xxx
        elif f < 100000.0:
            fmt = "%8.2f"    # xxxxx.xx
        elif f < 1000000.0:
            fmt = "%8.1f"    # xxxxxx.x
    else:
        # negative number, account for minus sign
        if f > -100.0:
            fmt = "%8.4f"      # -xx.xxxx  normal case
        elif f > -1000.0:
            fmt = "%8.3f"      # -xxx.xxx
        elif f > -10000.0:
            fmt = "%8.2f"      # -xxxx.xx
        elif f > -100000.0:
            fmt = "%8.1f"      # -xxxxx.x

    return fmt % f


def f8dot3(f):
    "Scaled floating point formatting to fit in 8 characters"
    if isinstance(f, str):
        # a string? pass it on as a signal
        return "%8s" % f
    if not isinstance(f, (int, float)):
        # huh?
        return "       X"
    if str(float(f)).lower() == 'nan':
        # yes, this is a better test than math.isnan()
        # it also catches None, strings, etc.
        return "     nan"

    fmt = "%8d"          # xxxxxxxx or -xxxxxxx
    if f >= 0:
        if f < 10000.0:
            fmt = "%8.3f"    # xxxx.xxx  normal case
        elif f < 100000.0:
            fmt = "%8.2f"    # xxxxx.xx
        elif f < 1000000.0:
            fmt = "%8.1f"    # xxxxxx.x
    else:
        # negative number, account for minus sign
        if f > -1000.0:
            fmt = "%8.3f"    # -xxx.xxx  normal case
        elif f > -10000.0:
            fmt = "%8.2f"    # -xxxx.xx
        elif f > -100000.0:
            fmt = "%8.1f"    # -xxxxx.x

    return fmt % f


def monoclock():
    "Try to get a monotonic clock value unaffected by NTP stepping."
    try:
        # Available in Python 3.3 and up.
        return time.monotonic()
    except AttributeError:
        return time.time()


class Cache:
    "Simple time-based cache"

    def __init__(self, defaultTimeout=300):  # 5 min default TTL
        self.defaultTimeout = defaultTimeout
        self._cache = {}

    def get(self, key):
        if key in self._cache:
            value, settime, ttl = self._cache[key]
            if settime >= monoclock() - ttl:
                return value
            else:  # key expired, delete it
                del self._cache[key]
                return None
        else:
            return None

    def set(self, key, value, customTTL=None):
        ttl = customTTL if customTTL is not None else self.defaultTimeout
        self._cache[key] = (value, monoclock(), ttl)


# A hack to avoid repeatedly hammering on DNS when ntpmon runs.
canonicalization_cache = Cache()


def canonicalize_dns(inhost, family=socket.AF_UNSPEC):
    "Canonicalize a hostname or numeric IP address."
    resname = canonicalization_cache.get(inhost)
    if resname is not None:
        return resname
    # Catch garbaged hostnames in corrupted Mode 6 responses
    m = re.match("([:.[\]]|\w)*", inhost)
    if not m:
        raise TypeError
    (hostname, portsuffix) = portsplit(inhost)
    try:
        ai = socket.getaddrinfo(hostname, None, family, 0, 0,
                                socket.AI_CANONNAME)
    except socket.gaierror:
        return "DNSFAIL:%s" % hostname
    (family, socktype, proto, canonname, sockaddr) = ai[0]
    try:
        name = socket.getnameinfo(sockaddr, socket.NI_NAMEREQD)
        result = name[0].lower() + portsuffix
    except socket.gaierror:
        # On OS X, canonname is empty for hosts without rDNS.
        # Fall back to the hostname.
        canonicalized = canonname or hostname
        result = canonicalized.lower() + portsuffix
    canonicalization_cache.set(inhost, result)
    return result


TermSize = collections.namedtuple("TermSize", ["width", "height"])


# Python 2.x does not have the shutil.get_terminal_size function.
# This conditional import is only needed by termsize() and should be kept
# near it. It is not inside the function because the unit tests need to be
# able to splice in a jig.
if str is bytes:  # We are on python 2.x
    import fcntl
    import termios
    import struct


def termsize():  # pragma: no cover
    "Return the current terminal size."
    # Alternatives at http://stackoverflow.com/questions/566746
    # The way this is used makes it not a big deal if the default is wrong.
    size = (80, 24)
    if os.isatty(1):
        if str is not bytes:
            # str is bytes means we are >py3.0, but this will still fail
            # on versions <3.3. We do not support those anyway.
            (w, h) = shutil.get_terminal_size((80, 24))
            size = (w, h)
        else:
            try:
                # OK, Python version < 3.3, cope
                h, w, hp, wp = struct.unpack(
                    'HHHH',
                    fcntl.ioctl(2, termios.TIOCGWINSZ,
                                struct.pack('HHHH', 0, 0, 0, 0)))
                size = (w, h)
            except IOError:
                pass
    return TermSize(*size)


class PeerStatusWord:
    "A peer status word from readstats(), dissected for display"

    def __init__(self, status, pktversion=ntp.magic.NTP_VERSION):
        # Event
        self.event = ntp.control.CTL_PEER_EVENT(status)
        # Event count
        self.event_count = ntp.control.CTL_PEER_NEVNT(status)
        statval = ntp.control.CTL_PEER_STATVAL(status)
        # Config
        if statval & ntp.control.CTL_PST_CONFIG:
            self.conf = "yes"
        else:
            self.conf = "no"
        # Reach
        if statval & ntp.control.CTL_PST_BCAST:
            self.reach = "none"
        elif statval & ntp.control.CTL_PST_REACH:
            self.reach = "yes"
        else:
            self.reach = "no"
        # Auth
        if (statval & ntp.control.CTL_PST_AUTHENABLE) == 0:
            self.auth = "none"
        elif statval & ntp.control.CTL_PST_AUTHENTIC:
            self.auth = "ok "
        else:
            self.auth = "bad"
        # Condition
        if pktversion > ntp.magic.NTP_OLDVERSION:
            seldict = {
                ntp.control.CTL_PST_SEL_REJECT: "reject",
                ntp.control.CTL_PST_SEL_SANE: "falsetick",
                ntp.control.CTL_PST_SEL_CORRECT: "excess",
                ntp.control.CTL_PST_SEL_SELCAND: "outlier",
                ntp.control.CTL_PST_SEL_SYNCCAND: "candidate",
                ntp.control.CTL_PST_SEL_EXCESS: "backup",
                ntp.control.CTL_PST_SEL_SYSPEER: "sys.peer",
                ntp.control.CTL_PST_SEL_PPS: "pps.peer",
                }
            self.condition = seldict[statval & 0x7]
        else:
            if (statval & 0x3) == OLD_CTL_PST_SEL_REJECT:
                if (statval & OLD_CTL_PST_SANE) == 0:
                    self.condition = "insane"
                elif (statval & OLD_CTL_PST_DISP) == 0:
                    self.condition = "hi_disp"
                else:
                    self.condition = ""
            elif (statval & 0x3) == OLD_CTL_PST_SEL_SELCAND:
                self.condition = "sel_cand"
            elif (statval & 0x3) == OLD_CTL_PST_SEL_SYNCCAND:
                self.condition = "sync_cand"
            elif (statval & 0x3) == OLD_CTL_PST_SEL_SYSPEER:
                self.condition = "sys_peer"
        # Last Event
        event_dict = {
            ntp.magic.PEVNT_MOBIL: "mobilize",
            ntp.magic.PEVNT_DEMOBIL: "demobilize",
            ntp.magic.PEVNT_UNREACH: "unreachable",
            ntp.magic.PEVNT_REACH: "reachable",
            ntp.magic.PEVNT_RESTART: "restart",
            ntp.magic.PEVNT_REPLY: "no_reply",
            ntp.magic.PEVNT_RATE: "rate_exceeded",
            ntp.magic.PEVNT_DENY: "access_denied",
            ntp.magic.PEVNT_ARMED: "leap_armed",
            ntp.magic.PEVNT_NEWPEER: "sys_peer",
            ntp.magic.PEVNT_CLOCK: "clock_alarm",
            }
        self.last_event = event_dict.get(ntp.magic.PEER_EVENT | self.event, "")

    def __str__(self):
        return ("conf=%(conf)s, reach=%(reach)s, auth=%(auth)s, "
                "cond=%(condition)s, event=%(last_event)s ec=%(event_count)s"
                % self.__dict__)


def cook(variables, showunits=False, sep=", "):
    "Cooked-mode variable display."
    width = ntp.util.termsize().width - 2
    text = ""
    specials = ("filtdelay", "filtoffset", "filtdisp", "filterror")
    longestspecial = len(max(specials, key=len))
    for (name, (value, rawvalue)) in variables.items():
        if name in specials:  # need special formatting for column alignment
            formatter = "%" + str(longestspecial) + "s ="
            item = formatter % name
        else:
            item = "%s=" % name
        if name in ("reftime", "clock", "org", "rec", "xmt"):
            item += ntp.ntpc.prettydate(value)
        elif name in ("srcadr", "peeradr", "dstadr", "refid"):
            # C ntpq cooked these in obscure ways.  Since they
            # came up from the daemon as human-readable
            # strings this was probably a bad idea, but we'll
            # leave this case separated in case somebody thinks
            # re-cooking them is a good idea.
            item += value
        elif name == "leap":
            item += ("00", "01", "10", "11")[value]
        elif name == "reach":
            item += "%03lo" % value
        elif name in specials:
            if showunits:
                item += stringfiltcooker(value)
            else:
                item += "\t".join(value.split())
        elif name == "flash":
            item += "%02x " % value
            if value == 0:
                item += "ok "
            else:
                # flasher bits
                tstflagnames = (
                    "pkt_dup",          # BOGON1
                    "pkt_bogus",        # BOGON2
                    "pkt_unsync",       # BOGON3
                    "pkt_denied",       # BOGON4
                    "pkt_auth",         # BOGON5
                    "pkt_stratum",      # BOGON6
                    "pkt_header",       # BOGON7
                    "pkt_autokey",      # BOGON8
                    "pkt_crypto",       # BOGON9
                    "peer_stratum",     # BOGON10
                    "peer_dist",        # BOGON11
                    "peer_loop",        # BOGON12
                    "peer_unreach"      # BOGON13
                )
                for (i, n) in enumerate(tstflagnames):
                    if (1 << i) & value:
                        item += tstflagnames[i] + " "
            item = item[:-1]
        elif name in MS_VARS:
            #  Note that this is *not* complete, there are definitely
            #   missing variables here.
            #  Completion cannot occur until all units are tracked down.
            if showunits:
                item += unitify(rawvalue, UNIT_MS, UNIT_NS, width=None)
            else:
                item += repr(value)
        elif name in S_VARS:
            if showunits:
                item += unitify(rawvalue, UNIT_S, UNIT_NS, width=None)
            else:
                item += repr(value)
        elif name in PPM_VARS:
            if showunits:
                item += unitify(rawvalue, UNIT_PPM, width=None)
            else:
                item += repr(value)
        else:
            item += repr(value)
        # add field separator
        item += sep
        # add newline so we don't overflow screen
        lastcount = 0
        for c in text:
            if c == '\n':
                lastcount = 0
            else:
                lastcount += 1
        if lastcount + len(item) > width:
            text = text[:-1] + "\n"
        text += item
    text = text[:-2] + "\n"
    return text


class PeerSummary:
    "Reusable report generator for peer statistics"

    def __init__(self, displaymode, pktversion, showhostnames,
                 wideremote, showunits=False, termwidth=None,
                 debug=0, logfp=sys.stderr):
        self.displaymode = displaymode          # peers/apeers/opeers
        self.pktversion = pktversion            # interpretation of flash bits
        self.showhostnames = showhostnames      # If false, display numeric IPs
        self.showunits = showunits              # If False show old style float
        self.wideremote = wideremote            # show wide remote names?
        self.debug = debug
        self.logfp = logfp
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
        self.__common = "st t when poll reach   delay   offset   "
        self.__header = None
        self.polls = []

    @staticmethod
    def prettyinterval(diff):
        "Print an interval in natural time units."
        if not isinstance(diff, int) or diff <= 0:
            return '-'
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

    @staticmethod
    def is_clock(variables):
        "Does a set of variables look like it returned from a clock?"
        return "srchost" in variables and '(' in variables["srchost"][0]

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
            self.__header = self.__remote + \
                "       refid      ".ljust(self.refidwidth) + \
                self.__common + "jitter"
        return self.__header

    def width(self):
        "Width of display"
        return 79 + self.horizontal_slack

    def summary(self, rstatus, variables, associd):
        "Peer status summary line."
        clock_name = ''
        dstadr_refid = ""
        dstport = 0
        estdelay = '.'
        estdisp = '.'
        estjitter = '.'
        estoffset = '.'
        filtdelay = 0.0
        filtdisp = 0.0
        filtoffset = 0.0
        flash = 0
        have_jitter = False
        headway = 0
        hmode = 0
        hpoll = 0
        keyid = 0
        last_sync = None
        leap = 0
        pmode = 0
        ppoll = 0
        precision = 0
        ptype = '?'
        reach = 0
        rec = None
        reftime = None
        rootdelay = 0.0
        saw6 = False        # x.6 floats for delay and friends
        srcadr = None
        srchost = None
        srcport = 0
        stratum = 20
        mode = 0
        unreach = 0
        xmt = 0

        now = time.time()

        for item in variables.items():
            if 2 != len(item) or 2 != len(item[1]):
                # bad item
                continue
            (name, (value, rawvalue)) = item
            if name == "delay":
                estdelay = rawvalue if self.showunits else value
                if len(rawvalue) > 6 and rawvalue[-7] == ".":
                    saw6 = True
            elif name == "dstadr":
                # The C code tried to get a fallback ptype from this in case
                # the hmode field was not included
                if "local" in self.__header:
                    dstadr_refid = rawvalue
            elif name == "dstport":
                # FIXME, dstport never used.
                dstport = value
            elif name == "filtdelay":
                # FIXME, filtdelay never used.
                filtdelay = value
            elif name == "filtdisp":
                # FIXME, filtdisp never used.
                filtdisp = value
            elif name == "filtoffset":
                # FIXME, filtoffset never used.
                filtoffset = value
            elif name == "flash":
                # FIXME, flash never used.
                flash = value
            elif name == "headway":
                # FIXME, headway never used.
                headway = value
            elif name == "hmode":
                hmode = value
            elif name == "hpoll":
                hpoll = value
                if hpoll < 0:
                    hpoll = ntp.magic.NTP_MINPOLL
            elif name == "jitter":
                if "jitter" in self.__header:
                    estjitter = rawvalue if self.showunits else value
                    have_jitter = True
            elif name == "keyid":
                # FIXME, keyid never used.
                keyid = value
            elif name == "leap":
                # FIXME, leap never used.
                leap = value
            elif name == "offset":
                estoffset = rawvalue if self.showunits else value
            elif name == "pmode":
                # FIXME, pmode never used.
                pmode = value
            elif name == "ppoll":
                ppoll = value
                if ppoll < 0:
                    ppoll = ntp.magic.NTP_MINPOLL
            elif name == "precision":
                # FIXME, precision never used.
                precision = value
            elif name == "reach":
                # Shipped as hex, displayed in octal
                reach = value
            elif name == "refid":
                # The C code for this looked crazily overelaborate.  Best
                # guess is that it was designed to deal with formats that
                # no longer occur in this field.
                if "refid" in self.__header:
                    dstadr_refid = rawvalue
            elif name == "rec":
                rec = value         # l_fp timestamp
                last_sync = int(now - ntp.ntpc.lfptofloat(rec))
            elif name == "reftime":
                reftime = value     # l_fp timestamp
                last_sync = int(now - ntp.ntpc.lfptofloat(reftime))
            elif name == "rootdelay":
                # FIXME, rootdelay never used.
                rootdelay = value   # l_fp timestamp
            elif name == "rootdisp" or name == "dispersion":
                estdisp = rawvalue if self.showunits else value
            elif name in ("srcadr", "peeradr"):
                srcadr = value
            elif name == "srchost":
                srchost = value
            elif name == "srcport" or name == "peerport":
                # FIXME, srcport never used.
                srcport = value
            elif name == "stratum":
                stratum = value
            elif name == "mode":
                # FIXME, mode never used.
                mode = value
            elif name == "unreach":
                # FIXME, unreach never used.
                unreach = value
            elif name == "xmt":
                # FIXME, xmt never used.
                xmt = value
            else:
                # unknown name?
                # line = " name=%s " % (name)    # debug
                # return line                    # debug
                continue
        if hmode == ntp.magic.MODE_BCLIENTX:
            # broadcastclient or multicastclient
            ptype = 'b'
        elif hmode == ntp.magic.MODE_BROADCAST:
            # broadcast or multicast server
            if srcadr.startswith("224."):       # IANA multicast address prefix
                ptype = 'M'
            else:
                ptype = 'B'
        elif hmode == ntp.magic.MODE_CLIENT:
            if PeerSummary.is_clock(variables):
                ptype = 'l'     # local refclock
            elif dstadr_refid == "POOL":
                ptype = 'p'     # pool
            elif srcadr.startswith("224."):
                ptype = 'a'     # manycastclient
            else:
                ptype = 'u'     # unicast
        elif hmode == ntp.magic.MODE_ACTIVE:
            ptype = 's'         # symmetric active
        elif hmode == ntp.magic.MODE_PASSIVE:
            ptype = 'S'         # symmetric passive

        #
        # Got everything, format the line
        #
        line = ""
        poll_sec = 1 << min(ppoll, hpoll)
        self.polls.append(poll_sec)
        if self.pktversion > ntp.magic.NTP_OLDVERSION:
            c = " x.-+#*o"[ntp.control.CTL_PEER_STATVAL(rstatus) & 0x7]
        else:
            c = " .+*"[ntp.control.CTL_PEER_STATVAL(rstatus) & 0x3]
        # Source host or clockname or poolname or servername
        # After new DNS, 2017-Apr-17
        # servers setup via numerical IP Address have only srcadr
        # servers setup via DNS have both srcadr and srchost
        # refclocks have both srcadr and srchost
        # pool has "0.0.0.0" (or "::") and srchost
        # slots setup via pool have only srcadr
        if srcadr is not None \
                and srcadr != "0.0.0.0" \
                and srcadr[:7] != "127.127" \
                and srcadr != "::":
            if self.showhostnames:
                try:
                    if self.debug:
                        self.logfp.write("DNS lookup begins...\n")
                    clock_name = canonicalize_dns(srcadr)
                    if self.debug:
                        self.logfp.write("DNS lookup ends.\n")
                except TypeError:  # pragma: no cover
                    return ''
            else:
                clock_name = srcadr
        else:
            clock_name = srchost
        if clock_name is None:
            if srcadr:
                clock_name = srcadr
            else:
                clock_name = ""
        if self.wideremote and len(clock_name) > self.namewidth:
            line += ("%c%s\n" % (c, clock_name))
            line += (" " * (self.namewidth + 2))
        else:
            line += ("%c%-*.*s " % (c, self.namewidth, self.namewidth,
                                    clock_name[:self.namewidth]))
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
        if last_sync is None:
            last_sync = now
        jd = estjitter if have_jitter else estdisp
        line += (
            " %2ld %c %4.4s %4.4s  %3lo"
            % (stratum, ptype,
               PeerSummary.prettyinterval(last_sync),
               PeerSummary.prettyinterval(poll_sec), reach))
        if saw6:
            if self.showunits:
                line += (
                    " %s %s %s" %
                    (unitify(estdelay, UNIT_MS),
                     unitify(estoffset, UNIT_MS),
                     unitify(jd, UNIT_MS)))
            else:
                line += (
                    " %s %s %s" %
                    (f8dot4(estdelay), f8dot4(estoffset), f8dot4(jd)))
        else:
            # old servers only have 3 digits of fraction
            # don't print a fake 4th digit
            if self.showunits:
                line += (
                    " %s %s %s" %
                    (unitify(estdelay, UNIT_MS),
                     unitify(estoffset, UNIT_MS),
                     unitify(jd, UNIT_MS)))
            else:
                line += (
                    " %s %s %s" %
                    (f8dot3(estdelay), f8dot3(estoffset), f8dot3(jd)))
        line += "\n"
        # for debugging both case
        # if srcadr != None and srchost != None:
        #   line += "srcadr: %s, srchost: %s\n" % (srcadr, srchost)
        return line

    def intervals(self):
        "Return and flush the list of actual poll intervals."
        res = self.polls[:]
        self.polls = []
        return res


class MRUSummary:
    "Reusable class for MRU entry summary generation."

    def __init__(self, showhostnames, wideremote=False,
                 debug=0, logfp=sys.stderr):
        self.debug = debug
        self.logfp = logfp
        self.now = None
        self.showhostnames = showhostnames      # If false, display numeric IPs
        self.wideremote = wideremote

    header = " lstint avgint rstr r m v  count rport remote address"

    def summary(self, entry):
        last = ntp.ntpc.lfptofloat(entry.last)
        if self.now:
            lstint = int(self.now - last + 0.5)
            stats = "%7d" % lstint
        else:
            # direct mode doesn't have a reference time
            MJD_1970 = 40587     # MJD for 1 Jan 1970, Unix epoch
            days, lstint = divmod(int(last), 86400)
            stats = "%5d %5d" % (days + MJD_1970, lstint)
        first = ntp.ntpc.lfptofloat(entry.first)
        active = float(last - first)
        if entry.ct == 1:
            favgint = 0
        else:
            favgint = active / (entry.ct-1)
        avgint = int(favgint + 0.5)
        if 5.0 < favgint or 1 == entry.ct:
            stats += " %6d" % avgint
        elif 1.0 <= favgint:
            stats += " %6.2f" % favgint
        else:
            stats += " %6.3f" % favgint
        if entry.rs & ntp.magic.RES_KOD:
            rscode = 'K'
        elif entry.rs & ntp.magic.RES_LIMITED:
            rscode = 'L'
        else:
            rscode = '.'
        (ip, port) = portsplit(entry.addr)
        try:
            if not self.showhostnames:
                dns = ip
            else:
                dns = canonicalize_dns(ip)
                # Forward-confirm the returned DNS
                confirmed = canonicalization_cache.get(dns)
                if confirmed is None:
                    confirmed = False
                    try:
                        ai = socket.getaddrinfo(dns, None)
                        for (_, _, _, _, sockaddr) in ai:
                            if sockaddr and sockaddr[0] == ip:
                                confirmed = True
                                break
                    except socket.gaierror:
                        pass
                    canonicalization_cache.set(dns, confirmed)
                if not confirmed:
                    dns = "%s (%s)" % (ip, dns)
            if not self.wideremote:
                # truncate for narrow display
                dns = dns[:40]
            stats += " %4hx %c %d %d %6d %5s %s" % \
                     (entry.rs, rscode,
                      ntp.magic.PKT_MODE(entry.mv),
                      ntp.magic.PKT_VERSION(entry.mv),
                      entry.ct, port[1:], dns)
            return stats
        except ValueError:
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
        prefix = sum([bin(int(x, base)).count('1')
                      for x in mask.split(sep) if x])
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
            if not c.isalnum() and c not in "/.: \n":
                return ''
        return s


class IfstatsSummary:
    "Reusable class for ifstats entry summary generation."
    header = """\
    interface name                                  send
 #  address/broadcast     drop flag received sent failed peers   uptime
 """
    width = 74
    # Numbers are the fieldsize
    fields = {'name':  '%-24.24s',
              'flags': '%4x',
              'rx':    '%6d',
              'tx':    '%6d',
              'txerr': '%6d',
              'pc':    '%5d',
              'up':    '%8d'}

    def summary(self, i, variables):
        formatted = {}
        try:
            # Format the fields
            for name in self.fields.keys():
                value = variables.get(name, "?")
                if value == "?":
                    fmt = value
                else:
                    fmt = self.fields[name] % value
                formatted[name] = fmt
            enFlag = '.' if variables.get('en', False) else 'D'
            address = variables.get("addr", "?")
            bcast = variables.get("bcast")
            # Assemble the fields into a line
            s = ("%3u %s %s %s %s %s %s %s %s\n    %s\n"
                 % (i,
                    formatted['name'],
                    enFlag,
                    formatted['flags'],
                    formatted['rx'],
                    formatted['tx'],
                    formatted['txerr'],
                    formatted['pc'],
                    formatted['up'],
                    address))
            if bcast:
                s += "    %s\n" % bcast
        except TypeError:  # pragma: no cover
            # Can happen when ntpd ships a corrupted response
            return ''

        # FIXME, a brutal and slow way to check for invalid chars..
        # maybe just strip non-printing chars?
        for c in s:
            if not c.isalnum() and c not in "/.:[] \%\n":
                return ''
        return s


try:
    from collections import OrderedDict
except ImportError:  # pragma: no cover
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
