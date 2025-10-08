#include "Dashboard.hpp"
#include "imgui.h"
#include <SDL.h>
#include "./ELRSMonitor.hpp"

namespace ui
{

    float dashboardWidth = 0.0f;
    constexpr float topBarHeight = 19.0f;

    void ShowApp()
    {
        // Create a docking space for flexible layout
        ImGuiViewport *viewport = ImGui::GetMainViewport();
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
        ShowCameraFeed();
    }

    void ShowTopBar()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 4.0f));
        ImGui::Begin("TopBar", nullptr, flags);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Flight"))
                { /* Show new flight overlay */
                }
                if (ImGui::MenuItem("Open"))
                { /* handle open */
                }
                if (ImGui::MenuItem("Exit"))
                {
                    SDL_Event event;
                    event.type = SDL_QUIT;
                    SDL_PushEvent(&event);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Dashboard");
                ImGui::MenuItem("Settings");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void ShowDashboard()
    {
        // Dock dashboard to the right, force position/size every frame
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        if (dashboardWidth == 0.0f)
            dashboardWidth = viewport->Size.x * 0.32f;
        // Always force dashboard position and width, but allow user to resize horizontally
        ImGui::SetNextWindowPos(ImVec2(
                                    viewport->Pos.x + viewport->Size.x - dashboardWidth,
                                    viewport->Pos.y + topBarHeight),
                                ImGuiCond_Always);

        ImGui::SetNextWindowSize(ImVec2(
                                     dashboardWidth,
                                     viewport->Size.y - topBarHeight),
                                 ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
        ImGui::Begin("Flight Dashboard", nullptr, flags);
        dashboardWidth = ImGui::GetWindowWidth();
        ImGui::Separator();

        static int state = 0;
        static ELRSMonitor monitor;
        static Flight flight;
        static bool triedConnect = false;
        static bool connectSuccess = false;
        switch (state)
        {
        case 0:
            ImGui::Text("No flight is currently \nbeing monitored.");
            ImGui::Spacing();
            if (ImGui::Button("Start Monitoring"))
            {
                connectSuccess = monitor.connectToPort();
                triedConnect = true;
                state = connectSuccess ? 1 : 2;
            }
            break;
        case 1:
        {
            monitor.monitorLoop(flight);
            if (!flight.getGpsData().empty())
            {
                auto &g = flight.getGpsData().back();
                ImGui::Text("Lat: %.6f", g.latitude);
                ImGui::Text("Lon: %.6f", g.longitude);
                ImGui::Text("Alt: %.2f m", g.altitude);
            }

            if (!flight.getAttitudeData().empty())
            {
                auto &a = flight.getAttitudeData().back();
                ImGui::Text("Roll: %.2f", a.roll);
                ImGui::Text("Pitch: %.2f", a.pitch);
                ImGui::Text("Yaw: %.2f", a.yaw);
            }

            if (!flight.getLinkRXData().empty())
            {
                auto &r = flight.getLinkRXData().back();
                ImGui::ProgressBar(r.rssi / 100.0f);
                ImGui::Text("RSSI: %d", r.rssi);
            }
            // Testdata voor live telemetry
            // ImGui::Separator();
            // ImGui::Text("GPS Data (Test):");
            // ImGui::Text("Lat: %.6f", 52.370216);
            // ImGui::Text("Lon: %.6f", 4.895168);
            // ImGui::Text("Alt: %.2f m", 15.2);

            // ImGui::Spacing();
            // ImGui::Text("Attitude Data (Test):");
            // ImGui::Text("Roll: %.2f", 2.5);
            // ImGui::Text("Pitch: %.2f", -1.3);
            // ImGui::Text("Yaw: %.2f", 180.0);

            // ImGui::Spacing();
            // ImGui::Text("Link RX Data (Test):");
            // float testRssi = 85.0f;
            // ImGui::ProgressBar(testRssi / 100.0f);
            // ImGui::Text("RSSI: %.0f", testRssi);
            break;
        }
        case 2:
            ImGui::Text("Failed to connect to port.");
            if (ImGui::Button("Retry"))
            {
                state = 0; // Go back to initial state to retry
                triedConnect = false;
            }
            break;
        default:
            break;
        }

        ImGui::End();
    }

    void ShowLogger()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
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

    void ShowCameraFeed()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        float left = viewport->Pos.x;
        float top = viewport->Pos.y + topBarHeight;
        float right = viewport->Pos.x + viewport->Size.x - dashboardWidth;
        float bottom = viewport->Pos.y + viewport->Size.y;

        float availWidth = right - left;
        float availHeight = bottom - top;
        float aspect = 16.0f / 9.0f;

        // Fit largest 16:9 rectangle in available area
        float videoWidth = availWidth;
        float videoHeight = videoWidth / aspect;
        if (videoHeight > availHeight)
        {
            videoHeight = availHeight;
            videoWidth = videoHeight * aspect;
        }

        // Align with topbar (no vertical centering)
        float videoX = left;
        float videoY = top;

        ImGui::SetNextWindowPos(ImVec2(videoX, videoY), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(videoWidth, videoHeight), ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
        ImGui::Begin("Camera Feed", nullptr, flags);
        // Placeholder for camera feed
        ImGui::Text("Camera feed would be displayed here.");
        ImGui::End();
    }

}