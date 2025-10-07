#pragma once
#include <variant>
#include <string>
#include <cstdint>
#include <vector>

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
    int16_t rssi;       // RSSI in dBm (negative values like -50 to -100)
    uint8_t lq;         // Link Quality percentage (0-100%)
    int8_t noise;       // SNR in dB
    uint8_t txPower;    // TX Power enum (0=0mW, 1=10mW, 2=25mW, 3=100mW, 4=500mW, 5=1000mW, 6=2000mW)
};

struct UnknownFrameData {
    uint8_t frameType;
    std::vector<uint8_t> rawPayload;
};

using CrsfFrameData = std::variant<
    GpsFrameData,
    AttitudeFrameData, 
    BatteryFrameData,
    FlightModeFrameData,
    LinkRXFrameData,
    UnknownFrameData
>;