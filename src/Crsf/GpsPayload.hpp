#pragma once
#include "Payload.hpp"
#include <vector>

class GpsPayload : public Payload
{
private:
    int32_t latitude;   // degrees * 1e7
    int32_t longitude;  // degrees * 1e7
    int16_t altitude;   // meters + 1000m offset
    int16_t speed;      // km/h * 10
    uint16_t heading;   // degrees * 100
    uint8_t satellites; // amount of satellites
public:
    GpsPayload(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~GpsPayload();

    PayloadData decode() const override;
};