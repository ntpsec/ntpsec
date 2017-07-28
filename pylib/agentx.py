# -*- coding: utf-8 -*-
# Common utility functions
# SPDX-License-Identifier: BSD-2-clause

from __future__ import print_function

import struct

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
# Packet encoders / decoders
#
#   Encoders are class methods which output a string version of the
#   packet, ready for transmission.
#
#   Decoders take just the body sliced away from everything else,
#   and a dict containing the already pared information from the header.
#   They return the relevant class instance for the packet in question,
#   they do not return extra data as they are never supposed to receive it.
#
#   Decoders are not meant to be called directly by external code,
#   only by decode_packet().
#
# ==========================================================================


class AgentXPDU:
    def __init__(self, pduType, bigEndian, sID, tactID, pktID, context=None):
        self.pduType = pduType
        self.bigEndian = bigEndian
        self.sessionID = sID
        self.transactionID = tactID
        self.packetID = pktID
        self.context = context

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
        if self.context != other.context:
            return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)


def decode_OpenPDU(data, header):
    flags = header["flags"]
    temp, data = slicedata(data, 4)
    timeout = struct.unpack("Bxxx", temp)[0]
    oid, data = decode_oid(data, header)
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
        self.oid = oid
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
        payload += encode_oid(self.bigEndian, self.oid, False)
        payload += encode_octetstr(self.bigEndian, self.description)
        header = encode_pduheader(self.pduType,
                                  False, False, False, False, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_ClosePDU(data, header):
    flags = header["flags"]
    reason = ord(data[0])  # Bxxx
    result = ClosePDU(flags["bigEndian"], header["session_id"],
                      header["transaction_id"], header["packet_id"],
                      reason)
    return result


class ClosePDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, reason):
        AgentXPDU.__init__(self, PDU_CLOSE, bigEndian, sID, tactID, pktID)
        if reason not in definedReasons:
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
    timeout, priority, rangeSubid = struct.unpack(endianToken + "BBBx", temp)
    oid, data = decode_oid(data, header)
    if rangeSubid != 0:
        temp, data = slicedata(data, 4)
        upperBound = struct.unpack(endianToken + "I", temp)[0]
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
                           bigEndian, sID, tactID, pktID, context)
        self.timeout = timeout
        self.priority = priority
        self.subtree = subtree
        self.rangeSubid = rangeSubid
        self.upperBound = upperBound
        self.instReg = True  # so we don't have to write two encode()s

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
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
        payload += encode_oid(self.bigEndian, self.subtree, False)
        if self.rangeSubid != 0:
            if self.upperBound is None:
                raise ValueError("upperBound must be set if rangeSubid is set")
            payload += struct.pack(endianToken + "I", self.upperBound)
        header = encode_pduheader(self.pduType, self.instReg, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        packet = header + payload
        return packet


class UnregisterPDU(RegisterPDU):  # These could inherit in either direction
    def __init__(self, bigEndian, sID, tactID, pktID, priority, subtree,
                 rangeSubid=0, upperBound=None, context=None):
        RegisterPDU.__init__(self, bigEndian, sID, tactID, pktID,
                             None, priority, subtree,
                             rangeSubid, upperBound, context)
        self.pduType = PDU_UNREGISTER
        self.instReg = False


def decode_xGetPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    if header["type"] == PDU_GET_NEXT:
        oidranges = decode_searchrange_list(data, header)
        result = GetNextPDU(flags["bigEndian"], header["session_id"],
                            header["transaction_id"], header["packet_id"],
                            oidranges, context)
    else:
        oidranges, data = decode_searchrange_list_nullterm(data, header)
        result = GetPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"],
                        oidranges, context)
    return result


class GetPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, oidranges, context=None):
        AgentXPDU.__init__(self, PDU_GET,
                           bigEndian, sID, tactID, pktID, context)
        self.oidranges = oidranges
        self.nullTerm = True

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.oidranges != other.oidranges:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_searchrange_list(self.bigEndian,
                                           self.oidranges, self.nullTerm)
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
        self.nullTerm = False


def decode_GetBulkPDU(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    context, data = decode_context(data, header)
    temp, data = slicedata(data, 4)
    nonReps, maxReps = struct.unpack(endianToken + "HH", temp)
    oidranges = decode_searchrange_list(data, header)
    result = GetBulkPDU(flags["bigEndian"], header["session_id"],
                        header["transaction_id"], header["packet_id"],
                        nonReps, maxReps, oidranges, context)
    return result


class GetBulkPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 nonReps, maxReps, oidranges, context=None):
        AgentXPDU.__init__(self, PDU_GET_BULK,
                           bigEndian, sID, tactID, pktID, context)
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
        payload += encode_searchrange_list(self.bigEndian,
                                           self.oidranges, False)
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
                           bigEndian, sID, tactID, pktID, context)
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
                           bigEndian, sID, tactID, pktID, context)

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
                           bigEndian, sID, tactID, pktID, context)
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
                           bigEndian, sID, tactID, pktID, context)
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
    oid, data = decode_oid(data, header)
    descr = decode_octetstr(data, header)[0]
    result = AddAgentCapsPDU(flags["bigEndian"], header["session_id"],
                             header["transaction_id"], header["packet_id"],
                             oid, descr, context)
    return result


class AddAgentCapsPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID,
                 oid, description, context=None):
        AgentXPDU.__init__(self, PDU_ADD_AGENT_CAPS,
                           bigEndian, sID, tactID, pktID, context)
        self.oid = oid
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
        payload += encode_oid(self.bigEndian, self.oid, False)
        payload += encode_octetstr(self.bigEndian, self.description)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_RMAgentCapsPDU(data, header):
    flags = header["flags"]
    context, data = decode_context(data, header)
    oid, data = decode_oid(data, header)
    result = RMAgentCapsPDU(flags["bigEndian"], header["session_id"],
                            header["transaction_id"], header["packet_id"],
                            oid, context)
    return result


class RMAgentCapsPDU(AgentXPDU):
    def __init__(self, bigEndian, sID, tactID, pktID, oid, context=None):
        AgentXPDU.__init__(self, PDU_RM_AGENT_CAPS,
                           bigEndian, sID, tactID, pktID, context)
        self.oid = oid

    def __eq__(self, other):
        if AgentXPDU.__eq__(self, other) is not True:
            return False
        if self.oid != other.oid:
            return False
        return True

    def encode(self):
        contextP, payload = encode_context(self.bigEndian, self.context)
        payload += encode_oid(self.bigEndian, self.oid, False)
        header = encode_pduheader(self.pduType, False, False, False,
                                  contextP, self.bigEndian,
                                  self.sessionID, self.transactionID,
                                  self.packetID, len(payload))
        return header + payload


def decode_ResponsePDU(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    temp, data = slicedata(data, 8)
    sysUptime, resError, resIndex = struct.unpack(endianToken + "IHH", temp)
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

def encode_oid(bigEndian, subids, include):
    subids = tuple(subids)
    numsubs = len(subids)
    if not (0 <= numsubs <= 128):  # Packet can have a maximum of 128 sections
        raise ValueError("OID has too many subids")
    if subids[:prefixCount] == internetPrefix:
        if numsubs > prefixCount:
            prefix = subids[prefixCount]  # the number after the prefix
            subids = subids[prefixCount + 1:]  # +1 to strip prefix arg
        else:  # Have a prefix, but nothing else. Leave as is
            prefix = 0
    else:
        prefix = 0
    n_subid = len(subids)
    include = int(bool(include))  # force integer bool
    endianToken = getendian(bigEndian)
    body = struct.pack(endianToken + "BBBx", n_subid, prefix, include)
    for subid in subids:
        body += struct.pack(endianToken + "I", subid)
    return body


def decode_oid(data, header):
    flags = header["flags"]
    # Need to split off the header to get the subid count
    header, data = slicedata(data, 4)
    n_subid, prefix, include = struct.unpack("BBBx", header)
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
    subids += struct.unpack(formatString, data)
    result = {"subids": subids, "include": include}
    return (result, rest)


def encode_octetstr(bigEndian, octets):
    numoctets = len(octets)
    endianToken = getendian(bigEndian)
    header = struct.pack(endianToken + "I", numoctets)
    pad = (numoctets % 4)
    if pad > 0:  # Pad out the data to word boundary
        pad = 4 - pad
    pad = "\x00" * pad
    if type(octets) is str:
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
    numoctets = struct.unpack(endianToken + "I", header)[0]
    if len(data) < numoctets:
        raise ValueError("Octet string shorter than length")
    pad = numoctets % 4
    if pad > 0:  # Pad out the data to word boundary
        pad = 4 - pad
    return data[:numoctets], data[numoctets + pad:]


def encode_varbind(bigEndian, valType, oid, *payload):
    if valType not in definedValueTypes.keys():
        raise ValueError("Value type %s not in defined types" % valType)
    endianToken = getendian(bigEndian)
    header = struct.pack(endianToken + "Hxx", valType)
    name = encode_oid(bigEndian, oid, False)
    handlers = definedValueTypes[valType]
    if handlers is None:
        data = header + name
    else:
        data = header + name + handlers[0](bigEndian, *payload)
    return data


def decode_varbind(data, header):
    flags = header["flags"]
    bindheader, data = slicedata(data, 4)
    endianToken = getendian(flags["bigEndian"])
    valType = struct.unpack(endianToken + "Hxx", bindheader)[0]
    name, data = decode_oid(data, header)
    if valType not in definedValueTypes.keys():
        raise ValueError("Value type %s not in defined types" % valType)
    handlers = definedValueTypes[valType]
    payload, data = handlers[1](data, header)
    result = {"type": valType, "name": name, "data": payload}
    return result, data


def encode_integer32(bigEndian, num):
    endianToken = getendian(bigEndian)
    return struct.pack(endianToken + "I", num)


def decode_integer32(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    num, data = slicedata(data, 4)
    num = struct.unpack(endianToken + "I", num)[0]
    return (num, data)


def encode_nullvalue(bigEndian):
    return ""


def decode_nullvalue(data, header):
    return (None, data)


def encode_integer64(bigEndian, num):
    endianToken = getendian(bigEndian)
    return struct.pack(endianToken + "Q", num)


def decode_integer64(data, header):
    flags = header["flags"]
    endianToken = getendian(flags["bigEndian"])
    num, data = slicedata(data, 8)
    num = struct.unpack(endianToken + "Q", num)[0]
    return (num, data)


def encode_ipaddr(bigEndian, octets):
    if len(octets) != 4:
        raise ValueError("IP Address incorrect length")
    return encode_octetstr(bigEndian, octets)


def decode_ipaddr(data, header):
    addr, data = decode_octetstr(data, header)
    addr = struct.unpack("BBBB", addr)
    return addr, data


def encode_searchrange(bigEndian, startOID, endOID, inclusiveP):
    startOIDstr = encode_oid(bigEndian, startOID, inclusiveP)
    endOIDstr = encode_oid(bigEndian, endOID, False)
    return startOIDstr + endOIDstr


def decode_searchrange(data, header):
    startOID, data = decode_oid(data, header)
    endOID, data = decode_oid(data, header)
    result = {"start": startOID, "end": endOID}
    return result, data


def encode_searchrange_list(bigEndian, oidranges, nullTerminate=False):
    encoded = []
    for oran in oidranges:
        encoded.append(encode_searchrange(bigEndian, *oran))
    if nullTerminate:
        encoded.append(encode_oid(bigEndian, tuple(), False))
    encoded = "".join(encoded)
    return encoded


def decode_searchrange_list(data, header):  # Cannot handle extra data
    oidranges = []
    while len(data) > 0:
        oids, data = decode_searchrange(data, header)
        oidranges.append(oids)
    return tuple(oidranges)


def decode_searchrange_list_nullterm(data, header):
    oidranges = []
    while len(data) > 0:
        one, data = decode_oid(data, header)
        if isnullOID(one):
            break
        two, data = decode_oid(data, header)
        oidranges.append({"start": one, "end": two})
    return tuple(oidranges), data


# =========================================
# Utilities, glue, and misc
# =========================================


def getendian(bigEndian):
    return ">" if bigEndian is True else "<"


def slicedata(data, slicepoint):
    return data[:slicepoint], data[slicepoint:]


def isnullOID(oid):
    if (len(oid["subids"]) == 0) and (oid["include"] is False):
        return True
    return False


def encode_varbindlist(bigEndian, varbinds):
    payload = ""
    for varbind in varbinds:
        payload += encode_varbind(bigEndian, *varbind)
    return payload


def decode_varbindlist(data, header):
    if len(data) > 0:
        varbinds = []
        while len(data) > 0:
            vb, data = decode_varbind(data, header)
            varbinds.append(vb)
        varbinds = tuple(varbinds)
    else:
        varbinds = None
    return varbinds


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
    flags = 0
    flags |= instanceRegistration
    flags |= newIndex << 1
    flags |= anyIndex << 2
    flags |= nonDefaultContext << 3
    flags |= bigEndian << 4
    # Yes, this is a kluge, it is less problematic then the next best kluge
    endianToken = getendian(bigEndian)
    data = struct.pack(endianToken + "BBBxIIII", 1, pduType, flags,
                       sessionID, transactionID,
                       packetID, payloadLength)
    return data


def decode_pduheader(data):  # Endianess is controlled from the PDU header
    lineone, data = slicedata(data, 4)
    version, pduType, flags = struct.unpack(">BBBx", lineone)
    if pduType not in definedPDUTypes:
        raise ValueError("PDU type %s not in defined types" % pduType)
    # Slice up the flags
    flagDict = {}
    flagDict["instReg"] = bool(flags & 0x1)
    flagDict["newIndex"] = bool(flags & 0x2)
    flagDict["anyIndex"] = bool(flags & 0x4)
    flagDict["contextP"] = bool(flags & 0x8)
    flagDict["bigEndian"] = bool(flags & 0x10)
    # Chop the remaining parts of the header from the rest of the datastream
    # then parse them
    fmt = getendian(flagDict["bigEndian"]) + "IIII"
    linen, data = slicedata(data, 16)  # 4 x 4-byte variables
    sID, tactionID, pktID, dataLen = struct.unpack(fmt, linen)
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
        payload = ""
    return (contextP, payload)


def decode_context(data, header):
    flags = header["flags"]
    if flags["contextP"] is True:
        context, data = decode_octetstr(data, header)
    else:
        context = None
    return (context, data)


def decode_packet(data):
    header, newData = slicedata(data, 20)
    header = decode_pduheader(header)
    if header["length"] > len(newData):
        raise IndexError("Packet data too short")
    if header["version"] != 1:
        raise ValueError("Unknown packet version", header["version"])
    pktType = header["type"]
    if pktType not in definedPDUTypes.keys():
        raise ValueError("PDU type %s not in defined types" % pktType)
    decoder = definedPDUTypes[pktType]
    if decoder is None:
        parsedPkt = None
    else:
        packetSlice, newData = slicedata(newData, header["length"])
        parsedPkt = decoder(packetSlice, header)
    return parsedPkt, newData


# Value types
INTEGER = 2
OCTET_STR = 4
NULL = 5
OID = 6
IP_ADDR = 64
COUNTER32 = 65
GAUGE32 = 66
TIME_TICKS = 67
OPAQUE = 68
COUNTER64 = 70
NO_SUCH_OBJECT = 128
NO_SUCH_INSTANCE = 129
END_OF_MIB_VIEW = 130
definedValueTypes = {  # Used by the varbind functions
    INTEGER: (encode_integer32, decode_integer32),
    OCTET_STR: (encode_octetstr, decode_octetstr),
    NULL: (encode_nullvalue, decode_nullvalue),
    OID: (encode_oid, decode_oid),
    IP_ADDR: (encode_ipaddr, decode_ipaddr),
    COUNTER32: (encode_integer32, decode_integer32),
    GAUGE32: (encode_integer32, decode_integer32),
    TIME_TICKS: (encode_integer32, decode_integer32),
    OPAQUE: (encode_octetstr, decode_octetstr),
    COUNTER64: (encode_integer64, decode_integer64),
    NO_SUCH_OBJECT: (encode_nullvalue, decode_nullvalue),
    NO_SUCH_INSTANCE: (encode_nullvalue, decode_nullvalue),
    END_OF_MIB_VIEW: (encode_nullvalue, decode_nullvalue)}

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
ERR_NOT_WRITABLE = 17
ERR_INCONSISTENT_NAME = 18
definedErrors = (ERR_NOERROR, ERR_GENERR, ERR_NO_ACCESS, ERR_WRONG_TYPE,
                 ERR_WRONG_LEN, ERR_WRONG_ENCODING, ERR_WRONG_VALUE,
                 ERR_NO_CREATION, ERR_INCONSISTENT_VALUE,
                 ERR_RESOURCE_UNAVAILABLE, ERR_NOT_WRITABLE,
                 ERR_INCONSISTENT_NAME)
