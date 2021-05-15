#!/usr/bin/python
# -*- coding: utf-8 -*-
 
from typing import Optional, List, Union
from abc import ABC, abstractmethod
import re


class Product:
    def __init__(self, name: str, price: float):
        if not re.fullmatch(r"\b^[a-zA-Z]+[0-9]+$\b", name):
            raise ValueError(f"{name} - Product's name did not meet naming criteria")
        self.name: str = name
        self.price: float = price

    def __eq__(self, other):
        return hash(self) == hash(other)
 
    def __hash__(self):
        return hash((self.name, self.price))


class ServerError(Exception):
    def __init__(self, msg=None):
        msg = msg if msg is not None else f'\nServer encountered an error\n'
        super().__init__(msg)

 
class TooManyProductsFoundError(ServerError):
    def __init__(self, num_el: int):
        super().__init__(msg=f'\nMore valid entries found than server allows:\n'
                         f'(entries found = {num_el}) > (n_max_returned_entries = {Server.n_max_returned_entries})')
        self.num_el = num_el


class Server(ABC):
    n_max_returned_entries: int = 3

    @abstractmethod
    def convert_products(self) -> List[Product]:
        pass

    def get_entries(self, n_letters: int) -> List[Optional[Product]]:
        if isinstance(n_letters, int):
            if n_letters < 1:
                raise ValueError("Incorrect argument value for n_letters.\n"
                                 f"n_letters must be greater or equal to 1\nValue provided: {n_letters}")
        else:
            raise TypeError(f"Incorrect argument type for n_letters.\n"
                            f"Type required: <class 'int'>\nType provided: {type(n_letters)}")

        list_prod = self.convert_products()
        valid_entries = [prod for prod in list_prod if re.fullmatch(f"^[a-zA-Z]{{{n_letters}}}[0-9]{{2,3}}$", prod.name)]
        if len(valid_entries) > Server.n_max_returned_entries:
            raise TooManyProductsFoundError(len(valid_entries))
        else:
            valid_entries.sort(key=lambda prod: prod.price)
            return valid_entries


class ListServer(Server):
    def __init__(self, prod_list: List[Product]) -> None:
        self.products = prod_list

    def convert_products(self) -> List[Product]:
        return self.products


class MapServer(Server):
    def __init__(self, prod_list: List[Product]) -> None:
        self.products = {prod.name: prod for prod in prod_list}

    def convert_products(self) -> List[Product]:
        return [prod for prod in self.products.values()]


class Client:
    def __init__(self, server: Union[ListServer, MapServer]) -> None:
        self.server = server
 
    def get_total_price(self, n_letters: Optional[int] = 1) -> Union[float, None]:
        try:
            elems = [prod.price for prod in self.server.get_entries(n_letters)]
            return float(sum(elems)) if len(elems) else None
        except TooManyProductsFoundError:
            return None
