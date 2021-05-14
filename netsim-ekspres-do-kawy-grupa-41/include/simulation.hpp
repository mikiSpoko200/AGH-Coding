//
// Created by mikolaj on 08.01.2021.
//

#ifndef NET_SIMULATION_SIMULATE_HPP
#define NET_SIMULATION_SIMULATE_HPP

#include <utility>

#include "factory.hpp"
#include "reports.hpp"
#include <set>

void simulate(Factory& f, TimeOffset d, std::function<void(Factory&, Time)> rf);

#endif //NET_SIMULATION_SIMULATE_HPP
