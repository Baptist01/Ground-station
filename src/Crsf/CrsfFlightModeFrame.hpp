#pragma once
#include <vector>
#include <string>
#include "CrsfFrame.hpp"

class CrsfFlightModeFrame : public CrsfFrame
{
private:
    std::string flightModeString;  // Flight mode as string (e.g., "ACRO", "ANGL", "HORI", etc.)
    
public:
    CrsfFlightModeFrame(uint8_t id, uint8_t length, uint8_t type, const std::vector<uint8_t>& value, uint8_t crc);
    ~CrsfFlightModeFrame();

    const std::string& getFlightMode() const { return flightModeString; }
    
    void printCrsfFrame() const;

    // getters
    std::string getMode() const {
        if (flightModeString.length() >= 4) {
            return flightModeString.substr(0, 4);
        }
        return flightModeString;
    }
};