#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdint>
#include "./Crsf/CrsfFrame.hpp"
#include "./Crsf/CrsfFrameData.hpp"
#include "Flight.hpp"
#include "SharedFlight.hpp"
#include <variant>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    using SerialHandle = HANDLE;
    using BytesType = DWORD;
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <dirent.h>
    #include <sys/select.h>
    using SerialHandle = int;
    using BytesType = ssize_t;
    constexpr SerialHandle INVALID_SERIAL = -1;
#endif

class ELRSMonitor {
private:
    SerialHandle hSerial;
    bool isConnected;
#ifdef _WIN32
    DWORD currentBaudRate;
#else
    int currentBaudRate;
#endif
    int packetCount;
    int totalBytes;
    int heartbeat;
    
public:
    ELRSMonitor();
    ~ELRSMonitor();

    bool connectToPort();
    void listAvailablePorts();
    bool autoBaudDetection();
    bool configureTimeouts();
    void parseAndDisplayData(char* buffer, BytesType bytesRead, SharedFlight& sharedFlight);
    void monitorLoop(SharedFlight& sharedFlight);
};