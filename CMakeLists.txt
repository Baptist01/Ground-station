#The root CMakeLists.txt file.
cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/bonderdonck/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

#The project name is set here. In this case it's Rocket, but you can change it to whatever you want.
project(groundStation  VERSION 1.0.0 LANGUAGES CXX)

#Require C++23
set(CXX_STANDARD_REQUIRED ON) #Make C++23 a hard requirement
set(CMAKE_CXX_STANDARD 23) # Default C++ standard for targets
# set(CMAKE_CXX_SCAN_FOR_MODULES ON)

find_package(Catch2 CONFIG REQUIRED)

add_executable(
    tests 
    tests/test.cpp
    src/ELRSMonitor.cpp 
    src/Crsf/CrsfFrame.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp 
    src/ELRSMonitor.cpp 
    src/Crsf/CrsfFrame.cpp
)

enable_testing()
add_test(NAME GroundStationTests COMMAND tests)