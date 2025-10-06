#pragma once
#include "CrsfFrameNew.hpp"
#include <vector>

class Flight
{
private:
    std::vector<CrsfFrame> frames;
    
public:
    Flight() = default;
    ~Flight() = default;

    // Add a frame to the flight data
    void addFrame(const CrsfFrame& frame) {
        frames.push_back(frame);
    }
    
    // Get all frames
    const std::vector<CrsfFrame>& getFrames() const {
        return frames;
    }
    
    // Get frames of specific type
    std::vector<CrsfFrame> getFramesByType(uint8_t frameType) const {
        std::vector<CrsfFrame> result;
        for (const auto& frame : frames) {
            if (frame.getType() == frameType) {
                result.push_back(frame);
            }
        }
        return result;
    }
    
    // Get decoded data of specific type
    template<typename T>
    std::vector<T> getDataOfType() const {
        std::vector<T> result;
        for (const auto& frame : frames) {
            auto data = frame.decode();
            if (std::holds_alternative<T>(data)) {
                result.push_back(std::get<T>(data));
            }
        }
        return result;
    }
    
    // Convenience methods for getting specific data types
    std::vector<GpsFrameData> getGpsData() const {
        return getDataOfType<GpsFrameData>();
    }
    
    std::vector<AttitudeFrameData> getAttitudeData() const {
        return getDataOfType<AttitudeFrameData>();
    }
    
    std::vector<BatteryFrameData> getBatteryData() const {
        return getDataOfType<BatteryFrameData>();
    }
    
    std::vector<FlightModeFrameData> getFlightModeData() const {
        return getDataOfType<FlightModeFrameData>();
    }
    
    std::vector<LinkRXFrameData> getLinkRXData() const {
        return getDataOfType<LinkRXFrameData>();
    }
};