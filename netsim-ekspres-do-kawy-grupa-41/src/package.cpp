//
// Created by mikolaj on 19.12.2020.
//

#include "package.hpp"

Package & Package::operator=(Package&& other) noexcept {
    this->ID_ = other.ID_;
    other.ID_ = Package::invalid_id;
    return *this;
}

Package::Package() {
//    Jesli nie ma wolnych ID
    if(Package::freed_IDs.empty()){
        ElementID max_ID;
//        Jesli sa przypisane jakies ID
        if (!Package::assigned_IDs.empty()) {
            max_ID = *Package::assigned_IDs.rbegin() + 1;
            Package::assigned_IDs.insert(max_ID);
            ID_ = max_ID;
        }
//        Jesli nie ma przypisanych zadnych ID
        else {
            max_ID = 1;
            Package::assigned_IDs.insert(max_ID);
            ID_ = max_ID;
        }
    }
//    Jesli są wolne ID
    else{
        auto iter = Package::freed_IDs.begin();
        ElementID free_ID = *iter;
        Package::freed_IDs.erase(iter);
        Package::assigned_IDs.insert(free_ID);
        ID_ = free_ID;
    }
}

Package::~Package() {
    if (is_id_valid()) {
        Package::assigned_IDs.erase(ID_);
        Package::freed_IDs.insert(ID_);
    }
}
