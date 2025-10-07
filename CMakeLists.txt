# The root CMakeLists.txt file.
cmake_minimum_required(VERSION 3.28)

# Set vcpkg toolchain file for both Windows and Linux
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/bonderdonck/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
elseif(UNIX)
    set(CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# The project name
project(groundStation VERSION 1.0.0 LANGUAGES CXX)

# Require C++23
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

# Find dependencies
find_package(Catch2 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Main executable
add_executable(
    ${PROJECT_NAME} 
    src/main.cpp 
    src/ELRSMonitor.cpp 
    src/Crsf/CrsfFrame.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
)

# Link libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2 GLEW::GLEW)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2 GLEW::GLEW pthread)
endif()

# Tests (only build on non-cross-compile)
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(
        tests 
        tests/test.cpp
        src/ELRSMonitor.cpp 
        src/Crsf/CrsfFrame.cpp
    )
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain SDL2::SDL2main SDL2::SDL2 GLEW::GLEW pthread)

    enable_testing()
    add_test(NAME GroundStationTests COMMAND tests)
endif()