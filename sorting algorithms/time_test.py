#!/usr/bin/python
# -*- coding: utf-8 -*-


from random import sample
from timeit import timeit
from copy import copy
from sort import bubblesort, quicksort

"""
Moja implementacja algorytmu sortowania szybkiego zakłada losowy wybór pivota. Skutkuje to 
zmniejszeniem prawdopodobieństwa na wystąpienie przypadków najkorzystniejszych i najmniej 
korzystnych. Oznacza to, że czas wykonania algorytmu będzie podobny bez względu na charakter 
danych, a złożoność obliczeniowa powinna być bliska nlogn.  

Algorytm sortowania bąbelkowego z kolei powinien mieć lepsze czasy wykonania dla list 
posortowanych (w szczególności wypełnionych tylko jedna wartością) lecz znacznie gorsze czasy w 
każdym innym przypadku ze względu na oczekiwaną złożoność obliczeniową n^2. 

Biorąc pod uwagę cechy obydwu algorytmów oczekuję, że algorytm sortowania szybkiego wykona 
się w podobnym czasie bez względu na rodzaj danych. Algorytm sortowania bąbelkowego 
osiągnie od niego lepsze czasy w przypadku list posortowanych i list zawierających elementy o 
równej wartości. Natomiast w pozostałych przypadkach tj. sortowania list odwróconych i list 
wypełnionych losowymi elementami sortowanie bąbelkowe okaże się znacznie wolniejsze od 
sortowania szybkiego.

Wyniki zostały przedstawione w następujacy sposób:

    x. <nazwa algorytmu>

Czas w sekundach 1000 wykonań algorytmów dla 1000-elementowych list:
1. posortowanych
2. posortowanych odwrotnie
3. zawierających elementy o tej samej wartości
4. wypełnionych elementami o losowej wartości

Uzyskane wyniki:

    1. quicksort:
    
1. time_quicksort_sorted : 2.325135
2. time_quicksort_reversed : 2.3389390999999997
3. time_quicksort_uniform : 3.6502473999999996
4. time_quicksort_random : 3.4853910999999993

    2. bubblesort:

1. time_bubblesort_sorted : 0.20520879999999941
2. time_bubblesort_reversed : 221.1383284
3. time_bubblesort_uniform : 0.13798260000001505
4. time_bubblesort_random : 141.01647859999997

Jak widać, powyższe wyniki całkowicie zgadzają się z teoretycznymi oczekiwaniami.
"""


sorted = list(range(1, 1001))
reversed = copy(sorted[::-1])
uniform = [1 for _ in range(1000)]
randomized = sample(range(1, 10000), 1000)
time_quicksort = {
    "time_quicksort_sorted": timeit("quicksort(sorted)", number=1000, globals=globals()),
    "time_quicksort_reversed": timeit("quicksort(reversed)", number=1000, globals=globals()),
    "time_quicksort_uniform": timeit('quicksort(uniform)', number=1000, globals=globals()),
    "time_quicksort_random": timeit('quicksort(randomized)', number=1000, globals=globals())
}
time_bubblesort = {
    "time_bubblesort_sorted": timeit('bubblesort(sorted)', number=1000, globals=globals()),
    "time_bubblesort_reversed": timeit('bubblesort(reversed)', number=1000, globals=globals()),
    "time_bubblesort_uniform": timeit('bubblesort(uniform)', number=1000, globals=globals()),
    "time_bubblesort_random": timeit('bubblesort(randomized)', number=1000, globals=globals())
}
# for timed_list_type, time in time_quicksort.items():
#     print(f"{timed_list_type} : {time}")
#
# for timed_list_type, time in time_bubblesort.items():
#     print(f"{timed_list_type} : {time}")
