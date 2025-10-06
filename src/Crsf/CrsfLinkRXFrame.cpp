#include "CrsfLinkRXFrame.hpp"

CrsfLinkRXFrame::CrsfLinkRXFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : CrsfFrame(addr, len, type, payload, crc)  // Initialize base class
{
    if (payload.size() >= 11 && type == CRSF_FRAMETYPE_LINK_RX) {
        // Link RX payload format: rssi(1) + lqi(1) + noise(2) + txPower(2) + frequency(4)
        rssi = payload[0];
        lqi = payload[1];
        noise = (payload[2] << 8) | payload[3];
        txPower = (payload[4] << 8) | payload[5];
        frequency = (payload[6] << 24) | (payload[7] << 16) | (payload[8] << 8) | payload[9];
    } else {
        // Invalid Link RX data - set defaults
        rssi = lqi = 0;
        noise = txPower = 0;
        frequency = 0;
    }
}

CrsfLinkRXFrame::~CrsfLinkRXFrame()
{
}