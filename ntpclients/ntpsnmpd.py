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

try:
    import ntp.packet
    import ntp.util
    import ntp.agentx
    ax = ntp.agentx
except ImportError as e:
    sys.stderr.write(
        "ntpsnmpd: can't find Python NTP library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)


# TODO This is either necessary, or a different workaround is.
ntp.util.deunicode_units()

logfile = "ntpsnmpd.log"
logfp = sys.stderr
nofork = True  # don't daemonize while still under construction
debug = 0
timeout = 5  # default timeout, what shuold this be?

ntpRootOID = (1, 3, 6, 1, 2, 1, 197)  # mib-2 . 197, aka: NTPv4-MIB

DEFHOST = "localhost"  # For now only know how to talk to the local ntp


class DataSource:  # This will be broken up in future to be less NTP-specific
    def __init__(self):
        node = ax.mibnode
        # This is defined as a dict tree because simpler, and avoids
        # certain edge cases
        # OIDs are relative from ntp root
        # ntpEntNotifications
        self.oidTree = {
            0:
            node(None, None, True,
                 # ntpEntNotifModeChange
                 {1: node(None, None, True, None),
                  # ntpEntNotifStratumChange
                  2: node(None, None, True, None),
                  # ntpEntNotifSyspeerChange
                  3: node(None, None, True, None),
                  # ntpEntNotifAddAssociation
                  4: node(None, None, True, None),
                  # ntpEntNotifRemoveAsociation
                  5: node(None, None, True, None),
                  # ntpEntNotifConfigChanged
                  6: node(None, None, True, None),
                  # ntpEntNotifLeapSecondAnnounced
                  7: node(None, None, True, None),
                  # ntpEntNotifHeartbeat
                  8: node(None, None, True, None)}),
            # ntpSnmpMIBObjects
            1:
            node(None, None, True,
                 # ntpEntInfo
                 {1:
                  node(None, None, True,
                       # ntpNetSoftwareName utf8str
                       {1: node((lambda oid:
                                 self.cbr_systemInfo(oid, "name")),
                                None, True, None),
                        # ntpEntSoftwareVersion utf8str
                        2: node((lambda oid:
                                 self.cbr_systemInfo(oid, "version")),
                                None, True, None),
                        # ntpEntSoftwareVendor utf8str
                        3: node((lambda oid:
                                 self.cbr_systemInfo(oid, "vendor")),
                                None, True, None),
                        # ntpEntSystemType utf8str
                        4: node((lambda oid:
                                 self.cbr_systemInfo(oid, "system")),
                                None, True, None),
                        # ntpEntTimeResolution uint32
                        5: node(self.cbr_timeResolution, None, True, None),
                        # ntpEntTimePrecision int32
                        6: node(self.cbr_timePrecision, None, True, None),
                        # ntpEntTimeDistance DisplayString
                        7: node(self.cbr_timeDistance, None, True, None)}),
                  # ntpEntStatus
                  2:
                  node(None, None, True,
                       # ntpEntStatusCurrentMode INTEGER {...}
                       {1: node(self.cbr_statusCurrentMode, None, True, None),
                        # ntpEntStatusStratum NtpStratum
                        2: node(self.cbr_statusStratum, None, True, None),
                        # ntpEntStatusActiveRefSourceId
                        #  uint32 (0..99999)
                        3: node(self.cbr_statusActiveRefSourceID,
                                None, True, None),
                        # ntpEntStatusActiveRefSourceName utf8str
                        4: node(self.cbr_statusActiveRefSourceName,
                                None, True, None),
                        # ntpEntStatusActiveOffset DisplayString
                        5: node(self.cbr_statusActiveOffset, None, True, None),
                        # ntpEntStatusNumberOfRefSources
                        #  unit32 (0..99)
                        6: node(self.cbr_statusNumRefSources, None, True, None),
                        # ntpEntStatusDispersion DisplayString
                        7: node(self.cbr_statusDispersion, None, True, None),
                        # ntpEntStatusEntityUptime TimeTicks
                        8: node(self.cbr_statusEntityUptime, None, True, None),
                        # ntpEntStatusDateTime NtpDateTime
                        9: node(self.cbr_statusDateTime, None, True, None),
                        # ntpEntStatusLeapSecond NtpDateTime
                        10: node(self.cbr_statusLeapSecond, None, True, None),
                        # ntpEntStatusLeapSecondDirection
                        #  int32 (-1..1)
                        11: node(self.cbr_statusLeapSecDirection,
                                 None, True, None),
                        # ntpEntStatusInPkts Counter32
                        12: node(self.cbr_statusInPkts, None, True, None),
                        # ntpEntStatusOutPkts Counter32
                        13: node(self.cbr_statusOutPkts, None, True, None),
                        # ntpEntStatusBadVersion Counter32
                        14: node(self.cbr_statusBadVersion, None, True, None),
                        # ntpEntStatusProtocolError Counter32
                        15: node(self.cbr_statusProtocolError,
                                 None, True, None),
                        # ntpEntStatusNotifications Counter32
                        16: node(self.cbr_statusNotifications,
                                 None, True, None),
                        # ntpEntStatPktModeTable
                        #  SEQUENCE of NtpEntStatPktModeEntry
                        17:
                        node(None, None, True,
                             # ntpEntStatPktModeEntry SEQUENCE {...}
                             {1:
                              node(None, None, True,
                                   # ntpEntStatPktMode INTEGER {...}
                                   {1: node(None, None, True, None),
                                    # ntpEntStatPktSent Counter32
                                    2: node(None, None, True, None),
                                    # ntpEntStatPktRecived Counter32
                                    3: node(None, None, True, None)})})}),
                  # ntpAssociation
                  3:
                  node(None, None, True,
                       # ntpAssociationTable
                       #  SEQUENCE of NtpAssociationEntry
                       {1:
                        node(None, None, True,
                             # ntpAssociationEntry SEQUENCE {...}
                             {1:
                              node(None, None, True,
                                   # ntpAssocId uint32 (1..99999)
                                   {1: node(None, None, False,
                                            self.sub_assocID),
                                    # ntpAssocName utf8str
                                    2: node(None, None, False,
                                            self.sub_assocName),
                                    # ntpAssocRefId DisplayString
                                    3: node(None, None, False,
                                            self.sub_assocRefID),
                                    # ntpAssocAddressType
                                    #  InetAddressType
                                    4: node(None, None, False,
                                            self.sub_assocAddrType),
                                    # ntpAssocAddress
                                    #  InetAddress SIZE (4|8|16|20)
                                    5: node(None, None, False,
                                            self.sub_assocAddr),
                                    # ntpAssocOffset DisplayString
                                    6: node(None, None, False,
                                            self.sub_assocOffset),
                                    # ntpAssocStratum NtpStratum
                                    7: node(None, None, False,
                                            self.sub_assocStratum),
                                    # ntpAssocStatusJitter
                                    #  DisplayString
                                    8: node(None, None, False,
                                            self.sub_assocJitter),
                                    # ntpAssocStatusDelay
                                    #  DisplayString
                                    9: node(None, None, False,
                                            self.sub_assocDelay),
                                    # ntpAssocStatusDispersion
                                    #  DisplayString
                                    10: node(None, None, False,
                                             self.sub_assocDispersion)})}),
                        # ntpAssociationStatisticsTable
                        #  SEQUENCE of ntpAssociationStatisticsEntry
                        2:
                        node(None, None, True,
                             # ntpAssociationStatisticsEntry
                             #  SEQUENCE {...}
                             {1:
                              node(None, None, True,
                                   # ntpAssocStatInPkts Counter32
                                   {1: node(None, None, False,
                                            self.sub_assocStatInPkts),
                                    # ntpAssocStatOutPkts Counter32
                                    2: node(None, None, False,
                                            self.sub_assocStatOutPkts),
                                    # ntpAssocStatProtocolError
                                    #  Counter32
                                    3: node(None, None, False,
                                            self.sub_assocStatProtoErr)})})}),
                  # ntpEntControl
                  4:
                  node(None, None, True,
                       # ntpEntHeartbeatInterval unit32
                       {1: node(self.cbr_entHeartbeatInterval,
                                self.cbw_entHeartbeatInterval,
                                True, None),
                        # ntpEntNotifBits BITS {...}
                        2: node(self.cbr_entNotifBits, self.cbw_entNotifBits,
                                True, None)}),
                  # ntpEntNotifObjects
                  5:
                  node(None, None, True,
                       # ntpEntNotifMessage utf8str
                       {1: node(self.cbr_entNotifMessage, None, True, None)})}),
            # ntpEntConformance
            2:
            node(None, None, True,
                 # ntpEntCompliances
                {1:
                 node(None, None, True,
                      # ntpEntNTPCompliance
                      {1: node(None, None, True, None),
                       # ntpEntSNTPCompliance
                       2: node(None, None, True, None)}),
                 # ntpEntGroups
                 2:
                 node(None, None, True,
                      # ntpEntObjectsGroup1 OBJECTS {...}
                      {1: node(None, None, True, None),
                       # ntpEntObjectsGroup2 OBJECTS {...}
                       2: node(None, None, True, None),
                       # ntpEntNotifGroup NOTIFICATIONS {...}
                       3: node(None, None, True, None)})})}
        self.session = ntp.packet.ControlSession()
        self.session.openhost(DEFHOST)  # only local for now
        # Cache so we don't hammer ntpd, default 1 second timeout
        # timeout default pulled from a hat
        self.cache = ntp.util.Cache(1)
        # The undo system is only for the last operation
        self.inSetP = False  # Are we currently in the set procedure?
        self.setVarbinds = []  # Varbind of the current set operation
        self.setHandlers = []  # Handlers for commit/undo/cleanup of set
        self.setUndoData = []  # Previous values for undoing
        self.heartbeatInterval = 0  # should save to disk
        self.sentNotifications = 0
        # Notify bits, these should be saved to disk
        # Also they currently have no effect
        self.notifyModeChange = False  # 1
        self.notifyStratumChange = False  # 2
        self.notifySyspeerChange = False  # 3
        self.notifyAddAssociation = False  # 4
        self.notifyRMAssociation = False  # 5
        self.notifyConfigChange = False  # 6
        self.notifyLeapSecondAnnounced = False  # 7
        self.notifyHeartbeat = False  # 8

    def getOID_core(self, nextP, searchoid, returnGenerator=False):
        gen = ax.walkMIBTree(self.oidTree, ntpRootOID)
        while True:
            try:
                oid, reader, writer = gen.next()
                if nextP is True:
                    oidhit = (oid > searchoid)
                else:
                    oidhit = (oid.subids == searchoid.subids)
                if oidhit and (reader is not None):
                    if returnGenerator is True:
                        return oid, reader, writer, gen
                    else:
                        return oid, reader, writer
            except StopIteration:
                if returnGenerator is True:
                    return None, None, None, None
                else:
                    return None, None, None

    # These exist instead of just using getOID_core for clearer semantics
    def getOID(self, searchoid, returnGenerator=False):
        "Get the requested OID"
        return self.getOID_core(False, searchoid, returnGenerator)

    def getNextOID(self, searchoid, returnGenerator=False):
        "Get the next lexicographical OID"
        return self.getOID_core(True, searchoid, returnGenerator)

    def getOIDsInRange(self, oidrange, firstOnly=False):
        "Get a list of every (optionally the first) OID in a range"
        oids = []
        gen = ax.walkMIBTree(self.oidTree, ntpRootOID)
        # Find the first OID
        while True:
            try:
                oid, reader, writer = gen.next()
                if reader is None:
                    continue  # skip unimplemented OIDs
                elif oid.subids == oidrange.start.subids:
                    # ok, found the start, do we need to skip it?
                    if oidrange.start.include is True:
                        oids.append((oid, reader, writer))
                        break
                    else:
                        continue
                elif oid > oidrange.start:
                    # If we are here it means we hit the start but skipped
                    oids.append((oid, reader, writer))
                    break
            except StopIteration:
                # Couldn't find *anything*
                return []
        if firstOnly is True:
            return oids
        # Start filling in the rest of the range
        while True:
            try:
                oid, reader = gen.next()
                if reader is None:
                    continue  # skip unimplemented OIDs
                elif (oidrange.end is not None) and (oid >= oidrange.end):
                    break  # past the end of a bounded range
                else:
                    oids.append((oid, reader, writer))
            except StopIteration:
                break  # We have run off the end of the MIB
        return oids

    # =============================================================
    # Data read callbacks start here
    # comment divider lines represent not yet implemented callbacks
    # =============================================================

    #########################

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

    def cbr_timeResolution(self, oid):  # DUMMY
        # Uinteger32
        return ax.Varbind(ax.VALUE_GAUGE32, oid, 42)

    def cbr_timePrecision(self, oid):
        return self.readCallbackSkeletonSimple(oid, "precision",
                                               ax.VALUE_INTEGER)

    def cbr_timeDistance(self, oid):  # DUMMY
        # Displaystring
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, "foo")

    # Blank: ntpEntStatus

    def cbr_statusCurrentMode(self, oid):  # DUMMY, partially implemented
        # Range of integers
        try:
            # Don't care about the data, this is a ploy to the the rstatus
            self.session.readvar(0, ["stratum"])
        except ntp.packet.ControlException as e:
            if e.message == ntp.packet.SERR_SOCKET:
                # Can't connect, ntpd probably not running
                return ax.Varbind(ax.VALUE_INTEGER, oid, 1)
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
        return ax.Varbind(ax.VALUE_INTEGER, oid, mode)

    def cbr_statusStratum(self, oid):
        # NTPstratum
        return self.readCallbackSkeletonSimple(oid, "stratum",
                                               ax.VALUE_GAUGE32)

    def cbr_statusActiveRefSourceID(self, oid):
        # range of uint32
        peers = self.misc_getPeerData()
        syspeer = 0
        for associd in peers.keys():
            rstatus = peers[associd]["peerstatus"]
            if (ntp.control.CTL_PEER_STATVAL(rstatus) & 0x7) == \
               ntp.control.CTL_PST_SEL_SYSPEER:
                syspeer = associd
                break
        return ax.Varbind(ax.VALUE_GAUGE32, oid, syspeer)

    def cbr_statusActiveRefSourceName(self, oid):
        # utf8
        data = self.safeReadvar(0, ["peeradr"])
        if data is None:
            return ax.Varbind(ax.VALUE_NULL, oid)
        data = ntp.util.canonicalize_dns(data["peeradr"])
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    def cbr_statusActiveOffset(self, oid):
        # DisplayString
        data = self.session.readvar(0, ["koffset"], raw=True)
        data = ntp.util.unitifyvar(data["koffset"][1], "koffset",
                                   width=None, unitSpace=True)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, data)

    def cbr_statusNumRefSources(self, oid):
        # range of uint32
        data = self.session.readstat()
        return ax.Varbind(ax.VALUE_GAUGE32, oid, len(data))

    def cbr_statusDispersion(self, oid):
        # DisplayString
        data = self.session.readvar(0, ["rootdisp"], raw=True)
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
        uptime = self.session.readvar(0, ["ss_reset"])["ss_reset"] * 100
        return ax.Varbind(ax.VALUE_TIME_TICKS, oid, uptime)

    def cbr_statusDateTime(self, oid):
        # NtpDateTime
        data = self.safeReadvar(0, ["reftime"])
        if data is None:
            return ax.Varbind(ax.VALUE_NULL, oid)
        txt = data["reftime"]
        txt = txt[2:]  # Strip '0x'
        txt = "".join(txt.split("."))  # Strip '.'
        value = ntp.util.hexstr2octets(txt)
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, value)

    def cbr_statusLeapSecond(self, oid):  # I am not confident in this yet
        # NtpDateTime
        DAY = 86400
        fmt = "%.8x%.8x"
        data = self.safeReadvar(0, ["reftime"])
        hasleap = self.safeReadvar(0, ["leap"])
        if (data is None) or (hasleap is None):
            return ax.Varbind(ax.VALUE_NULL, oid)
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
        leap = self.session.readvar(0, ["leap"])["leap"]
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
        data = self.session.readvar(0, ["ss_badformat", "ss_badauth"])
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

    def cbr_entNotifMessage(self, oid):  # DUMMY
        # utf8str
        return ax.Varbind(ax.VALUE_OCTET_STR, oid, "jabber")

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

    # =====================================
    # Dynamic tree generator callbacks
    # =====================================

    def sub_assocID(self):
        def handler(oid, associd):
            return ax.Varbind(ax.VALUE_GAUGE32, oid, associd)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocName(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            peername = pdata[associd]["srcadr"][1]
            peername = ntp.util.canonicalize_dns(peername)
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, peername)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocRefID(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
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
                return ax.Varbind(ax.VALUE_NULL, oid)
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
                return ax.Varbind(ax.VALUE_NULL, oid)
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
            if ipv6 is False:
                pieces = addr.split(".")
            elif ipv6 is True:
                pieces = addr.split(":")
            else:
                pieces = []
            srcadr = [int(x) for x in pieces]
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, srcadr)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocOffset(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            offset = pdata[associd]["offset"][1]
            offset = ntp.util.unitifyvar(offset, "offset", width=None,
                                         unitSpace=True)
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, offset)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStratum(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            stratum = pdata[associd]["stratum"][0]
            return ax.Varbind(ax.VALUE_GAUGE32, oid, stratum)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocJitter(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            jitter = pdata[associd]["jitter"][1]
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, jitter)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocDelay(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            delay = pdata[associd]["delay"][1]
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, delay)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocDispersion(self):
        def handler(oid, associd):
            pdata = self.misc_getPeerData()
            if pdata is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            dispersion = pdata[associd]["rootdisp"][1]
            return ax.Varbind(ax.VALUE_OCTET_STR, oid, dispersion)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStatInPkts(self):
        def handler(oid, associd):
            inpkts = self.safeReadvar(associd, ["received"])
            if inpkts is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            inpkts = inpkts["received"]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, inpkts)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStatOutPkts(self):
        def handler(oid, associd):
            outpkts = self.safeReadvar(associd, ["sent"])
            if outpkts is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            outpkts = outpkts["sent"]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, outpkts)
        return self.dynamicCallbackSkeleton(handler)

    def sub_assocStatProtoErr(self):
        def handler(oid, associd):
            pvars = self.safeReadvar(associd, ["badauth", "bogusorg",
                                               "seldisp", "selbroken"])
            if pvars is None:
                return ax.Varbind(ax.VALUE_NULL, oid)
            protoerr = 0
            for key in pvars.keys():
                protoerr += pvars[key]
            return ax.Varbind(ax.VALUE_COUNTER32, oid, protoerr)
        return self.dynamicCallbackSkeleton(handler)

    # =====================================
    # Misc data helpers (not part of the MIB proper)
    # =====================================

    def safeReadvar(self, associd, variables=None, raw=False):
        # Use this when we want to catch packet errors, but don't care
        # about what they are
        try:
            return self.session.readvar(associd, varlist=variables, raw=raw)
        except ntp.packet.ControlException:
            return None

    def dynamicCallbackSkeleton(self, handler):
        def readCallback(oid):
            index = oid.subids[-1]  # if called properly this works (Ha!)
            associd = self.misc_getPeerIDs()[index]
            return handler(oid, associd)
        subs = {}
        associds = self.misc_getPeerIDs()  # need the peer count
        for i in range(len(associds)):
            subs[i] = ax.mibnode(readCallback, None, None, None)
        return subs

    def readCallbackSkeletonSimple(self, oid, varname, dataType):
        # Used for entries that just need a simple variable retrevial
        # but do not need any processing.
        data = self.safeReadvar(0, [varname])[varname]
        if data is None:
            return ax.Varbind(ax.VALUE_NULL, oid)
        else:
            return ax.Varbind(dataType, oid, data)

    def misc_getPeerIDs(self):
        peerids = self.cache.get("peerids")
        if peerids is None:
            peerids = [x.associd for x in self.session.readstat()]
            peerids.sort()
            self.cache.set("peerids", peerids)
        return peerids

    def misc_getPeerData(self):
        # not fully implemented
        peerdata = self.cache.get("peerdata")
        if peerdata is None:
            associds = self.misc_getPeerIDs()
            peerdata = {}
            for aid in associds:
                try:
                    pdata = self.safeReadvar(aid, raw=True)
                    pdata["peerstatus"] = self.session.rstatus
                except IOError as e:
                    continue
                peerdata[aid] = pdata
            self.cache.set("peerdata", peerdata)
        return peerdata


def dolog(text, level):
    if debug >= level:
        logfp.write(text)


def connect():
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        sock.connect("/var/agentx/master")
    except socket.error as msg:
        dolog(repr(msg) + "\n", 2)
        sys.exit(1)
    return sock


class PacketControl:
    def __init__(self, sock, dbase, spinGap=0.001, timeout=5):
        # take a pre-made socket instead of making our own so that
        # PacketControl doesn't have to know or care about implementation
        self.socket = sock
        self.spinGap = spinGap  # sleep() time on each loop
        # indexed on: (session_id, transaction_id, packet_id)
        # contains: (timeout, packet class)
        self.packetLog = {}  # Sent packets kept until response is received
        self.loopCallback = None  # called each loop in runforever mode
        self.database = dbase  # class for handling data requests
        self.recievedData = ""  # buffer for data from incomplete packets
        self.recievedPackets = []  # use as FIFO
        self.timeout = timeout  # default 5s, arbitrary
        self.sessionID = None  # need this for all packets
        # indexed on pdu code
        self.pduHandlers = {ax.PDU_GET: self.handle_GetPDU,
                            ax.PDU_GET_NEXT: self.handle_GetNextPDU,
                            ax.PDU_GET_BULK: self.handle_GetBulkPDU,
                            ax.PDU_TEST_SET: self.handle_TestSetPDU,
                            ax.PDU_COMMIT_SET: self.handle_CommitSetPDU,
                            ax.PDU_UNDO_SET: self.handle_UndoSetPDU,
                            ax.PDU_CLEANUP_SET: self.handle_CleanupSetPDU}

    def mainloop(self, runforever):
        if runforever:
            while True:
                self._doloop()
                if self.loopCallback is not None:
                    self.loopCallback()
                time.sleep(self.spinGap)
        else:
            self._doloop()

    def _doloop(self):
        # loop body split out to seperate the one-shot/run-forever switches
        # from the actual logic
        self.packetEater()
        while len(self.recievedPackets) > 0:
            packet = self.recievedPackets.pop(0)
            if packet.sessionID != self.sessionID:
                resp = ax.ResponsePDU(True, packet.sessionID,
                                      packet.transactioID, packet.packetID,
                                      0, ax.REPERR_NOT_OPEN, 0)
                self.sendPacket(resp, False)
                continue
            ptype = packet.pduType
            if ptype in self.pduHandlers:
                self.pduHandlers[ptype](packet)
            else:
                dolog("dropping packet type %i, not implemented\n" % ptype, 1)

    def initNewSession(self):
        dolog("init new session...\n", 1)
        # We already have a connection, need to open a session.
        openpkt = ntp.agentx.OpenPDU(True, 23, 0, 0, self.timeout, (),
                                     "NTPsec SNMP subagent")
        self.sendPacket(openpkt, False)
        dolog("Sent open packet\n", 1)
        response = self.waitForResponse(openpkt, True)
        self.sessionID = response.sessionID
        # Register the tree
        register = ntp.agentx.RegisterPDU(True, self.sessionID, 1, 1,
                                          self.timeout, 1, ntpRootOID)
        self.sendPacket(register, False)
        dolog("Sent registration\n", 1)
        response = self.waitForResponse(register, True)

    def waitForResponse(self, opkt, ignoreSID=False):
        "Wait for a response to a specific packet, dropping everything else"
        while True:
            self.packetEater()
            while len(self.recievedPackets) > 0:
                packet = self.recievedPackets.pop(0)
                dolog("Waiting, got packet: " + repr(packet) + "\n\n", 3)
                if packet.__class__ != ntp.agentx.ResponsePDU:
                    continue
                haveit = (opkt.transactionID == packet.transactionID) and \
                         (opkt.packetID == packet.packetID)
                if ignoreSID is False:
                    haveit = haveit and (opkt.sessionID == packet.sessionID)
                if haveit is True:
                    return packet
            time.sleep(self.spinGap)

    def packetEater(self):
        "Slurps data from the input buffer and tries to parse packets from it"
        self.pollSocket()
        while True:
            datalen = len(self.recievedData)
            if datalen < 20:
                return None  # We don't even have a packet header, bail
            try:
                pkt, extraData = ntp.agentx.decode_packet(self.recievedData)
                self.recievedData = extraData
                if (self.sessionID is not None) and \
                   (pkt.sessionID != self.sessionID):
                    # sessionID does not match
                    header = {"session_id": pkt.sessionID,
                              "transaction_id": pkt.transactionID,
                              "packet_id": pkt.packetID,
                              "flags": {"bigEndian": pkt.bigEndian}}
                    self.sendErrorResponse(header, ax.RSPERR_NOT_OPEN, 0)
                    continue
                self.recievedPackets.append(pkt)
                dolog("\npacketEater got a full packet: %s\n" % repr(pkt), 3)
            except ax.ParseDataLengthError:
                return None  # this happens if we don't have all of a packet
            except (ax.ParseVersionError, ax.ParsePDUTypeError,
                    ax.ParseError) as e:
                if e.header["type"] != ax.PDU_RESPONSE:
                    # Response errors are silently dropped, per RFC
                    # Everything else sends an error response
                    self.sendErrorResponse(e.header, ax.RSPERR_PARSE_ERROR, 0)
                # *Hopefully* the packet length was correct.....
                #  if not, all packets will be scrambled. Maybe dump the
                #  whole buffer if too many failures in a row?
                self.receivedData = e.remainingData

    def sendPacket(self, packet, expectsReply):
        encoded = packet.encode()
        dolog("\nsending packet: %s\n%s \n" % (repr(packet), repr(encoded)), 4)
        self.socket.sendall(encoded)
        if expectsReply is True:
            index = (packet.sessionID,
                     packet.transactionID,
                     packet.packetID)
            self.packetLog[index] = packet

    def sendErrorResponse(self, errorHeader, errorType, errorIndex):
        err = ax.ResponsePDU(errorHeader["flags"]["bigEndian"],
                             errorHeader["session_id"],
                             errorHeader["transaction_id"],
                             errorHeader["packet_id"],
                             0, errorType, errorIndex)
        self.sendPacket(err, False)

    def pollSocket(self):
        "Reads all currently available data from the socket, non-blocking"
        data = ""
        while True:
            tmp = select.select([self.socket], [], [], 0)[0]
            if len(tmp) == 0:  # No socket, means no data available
                break
            tmp = tmp[0]
            newdata = tmp.recv(4096)  # Arbitrary value
            if len(newdata) > 0:
                dolog("Received data: " + repr(newdata) + "\n", 4)
                data += newdata
            else:
                break
        self.recievedData += data

    # ==========================
    # Packet handlers start here
    # ==========================

    def handle_GetPDU(self, packet):
        binds = []
        for oidr in packet.oidranges:
            target = oidr.start
            oid, reader, _ = self.database.getOID(target)
            if (oid != target) or (reader is None):
                binds.append(ax.Varbind(ax.VALUE_NO_SUCH_OBJECT, target))
            else:
                binds.append(reader(oid))
            # There should also be a situation that leads to noSuchInstance
            #  but I do not understand the requirements for that
        # Need to implement genError
        resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                              packet.packetID, 0, ax.ERR_NOERROR, 0, binds)
        self.sendPacket(resp, False)

    def handle_GetNextPDU(self, packet):
        binds = []
        for oidr in packet.oidranges:
            oids = self.database.getOIDsInRange(oidr, True)
            if len(oids) == 0:  # Nothing found
                binds.append(ax.Varbind(ax.VALUE_END_OF_MIB_VIEW, oidr.start))
            else:
                oid, reader, _ = oids[0]
                binds.append(reader(oid))
        # Need to implement genError
        resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                              packet.packetID, 0, ax.ERR_NOERROR, 0, binds)
        self.sendPacket(resp, False)

    def handle_GetBulkPDU(self, packet):
        binds = []
        nonreps = packet.oidranges[:packet.nonReps]
        repeats = packet.oidranges[packet.nonReps:]
        # Handle non-repeats
        for oidr in nonreps:
            oids = self.database.getOIDsInRange(oidr, True)
            if len(oids) == 0:  # Nothing found
                binds.append(ax.Varbind(ax.VALUE_END_OF_MIB_VIEW, oidr.start))
            else:
                oid, reader, _ = oids[0]
                binds.append(reader(oid))
        # Handle repeaters
        for oidr in repeats:
            oids = self.database.getOIDsInRange(oidr)
            if len(oids) == 0:  # Nothing found
                binds.append(ax.Varbind(ax.VALUE_END_OF_MIB_VIEW, oidr.start))
            else:
                for oid, reader, _ in oids[:packet.maxReps]:
                    binds.append(reader(oid))
        resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                              packet.packetID, 0, ax.ERR_NOERROR, 0, binds)
        self.sendPacket(resp, False)

    def handle_TestSetPDU(self, packet):  # WIP
        # Be advised: MOST OF THE VALIDATION IS DUMMY CODE OR DOESN'T EXIST
        # According to the RFC this is one of the most demanding parts and
        #  *has* to be gotten right
        if self.database.inSetP is True:
            pass  # Is this an error?
        # if (inSetP is True) is an error these will go in an else block
        self.database.inSetP = True
        self.database.setVarbinds = []
        self.database.setHandlers = []
        self.database.setUndoData = []
        error = None
        clearables = []
        for bindIndex in range(len(packet.varbinds)):
            varbind = packet.varbinds[bindIndex]
            # Find an OID, then validate it
            oid, reader, writer = self.database.getOID(varbind.oid)
            if oid is None:  # doesn't exist, can we create it?
                # Dummy, assume we can't create anything
                error = ax.ERR_NO_ACCESS
                break
            elif writer is None:  # exists, writing not implemented
                error = ax.ERR_NOT_WRITABLE
                break
            # Ok, we have an existing or new OID, assemble the orders
            # If we created a new bind undoData is None, must delete it
            undoData = reader(oid)
            error = writer("test", varbind)
            if error != ax.ERR_NOERROR:
                break
            self.database.setVarbinds.append(varbind)
            self.database.setHandlers.append(writer)
            self.database.setUndoData.append(undoData)
        if error != ax.ERR_NOERROR:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, 0, error, bindIndex)
            self.sendPacket(resp, False)
            for i in range(bindIndex):
                # Errored out, clear the sucessful ones
                self.database.setHandlers[i]("clear",
                                             self.database.setVarbinds[i])
            self.database.inSetP = False
        else:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, 0, ax.ERR_NOERROR, 0)
            self.sendPacket(resp, False)

    def handle_CommitSetPDU(self, packet):
        if self.database.inSetP is False:
            pass  # how to handle this?
        varbinds = self.database.setVarbinds
        handlers = self.database.setHandlers
        for i in range(len(varbinds)):
            error = handlers[i]("commit", varbinds[i])
            if error != ax.ERR_NOERROR:
                break
        if error != ax.ERR_NOERROR:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, 0, error, i)
        else:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, 0, ax.ERR_NOERROR, 0)
        print("About to send:", resp)
        self.sendPacket(resp, False)

    def handle_UndoSetPDU(self, packet):
        varbinds = self.database.setVarbinds
        handlers = self.database.setHandlers
        undoData = self.database.setUndoData
        for i in range(len(varbinds)):
            error = handlers[i]("undo", varbinds[i], undoData[i])
            if error != ax.ERR_NOERROR:
                break
        if error != ax.ERR_NOERROR:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, o, error, i)
        else:
            resp = ax.ResponsePDU(True, self.sessionID, packet.transactionID,
                                  packet.packetID, 0, ax.ERR_NOERROR, 0)
        self.sendPacket(resp, False)

    def handle_CleanupSetPDU(self, packet):
        varbinds = self.database.setVarbinds
        handlers = self.database.setHandlers
        undoData = self.database.setUndoData
        for i in range(len(varbinds)):
            handlers[i]("clean", varbinds[i])
        self.database.inSetP = False


def mainloop():
    dolog("initing loop\n", 1)
    sock = connect()
    dbase = DataSource()
    control = PacketControl(sock, dbase)
    control.initNewSession()
    control.mainloop(True)


def daemonize(runfunc):
    pid = os.fork()
    if pid < 0:
        print("Forking error", pid)
        sys.exit(pid)
    elif pid > 0:  # We are the parent
        print("Daemonization success, child pid:", pid)
        sys.exit(0)

    # We must be the child

    os.umask(0)

    global logfp
    logfp = open(logfile, "a", 1)

    sid = os.setsid()

    # chdir should be here, change to what? root?

    sys.stdin.close()
    sys.stdout.close()
    sys.stderr.close()

    runfunc()


usage = """
USAGE: ntpsnmpd [-n]
  Flg Arg Option-Name   Description
   -n no  no-fork         Do not fork and daemonize.
   -d no  debug-level     Increase output debug message level
                                - may appear multiple times
   -D Int set-debug-level Set the output debug message level
                                - may appear multiple times
   -V no  version         Output version information and exit
"""


if __name__ == "__main__":
    try:
        (options, arguments) = getopt.getopt(
            sys.argv[1:],
            "ndD:V",
            ["no-fork", "debug-level", "set-debug-level", "version"])
    except getopt.GetoptError as e:
        sys.stderr.write("%s\n" % e)
        sys.stderr.write(usage)
        raise SystemExit(1)

    for (switch, val) in options:
        if switch in ("-n", "--no-fork"):
            # currently non functional, as nofork is inited to True
            nofork = True
        elif switch in ("-d", "--debug-level"):
            debug += 1
        elif switch in ("-D", "--set-debug-level"):
            errmsg = "Error: -D parameter '%s' not a number\n"
            debug = ntp.util.safeargcast(val, int, errmsg, usage)
        elif switch in ("-V", "--version"):
            print("ntpsnmpd %s" % ntp.util.stdversion())
            raise SystemExit(0)

    if nofork is True:
        mainloop()
    else:
        daemonize(mainloop)
