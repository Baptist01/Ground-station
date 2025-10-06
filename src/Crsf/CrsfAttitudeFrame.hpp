#pragma once
#include <vector>
#include <cstdint>
#include "CrsfFrame.hpp"

struct CrsfAttitudeFrame : public CrsfFrame
{
private:
    int16_t pitch;  // radians * 10000
    int16_t roll;   // radians * 10000  
    int16_t yaw;    // radians * 10000
public:
    CrsfAttitudeFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~CrsfAttitudeFrame();
    
    
};