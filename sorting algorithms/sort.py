# Mikołaj Depta, 401964

from random import randint
from copy import copy


def quicksort(original_list):
    def quicksort_wrapped(list_copy, start, stop):
        i, j = start, stop
        pivot = list_copy[randint(i, j)]
        while i < j:
            while list_copy[i] < pivot:
                i += 1
            while list_copy[j] > pivot:
                j -= 1
            if i <= j:
                list_copy[i], list_copy[j] = list_copy[j], list_copy[i]
                i += 1
                j -= 1
        if start < j:
            quicksort_wrapped(list_copy, start, j)
        if i < stop:
            quicksort_wrapped(list_copy, i, stop)
        return list_copy

    sort_this = copy(original_list)
    return quicksort_wrapped(sort_this, 0, len(sort_this)-1)


def bubblesort(original_list):
    def bbsort_wrapper(sort_copy):
        n = len(sort_this)
        num_comparisons = 0
        while n > 1:
            new_swaps = 0
            for i in range(n-1):
                num_comparisons += 1
                if sort_copy[i] > sort_copy[i+1]:
                    new_swaps += 1
                    sort_copy[i], sort_copy[i+1] = sort_copy[i+1], sort_copy[i]
            if new_swaps:
                n -= 1
            else:
                return sort_this, num_comparisons
        return sort_this, num_comparisons

    sort_this = copy(original_list)
    return bbsort_wrapper(sort_this)
