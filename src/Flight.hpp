#pragma once
#include "Crsf/CrsfFrame.hpp"
#include "Crsf/CrsfFrameData.hpp"
#include <vector>

class Flight
{
private:
    // Store all frames for complete history
    std::vector<CrsfFrame> frames;
    
    // Separate vectors for each decoded frame type for efficient access
    std::vector<GpsFrameData> gpsFrames;
    std::vector<AttitudeFrameData> attitudeFrames;
    std::vector<BatteryFrameData> batteryFrames;
    std::vector<FlightModeFrameData> flightModeFrames;
    std::vector<LinkRXFrameData> linkRXFrames;
    std::vector<UnknownFrameData> unknownFrames;
    
public:
    Flight() = default;
    ~Flight() = default;

    // Add a frame to the flight data
    void addFrame(const CrsfFrame& frame) {
        frames.push_back(frame);
        
        // Decode and store in appropriate vector for fast access
        auto decodedData = frame.decode();
        std::visit([this](const auto& data) {
            using T = std::decay_t<decltype(data)>;
            if constexpr (std::is_same_v<T, GpsFrameData>) {
                gpsFrames.push_back(data);
            } else if constexpr (std::is_same_v<T, AttitudeFrameData>) {
                attitudeFrames.push_back(data);
            } else if constexpr (std::is_same_v<T, BatteryFrameData>) {
                batteryFrames.push_back(data);
            } else if constexpr (std::is_same_v<T, FlightModeFrameData>) {
                flightModeFrames.push_back(data);
            } else if constexpr (std::is_same_v<T, LinkRXFrameData>) {
                linkRXFrames.push_back(data);
            } else if constexpr (std::is_same_v<T, UnknownFrameData>) {
                unknownFrames.push_back(data);
            }
        }, decodedData);
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
    
    // Direct access to decoded data vectors (fast - no decoding needed)
    const std::vector<GpsFrameData>& getGpsData() const {
        return gpsFrames;
    }
    
    const std::vector<AttitudeFrameData>& getAttitudeData() const {
        return attitudeFrames;
    }
    
    const std::vector<BatteryFrameData>& getBatteryData() const {
        return batteryFrames;
    }
    
    const std::vector<FlightModeFrameData>& getFlightModeData() const {
        return flightModeFrames;
    }
    
    const std::vector<LinkRXFrameData>& getLinkRXData() const {
        return linkRXFrames;
    }
    
    const std::vector<UnknownFrameData>& getUnknownData() const {
        return unknownFrames;
    }
    
    // Get counts for each frame type
    size_t getGpsCount() const { return gpsFrames.size(); }
    size_t getAttitudeCount() const { return attitudeFrames.size(); }
    size_t getBatteryCount() const { return batteryFrames.size(); }
    size_t getFlightModeCount() const { return flightModeFrames.size(); }
    size_t getLinkRXCount() const { return linkRXFrames.size(); }
    size_t getUnknownCount() const { return unknownFrames.size(); }
    size_t getTotalFrameCount() const { return frames.size(); }
    
    // Get latest data of each type (if available)
    const GpsFrameData* getLatestGps() const {
        return gpsFrames.empty() ? nullptr : &gpsFrames.back();
    }
    
    const AttitudeFrameData* getLatestAttitude() const {
        return attitudeFrames.empty() ? nullptr : &attitudeFrames.back();
    }
    
    const BatteryFrameData* getLatestBattery() const {
        return batteryFrames.empty() ? nullptr : &batteryFrames.back();
    }
    
    const FlightModeFrameData* getLatestFlightMode() const {
        return flightModeFrames.empty() ? nullptr : &flightModeFrames.back();
    }
    
    const LinkRXFrameData* getLatestLinkRX() const {
        return linkRXFrames.empty() ? nullptr : &linkRXFrames.back();
    }
};