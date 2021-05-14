//
// Created by mikolaj on 26.12.2020.
//

#include "nodes.hpp"
#include <utility>

#include <iostream>


void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    double n = static_cast<float>(preferences_.size());
    double new_uniform = 1/(n + 1);
    for(auto& pair: preferences_) {
        pair.second = new_uniform;
    }
    preferences_.emplace(std::pair<IPackageReceiver*, double> (receiver, new_uniform));
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    preferences_.erase(receiver);
    double n = static_cast<float>(preferences_.size());
    double new_uniform = 1/n;
    for(auto& pair: preferences_){
        pair.second = new_uniform;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() const {
    double num = rng_();
    if (0 <= num and num <= 1) {
        auto iter = preferences_.begin();
        if(num <= iter->second){
            return iter->first;
        }
        auto prev_iter = iter;
        iter++;
        double p_left = 0;
        double p_right = prev_iter->second;
        for(; iter != preferences_.end(); iter++, prev_iter++){
            p_left += prev_iter->second;
            p_right += iter->second;
            if (p_left <= num and num <= p_right){
                return iter->first;
            }
        }
    }
    throw std::exception();
}

void PackageSender::send_package() {
    if (sending_buffer_) {
        auto picked_receiver = receiver_preferences_.choose_receiver();
        picked_receiver->receive_package(std::move(sending_buffer_.value()));
        sending_buffer_.reset();
    }
}


void Worker::do_work(Time t) {
    if (!processing_buffer_) {
        if (!q_->empty()){
            processing_buffer_.emplace(q_->pop());
            package_processing_start_time_ = t;
        }
    }
    if (t - package_processing_start_time_ >= pd_ - 1 ) {
        if(processing_buffer_) {
            push_package(std::move(processing_buffer_.value()));
            processing_buffer_.reset();
        }
    }
}

void Ramp::deliver_goods(Time t) {
    if (!((t - 1) % di_)) {
        sending_buffer_.emplace(Package());
    }
}
