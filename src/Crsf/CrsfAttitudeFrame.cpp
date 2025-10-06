#include "CrsfAttitudeFrame.hpp"

CrsfAttitudeFrame::CrsfAttitudeFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : CrsfFrame(addr, len, type, payload, crc)
{
    if (payload.size() >= 6 && type == 0x07) {
        // Attitude payload format: roll(2) + pitch(2) + yaw(2)
        roll = static_cast<int16_t>((payload[0] << 8) | payload[1]) / 10.0f;   // in degrees
        pitch = static_cast<int16_t>((payload[2] << 8) | payload[3]) / 10.0f;  // in degrees
        yaw = static_cast<int16_t>((payload[4] << 8) | payload[5]) / 10.0f;    // in degrees
    } else {
        // Invalid attitude data - set defaults
        roll = pitch = yaw = 0.0f;
    }
}

CrsfAttitudeFrame::~CrsfAttitudeFrame()
{
}
