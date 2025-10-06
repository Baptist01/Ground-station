#include "CrsfFrame.hpp"
#include <iostream>

CrsfFrame::CrsfFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc)
    : addr(addr), len(len), type(type), payload(payload), crc(crc), 
      timestamp(std::chrono::system_clock::now())
{
}

CrsfFrameData CrsfFrame::decode() const
{
    switch (type) {
        case CRSF_FRAMETYPE_GPS:
            return decodeGps();
        case CRSF_FRAMETYPE_ATTITUDE:
            return decodeAttitude();
        case CRSF_FRAMETYPE_BATTERY_STATUS:
            return decodeBattery();
        case CRSF_FRAMETYPE_FLIGHT_MODE:
            return decodeFlightMode();
        case CRSF_FRAMETYPE_RC_CHANNELS_PACKETS:
        case CRSF_FRAMETYPE_LINK_RX:
            return decodeLinkRX();
        default:
            return UnknownFrameData{type, payload};
    }
}

GpsFrameData CrsfFrame::decodeGps() const
{
    GpsFrameData data{};
    
    if (payload.size() >= 15) {
        int32_t lat = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
        int32_t lon = (payload[4] << 24) | (payload[5] << 16) | (payload[6] << 8) | payload[7];
        int16_t speed = (payload[8] << 8) | payload[9];
        int16_t heading = (payload[10] << 8) | payload[11];
        int16_t altitude = (payload[12] << 8) | payload[13];
        uint8_t satellites = payload[14];
        
        data.latitude = lat / 1e7;          // Convert to degrees
        data.longitude = lon / 1e7;         // Convert to degrees
        data.altitude = altitude - 1000.0f; // Convert to meters
        data.speed = speed / 3.6f;          // Convert to m/s
        data.heading = heading / 100;       // Convert to degrees
        data.satellites = satellites;
    }
    
    return data;
}

AttitudeFrameData CrsfFrame::decodeAttitude() const
{
    AttitudeFrameData data{};
    
    if (payload.size() >= 6) {
        int16_t roll = (payload[0] << 8) | payload[1];
        int16_t pitch = (payload[2] << 8) | payload[3];
        int16_t yaw = (payload[4] << 8) | payload[5];
        
        data.roll = roll / 10.0f;   // Convert to degrees
        data.pitch = pitch / 10.0f; // Convert to degrees
        data.yaw = yaw / 10.0f;     // Convert to degrees
    }
    
    return data;
}

BatteryFrameData CrsfFrame::decodeBattery() const
{
    BatteryFrameData data{};
    
    if (payload.size() >= 8) {
        uint16_t voltage = (payload[0] << 8) | payload[1];
        uint16_t current = (payload[2] << 8) | payload[3];
        uint32_t capacity = (payload[4] << 16) | (payload[5] << 8) | payload[6];
        uint8_t percentage = payload[7];
        
        data.voltage = voltage / 100.0f;    // Convert to volts
        data.current = current / 100.0f;    // Convert to amps
        data.capacity = capacity;           // mAh
        data.percentage = percentage;       // %
    }
    
    return data;
}

FlightModeFrameData CrsfFrame::decodeFlightMode() const
{
    FlightModeFrameData data{};
    
    // Extract string from payload (null-terminated or up to payload size)
    for (size_t i = 0; i < payload.size() && i < 16; i++) {
        if (payload[i] == 0) break; // Stop at null terminator
        if (payload[i] >= 32 && payload[i] <= 126) { // Printable ASCII only
            data.mode += static_cast<char>(payload[i]);
        }
    }
    
    // If empty, set to "UNKNOWN"
    if (data.mode.empty()) {
        data.mode = "UNKNOWN";
    }
    
    return data;
}

LinkRXFrameData CrsfFrame::decodeLinkRX() const
{
    LinkRXFrameData data{};
    
    if (payload.size() >= 10) {
        
        data.rssi = payload[0] - 130;  // Convert back to dBm (typically -30 to -100 dBm)
        data.lq = payload[2];          // Link quality percentage (0-100)
        data.noise = payload[3];       // SNR in dB
        data.txPower = payload[6];     // TX Power enum (0=0mW, 1=10mW, 2=25mW, 3=100mW, 4=500mW, 5=1000mW, 6=2000mW)
    }
    
    return data;
}