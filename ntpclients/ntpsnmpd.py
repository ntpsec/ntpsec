#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, division

import sys
import os
import getopt
import time
import socket
import select
import subprocess
import shlex

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

ntpRootOID = (1, 3, 6, 1, 2, 1, 197)  # mib-2 . 197, aka: NTPv4-MIB

snmpTrapOID = (1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0)
snmpSysUptime = (1, 3, 6, 1, 2, 1, 1, 3, 0)

DEFHOST = "localhost"
DEFLOG = "ntpsnmpd.log"


class DataSource(ntp.agentx.MIBControl):
    def __init__(self, hostname=DEFHOST):
        # This is defined as a dict tree because it is simpler, and avoids
        # certain edge cases
        # OIDs are relative from ntp root
        oidTree = {
            # ntpEntNotifications
            0: {"subids":
                # ntpEntNotifModeChange
                {1: {},
                 # ntpEntNotifStratumChange
                 2: {},
                 # ntpEntNotifSyspeerChange
                 3: {},
                 # ntpEntNotifAddAssociation
                 4: {},
                 # ntpEntNotifRemoveAsociation
                 5: {},
                 # ntpEntNotifConfigChanged
                 6: {},
                 # ntpEntNotifLeapSecondAnnounced
                 7: {},
                 # ntpEntNotifHeartbeat
                 8: {}}},
            # ntpSnmpMIBObjects
            1: {"subids":
                # ntpEntInfo
                {1:
                 {"subids":
                  # ntpNetSoftwareName utf8str
                  {1: {"reader":
                       (lambda oid: self.cbr_systemInfo(oid, "name"))},
                   # ntpEntSoftwareVersion utf8str
                   2: {"reader":
                       (lambda oid: self.cbr_systemInfo(oid, "version"))},
                   # ntpEntSoftwareVendor utf8str
                   3: {"reader":
                       (lambda oid: self.cbr_systemInfo(oid, "vendor"))},
                   # ntpEntSystemType utf8str
                   4: {"reader":
                       (lambda oid: self.cbr_systemInfo(oid, "system"))},
                   # ntpEntTimeResolution uint32
                   5: {"reader": self.cbr_timeResolution},
                   # ntpEntTimePrecision int32
                   6: {"reader": self.cbr_timePrecision},
                   # ntpEntTimeDistance DisplayString
                   7: {"reader": self.cbr_timeDistance}}},
                 # ntpEntStatus
                 2: {"subids":
                     # ntpEntStatusCurrentMode INTEGER {...}
                     {1: {"reader": self.cbr_statusCurrentMode},
                      # ntpEntStatusStratum NtpStratum
                      2: {"reader": self.cbr_statusStratum},
                      # ntpEntStatusActiveRefSourceId
                      #  uint32 (0..99999)
                      3: {"reader": self.cbr_statusActiveRefSourceID},
                      # ntpEntStatusActiveRefSourceName utf8str
                      4: {"reader": self.cbr_statusActiveRefSourceName},
                      # ntpEntStatusActiveOffset DisplayString
                      5: {"reader": self.cbr_statusActiveOffset},
                      # ntpEntStatusNumberOfRefSources
                      #  unit32 (0..99)
                      6: {"reader": self.cbr_statusNumRefSources},
                      # ntpEntStatusDispersion DisplayString
                      7: {"reader": self.cbr_statusDispersion},
                      # ntpEntStatusEntityUptime TimeTicks
                      8: {"reader": self.cbr_statusEntityUptime},
                      # ntpEntStatusDateTime NtpDateTime
                      9: {"reader": self.cbr_statusDateTime},
                      # ntpEntStatusLeapSecond NtpDateTime
                      10: {"reader": self.cbr_statusLeapSecond},
                      # ntpEntStatusLeapSecondDirection
                      #  int32 (-1..1)
                      11: {"reader": self.cbr_statusLeapSecDirection},
                      # ntpEntStatusInPkts Counter32
                      12: {"reader": self.cbr_statusInPkts},
                      # ntpEntStatusOutPkts Counter32
                      13: {"reader": self.cbr_statusOutPkts},
                      # ntpEntStatusBadVersion Counter32
                      14: {"reader": self.cbr_statusBadVersion},
                      # ntpEntStatusProtocolError Counter32
                      15: {"reader": self.cbr_statusProtocolError},
                      # ntpEntStatusNotifications Counter32
                      16: {"reader": self.cbr_statusNotifications},
                      # ntpEntStatPktModeTable
                      #  SEQUENCE of NtpEntStatPktModeEntry
                      17: {"subids":
                           # ntpEntStatPktModeEntry SEQUENCE {...}
                           {1: {"subids":
                                # ntpEntStatPktMode INTEGER {...}
                                {1: {},
                                 # ntpEntStatPktSent Counter32
                                 2: {},
                                 # ntpEntStatPktRecived Counter32
                                 3: {}}
                           }}
                      }}
                 },
                 # ntpAssociation
                 3: {"subids":
                     # ntpAssociationTable
                     #  SEQUENCE of NtpAssociationEntry
                     {1: {"subids":
                          # ntpAssociationEntry SEQUENCE {...}
                          {1: {"subids":
                               # ntpAssocId uint32 (1..99999)
                               {1: {"subids": self.sub_assocID},
                                # ntpAssocName utf8str
                                2: {"subids": self.sub_assocName},
                                # ntpAssocRefId DisplayString
                                3: {"subids": self.sub_assocRefID},
                                # ntpAssocAddressType
                                #  InetAddressType
                                4: {"subids": self.sub_assocAddrType},
                                # ntpAssocAddress
                                #  InetAddress SIZE (4|8|16|20)
                                5: {"subids": self.sub_assocAddr},
                                # ntpAssocOffset DisplayString
                                6: {"subids": self.sub_assocOffset},
                                # ntpAssocStratum NtpStratum
                                7: {"subids": self.sub_assocStratum},
                                # ntpAssocStatusJitter
                                #  DisplayString
                                8: {"subids": self.sub_assocJitter},
                                # ntpAssocStatusDelay
                                #  DisplayString
                                9: {"subids": self.sub_assocDelay},
                                # ntpAssocStatusDispersion
                                #  DisplayString
                                10: {"subids": self.sub_assocDispersion}}
                          }}
                     },
                      # ntpAssociationStatisticsTable
                      #  SEQUENCE of ntpAssociationStatisticsEntry
                      2: {"subids":
                          # ntpAssociationStatisticsEntry
                          #  SEQUENCE {...}
                          {1: {"subids":
                               # ntpAssocStatInPkts Counter32
                               {1: {"subids": self.sub_assocStatInPkts},
                                # ntpAssocStatOutPkts Counter32
                                2: {"subids": self.sub_assocStatOutPkts},
                                # ntpAssocStatProtocolError
                                #  Counter32
                                3: {"subids": self.sub_assocStatProtoErr}}
                          }}
                      }}
                 },
                 # ntpEntControl
                 4: {"subids":
                     # ntpEntHeartbeatInterval unit32
                     {1: {"reader": self.cbr_entHeartbeatInterval,
                          "writer": self.cbw_entHeartbeatInterval},
                      # ntpEntNotifBits BITS {...}
                      2: {"reader": self.cbr_entNotifBits,
                          "writer": self.cbw_entNotifBits,}}
                 },
                 # ntpEntNotifObjects
                 5: {"subids":
                     # ntpEntNotifMessage utf8str
                     {1: {"reader": self.cbr_entNotifMessage}}
                 }}
            },
            # ntpEntConformance
            2: {"subids":
                # ntpEntCompliances
                {1: {"subids":
                     # ntpEntNTPCompliance
                     {1: {},
                      # ntpEntSNTPCompliance
                      2: {}}
                },
                 # ntpEntGroups
                 2: {"subids":
                     # ntpEntObjectsGroup1 OBJECTS {...}
                     {1: {},
                      # ntpEntObjectsGroup2 OBJECTS {...}
                      2: {},
                      # ntpEntNotifGroup NOTIFICATIONS {...}
                      3: {}}
                 }}
            }
        }
        ntp.agentx.MIBControl.__init__(self, oidTree, mibRoot=ntpRootOID)
        self.session = ntp.packet.ControlSession()
        self.hostname = hostname if hostname else DEFHOST
        self.session.openhost(self.hostname)
        # Cache so we don't hammer ntpd, default 1 second timeout
        # Timeout default pulled from a hat: we don't want it to last for
        # long, just not flood ntpd when we don't need to.
        self.cache = ntp.util.Cache(1)
        self.oldValues = {}  # Used by notifications to detect changes
        self.lastHeartbeat = 0  # Timestamp used for heartbeat notifications
        self.heartbeatInterval = 0  # should save to disk
        self.sentNotifications = 0
        # Notify bits, they control whether the daemon sends notifications.
        # these should be saved to disk
        self.notifyModeChange = False  # 1
        self.notifyStratumChange = False  # 2
        self.notifySyspeerChange = False  # 3
        self.notifyAddAssociation = False  # 4
        self.notifyRMAssociation = False  # 5
        self.notifyConfigChange = False  # 6 [This is not implemented]
        self.notifyLeapSecondAnnounced = False  # 7
        self.notifyHeartbeat = False  # 8

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
        data = ax.bools2Bits((self.notifyModeChange,
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
            return ax.ERR_NOERROR
        elif action == "undo":
            self.heartbeatInterval = oldData
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
        print("Notify Change assoc:", changes)
        if which in ("add", "both"):
            print("adding", adds)
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
            print("removing", rms)
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
        oldLeap = self.oldValues["leap"]
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
            # Don't care about the data, this is a ploy to the the rstatus
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


def dolog(text, level):
    if debug >= level:
        try:
            logfp.write(text)
        except Exception:
            pass


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
        dolog(repr(msg) + "\n", 2)
        sys.exit(1)
    return sock


def mainloop(snmpSocket, reconnectionAddr, host=None):
    dolog("initing loop\n", 1)
    dbase = DataSource(host)
    while True:  # Loop reconnection attempts
        control = PacketControl(snmpSocket, dbase, logfp=logfp)
        control.loopCallback = dbase.checkNotifications
        control.initNewSession()
        if control.mainloop(True) is False:  # disconnected
            snmpSocket.close()
            snmpSocket = connect(reconnectionAddr)


def daemonize(runfunc, *runArgs):
    pid = os.fork()
    if pid < 0:
        print("Forking error", pid)
        sys.exit(pid)
    elif pid > 0:  # We are the parent
        print("Daemonization success, child pid:", pid)
        sys.exit(0)

    # We must be the child

    os.umask(0)

    sid = os.setsid()

    # chdir should be here, change to what? root?

    sys.stdin.close()
    sys.stdout.close()
    sys.stderr.close()

    runfunc(*runArgs)


optionList = ["master-address", "logfile", "ntp-address"]

def loadSettings(filename, optionList):
    if os.path.isfile(filename) is False:
        return None
    options = {}
    with open(filename) as f:
        data = f.read()
        parser = shlex.shlex(data)
        data = [x for x in parser]
        i = 0
        dataLen = len(data)
        while i < dataLen:
            if data[i] in optionList:
                options[data[i]] = data[i+1]
                i += 1
            i += 1
    return options


def storeSettings(filename, options):
    data = []
    for key in options.keys():
        data.append("%s %s\n" % (key, options[key]))
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
            "nx:dD:Vhl:",
            ["no-fork", "master-address=", "debug-level", "set-debug-level=",
             "version", "help", "logfile="])
    except getopt.GetoptError as e:
        sys.stderr.write("%s\n" % e)
        sys.stderr.write(usage)
        raise SystemExit(1)

    masterAddr = "/var/agentx/master"
    logfile = DEFLOG
    hostname = DEFHOST

    # Check for non-default config file lovation
    # Load configuration file
    conf = loadSettings("/etc/ntpsnmpd.conf",
                        ("master-addr", "logfile", "loglevel", "ntp-addr"))
    if conf is not None:
        for key in conf.keys():
            if key == "master-addr":  # Address of the SNMP master daemon
                val = conf[key]
                if ":" in val:
                    host, port = val.split(":")
                    port = int(port)
                    masterAddr = (host, port)
                else:
                    masterAddr = val
            elif key == "logfile":
                logfile = conf[key]
            elif key == "ntp-addr":  # Address of the NTP daemon
                hostname = conf[key]
            elif key == "loglevel":
                errmsg = "Error: loglevel parameter '%s' not a number\n"
                debug = ntp.util.safeargcast(val, int, errmsg, usage)

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
