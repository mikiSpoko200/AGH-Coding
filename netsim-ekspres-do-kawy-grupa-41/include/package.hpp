//
// Created by mikolaj on 19.12.2020.
//

#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP

#include "types.hpp"
#include <set>

class Package{
public:

    Package();
    explicit Package(ElementID ID) : ID_(ID) {}
    Package(Package&& other) : ID_(std::move(other.get_id())) {}
    ~Package();

    Package& operator=(Package&& other) noexcept;
    ElementID get_id() const { return ID_; }

    inline static std::set<ElementID> assigned_IDs;
    inline static std::set<ElementID> freed_IDs;
private:
    ElementID ID_;
    inline static ElementID invalid_id = -1;
    bool is_id_valid() const { return ID_ != Package::invalid_id; }
};

#endif //NET_SIMULATION_PACKAGE_HPP
