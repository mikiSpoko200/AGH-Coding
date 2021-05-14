//
// Created by mikolaj on 26.12.2020.
//

#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP

#include "nodes.hpp"
#include <list>
#include <utility>
#include <algorithm>
#include <string>
#include <sstream>
#include <istream>
#include <iostream>


enum class NodeColor {
    UNVISITED,
    VISITED,
    VERIFIED };

enum class ElementType{
    LOADING_RAMP ,
    WORKER,
    STOREHOUSE,
    LINK
};

enum class NodeType{
    RECEIVER,
    SENDER
};

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);

template <class Node>
class NodeCollection{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    iterator find_by_id(ElementID id) {
        return std::find_if(collection_.begin(), collection_.end(), [=](const Node& node){ return node.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(collection_.begin(), collection_.end(), [=](const Node& node){ return node.get_id() == id; } );
    }

    void add(Node&& node) { collection_.emplace_back(std::move(node)); }
    void remove_by_id(ElementID id) {
        const_iterator iter = find_by_id(id);
        if (iter != collection_.cend() ){
            collection_.erase(iter);
        }
    }

    iterator begin() { return collection_.begin(); }
    iterator end() { return collection_.end(); }
    const_iterator begin() const { return collection_.cbegin(); }
    const_iterator end() const { return collection_.cend(); }
    const_iterator cbegin() const  { return collection_.cbegin(); }
    const_iterator cend() const  { return collection_.cend(); }

private:
    container_t collection_;
};

class Factory {
public:
    void add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); }
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }

    void add_worker(Worker&& worker) { workers_.add(std::move(worker)); }
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }

    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }

    bool is_consistent() const;
    void do_deliveries (Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection , ElementID id) {
        for(auto& node: collection){
            auto pref_t_ptr = std::find_if(node.receiver_preferences_.begin(), node.receiver_preferences_.end(), [=](auto& pair) { return pair.first->get_id() == id; });
            auto receiver_ptr = pref_t_ptr->first;
            node.receiver_preferences_.remove_receiver(receiver_ptr);
        }
    }

    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;
};

struct ParsedLineData{
    using parameters_t = std::map<std::string, std::string>;
    ElementType element_type;
    parameters_t parameters;
};

ParsedLineData parse_line(std::string line);


Factory load_factory_structure(std::istream& is);


void save_factory_structure(Factory& factory, std::ostream& os);


#endif //NET_SIMULATION_FACTORY_HPP
