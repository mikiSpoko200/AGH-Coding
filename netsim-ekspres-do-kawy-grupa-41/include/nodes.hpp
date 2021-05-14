//
// Created by mikolaj on 26.12.2020.
//

#ifndef NET_SIMULATION_NODES_HPP
#define NET_SIMULATION_NODES_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "helpers.hpp"
#include "config.hpp"

#include <map>
#include <optional>
#include <memory>
#include <utility>


enum class ReceiverType{
    WORKER,
    STOREHOUSE
};

enum class SenderType{
    RAMP,
    WORKER
};


class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;

    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        virtual ReceiverType get_receiver_type() const = 0;
    #endif

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences{
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(ProbabilityGenerator rand_ng = probability_generator) : rng_(rand_ng) {}
    void add_receiver(IPackageReceiver* receiver);
    void remove_receiver(IPackageReceiver* receiver);
    IPackageReceiver* choose_receiver() const;
    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() const { return preferences_.cbegin(); }
    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator end() const { return preferences_.cend(); }
    const_iterator cend() const { return preferences_.cend(); }

private:
    preferences_t preferences_;
    ProbabilityGenerator rng_;
};

class PackageSender{
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer_; }
    void send_package();
    ReceiverPreferences receiver_preferences_;

protected:
    void push_package(Package&& p) { sending_buffer_.emplace(std::move(p)); }
    std::optional<Package> sending_buffer_;
};


class Ramp : public PackageSender{
public:
    Ramp(ElementID id, TimeOffset di) : PackageSender(), id_(id), di_(di) {}
    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const { return di_; }
    ElementID get_id() const { return id_; }

private:
    ElementID id_;
    TimeOffset di_;
};

class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}
    const std::optional<Package>& get_processing_buffer() const { return processing_buffer_; }

    void do_work(Time t);
    Time get_package_processing_start_time() const { return package_processing_start_time_; }
    IPackageQueue* get_queue() const { return q_.get(); }
    TimeOffset get_processing_duration() const { return pd_; }
    void receive_package(Package&& p) override { q_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }

    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        ReceiverType get_receiver_type() const override { return rt_; }
    #endif

    IPackageStockpile::const_iterator begin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator end() const override { return q_->cend(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }


private:
    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        ReceiverType rt_ = ReceiverType::WORKER;
    #endif
    Time package_processing_start_time_;
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> processing_buffer_;
};

class Storehouse : public IPackageReceiver{
public:
    explicit Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)) {}

    void receive_package(Package&& p) override { d_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }

    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        ReceiverType get_receiver_type() const override { return rt_; }
    #endif

    IPackageStockpile::const_iterator begin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator end() const override { return d_->cend(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }

private:
    #if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
        ReceiverType rt_ = ReceiverType::STOREHOUSE;
    #endif
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};


#endif //NET_SIMULATION_NODES_HPP
