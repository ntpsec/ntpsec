# -*- coding: utf-8 -*-
# Common utility functions
# SPDX-License-Identifier: BSD-2-clause

from __future__ import print_function

import struct

# Endianess is selectable in the AgentX packet headers. In this program
# *ALL* values will be transmitted in big / network endianess.

internetPrefix = (1, 3, 6, 1)  # Used by the prefix option of OID headers
prefixCount = len(internetPrefix)

# =======================================================================
#
# Function types fall into the following categories:
#   Data type encoder/decoders
#   Packet body encoder/decoders
#   Glue/Utility/Misc functions
#
# To encode a packet, call the relevant encode_*pdu function.
#
# To decode a packet, call the decode_packet function, which will select
# the correct decoder for that packet type.
#
# =======================================================================


# =======================================================================
#
# Packet body encoders / decoders
#
#   Encoders take information for both the header and body and
#   return a complete packet.
#
#   Decoders take just the body sliced away from everything else,
#   and a context flag. They return tuples containing the data.
#   They do not return extra data as they are never supposed to
#   receive it.
#   Decoders are not meant to be called directly by external code.
#
# =======================================================================


def encode_openpdu(sID, tactID, pktID, timeout, oid, description):
    payload = struct.pack("Bxxx", timeout)
    payload += encode_oid(oid, False)
    payload += encode_octetstr(description)
    header = encode_pduheader(PDU_OPEN, False, False, False, False,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_openpdu(data, flags):
    temp, data = slicedata(data, 4)
    timeout = struct.unpack("Bxxx", temp)[0]
    oid, data = decode_oid(data, flags)
    octets = decode_octetstr(data, flags)[0]
    result = {"timeout": timeout, "oid": oid, "description": octets}
    return result


def encode_closepdu(sID, tactID, pktID, reason):
    if reason not in definedReasons:
        raise ValueError("Close reason %s not in defined types" % reason)
    payload = struct.pack("Bxxx", reason)
    header = encode_pduheader(PDU_CLOSE, False, False, False, False,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_closepdu(data, flags):
    reason = {"reason": ord(data[0])}  # Bxxx
    return reason


def encode_registerpdu_core(isregister, sID, tactID, pktID,
                            timeout, priority, subtree,
                            rangeSubid=0, upperBound=None, context=None):
    if isregister:
        pkttype = PDU_REGISTER
        regbit = True
    else:
        pkttype = PDU_UNREGISTER
        timeout = 0
        regbit = False
    contextP, payload = encode_context(context)
    payload += struct.pack(">BBBx", timeout, priority, rangeSubid)
    payload += encode_oid(subtree, False)
    if rangeSubid != 0:
        if upperBound is None:
            raise ValueError("upperBound must be set if rangeSubid is set")
        payload += struct.pack(">I", upperBound)
    header = encode_pduheader(pkttype, regbit, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    packet = header + payload
    return packet


def decode_registerpdu_core(data, flags):
    endianToken = getendian(flags)
    context, data = decode_context(data, flags)
    temp, data = slicedata(data, 4)
    timeout, priority, rangeSubid = struct.unpack(endianToken + "BBBx", temp)
    oid, data = decode_oid(data, flags)
    if rangeSubid != 0:
        temp, data = slicedata(data, 4)
        upperBound = struct.unpack(endianToken + "I", temp)[0]
    else:
        upperBound = None
    result = {"oid": oid, "timeout": timeout, "priority": priority,
              "context": context, "range_subid": rangeSubid,
              "upper_bound": upperBound}
    return result


def encode_registerpdu(sID, tactID, pktID, timeout, priority, subtree,
                       rangeSubid=0, upperBound=None, context=None):
    return encode_registerpdu_core(True, sID, tactID, pktID, timeout,
                                   priority, subtree, rangeSubid,
                                   upperBound, context)


def encode_unregisterpdu(sID, tactID, pktID, priority, subtree,
                         rangeSubid=0, upperBound=None, context=None):
    return encode_registerpdu_core(False, sID, tactID, pktID, 0,
                                   priority, subtree, rangeSubid,
                                   upperBound, context)


decode_registerpdu = decode_registerpdu_core


def decode_unregisterpdu(data, flags):
    result = decode_registerpdu_core(data, flags)
    del result["timeout"]
    return result


def encode_getpdu_core(isnext, sID, tactID, pktID, oidranges, context=None):
    if isnext is True:
        pkttype = PDU_GET_NEXT
    else:
        pkttype = PDU_GET
    contextP, payload = encode_context(context)
    payload += encode_searchrange_list(oidranges)
    header = encode_pduheader(pkttype, False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_getpdu_core(data, flags):
    context, data = decode_context(data, flags)
    oidranges, data = decode_searchrange_list(data, flags)
    result = {"context": context, "oidranges": oidranges}
    return result


def encode_getpdu(sID, tactID, pktID, oidranges, context=None):
    return encode_getpdu_core(False, sID, tactID, pktID, oidranges, context)


def encode_getnextpdu(sID, tactID, pktID, oidranges, context=None):
    return encode_getpdu_core(True, sID, tactID, pktID, oidranges, context)


decode_getpdu = decode_getnextpdu = decode_getpdu_core


def encode_getbulkpdu(sID, tactID, pktID, nonReps, maxReps,
                      oidranges, context=None):
    contextP, payload = encode_context(context)
    payload += struct.pack(">HH", nonReps, maxReps)
    payload += encode_searchrange_list(oidranges)
    header = encode_pduheader(PDU_GET_BULK, False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_getbulkpdu(data, flags):
    endianToken = getendian(flags)
    context, data = decode_context(data, flags)
    temp, data = slicedata(data, 4)
    nonReps, maxReps = struct.unpack(endianToken + "HH", temp)
    oidranges, data = decode_searchrange_list(data, flags)
    result = {"context": context, "non_reps": nonReps, "max_reps": maxReps,
              "oidranges": oidranges}
    return result


def encode_testsetpdu(sID, tactID, pktID, varbinds, context=None):
    contextP, payload = encode_context(context)
    payload += encode_varbindlist(varbinds)
    header = encode_pduheader(PDU_TEST_SET, False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_testsetpdu(data, flags):
    context, data = decode_context(data, flags)
    varbinds = decode_varbindlist(data, flags)
    result = {"context": context, "varbinds": varbinds}
    return result


# CommitSet, UndoSet, and CleanupSet are bare headers. Don't need decoders

def encode_commitsetpdu(sID, tactID, pktID):
    return encode_pduheader(PDU_COMMIT_SET, False, False, False, False,
                            sID, tactID, pktID, 0)


def encode_undosetpdu(sID, tactID, pktID):
    return encode_pduheader(PDU_UNDO_SET, False, False, False, False,
                            sID, tactID, pktID, 0)


def encode_cleanupsetpdu(sID, tactID, pktID):
    return encode_pduheader(PDU_CLEANUP_SET, False, False, False, False,
                            sID, tactID, pktID, 0)


def encode_pingpdu(sID, tactID, pktID, context=None):
    contextP, payload = encode_context(context)
    header = encode_pduheader(PDU_PING, False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_pingpdu(data, flags):
    context, data = decode_context(data, flags)
    return {"context": context}


def encode_notifypdu(sID, tactID, pktID, varbinds, context=None):
    contextP, payload = encode_context(context)
    payload += encode_varbindlist(varbinds)
    header = encode_pduheader(PDU_NOTIFY, False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_notifypdu(data, flags):
    context, data = decode_context(data, flags)
    varbinds = decode_varbindlist(data, flags)
    result = {"context": context, "varbinds": varbinds}
    return result


def encode_indexallocpdu_core(isdealloc,
                              sID, tactID, pktID, newIndex, anyIndex,
                              varbinds, context=None):
    pkttype = PDU_INDEX_DEALLOC if isdealloc is True else PDU_INDEX_ALLOC
    contextP, payload = encode_context(context)
    payload += encode_varbindlist(varbinds)
    header = encode_pduheader(pkttype, False, newIndex, anyIndex,
                              contextP, sID, tactID, pktID, len(payload))
    return header + payload


def encode_indexallocpdu(sID, tactID, pktID, newIndex, anyIndex,
                         varbinds, context=None):
    return encode_indexallocpdu_core(False,
                                     sID, tactID, pktID, newIndex, anyIndex,
                                     varbinds, context)


def encode_indexdeallocpdu(sID, tactID, pktID, newIndex, anyIndex,
                           varbinds, context=None):
    return encode_indexallocpdu_core(True,
                                     sID, tactID, pktID, newIndex, anyIndex,
                                     varbinds, context)


def decode_indexallocpdu(data, flags):
    context, data = decode_context(data, flags)
    varbinds = decode_varbindlist(data, flags)
    result = {"context": context, "varbinds": varbinds}
    return result


decode_indexdeallocpdu = decode_indexallocpdu


def encode_addagentcapspdu(sID, tactID, pktID, oid, descr, context=None):
    contextP, payload = encode_context(context)
    payload += encode_oid(oid, False)
    payload += encode_octetstr(descr)
    header = encode_pduheader(PDU_ADD_AGENT_CAPS,
                              False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_addagentcapspdu(data, flags):
    context, data = decode_context(data, flags)
    oid, data = decode_oid(data, flags)
    descr = decode_octetstr(data, flags)[0]
    result = {"context": context, "oid": oid, "description": descr}
    return result


def encode_rmagentcapspdu(sID, tactID, pktID, oid, context=None):
    contextP, payload = encode_context(context)
    payload += encode_oid(oid, False)
    header = encode_pduheader(PDU_RM_AGENT_CAPS,
                              False, False, False, contextP,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_rmagentcapspdu(data, flags):
    context, data = decode_context(data, flags)
    oid, data = decode_oid(data, flags)
    result = {"context": context, "oid": oid}
    return result


def encode_responsepdu(sID, tactID, pktID,
                       sysUpTime, resError, resIndex,
                       varbinds=None):
    payload = struct.pack(">IHH", sysUpTime, resError, resIndex)
    if varbinds is not None:
            payload += encode_varbindlist(varbinds)
    header = encode_pduheader(PDU_RESPONSE, False, False, False, False,
                              sID, tactID, pktID, len(payload))
    return header + payload


def decode_responsepdu(data, flags):
    endianToken = getendian(flags)
    temp, data = slicedata(data, 8)
    sysUpTime, resError, resIndex = struct.unpack(endianToken + "IHH", temp)
    if len(data) > 0:
        varbinds = decode_varbindlist(data, flags)
    else:
        varbinds = None
    result = {"sys_uptime": sysUpTime, "res_err": resError,
              "res_index": resIndex, "varbinds": varbinds}
    return result


# ========================================================================
#
# Data type encoders / decoders
#
#   Encoders take data relevant to the type, return encoded string.
#
#   Decoders take encoded string, return a tuple of (value, restOfData).
#
# ========================================================================

def encode_oid(subids, include):
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
    body = struct.pack(">BBBx", n_subid, prefix, include)
    for subid in subids:
        body += struct.pack(">I", subid)
    return body


def decode_oid(data, flags):
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
    endianToken = getendian(flags)
    formatString = endianToken + ("I" * n_subid)
    subids += struct.unpack(formatString, data)
    result = {"subids": subids, "include": include}
    return (result, rest)


def encode_octetstr(octets):
    numoctets = len(octets)
    header = struct.pack(">I", numoctets)
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


def decode_octetstr(data, flags):
    header, data = slicedata(data, 4)
    endianToken = getendian(flags)
    numoctets = struct.unpack(endianToken + "I", header)[0]
    if len(data) < numoctets:
        raise ValueError("Octet string shorter than length")
    pad = numoctets % 4
    if pad > 0:  # Pad out the data to word boundary
        pad = 4 - pad
    return data[:numoctets], data[numoctets + pad:]


def encode_varbind(valType, oid, *payload):
    if valType not in definedValueTypes.keys():
        raise ValueError("Value type %s not in defined types" % valType)
    header = struct.pack(">Hxx", valType)
    name = encode_oid(oid, False)
    handlers = definedValueTypes[valType]
    if handlers is None:
        data = header + name
    else:
        data = header + name + handlers[0](*payload)
    return data


def decode_varbind(data, flags):
    header, data = slicedata(data, 4)
    endianToken = getendian(flags)
    valType = struct.unpack(endianToken + "Hxx", header)[0]
    name, data = decode_oid(data, flags)
    if valType not in definedValueTypes.keys():
        raise ValueError("Value type %s not in defined types" % valType)
    handlers = definedValueTypes[valType]
    payload, data = handlers[1](data, flags)
    result = {"type": valType, "name": name, "data": payload}
    return result, data


def encode_integer32(num):
    return struct.pack(">I", num)


def decode_integer32(data, flags):
    endianToken = getendian(flags)
    num, data = slicedata(data, 4)
    num = struct.unpack(endianToken + "I", num)[0]
    return (num, data)


def encode_nullvalue():
    return ""


def decode_nullvalue(data, flags):
    return (None, data)


def encode_integer64(num):
    return struct.pack(">Q", num)


def decode_integer64(data, flags):
    endianToken = getendian(flags)
    num, data = slicedata(data, 8)
    num = struct.unpack(endianToken + "Q", num)[0]
    return (num, data)


def encode_ipaddr(octets):
    if len(octets) != 4:
        raise ValueError("IP Address incorrect length")
    return encode_octetstr(octets)


def decode_ipaddr(data, flags):
    addr, data = decode_octetstr(data, flags)
    addr = struct.unpack("BBBB", addr)
    return addr, data


def encode_searchrange(startOID, endOID, inclusiveP):
    startOIDstr = encode_oid(startOID, inclusiveP)
    endOIDstr = encode_oid(endOID, False)
    return startOIDstr + endOIDstr


def decode_searchrange(data, flags):
    startOID, data = decode_oid(data, flags)
    endOID, data = decode_oid(data, flags)
    result = {"start": startOID, "end": endOID}
    return result, data


def encode_searchrange_list(oidranges):
    encoded = []
    for oran in oidranges:
        encoded.append(encode_searchrange(*oran))
    encoded.append(encode_oid(tuple(), False))
    encoded = "".join(encoded)
    return encoded


def decode_searchrange_list(data, flags):
    oidranges = []
    while True:
        one, data = decode_oid(data, flags)
        if isnullOID(one):
            break
        two, data = decode_oid(data, flags)
        oidranges.append({"start": one, "end": two})  # oid, oid, inclusive
    return tuple(oidranges), data


# =========================================
# Utilities, glue, and misc
# =========================================

def getendian(flags):
    return ">" if flags["bigEndian"] is True else "<"


def slicedata(data, slicepoint):
    return data[:slicepoint], data[slicepoint:]


def isnullOID(oid):
    if (len(oid["subids"]) == 0) and (oid["include"] is False):
        return True
    return False


def encode_varbindlist(varbinds):
    payload = ""
    for varbind in varbinds:
        payload += encode_varbind(*varbind)
    return payload


def decode_varbindlist(data, flags):
    if len(data) > 0:
        varbinds = []
        while len(data) > 0:
            vb, data = decode_varbind(data, flags)
            varbinds.append(vb)
        varbinds = tuple(varbinds)
    else:
        varbinds = None
    return varbinds


def encode_pduheader(pduType, instanceRegistration, newIndex,
                     anyIndex, nonDefaultContext, sessionID,
                     transactionID, packetID, payloadLength):
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
    flags |= True << 4  # byte order, we always send network / big endian
    data = struct.pack(">BBBxIIII", 1, pduType, flags,
                       sessionID,
                       transactionID,
                       packetID,
                       payloadLength)
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
    fmt = getendian(flagDict) + "IIII"
    linen, data = slicedata(data, 16)  # 4 x 4-byte variables
    sID, tactionID, pktID, dataLen = struct.unpack(fmt, linen)
    result = {"version": version, "type": pduType, "flags": flagDict,
              "session_id": sID, "transaction_id": tactionID,
              "packet_id": pktID, "length": dataLen}
    return result


def encode_context(context):
    if context is not None:
        contextP = True
        payload = encode_octetstr(context)
    else:
        contextP = False
        payload = ""
    return (contextP, payload)


def decode_context(data, flags):
    if flags["contextP"] is True:
        context, data = decode_octetstr(data, flags)
    else:
        context = None
    return (context, data)


def decode_packet(data):
    header, data = slicedata(data, 20)
    header = decode_pduheader(header)
    if header["version"] != 1:
        raise ValueError("Unknown packet version", header["version"])
    pktType = header["type"]
    if pktType not in definedPDUTypes.keys():
        raise ValueError("PDU type %s not in defined types" % pktType)
    decoder = definedPDUTypes[pktType]
    if decoder is None:
        parsedPkt = None
    else:
        packetSlice, data = slicedata(data, header["length"])
        parsedPkt = decoder(packetSlice, header["flags"])
    result = {"header": header, "body": parsedPkt}
    return result, data


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
    PDU_OPEN: decode_openpdu,
    PDU_CLOSE: decode_closepdu,
    PDU_REGISTER: decode_registerpdu,
    PDU_UNREGISTER: decode_unregisterpdu,
    PDU_GET: decode_getpdu,
    PDU_GET_NEXT: decode_getnextpdu,
    PDU_GET_BULK: decode_getbulkpdu,
    PDU_TEST_SET: decode_testsetpdu,
    PDU_COMMIT_SET: None,
    PDU_UNDO_SET: None,
    PDU_CLEANUP_SET: None,
    PDU_NOTIFY: decode_notifypdu,
    PDU_PING: decode_pingpdu,
    PDU_INDEX_ALLOC: decode_indexallocpdu,
    PDU_INDEX_DEALLOC: decode_indexdeallocpdu,
    PDU_ADD_AGENT_CAPS: decode_addagentcapspdu,
    PDU_RM_AGENT_CAPS: decode_rmagentcapspdu,
    PDU_RESPONSE: decode_responsepdu}

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
