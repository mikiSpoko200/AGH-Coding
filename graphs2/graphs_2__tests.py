#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import networkx as nx
from graphs_2 import find_min_trail, load_multigraph_from_file


class TestMinTrail(unittest.TestCase):

    def setUp(self) -> None:
        self.g = load_multigraph_from_file("data/directed_graph.dat")
        self.t13 = nx.dijkstra_path_length(self.g, 1, 3)
        self.t12 = nx.dijkstra_path_length(self.g, 1, 2)
        trail13 = find_min_trail(self.g, 1, 3)
        trail12 = find_min_trail(self.g, 1, 2)
        self.shortest_path13 = sum([tse.edge_w for tse in trail13])
        self.shortest_path12 = sum([tse.edge_w for tse in trail12])


    def test_find_min_trail(self):

        self.assertEqual(self.shortest_path13, self.t13)
        self.assertEqual(self.shortest_path12, self.t12)


if __name__ == "__main__":
    unittest.main()