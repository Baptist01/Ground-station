#pragma once
#include "Payload.hpp"
#include "GpsPayload.hpp"
#include <memory>

class PayloadFactory
{
    public:
        static std::unique_ptr<Payload> createPayload(uint8_t addr, uint8_t len, uint8_t type, const std::vector<uint8_t>& payload, uint8_t crc) {
            switch (type) {
                case CRSF_FRAMETYPE_GPS:
                    return std::make_unique<GpsPayload>(addr, len, type, payload, crc);
                default:
                    return nullptr; // Unknown or unsupported frame type
            }
        }
