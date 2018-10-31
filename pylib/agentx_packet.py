# -*- coding: utf-8 -*-
# Common utility functions
# SPDX-License-Identifier: BSD-2-clause

from __future__ import print_function

import struct
import ntp.poly
from ntp.util import slicedata


internetPrefix = (1, 3, 6, 1)  # Used by the prefix option of OID headers
prefixCount = len(internetPrefix)

# ==========================================================================
#
# Callables fall into the following categories:
#   Data type encoder/decoders
#   Packet classes/encoders
#   Packet body decoders
#   Glue/Utility/Misc functions
#
# To encode a packet, create an instance of that packet type's class, then
# call the encode() method.
#
# To decode a packet, call the decode_packet function, which will select
# the correct decoder for that packet type.
#
# ==========================================================================

# ==========================================================================
#
# Error classes
#
# ==========================================================================


class ParseError(Exception):
    def __init__(self, message, header=None, packetData="", remainingData=""):
        Exception.__init__(self)
        self.message = message
        self.header = header
        self.packetData = packetData
        self.remainingData = remainingData


class ParseDataLengthError(ParseError):
    pass


class ParseVersionError(ParseError):
    pass


class ParsePDUTypeError(ParseError):
    pass


# ==========================================================================
#
# Packet encoders / decoders
#
#   Encoders are class methods which output a string version of the
#   packet, ready for transmission.
#
#   Decoders take just the body sliced away from everything else,
#   and a dict containing the already parsed information from the header.
#   They return the relevant class instance for the packet in question,
#   they do not return extra data as they are never supposed to receive it.
#
#   Decoders are not meant to be called directly by external code,
#   only by decode_packet().
#
# ==========================================================================


class AgentXPDU:
    def __init__(self, pduType, bigEndian, sID, tactID, pktID,
                 hascontext=False, context=None):
        self.pduType = pduType
        self.bigEndian = bigEndian
        self.sessionID = sID
        self.transactionID = tactID
        self.packetID = pktID
        self.context = context
        self._hascontext = hascontext

    def packetVars(self):
        "Assembles a list of class variables that it is desirable to print"
        pktvars = {}
        names = dir(self)
        names.remove("context")
        for vname in names:
            if vname[0] == "_":  # ignores specials, and what we want ignored
                continue
            var = getattr(self, vname)
            if callable(var):  # ignores methods
                # *might* have to rethink this if contents get classified
                continue
            pktvars[vname] = var
        if self._hascontext is True:
            # context is always present, not always used
            pktvars["context"] = self.context
        return pktvars

    def __repr__(self):
        s = self.__class__.__name__ + "("
        v = []
        myvars = self.packetVars()
        keys = list(myvars.keys())
        keys.sort()  # they will always be in the same order: testable
        for name in keys:
            value = myvars[name]
            v.append("%s=%s" % (name, repr(value)))
        s += ", ".join(v) + ")"
        return s

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.pduType != other.pduType:
            return False
        if self.bigEndian != other.bigEndian:
            return False
        if self.sessionID != other.sessionID:
            return False
        if self.transactionID != other.transactionID:
            return False
        if self.packetID != other.packetID:
            return False
        if self._hascontext != other._hascontext:
            return False
        if self._hascontext is True:
            if self.context != other.context:
                return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)


def decode_OpenPDU(data, header):
    flags = header["flags"]
    temp, data = slicedata(data, 4)
    timeout = struct.unpack("Bxxx", ntp.poly.polybytes(temp))[0]
    oid, data = decode_OID(data, header)
    description = decode_octetstr(data, header)[0]
    result = OpenPDU(flags["bigEndian"], header["session_id"],
                     header["transaction_id"], header["packet_id"],
                     timeout, oid, description)
    return result


class OpenPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 timeout, oid, description):
        AgentXPDU.__init__(self, PDU_OPEN, bigEndian, sID, tactID, pktID)
        self.timeout = timeout
        self.oid = classifyOID(oid)
        self.description = description

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.timeout != other.timeout:
            return False
        if self.oid != other.oid:
            return False
        if self.description != other.description:
            return False
        return True

    def encode(self):
        payload = struct.pack("Bxxx", self.timeout)
        payload += self.oid.encode(self.bigEndian)
        payload += encode_octetstr(self.bigEndian, self.description)
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_ClosePDU(data, header):
    flags = header["flags"]
    reason = data[0]  # Bxxx
    if isinstance(reason, str):
        reason = ord(reason)
    result = ClosePDU(flags["bigEndian"], header["session_id"],
                      header["transaction_id"], header["packet_id"],
                      reason)
    return result


class ClosePDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, reason):
        AgentXPDU.__init__(self, PDU_CLOSE, bigEndian, sID, tactID, pktID)
        if reason not in definedReasons:  # pragma: no cover
            raise ValueError("Close reason %s not in defined types" % reason)
        self.reason = reason

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.reason != other.reason:
            return False
        return True

    def encode(self):
        payload = struct.pack("Bxxx", self.reason)
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_xRegisterPDU(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    context, data = decode_context(data, header)
    temp, data = slicedata(data, 4)
    timeout, priority, rangeSubid = struct.unpack(endianToken + "BBBx",
                                                  ntp.poly.polybytes(temp))
    oid, data = decode_OID(data, header)
    if rangeSubid != 0:
        temp, data = slicedata(data, 4)
        upperBound = struct.unpack(endianToken + "I",
                                   ntp.poly.polybytes(temp))[0]
    else:
        upperBound = None
    if header["type"] == PDU_REGISTER:
        result = RegisterPDU(flags["bigEndian"], header["session_id"],
                             header["transaction_id"], header["packet_id"],
                             timeout, priority, oid,
                             rangeSubid, upperBound, context)
    else:
        result = UnregisterPDU(flags["bigEndian"], header["session_id"],
                               header["transaction_id"], header["packet_id"],
                               priority, oid, rangeSubid, upperBound, context)
    return result


class RegisterPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 timeout, priority, subtree,
                 rangeSubid=0, upperBound=None, context=None):
        AgentXPDU.__init__(self, PDU_REGISTER,
                           bigEndian, sID, tactID, pktID, True, context)
        self.timeout = timeout
        self.priority = priority
        self.subtree = classifyOID(subtree)
        self.rangeSubid = rangeSubid
        self.upperBound = upperBound
        self._instReg = True  # so we don't have to write two encode()s

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if hasattr(self, "timeout"):  # Relevant to UnregisterPDU subclass
            if self.timeout != other.timeout:
                return False
        if self.priority != other.priority:
            return False
        if self.subtree != other.subtree:
            return False
        if self.rangeSubid != other.rangeSubid:
            return False
        if self.upperBound != other.upperBound:
            return False
        return True

    def encode(self):
        endianToken = getendian(self.bigEndian)
        contextP, payload = encode_context(self.bigEndian, self.context)
        if self.pduType == PDU_REGISTER:
            payload += struct.pack(endianToken + "BBBx", self.timeout,
                                   self.priority, self.rangeSubid)
        else:
            payload += struct.pack(endianToken + "xBBx",
                                   self.priority, self.rangeSubid)
        payload += self.subtree.encode(self.bigEndian)
        if self.rangeSubid != 0:
            if self.upperBound is None:
                raise ValueError("upperBound must be set if rangeSubid is set")
            payload += struct.pack(endianToken + "I", self.upperBound)
        header = encode_pduheader(self.pduType, self._instReg, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        packet = header + payload
        return packet


class UnregisterPDU(RegisterPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, priority, subtree,
                 rangeSubid=0, upperBound=None, context=None):
        RegisterPDU.__init__(self, bigEndian, sID, tactID, pktID,
                             None, priority, subtree,
                             rangeSubid, upperBound, context)
        self.pduType = PDU_UNREGISTER
        del self.timeout  # Unregister doesn't have a timeout
        self._instReg = False


def decode_xGetPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    oidranges = decode_searchrange_list(data, header)
    if header["type"] == PDU_GET_NEXT:
        result = GetNextPDU(flags["bigEndian"], header["session_id"],
                            header["transaction_id"], header["packet_id"],
                            oidranges, context)
    else:
        result = GetPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"],
                        oidranges, context)
    return result


class GetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, oidranges, context=None):
        AgentXPDU.__init__(self, PDU_GET,
                           bigEndian, sID, tactID, pktID, True, context)
        self.oidranges = oidranges

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.oidranges != other.oidranges:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_searchrange_list(self.bigEndian, self.oidranges)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


class GetNextPDU(GetPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, oidranges, context=None):
        GetPDU.__init__(self, bigEndian, sID, tactID, pktID,
                        oidranges, context)
        self.pduType = PDU_GET_NEXT


def decode_GetBulkPDU(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    context, data = decode_context(data, header)
    temp, data = slicedata(data, 4)
    nonReps, maxReps = struct.unpack(endianToken + "HH",
                                     ntp.poly.polybytes(temp))
    oidranges = decode_searchrange_list(data, header)
    result = GetBulkPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"],
                        nonReps, maxReps, oidranges, context)
    return result


class GetBulkPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 nonReps, maxReps, oidranges, context=None):
        AgentXPDU.__init__(self, PDU_GET_BULK,
                           bigEndian, sID, tactID, pktID, True, context)
        self.nonReps = nonReps
        self.maxReps = maxReps
        self.oidranges = oidranges

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.nonReps != other.nonReps:
            return False
        if self.maxReps != other.maxReps:
            return False
        if self.oidranges != other.oidranges:
            return False
        return True

    def encode(self):
        endianToken = getendian(self.bigEndian)
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += struct.pack(endianToken + "HH", self.nonReps, self.maxReps)
        payload += encode_searchrange_list(self.bigEndian, self.oidranges)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_TestSetPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    varbinds = decode_varbindlist(data, header)
    result = TestSetPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"],
                        varbinds, context)
    return result


class TestSetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, varbinds, context=None):
        AgentXPDU.__init__(self, PDU_TEST_SET,
                           bigEndian, sID, tactID, pktID, True, context)
        self.varbinds = varbinds

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.varbinds != other.varbinds:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_varbindlist(self.bigEndian, self.varbinds)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_CommitSetPDU(data, header):
    flags = header["flags"]
    result = CommitSetPDU(flags["bigEndian"], header["session_id"],
                          header["transaction_id"], header["packet_id"])
    return result


class CommitSetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID):
        AgentXPDU.__init__(self, PDU_COMMIT_SET,
                           bigEndian, sID, tactID, pktID)

    def encode(self):
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, 0)
        return header


def decode_UndoSetPDU(data, header):
    flags = header["flags"]
    result = UndoSetPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"])
    return result


class UndoSetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID):
        AgentXPDU.__init__(self, PDU_UNDO_SET,
                           bigEndian, sID, tactID, pktID)

    def encode(self):
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, 0)
        return header


def decode_CleanupSetPDU(data, header):
    flags = header["flags"]
    result = CleanupSetPDU(flags["bigEndian"], header["session_id"],
                           header["transaction_id"], header["packet_id"])
    return result


class CleanupSetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID):
        AgentXPDU.__init__(self, PDU_CLEANUP_SET,
                           bigEndian, sID, tactID, pktID)

    def encode(self):
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, 0)
        return header


def decode_PingPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    result = PingPDU(flags["bigEndian"], header["session_id"],
                     header["transaction_id"], header["packet_id"],
                     context)
    return result


class PingPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, context=None):
        AgentXPDU.__init__(self, PDU_PING,
                           bigEndian, sID, tactID, pktID, True, context)

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_NotifyPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    varbinds = decode_varbindlist(data, header)
    result = NotifyPDU(flags["bigEndian"], header["session_id"],
                       header["transaction_id"], header["packet_id"],
                       varbinds, context)
    return result


class NotifyPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, varbinds, context=None):
        AgentXPDU.__init__(self, PDU_NOTIFY,
                           bigEndian, sID, tactID, pktID, True, context)
        self.varbinds = varbinds

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.varbinds != other.varbinds:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_varbindlist(self.bigEndian, self.varbinds)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_xIndexAllocPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    varbinds = decode_varbindlist(data, header)
    isalloc = (header["type"] == PDU_INDEX_ALLOC)
    pdu = IndexAllocPDU if isalloc else IndexDeallocPDU
    result = pdu(flags["bigEndian"], header["session_id"],
                 header["transaction_id"], header["packet_id"],
                 flags["newIndex"], flags["anyIndex"],
                 varbinds, context)
    return result


class IndexAllocPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 newIndex, anyIndex, varbinds, context=None):
        AgentXPDU.__init__(self, PDU_INDEX_ALLOC,
                           bigEndian, sID, tactID, pktID, True, context)
        self.newIndex = newIndex
        self.anyIndex = anyIndex
        self.varbinds = varbinds

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.newIndex != other.newIndex:
            return False
        if self.anyIndex != other.anyIndex:
            return False
        if self.varbinds != other.varbinds:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_varbindlist(self.bigEndian, self.varbinds)
        header = encode_pduheader(self.pduType,
                                  False, self.newIndex, self.anyIndex,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


class IndexDeallocPDU(IndexAllocPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 newIndex, anyIndex, varbinds, context=None):
        IndexAllocPDU.__init__(self, bigEndian, sID, tactID, pktID,
                               newIndex, anyIndex, varbinds, context)
        self.pduType = PDU_INDEX_DEALLOC


def decode_AddAgentCapsPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    oid, data = decode_OID(data, header)
    descr = decode_octetstr(data, header)[0]
    result = AddAgentCapsPDU(flags["bigEndian"], header["session_id"],
                             header["transaction_id"], header["packet_id"],
                             oid, descr, context)
    return result


class AddAgentCapsPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 oid, description, context=None):
        AgentXPDU.__init__(self, PDU_ADD_AGENT_CAPS,
                           bigEndian, sID, tactID, pktID, True, context)
        self.oid = classifyOID(oid)
        self.description = description

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.oid != other.oid:
            return False
        if self.description != other.description:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += self.oid.encode(self.bigEndian)
        payload += encode_octetstr(self.bigEndian, self.description)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_RMAgentCapsPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    oid, data = decode_OID(data, header)
    result = RMAgentCapsPDU(flags["bigEndian"], header["session_id"],
                            header["transaction_id"], header["packet_id"],
                            oid, context)
    return result


class RMAgentCapsPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, oid, context=None):
        AgentXPDU.__init__(self, PDU_RM_AGENT_CAPS,
                           bigEndian, sID, tactID, pktID, True, context)
        self.oid = classifyOID(oid)

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.oid != other.oid:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += self.oid.encode(self.bigEndian)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_ResponsePDU(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    temp, data = slicedata(data, 8)
    sysUptime, resError, resIndex = struct.unpack(endianToken + "IHH",
                                                  ntp.poly.polybytes(temp))
    if len(data) > 0:
        varbinds = decode_varbindlist(data, header)
    else:
        varbinds = None
    result = ResponsePDU(flags["bigEndian"], header["session_id"],
                         header["transaction_id"], header["packet_id"],
                         sysUptime, resError, resIndex, varbinds)
    return result


class ResponsePDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 sysUptime, resError, resIndex, varbinds=None):
        AgentXPDU.__init__(self, PDU_RESPONSE, bigEndian, sID, tactID, pktID)
        self.sysUptime = sysUptime
        self.resError = resError
        self.resIndex = resIndex
        self.varbinds = varbinds

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.sysUptime != other.sysUptime:
            return False
        if self.resError != other.resError:
            return False
        if self.resIndex != other.resIndex:
            return False
        if self.varbinds != other.varbinds:
            return False
        return True

    def encode(self):
        endianToken = getendian(self.bigEndian)
        payload = struct.pack(endianToken + "IHH", self.sysUptime,
                              self.resError, self.resIndex)
        if self.varbinds is not None:
            payload += encode_varbindlist(self.bigEndian, self.varbinds)
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


# ========================================================================
#
# Data type encoders / decoders
#
#   Encoders take data relevant to the type, return encoded string.
#
#   Decoders take encoded string, return a tuple of (value, restOfData).
#
# ========================================================================


def classifyOID(oid):
    "Utility function to allow the user to send a bare tuple for some cases"
    if isinstance(oid, OID):
        return oid
    return OID(oid, False)


def decode_OID(data, header):
    flags = header["flags"]
    # Need to split off the header to get the subid count
    header, data = slicedata(data, 4)
    n_subid, prefix, include = struct.unpack("BBBx", ntp.poly.polybytes(header))
    if prefix != 0:
        subids = internetPrefix + (prefix,)
    else:
        subids = ()
    totalLen = len(subids) + n_subid
    if not (0 <= totalLen <= 128):
        raise ValueError("OID has too many subids")
    include = bool(include)  # could be anything, force bool
    # Now have the count, need to split the subids from the rest of the packet
    byteCount = n_subid * 4
    data, rest = slicedata(data, byteCount)
    endianToken = getendian(flags["bigEndian"])
    formatString = endianToken + ("I" * n_subid)
    subids += struct.unpack(formatString, ntp.poly.polybytes(data))
    result = OID(subids, include)
    return (result, rest)


class OID:
    def __init__(self, subids, include=False):
        self.subids = subids
        self.include = include
        self.sanity()

    def __eq__(self, other):
        if isinstance(other, OID) is False:
            return False
        if not (self.subids == other.subids):
            return False
        elif self.include != other.include:
            return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        return self.compareOID(other) == -1

    def __le__(self, other):
        return self.compareOID(other) in (-1, 0)

    def __gt__(self, other):
        return self.compareOID(other) == 1

    def __ge__(self, other):
        return self.compareOID(other) in (0, 1)

    def compareOID(self, other):
        if self.subids == other.subids:
            return 0
        lself = len(self.subids)
        lother = len(other.subids)
        if lself > lother:
            x = other.subids
            y = self.subids
            lx = lother
            flipped = True
        else:
            x = self.subids
            y = other.subids
            lx = lself
            flipped = False
        for i in range(lx):
            if x[i] == y[i]:
                continue
            else:
                # this is the Py3 version of c = cmp(x[i], y[i])
                c = (x[i] > y[i]) - (x[i] < y[i])
                c = -c if flipped is True else c
                return c
        # Only reach this if shorter, and each index is equal
        if flipped is True:
            return 1
        else:
            return -1

    def __repr__(self):
        return "OID(%s, %s)" % (repr(self.subids), repr(self.include))

    def sanity(self):
        if not isinstance(self.subids, (tuple, list)):
            raise TypeError
        if len(self.subids) > 128:
            raise ValueError

    def isNull(self):
        if (len(self.subids) == 0) and (self.include is False):
            return True
        return False

    def encode(self, bigEndian):
        subids = self.subids[:]  # we may need to modify the copy
        numsubs = len(subids)
        if not (0 <= numsubs <= 128):  # OIDs can have a maximum of 128 subs
            raise ValueError("OID has too many subids")
        if subids[:prefixCount] == internetPrefix:
            if numsubs > prefixCount:
                prefix = subids[prefixCount]  # the number after the prefix
                subids = subids[prefixCount + 1:]  # +1 to strip prefix arg
            else:  # Have a prefix but nothing else, leave it as is.
                prefix = 0
        else:
            prefix = 0
        n_subid = len(subids)
        include = int(bool(self.include))  # force integer bool
        endianToken = getendian(bigEndian)
        body = struct.pack(endianToken + "BBBx", n_subid, prefix, include)
        for subid in subids:
            body += struct.pack(endianToken + "I", subid)
        return body


def encode_octetstr(bigEndian, octets):
    numoctets = len(octets)
    endianToken = getendian(bigEndian)
    header = struct.pack(endianToken + "I", numoctets)
    pad = (numoctets % 4)
    if pad > 0:  # Pad out the data to word boundary
        pad = 4 - pad
    pad = b"\x00" * pad
    if type(octets) is str:
        octets = ntp.poly.polybytes(octets)
        data = header + octets + pad
    else:
        fmt = "B" * numoctets
        data = struct.pack(fmt, *octets)
        data = header + data + pad
    return data


def decode_octetstr(data, header):
    flags = header["flags"]
    header, data = slicedata(data, 4)
    endianToken = getendian(flags["bigEndian"])
    numoctets = struct.unpack(endianToken + "I", ntp.poly.polybytes(header))[0]
    if len(data) < numoctets:
        raise ValueError("Octet string shorter than length")
    pad = numoctets % 4
    if pad > 0:  # Pad out the data to word boundary
        pad = 4 - pad
    return ntp.poly.polystr(data[:numoctets]), data[numoctets + pad:]


def sanity_octetstr(data):
    if isinstance(data, str):
        return
    if isinstance(data, (list, tuple)):
        for c in data:
            if not (0 <= c < 256):
                raise ValueError
        return
    raise TypeError


def decode_Varbind(data, header):
    flags = header["flags"]
    bindheader, data = slicedata(data, 4)
    endianToken = getendian(flags["bigEndian"])
    valType = struct.unpack(endianToken + "Hxx",
                            ntp.poly.polybytes(bindheader))[0]
    name, data = decode_OID(data, header)
    if valType not in definedValueTypes.keys():
        raise ValueError("Value type %s not in defined types" % valType)
    handlers = definedValueTypes[valType]
    payload, data = handlers[2](data, header)
    result = Varbind(valType, name, payload)
    return result, data


class Varbind:
    def __init__(self, vtype, oid, payload=None):
        self.valueType = vtype
        self.oid = classifyOID(oid)
        self.payload = payload  # payload=None exists for Null types
        self.sanity()

    def __eq__(self, other):
        if self.valueType != other.valueType:
            return False
        if self.oid != other.oid:
            return False
        if self.payload != other.payload:
            return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        fmt = "Varbind(vtype=%s, oid=%s, payload=%s)"
        r = fmt % (repr(self.valueType), repr(self.oid), repr(self.payload))
        return r

    def sanity(self):
        self.oid.sanity()
        vt = self.valueType
        if vt not in definedValueTypes.keys():
            raise ValueError("Value type %s not in defined types" % vt)
        sanifyer, encoder, decoder = definedValueTypes[vt]
        if sanifyer is None:  # it is a class
            self.payload.sanity()
        else:
            sanifyer(self.payload)

    def encode(self, bigEndian):
        endianToken = getendian(bigEndian)
        header = struct.pack(endianToken + "Hxx", self.valueType)
        name = self.oid.encode(bigEndian)
        handlers = definedValueTypes[self.valueType]
        sanifyer, encoder, decoder = handlers
        if sanifyer is None:  # Classes have the .sanity() method
            data = header + name + self.payload.encode(bigEndian)
        else:  # Non-classes have an associated sanity_<vartype>() function
            data = header + name + encoder(bigEndian, self.payload)
        return data


def encode_integer32(bigEndian, num):
    endianToken = getendian(bigEndian)
    return struct.pack(endianToken + "i", num)


def decode_integer32(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    num, data = slicedata(data, 4)
    num = struct.unpack(endianToken + "i", num)[0]
    return (num, data)


def sanity_integer32(data):
    if type(data) is not int:
        raise TypeError("%s is not integer" % repr(data))


def encode_unsigned32(bigEndian, num):
    endianToken = getendian(bigEndian)
    return struct.pack(endianToken + "I", num)


def decode_unsigned32(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    num, data = slicedata(data, 4)
    num = struct.unpack(endianToken + "I", num)[0]
    return (num, data)


def sanity_unsigned32(data):  # pragma: no cover
    if data != (data & 0xFFFFFFFF):
        raise ValueError


def encode_nullvalue(bigEndian, data):
    return b""


def decode_nullvalue(data, header):
    return (None, data)


def sanity_nullvalue(data):
    pass  # Seriously?


def encode_integer64(bigEndian, num):
    endianToken = getendian(bigEndian)
    return struct.pack(endianToken + "Q", num)


def decode_integer64(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    num, data = slicedata(data, 8)
    num = struct.unpack(endianToken + "Q", ntp.poly.polybytes(num))[0]
    return (num, data)


def sanity_integer64(data):
    if data != (data & 0xFFFFFFFFFFFFFFFF):
        raise ValueError


def encode_ipaddr(bigEndian, octets):
    sanity_ipaddr(octets)
    return encode_octetstr(bigEndian, octets)


def decode_ipaddr(data, header):
    addr, data = decode_octetstr(data, header)
    addr = struct.unpack("BBBB", ntp.poly.polybytes(addr))
    return addr, data


def sanity_ipaddr(data):
    if len(data) not in (4, 16):
        raise ValueError


def decode_SearchRange(data, header):
    startOID, data = decode_OID(data, header)
    endOID, data = decode_OID(data, header)
    result = SearchRange(startOID, endOID)
    return result, data


class SearchRange:
    def __init__(self, start, end, include=None):
        self.start = classifyOID(start)
        self.end = classifyOID(end)
        self.end.include = False  # sanify
        if include is not None:
            # if the user does not provide include it defaults to whatever
            # start is already set to
            self.start.include = include
        self.sanity()

    def __eq__(self, other):
        if self.start != other.start:
            return False
        if self.end != other.end:
            return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        r = "SearchRange(%s, %s)" % (repr(self.start), repr(self.end))
        return r

    def sanity(self):
        self.start.sanity()
        self.end.sanity()
        if self.end.include is True:
            raise ValueError

    def encode(self, bigEndian):
        startOIDstr = self.start.encode(bigEndian)
        endOIDstr = self.end.encode(bigEndian)
        return startOIDstr + endOIDstr


def encode_searchrange_list(bigEndian, searchranges):
    encoded = []
    for sran in searchranges:
        encoded.append(sran.encode(bigEndian))
    encoded = b"".join(encoded)
    return encoded


def decode_searchrange_list(data, header):  # Cannot handle extra data
    oidranges = []
    while len(data) > 0:
        oids, data = decode_SearchRange(data, header)
        oidranges.append(oids)
    return tuple(oidranges)


def encode_varbindlist(bigEndian, varbinds):
    payload = b""
    for varbind in varbinds:
        payload += varbind.encode(bigEndian)
    return payload


def decode_varbindlist(data, header):
    if len(data) > 0:
        varbinds = []
        while len(data) > 0:
            vb, data = decode_Varbind(data, header)
            varbinds.append(vb)
        varbinds = tuple(varbinds)
    else:
        varbinds = None
    return varbinds


def encode_flagbyte(flags):
    flagbyte = 0
    flagbyte |= flags["instReg"]
    flagbyte |= flags["newIndex"] << 1
    flagbyte |= flags["anyIndex"] << 2
    flagbyte |= flags["contextP"] << 3  # nonDefaultContext
    flagbyte |= flags["bigEndian"] << 4
    return flagbyte


def decode_flagbyte(flags):
    flagDict = makeflags(bool(flags & 0x1), bool(flags & 0x2),
                         bool(flags & 0x4), bool(flags & 0x8),
                         bool(flags & 0x10))
    return flagDict


# =========================================
# Utilities, glue, and misc
# =========================================


def makeflags(iR, nI, aI, cP, bE):
    return {"instReg": iR,
            "newIndex": nI,
            "anyIndex": aI,
            "contextP": cP,
            "bigEndian": bE}


def getendian(bigEndian):
    return ">" if bigEndian is True else "<"


def encode_pduheader(pduType, instanceRegistration, newIndex,
                     anyIndex, nonDefaultContext, bigEndian,
                     sessionID, transactionID, packetID, payloadLength):
    # version type flags reserved
    # sessionID
    # transactionID
    # packetID
    # payload_length
    if pduType not in definedPDUTypes:
        raise ValueError("PDU type %s not in defined types" % pduType)
    flags = encode_flagbyte(makeflags(instanceRegistration, newIndex,
                                      anyIndex, nonDefaultContext,
                                      bigEndian))
    # Yes, this is a kluge, it is less problematic then the next best kluge
    endianToken = getendian(bigEndian)
    data = struct.pack(endianToken + "BBBxIIII", 1, pduType, flags,
                       sessionID, transactionID,
                       packetID, payloadLength)
    return data


def decode_pduheader(data):  # Endianness is controlled from the PDU header
    lineone, data = slicedata(data, 4)
    version, pduType, flags = struct.unpack(">BBBx",
                                            ntp.poly.polybytes(lineone))
    # Slice up the flags
    flagDict = decode_flagbyte(flags)
    # Chop the remaining parts of the header from the rest of the datastream
    # then parse them
    fmt = getendian(flagDict["bigEndian"]) + "IIII"
    linen, data = slicedata(data, 16)  # 4 x 4-byte variables
    sID, tactionID, pktID, dataLen = struct.unpack(fmt,
                                                   ntp.poly.polybytes(linen))
    result = {"version": version, "type": pduType, "flags": flagDict,
              "session_id": sID, "transaction_id": tactionID,
              "packet_id": pktID, "length": dataLen}
    return result


def encode_context(bigEndian, context):
    if context is not None:
        contextP = True
        payload = encode_octetstr(bigEndian, context)
    else:
        contextP = False
        payload = b""
    return (contextP, payload)


def decode_context(data, header):
    flags = header["flags"]
    if flags["contextP"] is True:
        context, data = decode_octetstr(data, header)
    else:
        context = None
    return (context, data)


def decode_packet(data):
    if len(data) < 20:
        raise ParseDataLengthError("Data too short for header")
    header, newData = slicedata(data, 20)
    header = decode_pduheader(header)
    if header["length"] > len(newData):
        return None, False, data  # pkt, isFull?, buffer
    packetSlice, newData = slicedata(newData, header["length"])
    if header["version"] != 1:
        raise ParseVersionError("Unknown packet version %i" %
                                header["version"],
                                header, packetSlice, newData)
    pktType = header["type"]
    if pktType not in definedPDUTypes.keys():
        raise ParsePDUTypeError("PDU type %s not in defined types" % pktType,
                                header, packetSlice, newData)
    decoder = definedPDUTypes[pktType]
    try:
        parsedPkt = decoder(packetSlice, header)
    except Exception:  # pragma: no cover
        err = ParseError("Body parsing error", header, packetSlice, newData)
        raise err
    return parsedPkt, True, newData  # pkt, isFull?, buffer


def bits2Bools(bitString, cropLength=None):
    bits = []
    for octet in bitString:
        octet = ord(octet)
        bits.append(bool(octet & 0x80))  # Yes, these are backwards, that is
        bits.append(bool(octet & 0x40))  # how SNMP wants them. It does make
        bits.append(bool(octet & 0x20))  # sense if you think about it as a
        bits.append(bool(octet & 0x10))  # stream of bits instead of octets.
        bits.append(bool(octet & 0x08))
        bits.append(bool(octet & 0x04))  # If you don't like it go yell at
        bits.append(bool(octet & 0x02))  # the SNMP designers.
        bits.append(bool(octet & 0x01))
    if cropLength is not None:  # used when a bitfield is not a multiple of 8
        bits = bits[:cropLength]
    return bits


def bools2Bits(bits):
    bitCounter = 0
    octets = []
    current = 0
    for bit in bits:
        current += (int(bit) << (7 - bitCounter))
        bitCounter += 1
        if bitCounter >= 8:  # end of byte
            bitCounter = 0
            octets.append(chr(current))
            current = 0
    else:
        if bitCounter != 0:
            octets.append(chr(current))
    octets = "".join(octets)
    return octets


# Value types
VALUE_INTEGER = 2
VALUE_OCTET_STR = 4
VALUE_NULL = 5
VALUE_OID = 6
VALUE_IP_ADDR = 64
VALUE_COUNTER32 = 65
VALUE_GAUGE32 = 66
VALUE_TIME_TICKS = 67
VALUE_OPAQUE = 68
VALUE_COUNTER64 = 70
VALUE_NO_SUCH_OBJECT = 128
VALUE_NO_SUCH_INSTANCE = 129
VALUE_END_OF_MIB_VIEW = 130
# Sub-tuple format: (sanityChecker, encoder/class, decoder)
# if the sanityChecker is None it means the data type is held in
# a class, which has it's own .sanity() method
definedValueTypes = {  # Used by the varbind functions
    VALUE_INTEGER: (sanity_integer32,
                    encode_integer32,
                    decode_integer32),
    VALUE_OCTET_STR: (sanity_octetstr,
                      encode_octetstr,
                      decode_octetstr),
    VALUE_NULL: (sanity_nullvalue,
                 encode_nullvalue,
                 decode_nullvalue),
    VALUE_OID: (None, OID, decode_OID),
    VALUE_IP_ADDR: (sanity_ipaddr,
                    encode_ipaddr,
                    decode_ipaddr),
    VALUE_COUNTER32: (sanity_unsigned32,
                      encode_unsigned32,
                      decode_unsigned32),
    VALUE_GAUGE32: (sanity_unsigned32,
                    encode_unsigned32,
                    decode_unsigned32),
    VALUE_TIME_TICKS: (sanity_unsigned32,
                       encode_unsigned32,
                       decode_unsigned32),
    VALUE_OPAQUE: (sanity_octetstr,
                   encode_octetstr,
                   decode_octetstr),
    VALUE_COUNTER64: (sanity_integer64,
                      encode_integer64,
                      decode_integer64),
    VALUE_NO_SUCH_OBJECT: (sanity_nullvalue,
                           encode_nullvalue,
                           decode_nullvalue),
    VALUE_NO_SUCH_INSTANCE: (sanity_nullvalue,
                             encode_nullvalue,
                             decode_nullvalue),
    VALUE_END_OF_MIB_VIEW: (sanity_nullvalue,
                            encode_nullvalue,
                            decode_nullvalue)}

# PDU types
PDU_OPEN = 1
PDU_CLOSE = 2
PDU_REGISTER = 3
PDU_UNREGISTER = 4
PDU_GET = 5
PDU_GET_NEXT = 6
PDU_GET_BULK = 7
PDU_TEST_SET = 8
PDU_COMMIT_SET = 9
PDU_UNDO_SET = 10
PDU_CLEANUP_SET = 11
PDU_NOTIFY = 12
PDU_PING = 13
PDU_INDEX_ALLOC = 14
PDU_INDEX_DEALLOC = 15
PDU_ADD_AGENT_CAPS = 16
PDU_RM_AGENT_CAPS = 17
PDU_RESPONSE = 18
definedPDUTypes = {  # Used by the decode_packet function
    PDU_OPEN: decode_OpenPDU,
    PDU_CLOSE: decode_ClosePDU,
    PDU_REGISTER: decode_xRegisterPDU,
    PDU_UNREGISTER: decode_xRegisterPDU,
    PDU_GET: decode_xGetPDU,
    PDU_GET_NEXT: decode_xGetPDU,
    PDU_GET_BULK: decode_GetBulkPDU,
    PDU_TEST_SET: decode_TestSetPDU,
    PDU_COMMIT_SET: decode_CommitSetPDU,
    PDU_UNDO_SET: decode_UndoSetPDU,
    PDU_CLEANUP_SET: decode_CleanupSetPDU,
    PDU_NOTIFY: decode_NotifyPDU,
    PDU_PING: decode_PingPDU,
    PDU_INDEX_ALLOC: decode_xIndexAllocPDU,
    PDU_INDEX_DEALLOC: decode_xIndexAllocPDU,
    PDU_ADD_AGENT_CAPS: decode_AddAgentCapsPDU,
    PDU_RM_AGENT_CAPS: decode_RMAgentCapsPDU,
    PDU_RESPONSE: decode_ResponsePDU}

# Closing reasons
RSN_OTHER = 1
RSN_PARSE_ERROR = 2
RSN_PROTOCOL_ERROR = 3
RSN_TIMEOUT = 4
RSN_SHUTDOWN = 5
RSN_BY_MANAGER = 6
definedReasons = (RSN_OTHER, RSN_PARSE_ERROR, RSN_PROTOCOL_ERROR,
                  RSN_TIMEOUT, RSN_SHUTDOWN, RSN_BY_MANAGER)

# Error reasons
ERR_NOERROR = 0
ERR_GENERR = 5
ERR_NO_ACCESS = 6
ERR_WRONG_TYPE = 7
ERR_WRONG_LEN = 8
ERR_WRONG_ENCODING = 9
ERR_WRONG_VALUE = 10
ERR_NO_CREATION = 11
ERR_INCONSISTENT_VALUE = 12
ERR_RESOURCE_UNAVAILABLE = 13
ERR_COMMIT_FAILED = 14
ERR_UNDO_FAILED = 15
ERR_NOT_WRITABLE = 17
ERR_INCONSISTENT_NAME = 18
definedErrors = (ERR_NOERROR, ERR_GENERR, ERR_NO_ACCESS, ERR_WRONG_TYPE,
                 ERR_WRONG_LEN, ERR_WRONG_ENCODING, ERR_WRONG_VALUE,
                 ERR_NO_CREATION, ERR_INCONSISTENT_VALUE,
                 ERR_RESOURCE_UNAVAILABLE, ERR_NOT_WRITABLE,
                 ERR_INCONSISTENT_NAME)

RSPERR_NO_AGENTX = 0
RSPERR_OPEN_FAILED = 265
RSPERR_NOT_OPEN = 257
RSPERR_INDEX_WRONG_TYPE = 258
RSPERR_INDEX_ALREADY_ALLOCATED = 259
RSPERR_INDEX_NONE_AVAILABLE = 260
RSPERR_INDEX_NOT_ALLOCATED = 261
RSPERR_UNSUPPORTED_CONTEXT = 262
RSPERR_DUPLICATE_REGISTRATION = 263
RSPERR_UNKNOWN_REGISTRATION = 264
RSPERR_UNKNOWN_AGENT_CAPS = 265
RSPERR_PARSE_ERROR = 266
RSPERR_REQUEST_DENIED = 267
RSPERR_PROCESSING_ERROR = 268
responseErrors = (RSPERR_NO_AGENTX, RSPERR_OPEN_FAILED, RSPERR_NOT_OPEN,
                  RSPERR_INDEX_WRONG_TYPE, RSPERR_INDEX_ALREADY_ALLOCATED,
                  RSPERR_INDEX_NONE_AVAILABLE, RSPERR_INDEX_NOT_ALLOCATED,
                  RSPERR_UNSUPPORTED_CONTEXT, RSPERR_DUPLICATE_REGISTRATION,
                  RSPERR_UNKNOWN_REGISTRATION, RSPERR_UNKNOWN_AGENT_CAPS,
                  RSPERR_PARSE_ERROR, RSPERR_REQUEST_DENIED,
                  RSPERR_PROCESSING_ERROR)
