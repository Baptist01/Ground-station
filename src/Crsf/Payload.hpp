#pragma once
#include "PayloadData.hpp"

class Payload
{
public:
    virtual ~Payload() = default;
    virtual PayloadData decode() const = 0;
};

Payload::~Payload()
{
}
