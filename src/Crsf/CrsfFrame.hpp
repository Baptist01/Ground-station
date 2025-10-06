#pragma once
#include <vector>
#include <cstdint>
#include <chrono>
#include <string>

using byte = uint8_t;

// ELRS/CRSF Protocol definitions
#define CRSF_SYNC_BYTE 0xEA
#define CRSF_MAX_FRAME_SIZE 64

// CRSF Frame Types
#define CRSF_FRAMETYPE_GPS 0x02
#define CRSF_FRAMETYPE_ATTITUDE 0x1E
#define CRSF_FRAMETYPE_FLIGHT_MODE 0x21
#define CRSF_FRAMETYPE_DEVICE_PING 0x28
#define CRSF_FRAMETYPE_DEVICE_INFO 0x29
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKETS 0x16
#define CRSF_FRAMETYPE_BATTERY_STATUS 0x08
#define CRSF_FRAMETYPE_VARIOMETER 0x07
#define CRSF_FRAMETYPE_HEARTBEAT 0x0A
#define CRSF_FRAMETYPE_LINK_RX 0x14
#define CRSF_FRAMETYPE_LINK_TX 0x1C

#define CRSF_FRAMETYPE_RADIO_ID 0x3A

struct CrsfFrame
{
private:
    uint8_t addr;
    uint8_t len;
    uint8_t type;
    std::vector<uint8_t> payload;
    uint8_t crc;
    bool valid = true;
    std::chrono::system_clock::time_point timestamp;

public:
    CrsfFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~CrsfFrame();

    virtual 

    // Getters
    uint8_t getType() const { return type; }
};