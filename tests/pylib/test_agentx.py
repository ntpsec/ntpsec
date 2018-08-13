#!/usr/bin/env python
# -*- coding: utf-8 -*-

import ntp.agentx as AX
import ntp.agentx_packet as AP
import ntp.util
import jigs

import unittest
import types


class TestMIBControl(unittest.TestCase):
    def test___init__(self):
        c = AX.MIBControl(1, 2, 3, 4, 5)
        self.assertEqual(c.oidTree, 1)
        self.assertEqual(c.inSetP, False)
        self.assertEqual(c.setVarbinds, [])
        self.assertEqual(c.setHandlers, [])
        self.assertEqual(c.setUndoData, [])
        self.assertEqual(c.mibRoot, 2)
        self.assertEqual(c.rangeSubid, 3)
        self.assertEqual(c.upperBound, 4)
        self.assertEqual(c.context, 5)

    def test_mib_rootOID(self):
        c = AX.MIBControl(mibRoot="foo")
        self.assertEqual(c.mib_rootOID(), "foo")

    def test_mib_rangeSubid(self):
        c = AX.MIBControl(rangeSubid="foo")
        self.assertEqual(c.mib_rangeSubid(), "foo")

    def test_mib_upperBound(self):
        c = AX.MIBControl(upperBound="foo")
        self.assertEqual(c.mib_upperBound(), "foo")

    def test_mib_context(self):
        c = AX.MIBControl(mibContext="foo")
        self.assertEqual(c.mib_context(), "foo")

    def test_addNode(self):
        c = AX.MIBControl()
        # Top level, non-dynamic
        c.addNode((0,), 1, 2)
        self.assertEqual(c.oidTree,
                         {0: {"reader": 1, "writer": 2, "subids": None}})
        # Top level, non-dynamic, class OID
        c.addNode(AP.OID((1,)), 1, 2)
        self.assertEqual(c.oidTree,
                         {0: {"reader": 1, "writer": 2, "subids": None},
                          1: {"reader": 1, "writer": 2, "subids": None}})
        # Top level, dynamic
        c.addNode((2,), dynamic=3)
        self.assertEqual(c.oidTree,
                         {0: {"reader": 1, "writer": 2, "subids": None},
                          1: {"reader": 1, "writer": 2, "subids": None},
                          2: {"reader": None, "writer": None, "subids": 3}})
        # Sub level
        c.addNode((1, 2, 3), 1, 2)
        self.assertEqual(c.oidTree,
                         {0: {"reader": 1, "writer": 2, "subids": None},
                          1: {"reader": 1, "writer": 2, "subids":
                              {2: {"reader": None, "writer": None, "subids": {
                                   3: {"reader": 1, "writer": 2, "subids": None}}}}},
                          2: {"reader": None, "writer": None, "subids": 3}})

    def test_getOID_andNext(self):
        c = AX.MIBControl()
        c.addNode((0, 1))
        c.addNode((0, 2, 0), 10, 11)
        c.addNode((0, 2, 1), 20, 21)
        c.addNode((0, 2, 2), 30, 31)
        c.addNode((0, 2, 3), 40, 41)
        c.addNode((0, 4, 1, 0), 50, 51)
        # Test getOID
        # Test non-existent, no generator
        self.assertEqual(c.getOID(AP.OID((0, 0))), (None, None, None))
        # Test empty, no generator
        self.assertEqual(c.getOID(AP.OID((0, 1))),
                         (None, None, None))
        # Test empty, with generator
        self.assertEqual(c.getOID(AP.OID((0, 1)), True),
                         (None, None, None, None))
        # Test existent, no generator
        self.assertEqual(c.getOID(AP.OID((0, 2, 1))),
                         (AP.OID((0, 2, 1)), 20, 21))
        # Test existent, with generator
        r = c.getOID(AP.OID((0, 2, 1)), True)
        self.assertEqual(r[:3], (AP.OID((0, 2, 1)), 20, 21))
        self.assertEqual(isinstance(r[3], types.GeneratorType), True)
        # Test getNextOID
        # Test non-existent, no generator
        self.assertEqual(c.getNextOID(AP.OID((0, 0))),
                         (AP.OID((0, 2, 0)), 10, 11))
        # Test empty, no generator
        self.assertEqual(c.getNextOID(AP.OID((0, 1))),
                         (AP.OID((0, 2, 0)), 10, 11))
        # Test empty, with generator
        r = c.getNextOID(AP.OID((0, 1)), True)
        self.assertEqual(r[:3], (AP.OID((0, 2, 0)), 10, 11))
        self.assertEqual(isinstance(r[3], types.GeneratorType), True)
        # Test existent, no generator
        self.assertEqual(c.getNextOID(AP.OID((0, 2, 1))),
                         (AP.OID((0, 2, 2)), 30, 31))
        # Test existent, with generator
        r = c.getNextOID(AP.OID((0, 2, 1)), True)
        self.assertEqual(r[:3], (AP.OID((0, 2, 2)), 30, 31))
        self.assertEqual(isinstance(r[3], types.GeneratorType), True)
        # Test walking off the end
        self.assertEqual(c.getNextOID(AP.OID((0, 4, 1, 0))),
                         (None, None, None))

    def test_getOIDsInRange(self):
        c = AX.MIBControl()
        c.addNode((0, 1))
        c.addNode((0, 2, 0), 10, 11)
        c.addNode((0, 2, 1), 20, 21)
        c.addNode((0, 2, 2), 30, 31)
        c.addNode((0, 2, 3), 40, 41)
        c.addNode((0, 4, 1, 0), 50, 51)
        # Test range too early
        rng = AP.SearchRange((0, 0, 0), (0, 0, 5))
        self.assertEqual(c.getOIDsInRange(rng), [])
        # Test range too late
        rng = AP.SearchRange((6, 0, 0), (6, 0, 5))
        self.assertEqual(c.getOIDsInRange(rng), [])
        # Test nothing implemented in range
        rng = AP.SearchRange((0,), (0, 1, 5))
        self.assertEqual(c.getOIDsInRange(rng), [])
        # Test in range
        rng = AP.SearchRange((0, 2, 0), (0, 2, 3))
        self.assertEqual(c.getOIDsInRange(rng),
                         [(AP.OID((0, 2, 1)), 20, 21),
                          (AP.OID((0, 2, 2)), 30, 31)])
        # Test unbounded range
        rng = AP.SearchRange((0, 2, 0), ())
        self.assertEqual(c.getOIDsInRange(rng),
                         [(AP.OID((0, 2, 1)), 20, 21),
                          (AP.OID((0, 2, 2)), 30, 31),
                          (AP.OID((0, 2, 3)), 40, 41),
                          (AP.OID((0, 4, 1, 0)), 50, 51)])


class TestPacketControl(unittest.TestCase):
    def test___init__(self):
        p = AX.PacketControl("sock", "base")
        self.assertEqual(p.socket, "sock")
        self.assertEqual(0.0005 < p.spinGap < 0.0015, True)
        self.assertEqual(p.packetLog, {})
        self.assertEqual(p.loopCallback, None)
        self.assertEqual(p.database, "base")
        self.assertEqual(p.receivedData, b"")
        self.assertEqual(p.timeout, 30)
        self.assertEqual(p.sessionID, None)
        self.assertEqual(p.highestTransactionID, 0)
        self.assertEqual(p.lastReception, None)
        self.assertEqual(p.stillConnected, False)
        self.assertEqual(p.pduHandlers,
                         {AP.PDU_GET: p.handle_GetPDU,
                          AP.PDU_GET_NEXT: p.handle_GetNextPDU,
                          AP.PDU_GET_BULK: p.handle_GetBulkPDU,
                          AP.PDU_TEST_SET: p.handle_TestSetPDU,
                          AP.PDU_COMMIT_SET: p.handle_CommitSetPDU,
                          AP.PDU_UNDO_SET: p.handle_UndoSetPDU,
                          AP.PDU_CLEANUP_SET: p.handle_CleanupSetPDU,
                          AP.PDU_RESPONSE: p.handle_ResponsePDU})

    def test_mainloop(self):
        stillCons = []
        loop_calls = [0]
        cb_calls = [0]

        def loop_jig(self):
            loop_calls[0] += 1
            self.stillConnected = stillCons.pop(0)

        def callback_jig(p):
            cb_calls[0] += 1

        p = AX.PacketControl(None, None)
        p.spinGap = 0
        p._doloop = (lambda: loop_jig(p))
        p.stillConnected = False
        # Test not connected
        self.assertEqual(p.mainloop(False), False)
        self.assertEqual(loop_calls, [0])
        # Test single shot
        p.stillConnected = True
        stillCons = [True] * 2
        self.assertEqual(p.mainloop(False), True)
        self.assertEqual(stillCons, [True])
        self.assertEqual(loop_calls, [1])
        # Test loop
        p.stillConnected = True
        loop_calls[0] = 0
        stillCons = [True] * 5 + [False]
        self.assertEqual(p.mainloop(True), False)
        self.assertEqual(stillCons, [])
        self.assertEqual(loop_calls, [6])
        # Test loop, with callback
        p.stillConnected = True
        loop_calls[0] = 0
        p.loopCallback = callback_jig
        stillCons = [True] * 5 + [False]
        self.assertEqual(p.mainloop(True), False)
        self.assertEqual(stillCons, [])
        self.assertEqual(loop_calls, [6])
        self.assertEqual(cb_calls, [6])

    def test__doloop(self):
        # need jigs for
        # time
        pkts = []
        # recpTimes = []
        handler_calls = [0]

        def packetEater_jig(self):
            self.receivedPackets = pkts
            self.lastReception = recpTime

        def pinghandler_jig(self):
            handler_calls[0] += 1

        faketimemod = jigs.TimeModuleJig()
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.packetEater = (lambda: packetEater_jig(p))
        p.checkResponses = (lambda: None)
        p.pduHandlers[AP.PDU_PING] = pinghandler_jig
        p.sessionID = 42
        try:
            timetemp = AX.time
            AX.time = faketimemod
            # Test first time, no reception
            recpTime = None
            faketimemod.time_returns = []
            p._doloop()
            self.assertEqual(sock.data, [])
            # Test wrong session ID
            pkts = [AP.PingPDU(True, 13, 0, 0)]
            recpTime = 50
            faketimemod.time_returns = [55]
            p._doloop()
            self.assertEqual(len(sock.data), 1)
            sentPkt = AP.decode_packet(sock.data[0])[0]
            respPkt = AP.ResponsePDU(True, 13, 0, 0, 0, AP.RSPERR_NOT_OPEN, 0)
            self.assertEqual(sentPkt, respPkt)
            # Test unimplemented packet type
            faketimemod.time_returns = [55]
            sock.data = []
            pkts = [AP.PingPDU(True, 42, 0, 0)]
            pkts[0].pduType = 1337  # spoof the type
            p._doloop()
            self.assertEqual(sock.data, [])
            self.assertEqual(handler_calls, [0])
            # Test successful
            faketimemod.time_returns = [55]
            sock.data = []
            pkts = [AP.PingPDU(True, 42, 0, 0)]
            p._doloop()
            self.assertEqual(sock.data, [])
            self.assertEqual(handler_calls, [1])
            # Test pinging
            handler_calls = [0]
            faketimemod.time_returns = [120]
            sock.data = []
            p._doloop()
            self.assertEqual(len(sock.data), 1)
            sentPkt = AP.decode_packet(sock.data[0])[0]
            respPkt = AP.PingPDU(True, 42, 5, 1)
            self.assertEqual(sentPkt, respPkt)
            self.assertEqual(handler_calls, [0])
        finally:
            AX.time = timetemp

    def test_initNewSession(self):
        resp_returns = []
        resp_calls = []

        def waitForResponse_jig(opkt, ignoreSID=False):
            resp_calls.append((opkt, ignoreSID))
            return resp_returns.pop(0)

        mjig = AX.MIBControl(mibRoot=(0, 1, 2), rangeSubid=0, upperBound=None,
                             mibContext="foo")
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.waitForResponse = waitForResponse_jig
        resp_returns = [AP.ResponsePDU(True, 42, 1, 1, 0, 0, 0),
                        AP.ResponsePDU(True, 42, 1, 1, 0, 0, 0)]
        p.initNewSession()
        self.assertEqual(resp_returns, [])
        self.assertEqual(len(sock.data), 2)
        openpkt = AP.decode_packet(sock.data[0])[0]
        registerpkt = AP.decode_packet(sock.data[1])[0]
        self.assertEqual(openpkt, AP.OpenPDU(True, 23, 0, 0, 30, (),
                                             "NTPsec SNMP subagent"))
        self.assertEqual(registerpkt, AP.RegisterPDU(True, 42, 1, 1, 30, 1,
                                                     mjig.mib_rootOID(),
                                                     mjig.mib_rangeSubid(),
                                                     mjig.mib_upperBound(),
                                                     mjig.mib_context()))
        self.assertEqual(p.sessionID, 42)
        self.assertEqual(p.stillConnected, True)

    def test_waitForResponse(self):
        pkts = [AP.PingPDU(True, 1, 2, 3),
                AP.ResponsePDU(True, 10, 20, 30, 0, 0, 0),
                AP.ResponsePDU(True, 5, 23, 100, 0, 0, 0),
                AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0),
                AP.ResponsePDU(True, 100, 200, 300, 0, 0, 0)]
        opkt = AP.PingPDU(True, 42, 23, 100)

        def packetEater_jig(self):
            self.receivedPackets.append(pkts.pop(0))

        p = AX.PacketControl(None, None)
        p.packetEater = (lambda : packetEater_jig(p))
        p.spinGap = 0
        # Test with session id match required
        ret = p.waitForResponse(opkt)
        self.assertEqual(pkts, [AP.ResponsePDU(True, 100, 200, 300, 0, 0, 0)])
        self.assertEqual(ret, AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0))
        # Test ignoring session id
        pkts = [AP.PingPDU(True, 1, 2, 3),
                AP.ResponsePDU(True, 10, 20, 30, 0, 0, 0),
                AP.ResponsePDU(True, 5, 23, 100, 0, 0, 0),
                AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)]
        ret = p.waitForResponse(opkt, True)
        self.assertEqual(pkts, [AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)])
        self.assertEqual(ret, AP.ResponsePDU(True, 5, 23, 100, 0, 0, 0))

    def test_checkResponses(self):
        returns = []
        callback = (lambda resp, pkt: returns.append((resp, pkt)))
        faketimemod = jigs.TimeModuleJig()
        p = AX.PacketControl(None, None)
        p.packetLog = {(42, 23, 100): (25, "foo", callback),
                       (42, 24, 200): (50, "bar", callback)}
        faketimemod.time_returns = [40]
        try:
            timetemp = AX.time
            AX.time = faketimemod
            p.checkResponses()
            self.assertEqual(returns, [(None, "foo")])
            self.assertEqual(p.packetLog,
                             {(42, 24, 200): (50, "bar", callback)})
        finally:
            AX.time = timetemp

    def test_packetEater(self):
        pkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)
        enc = pkt.encode()
        part1, rest = ntp.util.slicedata(enc, 15)
        part2, part3 = ntp.util.slicedata(rest, 6)
        pollReturns = [part1, part2, part3 + b"blah"]

        def pollSocket_jig(self):
            self.receivedData += pollReturns.pop(0)
        p = AX.PacketControl(None, None)
        p.sessionID = 42
        p.pollSocket = (lambda : pollSocket_jig(p))
        # Test incomplete header
        p.packetEater()
        self.assertEqual(p.receivedPackets, [])
        self.assertEqual(p.receivedData, part1)
        # Test incomplete packet
        p.packetEater()
        self.assertEqual(p.receivedPackets, [])
        self.assertEqual(p.receivedData, part1 + part2)
        # Test packet eaten
        p.packetEater()
        self.assertEqual(p.receivedPackets, [pkt])
        self.assertEqual(p.receivedData, b"blah")

    def test_sendPacket(self):
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        testpkt = AP.PingPDU(True, 0, 1, 2, "fake")
        # Test not expecting reply
        p.sendPacket(testpkt, False)
        self.assertEqual(sock.data, [b"\x01\r\x18\x00"
                                     b"\x00\x00\x00\x00\x00\x00\x00\x01"
                                     b"\x00\x00\x00\x02\x00\x00\x00\x08"
                                     b"\x00\x00\x00\x04fake"])
        self.assertEqual(p.packetLog, {})
        sock.data = []
        # Test expecting reply
        p.sendPacket(testpkt, True, callback="foo")
        self.assertEqual(sock.data, [b"\x01\r\x18\x00"
                                     b"\x00\x00\x00\x00\x00\x00\x00\x01"
                                     b"\x00\x00\x00\x02\x00\x00\x00\x08"
                                     b"\x00\x00\x00\x04fake"])
        self.assertEqual(p.packetLog, {(0, 1, 2): (30.0, testpkt, "foo")})

    def test_sendPing(self):
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.sessionID = 42
        p.sendPing()
        self.assertEqual(sock.data, [b"\x01\r\x10\x00"
                                     b"\x00\x00\x00*\x00\x00\x00\x05"
                                     b"\x00\x00\x00\x01\x00\x00\x00\x00"])
        self.assertEqual(list(p.packetLog.keys()), [(42, 5, 1)])

    def test_sendNotify(self):
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.sessionID = 42
        vb = AP.Varbind(AP.VALUE_INTEGER, (0, 1, 2), 23)
        p.sendNotify([vb])
        self.assertEqual(sock.data,
                         [b"\x01\x0c\x10\x00"
                          b"\x00\x00\x00*\x00\x00\x00\x05"
                          b"\x00\x00\x00\x01\x00\x00\x00\x18"
                          b"\x00\x02\x00\x00"
                          b"\x03\x00\x00\x00\x00\x00\x00\x00"
                          b"\x00\x00\x00\x01\x00\x00\x00\x02"
                          b"\x00\x00\x00\x17"])
        self.assertEqual(list(p.packetLog.keys()), [(42, 5, 1)])

    def test_sendErrorResponse(self):
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.sessionID = 42
        header = {"flags": {"bigEndian": True}, "session_id": 42,
                  "transaction_id": 32, "packet_id": 100}
        p.sendErrorResponse(header, AP.ERR_GENERR, 12)
        self.assertEqual(sock.data,
                         [b"\x01\x12\x10\x00"
                          b"\x00\x00\x00*\x00\x00\x00 "
                          b"\x00\x00\x00d\x00\x00\x00\x08"
                          b"\x00\x00\x00\x00\x00\x05\x00\x0c"])
        self.assertEqual(p.packetLog, {})

    def test_pollSocket(self):
        fakeselectmod = jigs.SelectModuleJig()
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.sessionID = 42
        # Test
        pkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)
        enc = pkt.encode()
        part1, part2 = ntp.util.slicedata(enc, 15)
        sock.return_data = [part1, part2, b"blah"]
        fakeselectmod.do_return = [True, True, True, False]
        try:
            selecttemp = AX.select
            AX.select = fakeselectmod
            p.pollSocket()
            self.assertEqual(p.receivedData, enc + b"blah")
        finally:
            AX.select = selecttemp

    def test_handleGetPDU(self):
        readReturns = [1]
        read = (lambda oid: AP.Varbind(AP.VALUE_INTEGER,
                                       oid, readReturns.pop(0)))
        mjig = AX.MIBControl()
        mjig.addNode((0, 0), read)
        mjig.addNode((0, 1), (lambda x: None))
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        # Test successful
        pkt = AP.GetPDU(True, 42, 23, 100, [AP.SearchRange((0, 0), ())])
        p.handle_GetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_INTEGER, (0, 0), 1),))
        self.assertEqual(sentPkt, respPkt)
        # Test non-existent
        sock.data = []
        pkt = AP.GetPDU(True, 42, 23, 100, [AP.SearchRange((1, 0), ())])
        p.handle_GetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_NO_SUCH_OBJECT,
                                             (1, 0)),))
        self.assertEqual(sentPkt, respPkt)
        # Test non-existent
        sock.data = []
        pkt = AP.GetPDU(True, 42, 23, 100, [AP.SearchRange((0, 1), ())])
        p.handle_GetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_NULL,
                                             (0, 1)),))
        self.assertEqual(sentPkt, respPkt)

    def test_handleGetNextPDU(self):
        readReturns = [1]
        read = (lambda oid: AP.Varbind(AP.VALUE_INTEGER,
                                       oid, readReturns.pop(0)))
        mjig = AX.MIBControl()
        mjig.addNode((0, 0), read)
        mjig.addNode((0, 1), read)
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        # Test successful
        pkt = AP.GetNextPDU(True, 42, 23, 100, [AP.SearchRange((0, 0), ())])
        p.handle_GetNextPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_INTEGER, (0, 1), 1),))
        self.assertEqual(sentPkt, respPkt)
        # Test non-existent
        sock.data = []
        pkt = AP.GetNextPDU(True, 42, 23, 100, [AP.SearchRange((1, 0), ())])
        p.handle_GetNextPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_END_OF_MIB_VIEW,
                                             (1, 0)),))
        self.assertEqual(sentPkt, respPkt)

    def test_handle_GetBulkPDU(self):
        readReturns = [1, 2, 3, 4, 5, 6]
        read = (lambda oid: AP.Varbind(AP.VALUE_INTEGER,
                                       oid, readReturns.pop(0)))
        mjig = AX.MIBControl()
        # non-repeated
        mjig.addNode((0, 0), read)
        mjig.addNode((0, 1), read)  # shouldn't see
        mjig.addNode((1, 0), read)
        mjig.addNode((1, 1), read)  # shouldn't see
        # repeated
        mjig.addNode((2, 0), read)
        mjig.addNode((2, 1), read)
        mjig.addNode((2, 2), read)  # shouldn't see
        mjig.addNode((3, 0), read)
        mjig.addNode((3, 1), read)
        mjig.addNode((3, 2), read)  # shouldn't see
        mjig.addNode((4, 0), read)  # shouldn't see
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        pkt = AP.GetBulkPDU(True, 42, 23, 100, 2, 2,
                            [AP.SearchRange((0,), (1,), True),
                             AP.SearchRange((1,), (2,), True),
                             AP.SearchRange((2, 0), (3,), True),
                             AP.SearchRange((3, 0), (), True)])
        p.handle_GetBulkPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0,
                                 (AP.Varbind(AP.VALUE_INTEGER, (0, 0), 1),
                                  AP.Varbind(AP.VALUE_INTEGER, (1, 0), 2),
                                  AP.Varbind(AP.VALUE_INTEGER, (2, 0), 3),
                                  AP.Varbind(AP.VALUE_INTEGER, (2, 1), 4),
                                  AP.Varbind(AP.VALUE_INTEGER, (3, 0), 5),
                                  AP.Varbind(AP.VALUE_INTEGER, (3, 1), 6)))
        self.assertEqual(sentPkt, respPkt)

    def test_handle_TestSetPDU(self):
        mjig = AX.MIBControl()
        readReturns = []
        read = (lambda oid: AP.Varbind(AP.VALUE_INTEGER,
                                       oid, readReturns.pop(0)))
        writeCalls = []
        writeReturns = []
        write = (lambda a, vb: (writeCalls.append((a, vb)),
                                writeReturns.pop(0))[1])
        mjig.addNode((0,), read, write)
        mjig.addNode((1,), read)
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        # Test success
        readReturns = [10]
        writeReturns = [AP.ERR_NOERROR]
        pkt = AP.TestSetPDU(True, 42, 23, 100,
                            [AP.Varbind(AP.VALUE_INTEGER, (0,), 16)])
        p.handle_TestSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(writeCalls,
                         [("test", AP.Varbind(AP.VALUE_INTEGER, (0,), 16))])
        self.assertEqual(mjig.setVarbinds,
                         [AP.Varbind(AP.VALUE_INTEGER, (0,), 16)])
        self.assertEqual(mjig.setHandlers, [write])
        self.assertEqual(mjig.setUndoData,
                         [AP.Varbind(AP.VALUE_INTEGER, (0,), 10)])
        # Test failure, no access
        mjig.setVarbinds = []
        mjig.setHandlers = []
        mjig.setUndoData = []
        sock.data = []
        readReturns = [10]
        writeReturns = [AP.ERR_NOERROR]
        writeCalls = []
        pkt = AP.TestSetPDU(True, 42, 23, 100,
                            [AP.Varbind(AP.VALUE_INTEGER, (0, 1), 16)])
        p.handle_TestSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 6, 0)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(writeCalls, [])
        self.assertEqual(mjig.setVarbinds, [])
        self.assertEqual(mjig.setHandlers, [])
        self.assertEqual(mjig.setUndoData, [])
        # Test failure, not writable
        mjig.setVarbinds = []
        mjig.setHandlers = []
        mjig.setUndoData = []
        sock.data = []
        readReturns = [10]
        writeReturns = [AP.ERR_NOERROR]
        writeCalls = []
        pkt = AP.TestSetPDU(True, 42, 23, 100,
                            [AP.Varbind(AP.VALUE_INTEGER, (1,), 16)])
        p.handle_TestSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 17, 0)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(writeCalls, [])
        self.assertEqual(mjig.setVarbinds, [])
        self.assertEqual(mjig.setHandlers, [])
        self.assertEqual(mjig.setUndoData, [])

    def test_handle_CommitSetPDU(self):
        handlerCalls = []
        handlerReturns = [AP.ERR_NOERROR, AP.ERR_NOERROR]
        handler = (lambda a, vb: (handlerCalls.append((a, vb)),
                   handlerReturns.pop(0))[1])
        mjig = AX.MIBControl()
        mjig.setVarbinds = ["foo", "bar"]
        mjig.setHandlers = [handler, handler]
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        # Test success
        pkt = AP.CommitSetPDU(True, 42, 23, 100)
        p.handle_CommitSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(handlerCalls, [("commit", "foo"),
                                        ("commit", "bar")])
        # Test failure
        sock.data = []
        handlerCalls = []
        handlerReturns = [AP.ERR_NOERROR, AP.ERR_GENERR]
        p.handle_CommitSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 5, 1)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(handlerCalls, [("commit", "foo"),
                                        ("commit", "bar")])

    def test_handle_UndoSetPDU(self):
        handlerCalls = []
        handlerReturns = [AP.ERR_NOERROR, AP.ERR_NOERROR]
        handler = (lambda a, vb, ud: (handlerCalls.append((a, vb, ud)),
                                      handlerReturns.pop(0))[1])
        mjig = AX.MIBControl()
        mjig.setVarbinds = ["foo", "bar"]
        mjig.setHandlers = [handler, handler]
        mjig.setUndoData = ["fey", "yar"]
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, mjig)
        p.sessionID = 42
        # Test success
        pkt = AP.UndoSetPDU(True, 42, 23, 100)
        p.handle_UndoSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 0, 0)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(handlerCalls, [("undo", "foo", "fey"),
                                        ("undo", "bar", "yar")])
        # Test failure
        sock.data = []
        handlerCalls = []
        handlerReturns = [AP.ERR_NOERROR, AP.ERR_GENERR]
        p.handle_UndoSetPDU(pkt)
        self.assertEqual(len(sock.data), 1)
        sentPkt = AP.decode_packet(sock.data[0])[0]
        respPkt = AP.ResponsePDU(True, 42, 23, 100, 0, 5, 1)
        self.assertEqual(sentPkt, respPkt)
        self.assertEqual(handlerCalls, [("undo", "foo", "fey"),
                                        ('undo', 'bar', 'yar')])

    def test_handle_CleanupSetPDU(self):
        handlerCalls = []
        handler = (lambda a, vb: handlerCalls.append((a, vb)))
        mjig = AX.MIBControl()
        mjig.setVarbinds = ["foo", "bar"]
        mjig.setHandlers = [handler, handler]
        p = AX.PacketControl(None, mjig)
        p.sessionID = 42
        pkt = AP.CleanupSetPDU(True, 42, 23, 100)
        p.handle_CleanupSetPDU(pkt)
        self.assertEqual(p.database.inSetP, False)
        self.assertEqual(handlerCalls, [("clean", "foo"), ("clean", "bar")])

    def test_handle_ResponsePDU(self):
        cbreturn = []

        def callback(pkt, origpkt):
            cbreturn.append((pkt, origpkt))
        opkt = AP.PingPDU(True, 42, 4, 50)
        incorrect_rpkt = AP.ResponsePDU(True, 23, 1, 99, 0, 0, 0)
        correct_rpkt = AP.ResponsePDU(True, 42, 4, 50, 0, 0, 0)
        sock = jigs.SocketJig()
        p = AX.PacketControl(sock, None)
        p.sessionID = 42
        p.packetLog = {(42, 4, 50): (30.0, opkt, callback)}
        # Test non-expected response (likely to happen in a timeout)
        p.handle_ResponsePDU(incorrect_rpkt)
        self.assertEqual(p.packetLog, {(42, 4, 50): (30.0, opkt, callback)})
        # Test expected response
        p.handle_ResponsePDU(correct_rpkt)
        self.assertEqual(p.packetLog, {})


class TestAgentx(unittest.TestCase):
    def test_walkMIBTree(self):
        f = AX.walkMIBTree

        # Test empty tree
        self.assertEqual(tuple(f({})), ())
        # Test flat, fully static tree
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "subids": None},
                                  1: {},
                                  2: {"reader": None, "writer": None,
                                      "subids": None},
                                  5: {"reader": None, "writer": None,
                                      "subids": None}})),
                         ((AP.OID((0,)), None, None),
                          (AP.OID((1,)), None, None),
                          (AP.OID((2,)), None, None),
                          (AP.OID((5,)), None, None)))
        # Test nested, fully static tree
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "subids": None},
                                  1: {"reader": None, "writer": None,
                                      "subids":
                                      {0: {"reader": None, "writer": None,
                                           "subids": None},
                                       1: {"reader": None, "writer": None,
                                           "subids":
                                           {42: {"reader": None,
                                                 "writer": None,
                                                 "subids": None}}}}},
                                  5: {"reader": None, "writer": None,
                                      "subids": None}})),
                         ((AP.OID((0,)), None, None),
                          (AP.OID((1,)), None, None),
                          (AP.OID((1, 0)), None, None),
                          (AP.OID((1, 1)), None, None),
                          (AP.OID((1, 1, 42)), None, None),
                          (AP.OID((5,)), None, None)))
        # Test nested, fully static tree, with rootpath
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "subids": None},
                                  1: {"reader": None, "writer": None,
                                      "subids":
                                      {0: {"reader": None, "writer": None,
                                           "subids": None},
                                       1: {"reader": None, "writer": None,
                                           "subids":
                                           {42: {"reader": None,
                                                 "writer": None,
                                                 "subids": None}}}}},
                                  5: {"reader": None, "writer": None,
                                      "subids": None}},
                                 (23,))),
                         ((AP.OID((23, 0)), None, None),
                          (AP.OID((23, 1)), None, None),
                          (AP.OID((23, 1, 0)), None, None),
                          (AP.OID((23, 1, 1)), None, None),
                          (AP.OID((23, 1, 1, 42)), None, None),
                          (AP.OID((23, 5)), None, None)))
        # subid lambda for dynamic tree testing
        submaker = (lambda: {0: {"reader": None, "writer": None,
                                 "subids": None},
                             1: {"reader": None, "writer": None,
                                 "subids":
                                 {0: {"reader": None, "writer": None,
                                      "subids": None}}},
                             2: {"reader": None, "writer": None,
                                 "subids": None}})
        # Test tree with dynamic nodes
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "subids": None},
                                  1: {"reader": None, "writer": None,
                                      "subids": submaker},
                                  2: {"reader": None, "writer": None,
                                      "subids": None}})),
                         ((AP.OID((0,)), None, None),
                          (AP.OID((1,)), None, None),
                          (AP.OID((1, 0)), None, None),
                          (AP.OID((1, 1)), None, None),
                          (AP.OID((1, 1, 0)), None, None),
                          (AP.OID((1, 2)), None, None),
                          (AP.OID((2,)), None, None)))
        # Test tree with dynamic nodes and root path
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "static": True, "subids": None},
                                  1: {"reader": None, "writer": None,
                                      "static": False, "subids": submaker},
                                  2: {"reader": None, "writer": None,
                                      "static": True, "subids": None}},
                                 (23,))),
                         ((AP.OID((23, 0)), None, None),
                          (AP.OID((23, 1)), None, None),
                          (AP.OID((23, 1, 0)), None, None),
                          (AP.OID((23, 1, 1)), None, None),
                          (AP.OID((23, 1, 1, 0)), None, None),
                          (AP.OID((23, 1, 2)), None, None),
                          (AP.OID((23, 2)), None, None)))


if __name__ == '__main__':
    unittest.main()
