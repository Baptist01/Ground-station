#pragma once
#include "CrsfFrameData.hpp"
#include <vector>
#include <cstdint>
#include <chrono>

// CRSF Frame Types (keeping your existing definitions)
#define CRSF_FRAMETYPE_GPS 0x02
#define CRSF_FRAMETYPE_ATTITUDE 0x1E
#define CRSF_FRAMETYPE_FLIGHT_MODE 0x21
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKETS 0x16
#define CRSF_FRAMETYPE_BATTERY_STATUS 0x08
#define CRSF_FRAMETYPE_LINK_RX 0x14

class CrsfFrame
{
private:
    uint8_t addr;
    uint8_t len;
    uint8_t type;
    std::vector<uint8_t> payload;
    uint8_t crc;
    std::chrono::system_clock::time_point timestamp;
    
public:
    CrsfFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~CrsfFrame() = default;

    CrsfFrameData decode() const;
    
    uint8_t getType() const { return type; }
    uint8_t getAddr() const { return addr; }
    uint8_t getLength() const { return len; }
    const std::vector<uint8_t>& getPayload() const { return payload; }
    uint8_t getCrc() const { return crc; }
    
private:
    GpsFrameData decodeGps() const;
    AttitudeFrameData decodeAttitude() const;
    BatteryFrameData decodeBattery() const;
    FlightModeFrameData decodeFlightMode() const;
    LinkRXFrameData decodeLinkRX() const;
};