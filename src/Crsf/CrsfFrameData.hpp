#pragma once
#include <variant>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>

struct GpsFrameData {
    double latitude;   // degrees
    double longitude;  // degrees
    float altitude;    // meters
    float speed;       // m/s
    uint16_t heading;  // degrees
    uint8_t satellites;// number of satellites
    std::chrono::system_clock::time_point timestamp; // time of the GPS fix
};

struct AttitudeFrameData {
    float pitch; // degrees
    float roll;  // degrees
    float yaw;   // degrees
    std::chrono::system_clock::time_point timestamp; // time of the attitude data
};

struct BatteryFrameData {
    float voltage;     // volts
    float current;     // amps
    float capacity;    // mAh
    float percentage;  // %
    std::chrono::system_clock::time_point timestamp; // time of the battery data
};

struct FlightModeFrameData {
    std::string mode; // e.g., "ACRO", "ANGL", "HORI", etc.
    std::chrono::system_clock::time_point timestamp; // time of the flight mode data
};

struct LinkRXFrameData {
    int16_t rssi;       // RSSI in dBm (negative values like -50 to -100)
    uint8_t lq;         // Link Quality percentage (0-100%)
    int8_t noise;       // SNR in dB
    uint8_t txPower;    // TX Power enum (0=0mW, 1=10mW, 2=25mW, 3=100mW, 4=500mW, 5=1000mW, 6=2000mW)
    std::chrono::system_clock::time_point timestamp; // time of the link data
};

struct UnknownFrameData {
    uint8_t frameType;
    std::vector<uint8_t> rawPayload;
    std::chrono::system_clock::time_point timestamp; // time when the unknown frame was received
};

using CrsfFrameData = std::variant<
    GpsFrameData,
    AttitudeFrameData, 
    BatteryFrameData,
    FlightModeFrameData,
    LinkRXFrameData,
    UnknownFrameData
>;