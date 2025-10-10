#include "Dashboard.hpp"
#include "imgui.h"
#include <SDL.h>
#include "./ELRSMonitor.hpp"
#include "./SharedFlight.hpp"
#include "./Flight.hpp"
#include <thread>
#include "./implot.h"
#include <chrono>
#ifdef _WIN32
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
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
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        ImGui::Begin("Flight Dashboard ", nullptr, flags);
        dashboardWidth = ImGui::GetWindowWidth();
        ImGui::Separator();
        ImGui::Text(std::ctime(&now_time));

        static int state = 0;
        static ELRSMonitor monitor;
        static Flight flight;
        static bool triedConnect = false;
        static bool connectSuccess = false;

        static SharedFlight sharedFlight;
        SharedFlight &sharedFlightRef = sharedFlight;
        static std::thread monitorThread;
        static bool monitorThreadStarted = false;

        switch (state)
        {
        case 0:
            ImGui::Text("No flight is currently \nbeing monitored.");
            ImGui::Spacing();
            if (ImGui::Button("Start Monitoring"))
            {
                connectSuccess = monitor.connectToPort();
                printf("connectSuccess: %s\n", connectSuccess ? "true" : "false");
                triedConnect = true;
                if (connectSuccess && !monitorThreadStarted)
                {
                    state = 1;
                    monitorThreadStarted = true;
                    monitorThread = std::thread([&]()
                                                { monitor.monitorLoop(sharedFlightRef); });
                    monitorThread.detach();
                }
                else if (!connectSuccess)
                {
                    state = 2;
                }
            }
            break;
        case 1:
        {
            ImGui::Text("Monitoring flight data...");
            static Flight localCopy;
            {
                std::lock_guard<std::mutex> lock(sharedFlightRef.mutex);
                if (sharedFlightRef.updated)
                {
                    localCopy = sharedFlightRef.snapshot();
                    sharedFlightRef.updated = false;
                }
            }
            if (!localCopy.getGpsData().empty())
            {
                if (ImGui::CollapsingHeader("GPS Data", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto &gpsFrames = localCopy.getGpsData();

                    std::vector<double> latitudes, longitudes, times, headings, altitudes, speeds;
                    std::vector<int> satellites;

                    latitudes.reserve(gpsFrames.size());
                    longitudes.reserve(gpsFrames.size());
                    headings.reserve(gpsFrames.size());
                    altitudes.reserve(gpsFrames.size());
                    speeds.reserve(gpsFrames.size());
                    satellites.reserve(gpsFrames.size());
                    times.reserve(gpsFrames.size());

                    for (const auto &g : gpsFrames)
                    {
                        latitudes.push_back(g.latitude);
                        longitudes.push_back(g.longitude);
                        headings.push_back(g.heading);
                        altitudes.push_back(g.altitude);
                        speeds.push_back(g.speed);
                        satellites.push_back(g.satellites);

                        auto t = std::chrono::system_clock::to_time_t(g.timestamp);
                        times.push_back(static_cast<double>(t));
                    }

                    if (ImPlot::BeginPlot("Drone Trajectory"))
                    {
                        ImPlot::SetupAxes("Longitude", "Latitude");
                        ImPlot::PlotLine("Path", longitudes.data(), latitudes.data(), latitudes.size());
                        if (!latitudes.empty())
                        {
                            ImPlot::PlotScatter("Current Position", &longitudes.back(), &latitudes.back(), 1);
                        }
                        ImPlot::EndPlot();
                    }

                    if (!gpsFrames.empty())
                    {
                        const auto &g = gpsFrames.back();
                        ImGui::Text("Lat: %.6f", g.latitude);
                        ImGui::Text("Lon: %.6f", g.longitude);
                        ImGui::Text("Alt: %.2f m", g.altitude);
                        ImGui::Text("Speed: %.2f m/s", g.speed);
                        ImGui::Text("Heading: %d°", g.heading);
                    }

                    if (ImPlot::BeginPlot("Altitude"))
                    {
                        ImPlot::SetupAxes("Time (s)", "Altitude (m)");
                        std::vector<double> relativeTimes = times;
                        double t0 = relativeTimes.front();
                        for (auto &t : relativeTimes)
                            t -= t0;
                        ImPlot::PlotLine("Altitude", relativeTimes.data(), altitudes.data(), altitudes.size());
                        ImPlot::EndPlot();
                    }

                    if (ImPlot::BeginPlot("Speed"))
                    {
                        ImPlot::SetupAxes("Time (s)", "Speed (m/s)");
                        std::vector<double> relativeTimes = times;
                        double t0 = relativeTimes.front();
                        for (auto &t : relativeTimes)
                            t -= t0;
                        ImPlot::PlotLine("Speed", relativeTimes.data(), speeds.data(), speeds.size());
                        ImPlot::EndPlot();
                    }

                    if (ImPlot::BeginPlot("Heading"))
                    {
                        ImPlot::SetupAxes("Time (s)", "Heading (°)");
                        std::vector<double> relativeTimes = times;
                        double t0 = relativeTimes.front();
                        for (auto &t : relativeTimes)
                            t -= t0;
                        ImPlot::PlotLine("Heading", relativeTimes.data(), headings.data(), headings.size());
                        ImPlot::EndPlot();
                    }
                }
            }

            if (!localCopy.getAttitudeData().empty())
            {
                const auto &attitudeFrames = localCopy.getAttitudeData();
                if (!attitudeFrames.empty())
                {
                    ShowAttitudeIndicator(attitudeFrames.back());
                }

                // std::vector<float> pitches, rolls, yaws, times;
                // pitches.reserve(attitudeFrames.size());
                // rolls.reserve(attitudeFrames.size());
                // yaws.reserve(attitudeFrames.size());
                // times.reserve(attitudeFrames.size());
            }

            if (!localCopy.getLinkRXData().empty())
            {
                auto &r = localCopy.getLinkRXData().back();
                ImGui::ProgressBar(r.rssi / 100.0f);
                ImGui::Text("RSSI: %d", r.rssi);
            }
            break;
        }
        case 2:
            ImGui::Text("Failed to connect to port.");
            if (ImGui::Button("Reset"))
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
        float loggerHeight = 328.0f;
        // Always force logger position and width, but allow user to resize vertically
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - loggerHeight), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - dashboardWidth, loggerHeight), ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
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
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
        ImGui::Begin("Camera Feed", nullptr, flags);
        // Placeholder for camera feed
        ImGui::Text("Camera feed would be displayed here.");
        ImGui::End();
    }

    void ShowAttitudeIndicator(const AttitudeFrameData &att)
    {
        ImGui::Begin("Attitude");

        ImVec2 size = ImGui::GetContentRegionAvail();
        if (size.x < 1.0f)
            size.x = 300;
        if (size.y < 1.0f)
            size.y = 300;

        // Save current viewport
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);

        // Set viewport for this window
        glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = size.x / size.y;
        gluPerspective(45.0f, aspect, 0.1f, 100.0f);

        // Setup modelview
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(3, 3, 5,  // camera position
                  0, 0, 0,  // look at origin
                  0, 1, 0); // up vector

        // Apply rotations: roll, pitch, yaw
        glRotatef(att.roll, 0, 0, 1);  // roll around Z
        glRotatef(att.pitch, 1, 0, 0); // pitch around X
        glRotatef(att.yaw, 0, 1, 0);   // yaw around Y

        // Draw cube (drone body)
        float cubeSize = 0.5f;
        glBegin(GL_QUADS);

        // Top face
        glColor3f(0.2f, 0.5f, 1.0f);
        glVertex3f(-cubeSize, cubeSize, -cubeSize);
        glVertex3f(cubeSize, cubeSize, -cubeSize);
        glVertex3f(cubeSize, cubeSize, cubeSize);
        glVertex3f(-cubeSize, cubeSize, cubeSize);

        // Bottom face
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex3f(-cubeSize, -cubeSize, -cubeSize);
        glVertex3f(cubeSize, -cubeSize, -cubeSize);
        glVertex3f(cubeSize, -cubeSize, cubeSize);
        glVertex3f(-cubeSize, -cubeSize, cubeSize);

        // Front face
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-cubeSize, -cubeSize, cubeSize);
        glVertex3f(cubeSize, -cubeSize, cubeSize);
        glVertex3f(cubeSize, cubeSize, cubeSize);
        glVertex3f(-cubeSize, cubeSize, cubeSize);

        // Back face
        glVertex3f(-cubeSize, -cubeSize, -cubeSize);
        glVertex3f(cubeSize, -cubeSize, -cubeSize);
        glVertex3f(cubeSize, cubeSize, -cubeSize);
        glVertex3f(-cubeSize, cubeSize, -cubeSize);

        // Left face
        glVertex3f(-cubeSize, -cubeSize, -cubeSize);
        glVertex3f(-cubeSize, -cubeSize, cubeSize);
        glVertex3f(-cubeSize, cubeSize, cubeSize);
        glVertex3f(-cubeSize, cubeSize, -cubeSize);

        // Right face
        glVertex3f(cubeSize, -cubeSize, -cubeSize);
        glVertex3f(cubeSize, -cubeSize, cubeSize);
        glVertex3f(cubeSize, cubeSize, cubeSize);
        glVertex3f(cubeSize, cubeSize, -cubeSize);

        glEnd();

        // Draw axes
        float axisLen = 1.5f;

        // X-axis = Red
        glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(axisLen, 0, 0);
        glEnd();

        // Y-axis = Green
        glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, axisLen, 0);
        glEnd();

        // Z-axis = Blue
        glBegin(GL_LINES);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, axisLen);
        glEnd();

        // Draw forward arrow (yellow)
        glBegin(GL_LINES);
        glColor3f(1, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2.0f);
        glEnd();

        glDisable(GL_DEPTH_TEST);

        // Restore previous viewport
        glViewport(vp[0], vp[1], vp[2], vp[3]);

        ImGui::End();
    }

}