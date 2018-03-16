#!/usr/bin/env python
# -*- coding: utf-8 -*-

import ntp.agentx as AX

import unittest

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
                         ((AX.OID((0,)), None, None),
                          (AX.OID((1,)), None, None),
                          (AX.OID((2,)), None, None),
                          (AX.OID((5,)), None, None)))
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
                         ((AX.OID((0,)), None, None),
                          (AX.OID((1,)), None, None),
                          (AX.OID((1, 0)), None, None),
                          (AX.OID((1, 1)), None, None),
                          (AX.OID((1, 1, 42)), None, None),
                          (AX.OID((5,)), None, None)))
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
                         ((AX.OID((23, 0)), None, None),
                          (AX.OID((23, 1)), None, None),
                          (AX.OID((23, 1, 0)), None, None),
                          (AX.OID((23, 1, 1)), None, None),
                          (AX.OID((23, 1, 1, 42)), None, None),
                          (AX.OID((23, 5)), None, None)))
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
                         ((AX.OID((0,)), None, None),
                          (AX.OID((1,)), None, None),
                          (AX.OID((1, 0)), None, None),
                          (AX.OID((1, 1)), None, None),
                          (AX.OID((1, 1, 0)), None, None),
                          (AX.OID((1, 2)), None, None),
                          (AX.OID((2,)), None, None)))
        # Test tree with dynamic nodes and root path
        self.assertEqual(tuple(f({0: {"reader": None, "writer": None,
                                      "static": True, "subids": None},
                                  1: {"reader": None, "writer": None,
                                      "static": False, "subids": submaker},
                                  2: {"reader": None, "writer": None,
                                      "static": True, "subids": None}},
                                 (23,))),
                         ((AX.OID((23, 0)), None, None),
                          (AX.OID((23, 1)), None, None),
                          (AX.OID((23, 1, 0)), None, None),
                          (AX.OID((23, 1, 1)), None, None),
                          (AX.OID((23, 1, 1, 0)), None, None),
                          (AX.OID((23, 1, 2)), None, None),
                          (AX.OID((23, 2)), None, None)))
