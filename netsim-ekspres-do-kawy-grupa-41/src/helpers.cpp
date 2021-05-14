//
// Created by mikolaj on 02.01.2021.
//

#include "helpers.hpp"

#include <cstdlib>
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    return std::generate_canonical<double, 10>(rng);
}

double rigged_probability_generator() {
    static int calls = 0;
    calls ++;
    return calls == 1 ? 0.3 : 0.7;
}


std::function<double()> probability_generator = rigged_probability_generator;
