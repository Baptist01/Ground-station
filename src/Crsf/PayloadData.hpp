#pragma once
#include <variant>
#include <string>

struct GpsData {
    double latitude;   // degrees
    double longitude;  // degrees
    float altitude;    // meters
    float speed;       // m/s
    uint8_t satellites;// number of satellites
};

struct AttitudeData {
    float pitch; // degrees
    float roll;  // degrees
    float yaw;   // degrees
};

struct BatteryData {
    float voltage;     // volts
    float current;     // amps
    float capacity;    // mAh
    float percentage;  // %
};

struct FlightModeData {
    std::string mode; // e.g., "ACRO", "ANGL", "HORI", etc.
};

struct LinkRXData {
    uint16_t rssi;      // dBm
    uint16_t lq;        // Link Quality %
    uint16_t noise;     // dBm
    uint16_t txPower;   // dBm
};

using PayloadData = std::variant<GpsData, AttitudeData, BatteryData, FlightModeData, LinkRXData>;