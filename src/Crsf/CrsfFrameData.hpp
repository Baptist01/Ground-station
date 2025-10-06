#pragma once
#include <variant>
#include <string>
#include <cstdint>

// Forward declarations for frame data types
struct GpsFrameData {
    double latitude;   // degrees
    double longitude;  // degrees
    float altitude;    // meters
    float speed;       // m/s
    uint16_t heading;  // degrees
    uint8_t satellites;// number of satellites
};

struct AttitudeFrameData {
    float pitch; // degrees
    float roll;  // degrees
    float yaw;   // degrees
};

struct BatteryFrameData {
    float voltage;     // volts
    float current;     // amps
    float capacity;    // mAh
    float percentage;  // %
};

struct FlightModeFrameData {
    std::string mode; // e.g., "ACRO", "ANGL", "HORI", etc.
};

struct LinkRXFrameData {
    uint16_t rssi;      // dBm
    uint16_t lq;        // Link Quality %
    uint16_t noise;     // dBm
    uint16_t txPower;   // dBm
};

#include <vector>

struct UnknownFrameData {
    uint8_t frameType;
    std::vector<uint8_t> rawPayload;
};

// Variant type that can hold any frame data
using CrsfFrameData = std::variant<
    GpsFrameData,
    AttitudeFrameData, 
    BatteryFrameData,
    FlightModeFrameData,
    LinkRXFrameData,
    UnknownFrameData
>;