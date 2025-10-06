#include "CrsfBatteryFrame.hpp"

CrsfBatteryFrame::CrsfBatteryFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : CrsfFrame(addr, len, type, payload, crc)  // Initialize base class
{
    // Parse battery data from payload (assuming payload has at least 7 bytes for battery)
    if (payload.size() >= 7 && type == CRSF_FRAMETYPE_BATTERY_STATUS) {
        // Battery payload format: voltage(2) + current(2) + capacity(2) + percentage(1)
        voltage = (payload[0] << 8) | payload[1];
        current = (payload[2] << 8) | payload[3];
        capacity = (payload[4] << 8) | payload[5];
        percentage = payload[6];
    } else {
        // Invalid battery data - set defaults
        voltage = current = capacity = 0;
        percentage = 0;
    }
}

CrsfBatteryFrame::~CrsfBatteryFrame()
{
}