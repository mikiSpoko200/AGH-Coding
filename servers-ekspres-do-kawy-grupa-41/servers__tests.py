#!/usr/bin/python
# -*- coding: utf-8 -*-
import unittest
from collections import Counter

from servers import ListServer, Product, Client, MapServer, Server

server_types = (ListServer, MapServer)


class ServerTest(unittest.TestCase):
    def test_get_entries_returns_proper_entries(self):
        products = [Product('P12', 1), Product('PP234', 2), Product('PP235', 1)]
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(2)
            self.assertEqual(Counter([products[2], products[1]]), Counter(entries))

    def test_incorrect_arg_type_error(self):
        products = [Product('P12', 1), Product('AB234', 2), Product('PP235', 1)]
        with self.assertRaises(TypeError):
            for server_type in server_types:
                server = server_type(products)
                _ = server.get_entries('a')

    def test_incorrect_arg_val_error(self):
        products = [Product('P12', 1), Product('PP234', 2)]
        with self.assertRaises(ValueError):
            for server_type in server_types:
                server = server_type(products)
                _ = server.get_entries(0)


class ClientTest(unittest.TestCase):
    def test_total_price_for_normal_execution(self):
        products = [Product('PP234', 2), Product('PP235', 3)]
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(5, client.get_total_price(2))

    def test_too_many_products_search_error(self):
        products = [Product(f'PP{x}{x}', 3) for x in range(Server.n_max_returned_entries + 1)]
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            entries = client.get_total_price(2)
            self.assertIsNone(entries)


class ProductTest(unittest.TestCase):
    def test_product_wrong_name_criteria_error(self):
        with self.assertRaises(ValueError):
            _ = [Product('P2P34', 2), Product('PP235', 3)]

    def test_product_name_criteria(self):
        _ = [Product('PadfddsP344', 2), Product('PP2354215415', 3)]


if __name__ == '__main__':
    unittest.main()
