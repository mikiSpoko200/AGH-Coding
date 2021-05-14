//
// Created by mikolaj on 19.12.2020.
//

#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP


#include "package.hpp"

#include <list>
#include <utility>

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual ~IPackageStockpile() = default;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void push(Package&& package) = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;
};

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageQueue : public IPackageStockpile{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
    virtual ~IPackageQueue() = default;
};

class PackageQueue : public IPackageQueue{
public:
    explicit PackageQueue(PackageQueueType pqtype) : pqtype_(pqtype) {}
    std::size_t size() const override { return que_.size(); }
    bool empty() const override { return que_.empty(); }
    void push(Package&& package) override { que_.emplace_back(std::move(package)); }

    const_iterator begin() const override { return que_.cbegin(); }
    const_iterator end() const override { return que_.cend(); }
    const_iterator cbegin() const override { return que_.cbegin(); }
    const_iterator cend() const override { return que_.cend(); }

    Package pop() override;
    PackageQueueType get_queue_type() const override { return pqtype_; }

private:
    std::list<Package> que_;
    PackageQueueType pqtype_;
};

#endif //NET_SIMULATION_STORAGE_TYPES_HPP
