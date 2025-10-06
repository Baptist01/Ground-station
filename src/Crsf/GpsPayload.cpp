#include "GpsPayload.hpp"

GpsPayload::GpsPayload(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : Payload(addr, len, type, payload, crc)  // Initialize base class
{
    if (payload.size() >= 15) {
        latitude = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
        longitude = (payload[4] << 24) | (payload[5] << 16) | (payload[6] << 8) | payload[7];
        speed = (payload[8] << 8) | payload[9];
        heading = (payload[10] << 8) | payload[11];
        altitude = (payload[12] << 8) | payload[13];
        satellites = payload[14];
    } else {
        // Invalid GPS data - set defaults
        latitude = longitude = 0;
        speed = altitude = 0;
        heading = 0;
        satellites = 0;
    }
}

GpsPayload::~GpsPayload()
{
}

PayloadData GpsPayload::decode() const
{
    GpsData data;
    data.latitude = latitude / 1e7;          // Convert to degrees
    data.longitude = longitude / 1e7;        // Convert to degrees
    data.altitude = altitude - 1000.0f;      // Convert to meters
    data.speed = speed / 3.6f;               // Convert to m/s
    data.satellites = satellites;            // Number of satellites
    return data;
}