#include "Crsf/CrsfFrame.hpp"
#include "Flight.hpp"
#include <iostream>

// Example visitor class for processing different frame types
class FrameProcessor {
public:
    void operator()(const GpsFrameData& data) {
        std::cout << "Processing GPS: " << data.latitude << ", " << data.longitude << std::endl;
    }
    
    void operator()(const AttitudeFrameData& data) {
        std::cout << "Processing Attitude: R=" << data.roll << " P=" << data.pitch << " Y=" << data.yaw << std::endl;
    }
    
    void operator()(const BatteryFrameData& data) {
        std::cout << "Processing Battery: " << data.voltage << "V " << data.percentage << "%" << std::endl;
    }
    
    void operator()(const FlightModeFrameData& data) {
        std::cout << "Processing Flight Mode: " << data.mode << std::endl;
    }
    
    void operator()(const LinkRXFrameData& data) {
        std::cout << "Processing Link: RSSI=" << data.rssi << "dBm LQ=" << (int)data.lq << "% SNR=" << (int)data.noise << "dB" << std::endl;
    }
    
    void operator()(const UnknownFrameData& data) {
        std::cout << "Processing Unknown frame type: 0x" << std::hex << (int)data.frameType << std::dec << std::endl;
    }
};

// Example of how to use the new system
void processFlightData(const Flight& flight) {
    FrameProcessor processor;
    
    // Process all frames
    for (const auto& frame : flight.getFrames()) {
        auto decodedData = frame.decode();
        std::visit(processor, decodedData);
    }
    
    // Or get specific data types
    auto gpsData = flight.getGpsData();
    std::cout << "Found " << gpsData.size() << " GPS frames" << std::endl;
    
    auto attitudeData = flight.getAttitudeData();
    std::cout << "Found " << attitudeData.size() << " attitude frames" << std::endl;
}