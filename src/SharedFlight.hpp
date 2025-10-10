#pragma once
#include <mutex>
#include <functional>
#include <vector>
#include "Flight.hpp"

class SharedFlight {
public:
    using Callback = std::function<void()>;

    std::mutex mutex;
    Flight flight_snapshot;
    bool updated = false;

    void subscriber(Callback cb) {
        std::lock_guard<std::mutex> lock(flight_mutex);
        subscribers.push_back(std::move(cb));
    }

    Flight snapshot() const {
        std::lock_guard<std::mutex> lock(flight_mutex);
        return flight;
    }

    void addFrame(const CrsfFrame& frame) {
        {
            std::lock_guard<std::mutex> lock(flight_mutex);
            flight.addFrame(frame);
            updated = true;
        }
        notifySubscribers();
    }
private:
    mutable std::mutex flight_mutex;
    Flight flight;

    std::mutex cb_mutex;
    std::vector<Callback> subscribers;

    void notifySubscribers() {
        std::lock_guard<std::mutex> lock(cb_mutex);
        for (auto& cb : subscribers) {
            cb();
        }
    }
};