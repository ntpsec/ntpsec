#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, division

import sys
import os
import getopt
import time
import socket
import subprocess

try:
    import ntp.packet
    import ntp.util
    import ntp.agentx_packet
    ax = ntp.agentx_packet
    from ntp.agentx import PacketControl
except ImportError as e:
    sys.stderr.write(
        "ntpsnmpd: can't find Python NTP library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


# TODO This is either necessary, or a different workaround is.
ntp.util.deunicode_units()

logfp = sys.stderr
nofork = False
debug = 0
defaultTimeout = 30

log = (lambda msg, msgdbg: ntp.util.dolog(logfp, msg, debug, msgdbg))

ntpRootOID = (1, 3, 6, 1, 2, 1, 197)  # mib-2 . 197, aka: NTPv4-MIB

snmpTrapOID = (1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0)
snmpSysUptime = (1, 3, 6, 1, 2, 1, 1, 3, 0)

DEFHOST = "localhost"
DEFLOG = "ntpsnmpd.log"


class DataSource(ntp.agentx.MIBControl):
    def __init__(self, hostname=DEFHOST, settingsFile=None, notifySpin=0.1):
        # This is defined as a dict tree because it is simpler, and avoids
        # certain edge cases
        # OIDs are relative from ntp root
        ntp.agentx.MIBControl.__init__(self, mibRoot=ntpRootOID)
        # MIB node init
        # block 0
        self.addNode((0,))  # ntpEntNotifications
        self.addNode((0, 1))  # ntpEntNotifModeChange
        self.addNode((0, 2))  # ntpEntNotifStratumChange
        self.addNode((0, 3))  # ntpEntNotifSyspeerChange
        self.addNode((0, 4))  # ntpEntNotifAddAssociation
        self.addNode((0, 5))  # ntpEntNotifRemoveAsociation
        self.addNode((0, 6))  # ntpEntNotifConfigChanged
        self.addNode((0, 7))  # ntpEntNotifLeapSecondAnnounced
        self.addNode((0, 8))  # ntpEntNotifHeartbeat
        # block 1
        # block 1, 1
        self.addNode((1, 1, 1, 0),  # ntpNetSoftwareName utf8str
                     (lambda oid: self.cbr_systemInfo(oid, "name")))
        self.addNode((1, 1, 2, 0),  # ntpEntSoftwareVersion utf8str
                     (lambda oid: self.cbr_systemInfo(oid, "version")))
        self.addNode((1, 1, 3, 0),  # ntpEntSoftwareVendor utf8str
                     (lambda oid: self.cbr_systemInfo(oid, "vendor")))
        self.addNode((1, 1, 4, 0),  # ntpEntSystemType utf8str
                     (lambda oid: self.cbr_systemInfo(oid, "system")))
        self.addNode((1, 1, 5, 0),  # ntpEntTimeResolution uint32
                     self.cbr_timeResolution)
        self.addNode((1, 1, 6, 0),  # ntpEntTimePrecision int32
                     self.cbr_timePrecision)
        self.addNode((1, 1, 7, 0),  # ntpEntTimeDistance DisplayString
                     self.cbr_timeDistance)
        # block 1, 2
        self.addNode((1, 2, 1, 0),  # ntpEntStatusCurrentMode INTEGER {...}
                     self.cbr_statusCurrentMode)
        self.addNode((1, 2, 2, 0),  # ntpEntStatusStratum NtpStratum
                     self.cbr_statusStratum)
        self.addNode((1, 2, 3, 0),  # ntpEntStatusActiveRefSourceId uint32
                     self.cbr_statusActiveRefSourceID)
        self.addNode((1, 2, 4, 0),  # ntpEntStatusActiveRefSourceName utf8str
                     self.cbr_statusActiveRefSourceName)
        self.addNode((1, 2, 5, 0),  # ntpEntStatusActiveOffset DisplayString
                     self.cbr_statusActiveOffset)
        self.addNode((1, 2, 6, 0),  # ntpEntStatusNumberOfRefSources unit32
                     self.cbr_statusNumRefSources)
        self.addNode((1, 2, 7, 0),  # ntpEntStatusDispersion DisplayString
                     self.cbr_statusDispersion)
        self.addNode((1, 2, 8, 0),  # ntpEntStatusEntityUptime TimeTicks
                     self.cbr_statusEntityUptime)
        self.addNode((1, 2, 9, 0),  # ntpEntStatusDateTime NtpDateTime
                     self.cbr_statusDateTime)
        self.addNode((1, 2, 10, 0),  # ntpEntStatusLeapSecond NtpDateTime
                     self.cbr_statusLeapSecond)
        self.addNode((1, 2, 11, 0),  # ntpEntStatusLeapSecondDirection int32
                     self.cbr_statusLeapSecDirection)
        self.addNode((1, 2, 12, 0),  # ntpEntStatusInPkts Counter32
                     self.cbr_statusInPkts)
        self.addNode((1, 2, 13, 0),  # ntpEntStatusOutPkts Counter32
                     self.cbr_statusOutPkts)
        self.addNode((1, 2, 14, 0),  # ntpEntStatusBadVersion Counter32
                     self.cbr_statusBadVersion)
        self.addNode((1, 2, 15, 0),  # ntpEntStatusProtocolError Counter32
                     self.cbr_statusProtocolError)
        self.addNode((1, 2, 16, 0),  # ntpEntStatusNotifications Counter32
                     self.cbr_statusNotifications)
        self.addNode((1, 2, 17, 1, 1))  # ntpEntStatPktMode INTEGER {...}
        self.addNode((1, 2, 17, 1, 2))  # ntpEntStatPktSent Counter32
        self.addNode((1, 2, 17, 1, 3))  # ntpEntStatPktRecived Counter32
        # block 1, 3
        self.addNode((1, 3, 1, 1, 1),  # ntpAssocId uint32 (1..99999)
                     dynamic=self.sub_assocID)
        self.addNode((1, 3, 1, 1, 2),  # ntpAssocName utf8str
                     dynamic=self.sub_assocName)
        self.addNode((1, 3, 1, 1, 3),  # ntpAssocRefId DisplayString
                     dynamic=self.sub_assocRefID)
        self.addNode((1, 3, 1, 1, 4),  # ntpAssocAddressType InetAddressType
                     dynamic=self.sub_assocAddrType)
        self.addNode((1, 3, 1, 1, 5),  # ntpAssocAddress InetAddress SIZE
                     dynamic=self.sub_assocAddr)
        self.addNode((1, 3, 1, 1, 6),  # ntpAssocOffset DisplayString
                     dynamic=self.sub_assocOffset)
        self.addNode((1, 3, 1, 1, 7),  # ntpAssocStratum NtpStratum
                     dynamic=self.sub_assocStratum)
        self.addNode((1, 3, 1, 1, 8),  # ntpAssocStatusJitter DisplayString
                     dynamic=self.sub_assocJitter)
        self.addNode((1, 3, 1, 1, 9),  # ntpAssocStatusDelay DisplayString
                     dynamic=self.sub_assocDelay)
        self.addNode((1, 3, 1, 1, 10),  # ntpAssocStatusDispersion DisplayStr
                     dynamic=self.sub_assocDispersion)
        self.addNode((1, 3, 2, 1, 1),  # ntpAssocStatInPkts Counter32
                     dynamic=self.sub_assocStatInPkts)
        self.addNode((1, 3, 2, 1, 2),  # ntpAssocStatOutPkts Counter32
                     dynamic=self.sub_assocStatOutPkts)
        self.addNode((1, 3, 2, 1, 3),  # ntpAssocStatProtocolError Counter32
                     dynamic=self.sub_assocStatProtoErr)
        # block 1, 4
        self.addNode((1, 4, 1, 0),  # ntpEntHeartbeatInterval unit32
                     self.cbr_entHeartbeatInterval,
                     self.cbw_entHeartbeatInterval)
        self.addNode((1, 4, 2, 0),  # ntpEntNotifBits BITS {...}
                     self.cbr_entNotifBits,
                     self.cbw_entNotifBits)
        # block 1, 5
        self.addNode((1, 5, 1, 0),  # ntpEntNotifMessage utf8str
                     self.cbr_entNotifMessage)
        # block 2 # all compliance statements
        # print(repr(self.oidTree))
        # print(self.oidTree[1]["subids"][1][1][0])
        self.session = ntp.packet.ControlSession()
        self.hostname = hostname if hostname else DEFHOST
        self.session.openhost(self.hostname)
        self.settingsFilename = settingsFile
        # Cache so we don't hammer ntpd, default 1 second timeout
        # Timeout default pulled from a hat: we don't want it to last for
        # long, just not flood ntpd with duplicatte requests during a walk.
        self.cache = ntp.util.Cache(1)
        self.oldValues = {}  # Used by notifications to detect changes
        # spinGap so we don't spam ntpd with requests during notify checks
        self.notifySpinTime = notifySpin
        self.lastNotifyCheck = 0
        self.lastHeartbeat = 0  # Timestamp used for heartbeat notifications
        self.heartbeatInterval = 0  # should save to disk
        self.sentNotifications = 0
        # Notify bits, they control whether the daemon sends notifications.
        # these are saved to disk
        self.notifyModeChange = False  # 1
        self.notifyStratumChange = False  # 2
        self.notifySyspeerChange = False  # 3
        self.notifyAddAssociation = False  # 4
        self.notifyRMAssociation = False  # 5
        self.notifyConfigChange = False  # 6 [This is not implemented]
        self.notifyLeapSecondAnnounced = False  # 7
        self.notifyHeartbeat = False  # 8
        self.misc_loadDynamicSettings()

    # =============================================================
    # Data read callbacks start here
    # comment divider lines represent not yet implemented callbacks
    # =============================================================

    # Blank: notification OIDs

    def cbr_systemInfo(self, oid, category=None):
        if category == "name":  # The product name of the running NTP
            data = "NTPsec"
        elif category == "version":  # version string
            data = ntp.util.stdversion()
        elif category == "vendor":  # vendor/author name
            data = "Internet Civil Engineering Institute"
        elif category == "system":  # system / hardware info
            proc = subprocess.Popen(["uname", "-srm"],
                                    stdout=subprocess.PIPE)
            data = proc.communicate()[0]
        vb = ax.Varbind(ax.VALUE_OCTET_STR, oid, data)
        return vb

    def cbr_timeResolution(self, oid):
        # Uinteger32
        # Arrives in fractional milliseconds
        fuzz = self.safeReadvar(0, ["fuzz"])
        if fuzz is None:
            return None
        fuzz = fuzz["fuzz"]
        # We want to emit fractions of seconds
        # Yes we are flooring instead of rounding: don't want to emit a
        # resolution value higher than ntpd actually produces.
        if fuzz != 0:
            fuzz = int(1 / fuzz)
        else:
            fuzz = 0
        return ax.Varbind(ax.VALUE_GAUGE32, oid, fuzz)

    def cbr_timePrecision(self, oid):
        return self.readCallbackSkeletonSimple(oid, "precision",
                                               ax.VALUE_INTEGER)

    def cbr_timeDistance(self, oid):
        # Displaystring
        data = self.safeReadvar(0, ["rootdist"], raw=True)
        if data is None:
            return None
        data = ntp.util.unitifyvar(data["rootdist"][1], "rootdist",
                                   width=None, unitSpace=True)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    # Blank: ntpEntStatus

    def cbr_statusCurrentMode(self, oid):
        mode = self.misc_getMode()
        return ax.Varbind(ax.VALUE_INTEGER, oid, mode)

    def cbr_statusStratum(self, oid):
        # NTPstratum
        return self.readCallbackSkeletonSimple(oid, "stratum",
                                               ax.VALUE_GAUGE32)

    def cbr_statusActiveRefSourceID(self, oid):
        # range of uint32
        syspeer = self.misc_getSyspeerID()
        return ax.Varbind(ax.VALUE_GAUGE32, oid, syspeer)

    def cbr_statusActiveRefSourceName(self, oid):
        # utf8
        data = self.safeReadvar(0, ["peeradr"])
        if data is None:
            return None
        data = ntp.util.canonicalize_dns(data["peeradr"])
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    def cbr_statusActiveOffset(self, oid):
        # DisplayString
        data = self.safeReadvar(0, ["koffset"], raw=True)
        if data is None:
            return None
        data = ntp.util.unitifyvar(data["koffset"][1], "koffset",
                                   width=None, unitSpace=True)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    def cbr_statusNumRefSources(self, oid):
        # range of uint32
        try:
            data = self.session.readstat()
            return ax.Varbind(ax.VALUE_GAUGE32, oid, len(data))
        except ntp.packet.ControlException:
            return None

    def cbr_statusDispersion(self, oid):
        # DisplayString
        data = self.safeReadvar(0, ["rootdisp"], raw=True)
        if data is None:
            return None
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data["rootdisp"][1])

    def cbr_statusEntityUptime(self, oid):
        # TimeTicks
        # What the spec claims:
        #   The uptime of the NTP entity, (i.e., the time since ntpd was
        #   (re-)initialized not sysUptime!).  The time is represented in
        #   hundreds of seconds since Jan 1, 1970 (00:00:00.000) UTC.
        #
        # First problem: TimeTicks represents hundred*ths* of seconds, could
        #  easily be a typo.
        # Second problem: snmpwalk will happily give you a display of
        #  how long a period of time a value is, such as uptime since start.
        #  That is the opposite of what the spec claims.
        #
        # I am abandoning the spec, and going with what makes a lick of sense
        uptime = self.safeReadvar(0, ["ss_reset"])
        if uptime is None:
            return None
        uptime = uptime["ss_reset"] * 100
        return ax.Varbind(ax.VALUE_TIME_TICKS, oid, uptime)

    def cbr_statusDateTime(self, oid):
        # NtpDateTime
        data = self.safeReadvar(0, ["reftime"])
        if data is None:
            return None
        txt = data["reftime"]
        value = ntp.util.deformatNTPTime(txt)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, value)

    def cbr_statusLeapSecond(self, oid):  # I am not confident in this yet
        # NtpDateTime
        DAY = 86400
        fmt = "%.8x%.8x"
        data = self.safeReadvar(0, ["reftime"])
        hasleap = self.safeReadvar(0, ["leap"])
        if (data is None) or (hasleap is None):
            return None
        data = data["reftime"]
        hasleap = hasleap["leap"]
        if hasleap in (1, 2):
            seconds = int(data.split(".")[0], 0)
            days = seconds // DAY
            scheduled = (days * DAY) + (DAY - 1)  # 23:59:59 of $CURRENT_DAY
            formatted = fmt % (scheduled, 0)
        else:
            formatted = fmt % (0, 0)
        value = ntp.util.hexstr2octets(formatted)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, value)

    def cbr_statusLeapSecDirection(self, oid):
        # range of int32
        leap = self.safeReadvar(0, ["leap"])
        if leap is None:
            return None
        leap = leap["leap"]
        if leap == 1:
            pass  # leap 1 == forward
        elif leap == 2:
            leap = -1  # leap 2 == backward
        else:
            leap = 0  # leap 0 or 3 == no change
        return ax.Varbind(ax.VALUE_INTEGER, oid, leap)

    def cbr_statusInPkts(self, oid):
        return self.readCallbackSkeletonSimple(oid, "io_received",
                                               ax.VALUE_COUNTER32)

    def cbr_statusOutPkts(self, oid):
        return self.readCallbackSkeletonSimple(oid, "io_sent",
                                               ax.VALUE_COUNTER32)

    def cbr_statusBadVersion(self, oid):
        return self.readCallbackSkeletonSimple(oid, "ss_oldver",
                                               ax.VALUE_COUNTER32)

    def cbr_statusProtocolError(self, oid):
        data = self.safeReadvar(0, ["ss_badformat", "ss_badauth"])
        if data is None:
            return None
        protoerr = 0
        for key in data.keys():
            protoerr += data[key]
        return ax.Varbind(ax.VALUE_COUNTER32, oid, protoerr)

    def cbr_statusNotifications(self, oid):
        return ax.Varbind(ax.VALUE_COUNTER32, oid, self.sentNotifications)

    ##############################

    # == Dynamics ==
    # assocID
    # assocName
    # assocRefID
    # assocAddrType
    # assocAddr
    # assocOffset
    # assocStratum
    # assocJitter
    # assocDelay
    # assocDispersion
    # assocInPackets
    # assocOutPackets
    # assocProtocolErrors

    #########################

    def cbr_entHeartbeatInterval(self, oid):
        # uint32
        return ax.Varbind(ax.VALUE_GAUGE32, oid, self.heartbeatInterval)

    def cbr_entNotifBits(self, oid):
        # BITS
        data = ax.bools2Bits((False,  # notUsed(0)
                              self.notifyModeChange,
                              self.notifyStratumChange,
                              self.notifySyspeerChange,
                              self.notifyAddAssociation,
                              self.notifyRMAssociation,
                              self.notifyConfigChange,
                              self.notifyLeapSecondAnnounced,
                              self.notifyHeartbeat))
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    ##########################

    def cbr_entNotifMessage(self, oid):
        # utf8str
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, "no event")

    #########################

    # =====================================
    # Data write callbacks
    # Returns an error value (or noError)
    # Must check that the value is correct for the bind, this does not mean
    #  the type: the master agent handles that
    # Actions: test, undo, commit, cleanup
    # =====================================

    def cbw_entHeartbeatInterval(self, action, varbind, oldData=None):
        if action == "test":
            return ax.ERR_NOERROR
        elif action == "commit":
            self.heartbeatInterval = varbind.payload
            self.misc_storeDynamicSettings()
            return ax.ERR_NOERROR
        elif action == "undo":
            self.heartbeatInterval = oldData
            self.misc_storeDynamicSettings()
            return ax.ERR_NOERROR
        elif action == "cleanup":
            pass

    def cbw_entNotifBits(self, action, varbind, oldData=None):
        if action == "test":
            return ax.ERR_NOERROR
        elif action == "commit":
            (self.notifyModeChange,
             self.notifyStratumChange,
             self.notifySyspeerChange,
             self.notifyAddAssociation,
             self.notifyRMAssociation,
             self.notifyConfigChange,
             self.notifyLeapSecondAnnounced,
             self.notifyHeartbeat) = ax.bits2Bools(varbind.payload, 8)
            self.misc_storeDynamicSettings()
            return ax.ERR_NOERROR
        elif action == "undo":
            (self.notifyModeChange,
             self.notifyStratumChange,
             self.notifySyspeerChange,
             self.notifyAddAssociation,
             self.notifyRMAssociation,
             self.notifyConfigChange,
             self.notifyLeapSecondAnnounced,
             self.notifyHeartbeat) = ax.bits2Bools(oldData, 8)
            self.misc_storeDynamicSettings()
            return ax.ERR_NOERROR
        elif action == "cleanup":
            pass

    # ========================================================================
    # Dynamic tree generator callbacks
    #
    # The structure of these callbacks is somewhat complicated because they
    # share code that is potentially finicky.
    #
    # The dynamicCallbackSkeleton() method handles the construction of the
    # MIB tree, and the placement of the handler() within it. It also provides
    # some useful data to the handler() via the readCallback() layer.
    # ========================================================================

    # Packet Mode Table
    # These are left as stubs for now. Information is lacking on where the
    # data should come from.

    def sub_statPktMode(self):
        pass

    def sub_statPktSent(self):
        pass

    def sub_statPktRecv(self):
        pass

    # Association Table

    def sub_assocID(self):
        def handler(oid, associd):
            return ax.Varbind(ax.VALUE_GAUGE32, oid, associd)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocName(self):
        return self.dynamicCallbackPeerdata("srcadr", True,
                                            ax.VALUE_OCTET_STR)

    def sub_assocRefID(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return None
            # elaborate code in util.py indicates this may not be stable
            try:
                refid = pdata[associd]["refid"][1]
            except IndexError:
                refid = ""
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, refid)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocAddrType(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return None
            srcadr = pdata[associd]["srcadr"][1]
            try:
                socklen = len(socket.getaddrinfo(srcadr, None)[0][-1])
            except socket.gaierror:
                socklen = None
            if socklen == 2:  # ipv4
                addrtype = 1
            elif socklen == 4:  # ipv6
                addrtype = 2
            else:
                # there is also ipv4z and ipv6z..... don't know how to
                # detect those yet. Or if I even need to.
                addrtype = 0  # is this ok? or should it return a NULL?
            return ax.Varbind(ax.VALUE_INTEGER, oid, addrtype)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocAddr(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return None
            srcadr = pdata[associd]["srcadr"][1]
            # WARNING: I am only guessing that this is correct
            # Discover what type of address we have
            try:
                sockinfo = socket.getaddrinfo(srcadr, None)[0][-1]
                addr = sockinfo[0]
                ipv6 = True if len(sockinfo) == 4 else False
            except socket.gaierror:
                addr = None  # how to handle?
                ipv6 = None
            # Convert address string to octets
            srcadr = []
            if ipv6 is False:
                pieces = addr.split(".")
                for piece in pieces:
                    try:
                        srcadr.append(int(piece))  # feed it a list of ints
                    except ValueError:
                        # Have gotten piece == "" before. Skip over that.
                        # Still try to return data because it is potential
                        # debugging information.
                        continue
            elif ipv6 is True:
                pieces = addr.split(":")
                for piece in pieces:
                    srcadr.append(ntp.util.hexstr2octets(piece))
                srcadr = "".join(srcadr)  # feed it an octet string
            # The octet string encoder can handle either chars or 0-255
            # ints. We use both of those options.
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, srcadr)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocOffset(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return None
            offset = pdata[associd]["offset"][1]
            offset = ntp.util.unitifyvar(offset, "offset", width=None,
                                         unitSpace=True)
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, offset)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStratum(self):
        return self.dynamicCallbackPeerdata("stratum", False,
                                            ax.VALUE_GAUGE32)

    def sub_assocJitter(self):
        return self.dynamicCallbackPeerdata("jitter", True,
                                            ax.VALUE_OCTET_STR)

    def sub_assocDelay(self):
        return self.dynamicCallbackPeerdata("delay", True,
                                            ax.VALUE_OCTET_STR)

    def sub_assocDispersion(self):
        return self.dynamicCallbackPeerdata("rootdisp", True,
                                            ax.VALUE_OCTET_STR)

    def sub_assocStatInPkts(self):
        def handler(oid, associd):
            inpkts = self.safeReadvar(associd, ["received"])
            if inpkts is None:
                return None
            inpkts = inpkts["received"]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, inpkts)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStatOutPkts(self):
        def handler(oid, associd):
            outpkts = self.safeReadvar(associd, ["sent"])
            if outpkts is None:
                return None
            outpkts = outpkts["sent"]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, outpkts)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStatProtoErr(self):
        def handler(oid, associd):
            pvars = self.safeReadvar(associd, ["badauth", "bogusorg",
                                               "seldisp", "selbroken"])
            if pvars is None:
                return None
            protoerr = 0
            for key in pvars.keys():
                protoerr += pvars[key]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, protoerr)
        return self.dynamicCallbackSkeleton(handler)

    # =====================================
    # Notification handlers
    # =====================================

    def checkNotifications(self, control):
        currentTime = time.time()
        if (currentTime - self.lastNotifyCheck) < self.notifySpinTime:
            return
        self.lastNotifyCheck = currentTime

        if self.notifyModeChange is True:
            self.doNotifyModeChange(control)

        if self.notifyStratumChange is True:
            self.doNotifyStratumChange(control)

        if self.notifySyspeerChange is True:
            self.doNotifySyspeerChange(control)

        # Both add and remove have to look at the same data, don't want them
        # stepping on each other. Therefore the functions are combined.
        if (self.notifyAddAssociation is True) and \
           (self.notifyRMAssociation is True):
            self.doNotifyChangeAssociation(control, "both")
        elif self.notifyAddAssociation is True:
            self.doNotifyChangeAssociation(control, "add")
        elif self.notifyRMAssociation is True:
            self.doNotifyChangeAssociation(control, "rm")

        if self.notifyConfigChange is True:
            self.doNotifyConfigChange(control)

        if self.notifyLeapSecondAnnounced is True:
            self.doNotifyLeapSecondAnnounced(control)

        if self.notifyHeartbeat is True:
            self.doNotifyHeartbeat(control)

    def doNotifyModeChange(self, control):
        oldMode = self.oldValues.get("mode")
        newMode = self.misc_getMode()  # connection failure handled by method
        if oldMode is None:
            self.oldValues["mode"] = newMode
            return
        elif oldMode != newMode:
            self.oldValues["mode"] = newMode
            vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                             ax.OID(ntpRootOID + (0, 1))),
                  ax.Varbind(ax.VALUE_INTEGER, ntpRootOID + (1, 2, 1),
                             newMode)]
            control.sendNotify(vl)
            self.sentNotifications += 1

    def doNotifyStratumChange(self, control):
        oldStratum = self.oldValues.get("stratum")
        newStratum = self.safeReadvar(0, ["stratum"])
        if newStratum is None:
            return  # couldn't read
        newStratum = newStratum["stratum"]
        if oldStratum is None:
            self.oldValues["stratum"] = newStratum
            return
        elif oldStratum != newStratum:
            self.oldValues["stratum"] = newStratum
            datetime = self.safeReadvar(0, ["reftime"])
            if datetime is None:
                datetime = ""
            else:
                datetime = ntp.util.deformatNTPTime(datetime["reftime"])
            vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                             ax.OID(ntpRootOID + (0, 2))),
                  ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 2, 9),
                             datetime),
                  ax.Varbind(ax.VALUE_GAUGE32, ntpRootOID + (1, 2, 2),
                             newStratum),
                  ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 5, 1),
                             "Stratum changed")]  # Uh... what goes here?
            control.sendNotify(vl)
            self.sentNotifications += 1

    def doNotifySyspeerChange(self, control):
        oldSyspeer = self.oldValues.get("syspeer")
        newSyspeer = self.safeReadvar(0, ["peeradr"])
        if newSyspeer is None:
            return  # couldn't read
        newSyspeer = newSyspeer["peeradr"]
        if oldSyspeer is None:
            self.oldValues["syspeer"] = newSyspeer
            return
        elif oldSyspeer != newSyspeer:
            self.oldValues["syspeer"] = newSyspeer
            datetime = self.safeReadvar(0, ["reftime"])
            if datetime is None:
                datetime = ""
            else:
                datetime = ntp.util.deformatNTPTime(datetime["reftime"])
            syspeer = self.misc_getSyspeerID()
            vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                             ax.OID(ntpRootOID + (0, 3))),
                  ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 2, 9),
                             datetime),
                  ax.Varbind(ax.VALUE_GAUGE32, ntpRootOID + (1, 2, 3),
                             syspeer),
                  ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 5, 1),
                             "SysPeer changed")]  # Uh... what goes here?
            control.sendNotify(vl)
            self.sentNotifications += 1

    def doNotifyChangeAssociation(self, control, which):
        # Add and remove are combined because they use the same data source
        # and it would be easy to have them stepping on each other.
        changes = self.misc_getAssocListChanges()
        if changes is None:
            return
        datetime = self.safeReadvar(0, ["reftime"])
        if datetime is None:
            datetime = ""
        else:
            datetime = ntp.util.deformatNTPTime(datetime["reftime"])
        adds, rms = changes
        if which in ("add", "both"):
            for name in adds:
                vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                                 ax.OID(ntpRootOID + (0, 4))),  # Add
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 2, 9),
                                 datetime),
                      ax.Varbind(ax.VALUE_OCTET_STR,
                                 ntpRootOID + (1, 3, 1, 1, 2),
                                 name),
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 5, 1),
                                 "Association added")]
                control.sendNotify(vl)
                self.sentNotifications += 1
        if which in ("rm", "both"):
            for name in rms:
                vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                                 ax.OID(ntpRootOID + (0, 5))),  # Remove
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 2, 9),
                                 datetime),
                      ax.Varbind(ax.VALUE_OCTET_STR,
                                 ntpRootOID + (1, 3, 1, 1, 2),
                                 name),
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 5, 1),
                                 "Association removed")]
                control.sendNotify(vl)
                self.sentNotifications += 1

    def doNotifyConfigChange(self, control):
        # This left unimplemented because the MIB wants something we can't
        # and/or shouldn't provide
        pass

    def doNotifyLeapSecondAnnounced(self, control):
        oldLeap = self.oldValues.get("leap")
        newLeap = self.safeReadvar(0, ["leap"])
        if newLeap is None:
            return
        newLeap = newLeap["leap"]
        if oldLeap is None:
            self.oldValues["leap"] = newLeap
            return
        if oldLeap != newLeap:
            self.oldValues["leap"] = newLeap
            if (oldLeap in (0, 3)) and (newLeap in (1, 2)):
                # changed noleap or unsync to a leap announcement
                datetime = self.safeReadvar(0, ["reftime"])
                if datetime is None:
                    datetime = ""
                else:
                    datetime = ntp.util.deformatNTPTime(datetime["reftime"])
                vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                                 ax.OID(ntpRootOID + (0, 7))),
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 2, 9),
                                 datetime),
                      ax.Varbind(ax.VALUE_OCTET_STR, ntpRootOID + (1, 5, 1),
                                 "Leap second announced")]
                control.sendNotify(vl)
                self.sentNotifications += 1

    def doNotifyHeartbeat(self, control):  # TODO: check if ntpd running?
        vl = [ax.Varbind(ax.VALUE_OID, snmpTrapOID,
                         ax.OID(ntpRootOID + (0, 8))),
              ax.Varbind(ax.VALUE_GAUGE32, ntpRootOID + (0, 1, 4, 1),
                         self.heartbeatInterval)]
        if self.heartbeatInterval == 0:  # interval == 0 means send once
            self.notifyHeartbeat = False
            control.sendNotify(vl)
            self.sentNotifications += 1
        else:
            current = ntp.util.monoclock()
            if (current - self.lastHeartbeat) > self.heartbeatInterval:
                self.lastHeartbeat = current
                control.sendNotify(vl)
                self.sentNotifications += 1

    # =====================================
    # Misc data helpers (not part of the MIB proper)
    # =====================================

    def misc_loadDynamicSettings(self):
        if self.settingsFilename is None:
            return

        def boolify(d, k):
            return True if d[k][0][1] == "True" else False
        optionList = ("notify-mode-change", "notify-stratum-change",
                      "notify-syspeer-change", "notify-add-association",
                      "notify-rm-association", "notify-leap-announced",
                      "notify-heartbeat", "heartbeat-interval")
        settings = loadSettings(self.settingsFilename, optionList)
        if settings is None:
            return
        for key in settings.keys():
            if key == "notify-mode-change":
                self.notifyModeChange = boolify(settings, key)
            elif key == "notify-stratum-change":
                self.notifyStratumChange = boolify(settings, key)
            elif key == "notify-syspeer-change":
                self.notifySyspeerChange = boolify(settings, key)
            elif key == "notify-add-association":
                self.notifyAddAssociation = boolify(settings, key)
            elif key == "notify-rm-association":
                self.notifyRMAssociation = boolify(settings, key)
            elif key == "notify-leap-announced":
                self.notifyLeapSecondAnnounced = boolify(settings, key)
            elif key == "notify-heartbeat":
                self.notifyHeartbeat = boolify(settings, key)
            elif key == "heartbeat-interval":
                self.heartbeatInterval = settings[key][0][1]

    def misc_storeDynamicSettings(self):
        if self.settingsFilename is None:
            return
        settings = {}
        settings["notify-mode-change"] = str(self.notifyModeChange)
        settings["notify-stratum-change"] = str(self.notifyStratumChange)
        settings["notify-syspeer-change"] = str(self.notifySyspeerChange)
        settings["notify-add-association"] = str(self.notifyAddAssociation)
        settings["notify-rm-association"] = str(self.notifyRMAssociation)
        settings["notify-leap-announced"] = str(self.notifyLeapSecondAnnounced)
        settings["notify-heartbeat"] = str(self.notifyHeartbeat)
        settings["heartbeat-interval"] = str(self.heartbeatInterval)
        storeSettings(self.settingsFilename, settings)

    def misc_getAssocListChanges(self):
        # We need to keep the names, because those won't be available
        # after they have been removed.
        oldAssoc = self.oldValues.get("assoc")
        newAssoc = {}
        # Yes, these are cached, for a very short time
        pdata = self.misc_getPeerData()
        if pdata is None:
            return
        ids = self.misc_getPeerIDs()
        if ids is None:
            return
        for associd in ids:
            addr = pdata[associd]["srcadr"][1]
            name = ntp.util.canonicalize_dns(addr)
            newAssoc[associd] = name
        if oldAssoc is None:
            self.oldValues["assoc"] = newAssoc
            return
        elif oldAssoc != newAssoc:
            oldIDs = oldAssoc.keys()
            newIDs = newAssoc.keys()
            adds = []
            rms = []
            for associd in oldIDs + newIDs:
                if associd not in newIDs:  # removed
                    rms.append(oldAssoc[associd])
                if associd not in oldIDs:  # added
                    adds.append(newAssoc[associd])
            return (adds, rms)
        return

    def misc_getMode(self):  # FIXME: not fully implemented
        try:
            # Don't care about the data, this is a ploy to get the rstatus
            self.session.readvar(0, ["stratum"])
        except ntp.packet.ControlException as e:
            if e.message == ntp.packet.SERR_SOCKET:
                # Can't connect, ntpd probably not running
                return 1
            else:
                raise e
        rstatus = self.session.rstatus  # a ploy to get the system status
        source = ntp.control.CTL_SYS_SOURCE(rstatus)
        if source == ntp.control.CTL_SST_TS_UNSPEC:
            mode = 2  # Not yet synced
        elif False:
            mode = 3  # No reference configured
        elif source == ntp.control.CTL_SST_TS_LOCAL:
            mode = 4  # Distributing local clock (low accuracy)
        elif source in (ntp.control.CTL_SST_TS_ATOM,
                        ntp.control.CTL_SST_TS_LF,
                        ntp.control.CTL_SST_TS_HF,
                        ntp.control.CTL_SST_TS_UHF):
            # I am not sure if I should be including the radios in this
            mode = 5  # Synced to local refclock
        elif source == ntp.control.CTL_SST_TS_NTP:
            # Should this include "other"? That covers things like chrony...
            mode = 6  # Sync to remote NTP
        else:
            mode = 99  # Unknown
        return mode

    def misc_getSyspeerID(self):
        peers = self.misc_getPeerData()
        syspeer = 0
        for associd in peers.keys():
            rstatus = peers[associd]["peerstatus"]
            if (ntp.control.CTL_PEER_STATVAL(rstatus) & 0x7) == \
               ntp.control.CTL_PST_SEL_SYSPEER:
                syspeer = associd
                break
        return syspeer

    def safeReadvar(self, associd, variables=None, raw=False):
        # Use this when we want to catch packet errors, but don't care
        # about what they are
        try:
            return self.session.readvar(associd, varlist=variables, raw=raw)
        except ntp.packet.ControlException:
            return None

    def dynamicCallbackPeerdata(self, variable, raw, valueType):
        rawindex = 1 if raw is True else 0

        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return None
            value = pdata[associd][variable][rawindex]
            return ax.Varbind(valueType, oid, value)
        return self.dynamicCallbackSkeleton(handler)

    def dynamicCallbackSkeleton(self, handler):
        # Build a dynamic MIB tree, installing the provided handler in it
        def readCallback(oid):
            # This function assumes that it is a leaf node and that the
            # last number in the OID is the index.
            index = oid.subids[-1]  # if called properly this works (Ha!)
            index -= 1  # SNMP reserves index 0, effectively 1-based lists
            associd = self.misc_getPeerIDs()[index]
            return handler(oid, associd)
        subs = {}
        associds = self.misc_getPeerIDs()  # need the peer count
        for i in range(len(associds)):
            subs[i+1] = {"reader": readCallback}
        return subs

    def readCallbackSkeletonSimple(self, oid, varname, dataType):
        # Used for entries that just need a simple variable retrevial
        # but do not need any processing.
        data = self.safeReadvar(0, [varname])
        if data is None:
            return None
        else:
            return ax.Varbind(dataType, oid, data[varname])

    def misc_getPeerIDs(self):
        peerids = self.cache.get("peerids")
        if peerids is None:
            try:
                peerids = [x.associd for x in self.session.readstat()]
            except ntp.packet.ControlException:
                peerids = []
            peerids.sort()
            self.cache.set("peerids", peerids)
        return peerids

    def misc_getPeerData(self):
        peerdata = self.cache.get("peerdata")
        if peerdata is None:
            associds = self.misc_getPeerIDs()
            peerdata = {}
            for aid in associds:
                try:
                    pdata = self.safeReadvar(aid, raw=True)
                    pdata["peerstatus"] = self.session.rstatus
                except IOError:
                    continue
                peerdata[aid] = pdata
            self.cache.set("peerdata", peerdata)
        return peerdata


def connect(address):
    try:
        if type(address) is str:
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            sock.connect(address)
        else:
            host, port = address[0], address[1]
            af, _, _, _, _ = socket.getaddrinfo(host, port)[0]
            sock = socket.socket(af, socket.SOCK_STREAM)
            sock.connect((host, port))
    except socket.error as msg:
        log("Connection to %s failure: %s" % (repr(address), repr(msg)), 1)
        sys.exit(1)
    log("connected to master agent at " + repr(address), 3)
    return sock


def mainloop(snmpSocket, reconnectionAddr, host=None):
    log("initing loop", 3)
    dbase = DataSource(host, "/var/ntpsntpd/notify.conf")
    while True:  # Loop reconnection attempts
        control = PacketControl(snmpSocket, dbase, logfp=logfp, debug=debug)
        control.loopCallback = dbase.checkNotifications
        control.initNewSession()
        if control.mainloop(True) is False:  # disconnected
            snmpSocket.close()
            snmpSocket = connect(reconnectionAddr)
            log("disconnected from master, attempting reconnect", 2)
        else:  # Something else happened
            break


def daemonize(runfunc, *runArgs):
    pid = os.fork()
    if pid < 0:
        log("Forking error " + str(pid), 1)
        sys.exit(pid)
    elif pid > 0:  # We are the parent
        log("Daemonization success, child pid: " + str(pid), 3)
        sys.exit(0)

    # We must be the child

    os.umask(0)

    sid = os.setsid()

    # chdir should be here, change to what? root?

    global logfp
    if logfp == sys.stderr:
        logfp = None

    sys.stdin.close()
    sys.stdin = None
    sys.stdout.close()
    sys.stdout = None
    sys.stderr.close()
    sys.stderr = None

    runfunc(*runArgs)


def loadSettings(filename, optionList):
    log("Loading config file: %s" % filename, 3)
    if os.path.isfile(filename) is False:
        return None
    options = {}
    with open(filename) as f:
        data = f.read()
        lines = ntp.util.parseConf(data)
        for line in lines:
            isQuote, token = line[0]
            if token in optionList:
                options[token] = line[1:]
    return options


def storeSettings(filename, settings):
    dirname = os.path.dirname(filename)
    if os.path.exists(dirname) is False:
        os.makedirs(dirname)
    data = []
    for key in settings.keys():
        data.append("%s %s\n" % (key, settings[key]))
    data = "".join(data)
    with open(filename, "w") as f:
        f.write(data)


usage = """
USAGE: ntpsnmpd [-n] [ntp host]
  Flg Arg Option-Name   Description
   -n no  no-fork         Do not fork and daemonize.
   -x Adr master-addr     Specify address for connecting to the master agent
                                - default /var/agentx/master
   -d no  debug-level     Increase output debug message level
                                - may appear multiple times
   -l Str logfile         Logs debug messages to the provided filename
   -D Int set-debug-level Set the output debug message level
                                - may appear multiple times
   -h no  help            Print a usage message.
   -V no  version         Output version information and exit
"""


if __name__ == "__main__":
    try:
        (options, arguments) = getopt.getopt(
            sys.argv[1:],
            "nx:dD:Vhl:c:",
            ["no-fork", "master-address=", "debug-level", "set-debug-level=",
             "version", "help", "logfile=", "configfile="])
    except getopt.GetoptError as e:
        sys.stderr.write("%s\n" % e)
        sys.stderr.write(usage)
        raise SystemExit(1)

    masterAddr = "/var/agentx/master"
    logfile = DEFLOG
    hostname = DEFHOST

    # Check for non-default config-file
    conffile = "/etc/ntpsnmpd.conf"
    for (switch, val) in options:
        if switch in ("-c", "--configfile"):
            conffile = val
            break

    # Load configuration file
    conf = loadSettings(conffile,
                        ("master-addr", "logfile", "loglevel", "ntp-addr"))
    if conf is not None:
        for key in conf.keys():
            if key == "master-addr":  # Address of the SNMP master daemon
                val = conf[key][0][1]
                if ":" in val:
                    host, port = val.split(":")
                    port = int(port)
                    masterAddr = (host, port)
                else:
                    masterAddr = val
            elif key == "logfile":
                logfile = conf[key][0][1]
            elif key == "ntp-addr":  # Address of the NTP daemon
                hostname = conf[key][0][1]
            elif key == "loglevel":
                errmsg = "Error: loglevel parameter '%s' not a number\n"
                debug = conf[key][0][1]

    fileLogging = False
    for (switch, val) in options:
        if switch in ("-n", "--no-fork"):
            nofork = True
        elif switch in ("-x", "--master-addr"):
            if ":" in val:
                host, port = val.split(":")
                port = int(port)
                masterAddr = (host, port)
            else:
                masterAddr = val
        elif switch in ("-d", "--debug-level"):
            debug += 1
        elif switch in ("-D", "--set-debug-level"):
            errmsg = "Error: -D parameter '%s' not a number\n"
            debug = ntp.util.safeargcast(val, int, errmsg, usage)
        elif switch in ("-V", "--version"):
            print("ntpsnmpd %s" % ntp.util.stdversion())
            raise SystemExit(0)
        elif switch in ("-h", "--help"):
            print(usage)
            raise SystemExit(0)
        elif switch in ("-l", "--logfile"):
            logfile = val
            fileLogging = True

    if nofork is False:
        fileLogging = True

    if fileLogging is True:
        if logfp != sys.stderr:
            logfp.close()
        logfp = open(logfile, "a", 1)  # 1 => line buffered

    hostname = arguments[0] if arguments else DEFHOST

    # Connect here so it can always report a connection error
    sock = connect(masterAddr)

    if nofork is True:
        mainloop(sock, hostname)
    else:
        daemonize(mainloop, sock, hostname)
