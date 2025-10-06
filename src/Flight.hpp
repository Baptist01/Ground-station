#pragma once
#include <vector>
#include "Crsf/CrsfFrame.hpp"
#include "Crsf/CrsfGpsFrame.hpp"
#include "Crsf/CrsfAttitudeFrame.hpp"
#include "Crsf/CrsfBatteryFrame.hpp"
#include "Crsf/CrsfFlightModeFrame.hpp"
#include "Crsf/CrsfLinkRXFrame.hpp"

class Flight
{
private:
    std::vector<CrsfGpsFrame> gpsFrames;
    std::vector<CrsfAttitudeFrame> attitudeFrames;
    std::vector<CrsfBatteryFrame> batteryFrames;
    std::vector<CrsfFlightModeFrame> flightModeFrames;
    std::vector<CrsfLinkRXFrame> linkRXFrames;
    std::vector<CrsfFrame> otherFrames;
public:
    Flight();
    ~Flight();

    // setters
    void addGpsFrame(const CrsfGpsFrame& frame) { gpsFrames.push_back(frame); }
    void addAttitudeFrame(const CrsfAttitudeFrame& frame) { attitudeFrames.push_back(frame); }
    void addBatteryFrame(const CrsfBatteryFrame& frame) { batteryFrames.push_back(frame); }
    void addFlightModeFrame(const CrsfFlightModeFrame& frame) { flightModeFrames.push_back(frame); }
    void addLinkRXFrame(const CrsfLinkRXFrame& frame) { linkRXFrames.push_back(frame); }
    void addOtherFrame(const CrsfFrame& frame) { otherFrames.push_back(frame); }

    // getters
    const std::vector<CrsfGpsFrame>& getGpsFrames() const { return gpsFrames; }
    const std::vector<CrsfAttitudeFrame>& getAttitudeFrames() const { return attitudeFrames; }
    const std::vector<CrsfBatteryFrame>& getBatteryFrames() const { return batteryFrames; }
    const std::vector<CrsfFlightModeFrame>& getFlightModeFrames() const { return flightModeFrames; }
    const std::vector<CrsfLinkRXFrame>& getLinkRXFrames() const { return linkRXFrames; }
    const std::vector<CrsfFrame>& getOtherFrames() const { return otherFrames; }
};