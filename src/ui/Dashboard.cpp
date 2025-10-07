#include "Dashboard.hpp"
#include "imgui.h"
#include <SDL.h>

namespace ui {

float dashboardWidth = 0.0f;
constexpr float topBarHeight = 19.0f;

void ShowApp() {
    // Create a docking space for flexible layout
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    // ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, dockspace_flags);
    ImGui::PopStyleVar(3);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    ShowTopBar();
    ShowDashboard();
    ShowLogger();
}

void ShowTopBar() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 4.0f));
    ImGui::Begin("TopBar", nullptr, flags);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Flight")) { /* Show new flight overlay */ }
            if (ImGui::MenuItem("Open")) { /* handle open */ }
            if (ImGui::MenuItem("Exit")) { 
                SDL_Event event;
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Dashboard");
            ImGui::MenuItem("Settings");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void ShowDashboard() {
    // Dock dashboard to the right, force position/size every frame
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (dashboardWidth == 0.0f) dashboardWidth = viewport->Size.x * 0.28f;
    // Always force dashboard position and width, but allow user to resize horizontally
    ImGui::SetNextWindowPos(ImVec2(
        viewport->Pos.x + viewport->Size.x - dashboardWidth, 
        viewport->Pos.y + topBarHeight
    ), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(
        dashboardWidth, 
        viewport->Size.y - topBarHeight
    ), ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Dashboard", nullptr, flags);
    dashboardWidth = ImGui::GetWindowWidth();
    ImGui::Separator();
    // here should come GPS Data and other widgets
    // ImGui::Begin("GPS");
    // if (!flight.gps.empty()) {
    //     auto& g = flight.gps.back();
    //     ImGui::Text("Lat: %.6f", g.lat);
    //     ImGui::Text("Lon: %.6f", g.lon);
    //     ImGui::Text("Alt: %.2f m", g.alt);
    // }
    // ImGui::End();

    // ImGui::Begin("Attitude");
    // if (!flight.attitude.empty()) {
    //     auto& a = flight.attitude.back();
    //     ImGui::Text("Roll: %.2f", a.roll);
    //     ImGui::Text("Pitch: %.2f", a.pitch);
    //     ImGui::Text("Yaw: %.2f", a.yaw);
    // }
    // ImGui::End();

    // ImGui::Begin("Receiver Signal");
    // if (!flight.rx.empty()) {
    //     auto& r = flight.rx.back();
    //     ImGui::ProgressBar(r.rssi / 100.0f);
    //     ImGui::Text("RSSI: %d", r.rssi);
    // }
    // ImGui::End();

    ImGui::End();
}

void ShowLogger() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float loggerHeight = 180.0f;
    // Always force logger position and width, but allow user to resize vertically
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - loggerHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - dashboardWidth, loggerHeight), ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Logger", nullptr, flags);
    // Update loggerHeight if user resizes vertically
    loggerHeight = ImGui::GetWindowHeight();
    ImGui::Separator();
    static char logBuffer[2048] = "Logger initialized...\n";
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::InputTextMultiline("##log", logBuffer, sizeof(logBuffer), avail, ImGuiInputTextFlags_ReadOnly);
    ImGui::End();
}

}