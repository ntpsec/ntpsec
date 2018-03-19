#!/usr/bin/env python
# -*- coding: utf-8 -*-

import ntp.agentx as AX
import ntp.agentx_packet as AP

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
                         {0:{"reader":1, "writer":2, "subids":None}})
        # Top level, non-dynamic, class OID
        c.addNode(AP.OID((1,)), 1, 2)
        self.assertEqual(c.oidTree,
                         {0:{"reader":1, "writer":2, "subids":None},
                          1:{"reader":1, "writer":2, "subids":None}})
        # Top level, dynamic
        c.addNode((2,), dynamic=3)
        self.assertEqual(c.oidTree,
                         {0:{"reader":1, "writer":2, "subids":None},
                          1:{"reader":1, "writer":2, "subids":None},
                          2:{"reader":None, "writer":None, "subids":3}})
        # Sub level
        c.addNode((1, 2, 3), 1, 2)
        self.assertEqual(c.oidTree,
                         {0:{"reader":1, "writer":2, "subids":None},
                          1:{"reader":1, "writer":2, "subids":
                             {2:{"reader":None, "writer":None, "subids":{
                                 3:{"reader":1, "writer":2, "subids":None}}}}},
                          2:{"reader":None, "writer":None, "subids":3}})

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
