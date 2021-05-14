//
// Created by mikolaj on 06.01.2021.
//

#ifndef NET_SIMULATION_REPORTS_HPP
#define NET_SIMULATION_REPORTS_HPP

#include "factory.hpp"

struct ProcessedReceiverPreferences{
    std::vector<std::pair<ElementID, std::string>> mapping_receiver_worker;
    std::vector<std::pair<ElementID, std::string>> mapping_receiver_storehouse;
};

ProcessedReceiverPreferences sort_preferences(const ReceiverPreferences::preferences_t& preferences);

void generate_structure_report(const Factory& f,std::ostream& os);
void generate_simulation_turn_report(const Factory& f,std::ostream& os,Time t);

class IntervalReportNotifier{
public:
    IntervalReportNotifier(TimeOffset to) : to_(to) {}
    bool should_generate_report(Time t) { return !static_cast<bool>((t - 1) % to_); }

private:
    TimeOffset to_;
};

class SpecificTurnsReportNotifier{
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(turns) {}
    bool should_generate_report(Time t) { return turns_.find(t) != turns_.end(); }

private:
    std::set<Time> turns_;
};
#endif //NET_SIMULATION_REPORTS_HPP
