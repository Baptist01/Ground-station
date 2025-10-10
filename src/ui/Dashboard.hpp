#pragma once
#include "../Crsf/CrsfFrameData.hpp"

namespace ui {
    void ShowApp();
    void ShowTopBar();
    void ShowDashboard();
    void ShowLogger();
    void ShowCameraFeed();
    void ShowAttitudeIndicator(const AttitudeFrameData& att);
}
