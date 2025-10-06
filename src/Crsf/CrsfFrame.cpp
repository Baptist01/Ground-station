#include "CrsfFrame.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

CrsfFrame::CrsfFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : addr(addr), len(len), type(type), payload(payload), crc(crc)
{
    timestamp = std::chrono::system_clock::now();
    if (len == payload.size() + 2)
    {
        valid = true;
    } else
    {
        valid = false;
    }
    
    
}

CrsfFrame::~CrsfFrame()
{
}

std::string CrsfFrame::getFrameTypeDescription() const {
    switch (type) {
        case CRSF_FRAMETYPE_RC_CHANNELS_PACKETS: return "RC Channels";
        case CRSF_FRAMETYPE_GPS: return "GPS Data";
        case CRSF_FRAMETYPE_BATTERY_STATUS: return "Battery Sensor";
        case CRSF_FRAMETYPE_VARIOMETER: return "Variometer";
        case CRSF_FRAMETYPE_ATTITUDE: return "Attitude";
        case CRSF_FRAMETYPE_DEVICE_INFO: return "Device Info";
        case CRSF_FRAMETYPE_FLIGHT_MODE: return "Flight Mode";
        case CRSF_FRAMETYPE_DEVICE_PING: return "Device Ping";
        case CRSF_FRAMETYPE_HEARTBEAT: return "Heartbeat";
        case CRSF_FRAMETYPE_LINK_RX: return "Link RX";
        case CRSF_FRAMETYPE_LINK_TX: return "Link TX";
        default: return "Unknown (0x" + std::to_string(type) + ")";
    }
}

void CrsfFrame::printCrsfFrame() const
{
    std::cout << "Frame:" << std::endl;
    std::cout << "  Length: " << static_cast<int>(len) << std::endl;
    std::cout << "  Type: " << std::hex << getFrameTypeDescription() << std::dec << " (" << static_cast<int>(type) << ")" << std::endl;
    std::cout << "  Payload: ";
    for (const auto& byte : payload) {
        std::cout << "[" << std::hex << static_cast<int>(byte) << "] ";
    }
    std::cout << std::dec << std::endl;
    std::cout << "  CRC: 0x" << std::hex << static_cast<int>(crc) << std::dec << " (" << static_cast<int>(crc) << ")" << std::endl;
    std::cout << "  Valid: " << (valid ? "Yes" : "No") << std::endl;
    
    std::cout << timestamp << std::endl;
}