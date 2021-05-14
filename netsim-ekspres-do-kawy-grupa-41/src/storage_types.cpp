//
// Created by mikolaj on 19.12.2020.
//

#include "storage_types.hpp"
#include <iostream>

Package PackageQueue::pop() {

    Package output;
    switch (pqtype_) {
        case PackageQueueType::FIFO:
            Package::assigned_IDs.erase(que_.begin()->get_id());
            Package::freed_IDs.insert(que_.begin()->get_id());
            output = std::move(*que_.begin());
            que_.pop_front();
            break;
        case PackageQueueType::LIFO:
            Package::assigned_IDs.erase(que_.rbegin()->get_id());
            Package::freed_IDs.insert(que_.rbegin()->get_id());
            output = std::move(*que_.rbegin());
            que_.pop_back();
            break;
    }
    return output;
}