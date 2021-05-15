#!/usr/bin/python
# -*- coding: utf-8 -*-


from typing import List, Set, Dict
from collections import deque

# Pomocnicza definicja podpowiedzi typu reprezentującego etykietę
# wierzchołka (liczba 1..n).
VertexID = int

# Pomocnicza definicja podpowiedzi typu reprezentującego listę sąsiedztwa.
AdjList = Dict[VertexID, List[VertexID]]

Distance = int


def neighbors(adjlist: AdjList, start_vertex_id: VertexID,
              max_distance: Distance) -> Set[VertexID]:

    """
    Przekształcenie listy sąsiedztwa z postaci gdzie węzły bez wychodzących krawędzi są omijane
    do reprezentacji gdzie takim węzłom przypisane są puste listy
    """

    allVertexes = set()
    for neighbourVertexList in adjlist.values():
        for vertex in neighbourVertexList:
            allVertexes.add(vertex)
    missingVertexes = allVertexes - set(adjlist.keys())
    if missingVertexes:
        vertexesToAdd = [(vertex, []) for vertex in missingVertexes]
        adjlist.update(vertexesToAdd)

    currentDepth = 1
    que = deque()
    visited = {vertexID: False for vertexID in adjlist.keys()}
    que.append(start_vertex_id)
    visited[start_vertex_id] = True
    while que:
        currentDepth += 1
        processedVertex = que.pop()
        for neighbourVertex in adjlist[processedVertex]:
            if not visited[neighbourVertex]:

                """
                Jeśli przekroczymy poziom zagłębienia w graf musimy przestać
                Dodawać nowe węzły do kolejki
                """
                if currentDepth <= max_distance:
                    que.append(neighbourVertex)
                visited[neighbourVertex] = True
    N = {vertexID for vertexID, wasVisited in visited.items() if wasVisited} - {start_vertex_id}
    return N



from typing import List, NamedTuple
import networkx as nx
 
VertexID = int
EdgeID = int
 
 
# Nazwana krotka reprezentująca segment ścieżki.
class TrailSegmentEntry(NamedTuple):
    v_start: VertexID
    v_end: VertexID
    edge_id: EdgeID
    edge_w: float
 
 
Trail = List[TrailSegmentEntry]
 
 
def load_multigraph_from_file(filepath: str) -> nx.MultiDiGraph:
    fileList = []
    with open(filepath) as graphTxt:
        for line in graphTxt:
            line_comp = line.strip()
            if line_comp:
                line_dev = line_comp.split(" ")
                fileList += line_dev
    fileListMod = [(int(u), int(v), float(w)) for u, v, w in
                    zip(fileList[slice(None, -2, 3)], fileList[slice(1, -1, 3)],
                        fileList[slice(2, None, 3)])]
    g = nx.MultiDiGraph()
    g.add_weighted_edges_from(fileListMod)
    return g

 
def find_min_trail(g: nx.MultiDiGraph, v_start: VertexID, v_end: VertexID) -> Trail:
    
    class priorityQueue:
        def __init__(self, listin):
            self._queue = sorted(listin, key=lambda x: x[1])
            
        def append(self, other):
            self._queue.append(other)
            self._queue.sort(key=lambda x: x.keys())
            
        def pop(self, index):
            return self._queue.pop(index)
        
        def __len__(self):
            return len(self._queue)
        
        def __str__(self):
            return str(self._queue)
    
    
    def dijkstra(g, v_start):
        best_dist = {node: float("inf") for node in g}
        succesors = {node: None for node in g}
        succesors[v_start] = "undef"
        best_dist[v_start] = 0
        pq = priorityQueue([(node, best_dist[node]) for node in g])
        while pq:
            cur_node, min_dist = pq.pop(0)
            for nbr in g[cur_node]:
                chosen_edge, best_weight = min(g[cur_node][nbr].items(), key=lambda Ditem: Ditem[1]['weight'])
                best_weight = best_weight['weight']
                if best_dist[nbr] > best_dist[cur_node] + best_weight:
                    best_dist[nbr] = best_dist[cur_node] + best_weight
                    succesors[nbr] = cur_node, chosen_edge, best_weight
        return best_dist, succesors
    
    distance, succesors = dijkstra(g, v_start)
    if succesors[v_end] is None:
        return []
    path = []
    v_procesed, chosen_edge, chosen_weight = succesors[v_end]
    path.append(TrailSegmentEntry(v_procesed, v_end, chosen_edge, chosen_weight))
    while v_procesed is not v_start:
        enen = v_procesed
        v_procesed, chosen_edge, chosen_weight = succesors[v_procesed]
        path.append(TrailSegmentEntry(v_procesed, enen, chosen_edge, chosen_weight))
    return path[::-1]


def trail_to_str(trail: Trail) -> str:
    path = ""
    suma = 0
    for TSE in trail:
        suma += TSE.edge_w
        path += f"{TSE.v_start} -[{TSE.edge_id}: {TSE.edge_w}]-> "
    path += f"{trail[-1].v_end}  (total = {suma})"
    return path


