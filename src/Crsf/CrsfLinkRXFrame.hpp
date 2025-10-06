#pragma once
#include <vector>
#include "CrsfFrame.hpp"

class CrsfLinkRXFrame : public CrsfFrame
{
private:
    uint8_t rssi;       // Received Signal Strength Indicator
    uint8_t lqi;        // Link Quality Indicator
    uint16_t noise;     // Noise level in dBm
    uint16_t txPower;   // Transmit power in dBm
    uint32_t frequency; // Frequency in Hz
public:
    CrsfLinkRXFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~CrsfLinkRXFrame();

    uint8_t getRssi() const { return rssi; }
    uint8_t getLqi() const { return lqi; }
    uint16_t getNoise() const { return noise; }
    uint16_t getTxPower() const { return txPower; }
    uint32_t getFrequency() const { return frequency; }

};