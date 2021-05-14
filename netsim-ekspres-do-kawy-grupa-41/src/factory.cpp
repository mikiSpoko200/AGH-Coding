//
// Created by mikolaj on 26.12.2020.
//

#include "factory.hpp"
#include <unordered_map>
#include <iostream>

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if (sender->receiver_preferences_.cbegin() == sender->receiver_preferences_.cend()){
        throw std::logic_error("No receivers found");
    }
    bool does_sender_have_unique_receiver = false;
    for(const auto& pair: sender->receiver_preferences_){
        IPackageReceiver* receiver_p = pair.first;
        if (receiver_p->get_receiver_type() == ReceiverType::STOREHOUSE) {
            does_sender_have_unique_receiver = true;
        }
        else {
            auto worker_ptr = dynamic_cast<Worker*>(receiver_p);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendrecv_ptr != sender) {
                does_sender_have_unique_receiver = true;
            }
            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                return has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if (does_sender_have_unique_receiver) {
        return true;
    } else {
        throw std::logic_error("No unique receiver");
    }
}


bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> color_mapping;
    for(auto& node: ramps_){
        color_mapping.emplace(&node, NodeColor::UNVISITED);
    }
    for(auto& node: workers_){
        color_mapping.emplace(&node, NodeColor::UNVISITED);
    }
    try{
        for(auto& ramp: ramps_){
            if (!has_reachable_storehouse(&ramp, color_mapping)) {
                return false;
            }
        }
        return true;
    }
    catch (std::logic_error&) {
        return false;
    }
}

void Factory::remove_worker(ElementID id) {
    remove_receiver(ramps_, id);
    remove_receiver(workers_, id);
    workers_.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(workers_, id);
    storehouses_.remove_by_id(id);
}

void Factory::do_deliveries(Time t) {
    for(auto& ramp: ramps_){
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for(auto& ramp: ramps_){
        ramp.send_package();
    }
    for(auto& worker: workers_){
        worker.send_package();
    }
}

void Factory::do_work(Time t) {
    for(auto& worker: workers_){
        worker.do_work(t);
    }
}


std::vector<std::string> split (std::string& line, char delim){
    std::istringstream token_stream(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(token_stream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

ParsedLineData parse_line(std::string line) {

    const static std::unordered_map<std::string, ElementType> element_type_lookup {
            {"LOADING_RAMP", ElementType::LOADING_RAMP },
            {"WORKER", ElementType::WORKER},
            {"STOREHOUSE", ElementType::STOREHOUSE},
            {"LINK", ElementType::LINK}
    };

    std::map<std::string, std::string> id_tokens;
    ParsedLineData parsed_data;
    std::vector<std::string> tokens = split(line, ' ');
    std::string tag = tokens[0];
    try{
        parsed_data.element_type = element_type_lookup.at(tag);
    } catch (const std::out_of_range&) {
        throw std::logic_error("bledny identyfikator ElementType");
    }
    for(auto iter = tokens.begin() + 1; iter != tokens.end(); iter++){
        std::vector<std::string> id_val = split(*iter, '=');
        id_tokens[id_val[0]] = id_val[1];
    }
    parsed_data.parameters = id_tokens;
    return parsed_data;
}

Factory load_factory_structure(std::istream& is){
    Factory factory;
    std::vector<ParsedLineData> parsed_lines;
    std::string line;
    while (std::getline(is, line)) {
        if(line[0] != ';' and !line.empty()){
            parsed_lines.emplace_back(parse_line(line));
        }
    }
    for(auto& parsed_line: parsed_lines){
        switch (parsed_line.element_type) {
            case ElementType::LOADING_RAMP: {
                ElementID id_ramp;
                TimeOffset di_ramp;
                for (const auto& pair: parsed_line.parameters) {
                    if (pair.first == "id") {
                        id_ramp = std::stoi(pair.second);
                    } else if (pair.first == "delivery-interval") {
                        di_ramp = std::stoi(pair.second);
                    }
                }
                factory.add_ramp(Ramp(id_ramp, di_ramp));
                break;
            }
            case ElementType::WORKER: {
                ElementID id_worker;
                TimeOffset di_worker;
                std::unique_ptr<IPackageQueue> q;
                for (const auto& pair: parsed_line.parameters) {
                    if (pair.first == "id") {
                        id_worker = std::stoi(pair.second);
                    } else if (pair.first == "processing-time") {
                        di_worker = std::stoi(pair.second);
                    } else if (pair.first == "queue-type") {
                        const static std::unordered_map<std::string, PackageQueueType> package_queue_type_lookup{
                                {"FIFO", PackageQueueType::FIFO},
                                {"LIFO", PackageQueueType::LIFO}
                        };
                        PackageQueueType chosen_type = package_queue_type_lookup.at(pair.second);
                        switch (chosen_type) {
                            case PackageQueueType::FIFO:
                                q = std::make_unique<PackageQueue>(PackageQueueType::FIFO);
                                break;
                            case PackageQueueType::LIFO:
                                q = std::make_unique<PackageQueue>(PackageQueueType::LIFO);
                                break;
                        }
                    }
                }
                factory.add_worker(Worker(id_worker, di_worker, std::move(q)));
                break;
            }
            case ElementType::STOREHOUSE: {
                ElementID id_store;
                for (const auto& pair: parsed_line.parameters) {
                    if (pair.first == "id") {
                        id_store = std::stoi(pair.second);
                    }
                }
                factory.add_storehouse(Storehouse(id_store));
                break;
            }
            case ElementType::LINK: {

                const static std::unordered_map<std::string, SenderType> sender_type_lookup{
                        {"ramp",   SenderType::RAMP},
                        {"worker", SenderType::WORKER}
                };

                const static std::unordered_map<std::string, ReceiverType> receiver_type_lookup{
                        {"worker", ReceiverType::WORKER},
                        {"store",  ReceiverType::STOREHOUSE}
                };

                const static std::unordered_map<std::string, NodeType> node_type_lookup{
                        {"src",  NodeType::SENDER},
                        {"dest", NodeType::RECEIVER}
                };

                IPackageReceiver* rec_p;
                PackageSender* send_p;
                ReceiverType receiver_type;
                SenderType sender_type;
                for (auto pair: parsed_line.parameters) {
                    std::vector<std::string> type_id = split(pair.second, '-');
                    NodeType node_type = node_type_lookup.at(pair.first);
                    ElementID node_id = std::stoi(type_id[1]);
                    switch (node_type) {
                        case NodeType::RECEIVER:
                            receiver_type = receiver_type_lookup.at(type_id[0]);
                            switch (receiver_type) {
                                case ReceiverType::WORKER:
                                    rec_p = &*factory.find_worker_by_id(node_id);
                                    break;
                                case ReceiverType::STOREHOUSE:
                                    rec_p = &*factory.find_storehouse_by_id(node_id);
                                    break;
                            } break;
                        case NodeType::SENDER:
                            sender_type = sender_type_lookup.at(type_id[0]);
                            switch (sender_type) {
                                case SenderType::RAMP:
                                    send_p = &*factory.find_ramp_by_id(node_id);
                                    break;
                                case SenderType::WORKER:
                                    send_p = &*factory.find_worker_by_id(node_id);
                                    break;
                            } break;
                }
            }
            send_p->receiver_preferences_.add_receiver(rec_p);
            break;
            }
        }
    }
    return factory;
}


void save_factory_structure(Factory& factory, std::ostream& os){

    const static std::unordered_map<PackageQueueType, std::string> package_queue_type_to_string {
            {PackageQueueType::FIFO, "FIFO"},
            {PackageQueueType::LIFO, "LIFO"}
    };

    bool any_ramps = factory.ramp_cbegin() != factory.ramp_cend();
    bool any_workers = factory.worker_cbegin() != factory.worker_cend();
    bool any_stores = factory.storehouse_cbegin() != factory.storehouse_cend();

    if (any_ramps){
        os << "; == LOADING RAMPS ==" << std::endl << std::endl;
        for(auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++){
            os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << std::endl;
        }
        os << std::endl;
    }
    if(any_workers){
        os << "; == WORKERS ==" << std::endl << std::endl;
        for(auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++){
            os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration() << " queue-type=";
            os << package_queue_type_to_string.at(it->get_queue()->get_queue_type()) << std::endl;
        }
        os << std::endl;
    }
    if(any_stores){
        os << "; == STOREHOUSES ==" << std::endl << std::endl;
        for(auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++){
            os << "STOREHOUSE id=" << it->get_id() << std::endl;
        }
        os << std::endl;
    }
    if(any_ramps or any_workers){
        os << "; == LINKS ==" << std::endl << std::endl;
        for(auto it_ramp = factory.ramp_cbegin(); it_ramp != factory.ramp_cend(); it_ramp++){
            for(auto it_pref = it_ramp->receiver_preferences_.cbegin(); it_pref != it_ramp->receiver_preferences_.cend(); it_pref++){
                os << "LINK src=ramp-" << it_ramp->get_id() << " dest=worker-" << it_pref->first->get_id() << std::endl;
            }
            os << std::endl;
        }
        for(auto it_worker = factory.worker_cbegin(); it_worker != factory.worker_cend(); it_worker++){
            for(auto it_pref = it_worker->receiver_preferences_.cbegin(); it_pref != it_worker->receiver_preferences_.cend(); it_pref++){
                if(it_pref->first->get_receiver_type() == ReceiverType::WORKER) {
                    os << "LINK src=worker-" << it_worker->get_id() << " dest=worker-" << it_pref->first->get_id() << std::endl;
                } else if (it_pref->first->get_receiver_type() == ReceiverType::STOREHOUSE) {
                    os << "LINK src=worker-" << it_worker->get_id() << " dest=store-" << it_pref->first->get_id() << std::endl;
                }
            }
            os << std::endl;
        }
    }
    std::flush(os);
}

