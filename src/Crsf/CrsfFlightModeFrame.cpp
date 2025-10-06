#include "CrsfFlightModeFrame.hpp"

CrsfFlightModeFrame::CrsfFlightModeFrame(uint8_t id, uint8_t length, uint8_t type, const std::vector<uint8_t>& value, uint8_t crc)
    : CrsfFrame(id, length, type, value, crc)  // Initialize base class
{
    // Parse flight mode string from payload (6 bytes max in Betaflight)
    if (value.size() >= 1 && type == CRSF_FRAMETYPE_FLIGHT_MODE) {
        // Extract string from payload (null-terminated or up to payload size)
        flightModeString.clear();
        for (size_t i = 0; i < value.size() && i < 6; i++) {
            if (value[i] == 0) break; // Stop at null terminator
            if (value[i] >= 32 && value[i] <= 126) { // Printable ASCII only
                flightModeString += static_cast<char>(value[i]);
            }
        }
        
        // If empty, set to "UNKNOWN"
        if (flightModeString.empty()) {
            flightModeString = "UNKNOWN";
        }
    } else {
        flightModeString = "INVALID";
    }
}

CrsfFlightModeFrame::~CrsfFlightModeFrame()
{
}