#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <iomanip>
#include <conio.h>
#include <cstdint>
#include "./Crsf/CrsfFrame.hpp"
#include "./Crsf/CrsfFrameData.hpp"
#include "Flight.hpp"
#include <variant>

class ELRSMonitor {
private:
    HANDLE hSerial;
    bool isConnected;
    DWORD currentBaudRate;
    int packetCount;
    int totalBytes;
    int heartbeat;
    
public:
    ELRSMonitor();
    ~ELRSMonitor();
    
    bool connectToPort(const char* portName);
    void listAvailablePorts();
    bool autoBaudDetection();
    bool configureTimeouts();
    void parseAndDisplayData(char* buffer, DWORD bytesRead, Flight& flight);
    void monitorLoop(Flight& flight);
};