#pragma once
#include <vector>
#include <cstdint>
#include "CrsfFrame.hpp"


class CrsfBatteryFrame : public CrsfFrame
{
private:
    uint16_t voltage;      // in millivolts
    uint16_t current;      // in milliamps
    uint16_t capacity;     // in milliamp-hours
    uint8_t percentage;    // in percent
public:   
    CrsfBatteryFrame(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc);
    ~CrsfBatteryFrame();

    uint16_t getVoltage() const { return voltage; }
    uint16_t getCurrent() const { return current; }
    uint16_t getCapacity() const { return capacity; }
    uint8_t getPercentage() const { return percentage; }

    double getVoltageVolts() const { return voltage / 1000.0; }
    double getCurrentAmps() const { return current / 1000.0; }
    double getCapacityAh() const { return capacity / 1000.0; }
};