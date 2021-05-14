//
// Created by mikolaj on 06.01.2021.
//

#include "reports.hpp"
#include <map>


ProcessedReceiverPreferences sort_preferences(const ReceiverPreferences::preferences_t& preferences){

    ProcessedReceiverPreferences prefs;

    for (const auto& pair: preferences) {
        std::string message_receiver;
        if (pair.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
            message_receiver = "    storehouse #" + std::to_string(pair.first->get_id()) + "\n";
            prefs.mapping_receiver_storehouse.push_back(
                    std::pair<ElementID, std::string>(pair.first->get_id(), message_receiver));
        }
    }
    for (const auto& pair: preferences) {
        std::string message_receiver;
        if (pair.first->get_receiver_type() == ReceiverType::WORKER) {
            message_receiver = "    worker #" + std::to_string(pair.first->get_id()) + "\n";
            prefs.mapping_receiver_worker.push_back(
                    std::pair<ElementID, std::string>(pair.first->get_id(), message_receiver));
        }
    }
    std::sort(prefs.mapping_receiver_worker.begin(), prefs.mapping_receiver_worker.end(),
              [](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
    std::sort(prefs.mapping_receiver_storehouse.begin(), prefs.mapping_receiver_storehouse.end(),
              [](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
    return prefs;
}


void generate_structure_report(const Factory& f,std::ostream& os) {

    const static std::unordered_map<PackageQueueType, std::string> package_queue_type_to_string {
            {PackageQueueType::FIFO, "FIFO"},
            {PackageQueueType::LIFO, "LIFO"}
    };

    bool any_ramps = f.ramp_cbegin() != f.ramp_cend();
    bool any_workers = f.worker_cbegin() != f.worker_cend();
    bool any_stores = f.storehouse_cbegin() != f.storehouse_cend();


    std::vector<std::pair<ElementID, std::string>> mapping_ramps;
    std::vector<std::pair<ElementID, std::string>> mapping_workers;
    std::vector<std::pair<ElementID, std::string>> mapping_storehouses;

    for(auto it = f.ramp_cbegin(); it != f.ramp_cend(); it++){

        std::string message_ramp = "LOADING RAMP #" + std::to_string(it->get_id()) + "\n";
        message_ramp += "  Delivery interval: " + std::to_string(it->get_delivery_interval()) + "\n";
        message_ramp += "  Receivers:\n";

        ProcessedReceiverPreferences mapping_id_str = sort_preferences(it->receiver_preferences_.get_preferences());

        for (const auto& mess_receiver: mapping_id_str.mapping_receiver_storehouse) {
            message_ramp += mess_receiver.second;
        }
        for (const auto& mess_receiver: mapping_id_str.mapping_receiver_worker) {
            message_ramp += mess_receiver.second;
        }

        message_ramp += "\n";
        mapping_ramps.push_back(std::pair<ElementID, std::string>(it->get_id(), message_ramp));
    }

    for(auto it = f.worker_cbegin(); it != f.worker_cend(); it++){

        std::string message_worker = "WORKER #" + std::to_string(it->get_id()) + "\n";
        message_worker += "  Processing time: " + std::to_string(it->get_processing_duration()) + "\n";
        message_worker += "  Queue type: " + package_queue_type_to_string.at(it->get_queue()->get_queue_type()) + "\n";
        message_worker += "  Receivers:\n";

        ProcessedReceiverPreferences mapping_id_str = sort_preferences(it->receiver_preferences_.get_preferences());

        for (const auto& mess_receiver: mapping_id_str.mapping_receiver_storehouse){
            message_worker += mess_receiver.second;
        }
        for (const auto& mess_receiver: mapping_id_str.mapping_receiver_worker){
            message_worker += mess_receiver.second;
        }

        message_worker += "\n";
        mapping_workers.push_back(std::pair<ElementID, std::string>(it->get_id(), message_worker));
    }

    for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); it++){
        std::string message_storehouse = "STOREHOUSE #" + std::to_string(it->get_id()) + "\n";
        message_storehouse += "\n";
        mapping_storehouses.push_back(std::pair<ElementID, std::string>(it->get_id(), message_storehouse));
    }

    std::sort(mapping_ramps.begin(), mapping_ramps.end(), []( auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
    std::sort(mapping_workers.begin(), mapping_workers.end(), []( auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
    std::sort(mapping_storehouses.begin(), mapping_storehouses.end(), []( auto& pair1, auto& pair2) { return pair1.first < pair2.first; });

    if (any_ramps) {
        os << std::endl;
        os << "== LOADING RAMPS ==" << std::endl;
        os << std::endl;
    }
    for(const auto& pair: mapping_ramps){
        os << pair.second;
    }
    if (any_workers) {
        os << std::endl;
        os << "== WORKERS ==" << std::endl;
        os << std::endl;
    }
    for(const auto& pair: mapping_workers){
        os << pair.second;
    }
    if (any_stores) {
        os << std::endl;
        os << "== STOREHOUSES ==" << std::endl;
        os << std::endl;
    }
    for(const auto& pair: mapping_storehouses){
        os << pair.second;
    }

}


void generate_simulation_turn_report(const Factory& f,std::ostream& os,Time t) {

    std::vector<std::pair<ElementID , std::string>> mapping_worker;
    for(auto it = f.worker_cbegin(); it != f.worker_cend(); it++){

        std::vector<std::pair<ElementID , std::string>> mapping_package;
        bool pbuffer_has_val = it->get_processing_buffer().has_value();
        bool sbuffer_has_val = it->get_sending_buffer().has_value();
        bool queue_has_val = it->cbegin() != it->cend();

        std::string pbuffer_stat = pbuffer_has_val ? "#" + std::to_string(it->get_processing_buffer().value().get_id()) +
                                                     " (pt = " + std::to_string(t - it->get_package_processing_start_time() + 1) + ")" : "(empty)";
        std::string sbuffer_stat = sbuffer_has_val ? "#" + std::to_string(it->get_sending_buffer().value().get_id()) : "(empty)";
        std::string queue_stat;

        if (queue_has_val){
            auto it_package = it->cbegin();
            queue_stat = "#" + std::to_string(it_package->get_id());
            it_package++;
            for(; it_package != it->cend(); it_package++){
                queue_stat += ", #" + std::to_string(it_package->get_id());
            }
        } else {
            queue_stat = "(empty)";
        }
        std::string message = "WORKER #" + std::to_string(it->get_id()) + "\n";
        message += "  PBuffer: " + pbuffer_stat + "\n";
        message += "  Queue: " + queue_stat + "\n";
        message += "  SBuffer: " + sbuffer_stat + "\n\n";
        mapping_worker.push_back(std::pair<ElementID, std::string>(it->get_id(), message));
    }

    std::vector<std::pair<ElementID , std::string>> mapping_storehouse;
    for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); it++){
        bool queue_has_val = it->cbegin() != it->cend();
        std::string queue_stat;
        if (queue_has_val){
            auto it_package = it->cbegin();
            queue_stat = "#" + std::to_string(it_package->get_id());
            it_package++;
            for(; it_package != it->cend(); it_package++){
                queue_stat += ", #" + std::to_string(it_package->get_id());
            }
        } else {
            queue_stat = "(empty)";
        }
        std::string message = "STOREHOUSE #" + std::to_string(it->get_id()) + "\n";
        message += "  Stock: " + queue_stat + "\n\n";
        mapping_storehouse.push_back(std::pair<ElementID, std::string>(it->get_id(), message));
    }

    std::sort(mapping_worker.begin(), mapping_worker.end(), []( auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
    std::sort(mapping_storehouse.begin(), mapping_storehouse.end(), []( auto& pair1, auto& pair2) { return pair1.first < pair2.first; });

    os << "=== [ Turn: " + std::to_string(t)  + " ] ===" << std::endl;
    os << std::endl;
    os << "== WORKERS ==" << std::endl;
    os << std::endl;
    for(const auto& pair: mapping_worker){
        os << pair.second;
    }
    os << std::endl;
    os << "== STOREHOUSES ==" << std::endl;
    os << std::endl;
    for(const auto& pair: mapping_storehouse){
        os << pair.second;
    }

    std::flush(os);
}
