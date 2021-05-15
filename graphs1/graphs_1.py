#!/usr/bin/python
# -*- coding: utf-8 -*-

from typing import List, Dict
from collections import deque


def adjmat_to_adjlist(adjmat: List[List[int]]) -> Dict[int, List[int]]:
    # Stwórz słownik i przygotuj listy dla odpowiednich węzłów
    adjlist = {rindex + 1: [] for rindex in range(len(adjmat)) if sum(adjmat[rindex]) > 0}
    for rindex, row in enumerate(adjmat):
        # z (rindex + 1)'tego elementu mam elem połączeń do eindex
        for eindex, elem in enumerate(row):
            if elem:
                adjlist[rindex + 1] += elem * [eindex + 1]
    return adjlist


def dfs_recursive(adjlist: Dict[int, List[int]], curNode: int) -> List[int]:
    def dfs_wrapper(adjlist, curNode, visited):
        visited += [curNode]
        for neighbourNode in adjlist[curNode]:
            if neighbourNode not in visited:
                visited = dfs_wrapper(adjlist, neighbourNode, visited)
        return visited

    return dfs_wrapper(adjlist, curNode, [])


def dfs_iterative(adjlist: Dict[int, List[int]], curNode: int) -> List[int]:
    S = deque()
    S.append(curNode)
    path = []
    visited = {node: False for node in adjlist.keys()}
    while S:
        processed_node = S.pop()
        if not visited[processed_node]:
            path.append(processed_node)
            visited[processed_node] = True
            for neighbour_node in adjlist[processed_node][::-1]:
                S.append(neighbour_node)
    return path


def is_acyclic(adjlist: Dict[int, List[int]]) -> bool:
    S = deque()
    path = []
    visited = {node: False for node in adjlist.keys()}

    # Poniższa pętla zapewanie, że odwiedzony będzie każdy wierzchołek odwiedzony.
    while list(visited.values()) != [True]*len(visited.keys()):
        for key, value in visited.items():
            if not value:
                curNode = key
                S.append(curNode)
                break

        """
        Zodyfikowany dfs, sprawdza on czy aktualnie przetwarzany wierzchołek był już wcześniej odwiedzony
        Znajduje się w liście path - jeśli tak to mamy doczyniania z krawędzią wsteczną co oznacza, że graf na pewno
        jest cykliczny.
        """
        while S:
            processed_node = S.pop()
            if processed_node in path:
                return False

            """
            Wyłapanie błędu KeyError pozwala na obsługę przez algorytm list sąsiedztwa w postaci gdzie wierzchołki
            bez krawędzi wychodzących nie są dodawane jako pary <numer wierzchołka>: [] a są całkowicie ignorowane.
            """
            try:
                if not visited[processed_node]:
                    path.append(processed_node)
                    visited[processed_node] = True
                    for neighbour_node in adjlist[processed_node][::-1]:
                        S.append(neighbour_node)
            except KeyError:
                pass
    return True
