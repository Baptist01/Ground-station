#include "ELRSMonitor.hpp"

ELRSMonitor::ELRSMonitor() : hSerial(INVALID_HANDLE_VALUE), isConnected(false), 
                currentBaudRate(0), packetCount(0), totalBytes(0), heartbeat(0) {}

ELRSMonitor::~ELRSMonitor() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
}

bool ELRSMonitor::connectToPort(const char* portName) {
    std::cout << "=== ELRS Ground Station Monitor ===" << std::endl;
    std::cout << "Connecting to ELRS receiver on " << portName << " for wireless telemetry..." << std::endl;
    
    // Open port READ-ONLY to not interfere with controller
    hSerial = CreateFileA(
        portName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cout << "ERROR: Cannot open " << portName << " (Error: " << error << ")" << std::endl;
        
        switch (error) {
            case ERROR_FILE_NOT_FOUND:
                std::cout << "Port does not exist. Available ports:" << std::endl;
                listAvailablePorts();
                break;
            case ERROR_ACCESS_DENIED:
                std::cout << "Port is being used by another application." << std::endl;
                std::cout << "Close ELRS Configurator or other serial software first." << std::endl;
                break;
            default:
                std::cout << "Unknown error accessing the port." << std::endl;
                break;
        }
        return false;
    }
    
    return autoBaudDetection();
}

void ELRSMonitor::listAvailablePorts() {
    std::cout << "Scanning for available COM ports..." << std::endl;
    for (int i = 1; i <= 20; i++) {
        std::string portName = "COM" + std::to_string(i);
        HANDLE testHandle = CreateFileA(
            portName.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        
        if (testHandle != INVALID_HANDLE_VALUE) {
            std::cout << "  " << portName << " - Available" << std::endl;
            CloseHandle(testHandle);
        }
    }
}

bool ELRSMonitor::autoBaudDetection() {
    std::vector<DWORD> baudRates = {9600, 115200, 57600, 38400, 19200, 420000};
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    for (DWORD baudRate : baudRates) {
        if (GetCommState(hSerial, &dcbSerialParams)) {
            dcbSerialParams.BaudRate = baudRate;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
            dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
            
            if (SetCommState(hSerial, &dcbSerialParams)) {
                std::cout << "Testing " << baudRate << " baud..." << std::endl;
                
                // Test for data at this baud rate
                Sleep(200);
                COMSTAT comStat;
                DWORD errors;
                if (ClearCommError(hSerial, &errors, &comStat)) {
                    if (comStat.cbInQue > 0) {
                        std::cout << "*** Data detected at " << baudRate << " baud! ***" << std::endl;
                        currentBaudRate = baudRate;
                        isConnected = true;
                        return configureTimeouts();
                    }
                }
            }
        }
        Sleep(100);
    }
    
    // Default to 9600 if no data detected
    std::cout << "No data detected during auto-detection, using 9600 baud" << std::endl;
    dcbSerialParams.BaudRate = 9600;
    SetCommState(hSerial, &dcbSerialParams);
    currentBaudRate = 9600;
    isConnected = true;
    
    return configureTimeouts();
}

bool ELRSMonitor::configureTimeouts() {
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    return SetCommTimeouts(hSerial, &timeouts);
}

void ELRSMonitor::parseAndDisplayData(char* buffer, DWORD bytesRead, Flight& flight) {
    packetCount++;
    totalBytes += bytesRead;
    
    for (DWORD i = 0; i < bytesRead; i++) 
    {
        if ((unsigned char)buffer[i] == 0xEA) 
        {
            uint8_t frameLength = (unsigned char)buffer[i + 1];
            uint8_t frameType = (unsigned char)buffer[i + 2];

            std::vector<uint8_t> frameValue {};
            bool validFrame = true;
            for (int j = 0; j < frameLength - 2; j++)
            {
                if ((unsigned char)buffer[i + 3 + j] == 0xEA) {
                    i += j;
                    validFrame = false;
                    break;
                }
                frameValue.push_back((uint8_t)(unsigned char)buffer[i + 3 + j]);
            }
            if (!validFrame) 
            { 
                continue; 
            }
            uint8_t crc = (unsigned char)buffer[i + 3 + frameLength];

            // Create frame and decode it using variant approach
            CrsfFrame frame(buffer[i], frameLength, frameType, frameValue, crc);
            flight.addFrame(frame);
            
            // Decode and display the frame data
            auto decodedData = frame.decode();
            
            std::visit([frameType](const auto& data) {
                using T = std::decay_t<decltype(data)>;
                if constexpr (std::is_same_v<T, GpsFrameData>) {
                    std::cout << "GPS Frame - Lat: " << data.latitude << ", Lon: " << data.longitude 
                              << ", Alt: " << data.altitude << "m, Sats: " << (int)data.satellites << std::endl;
                } else if constexpr (std::is_same_v<T, AttitudeFrameData>) {
                    std::cout << "Attitude Frame - Roll: " << data.roll << ", Pitch: " << data.pitch 
                              << ", Yaw: " << data.yaw << std::endl;
                } else if constexpr (std::is_same_v<T, BatteryFrameData>) {
                    std::cout << "Battery Frame - Voltage: " << data.voltage << "V, Current: " << data.current 
                              << "A, " << data.percentage << "%" << std::endl;
                } else if constexpr (std::is_same_v<T, FlightModeFrameData>) {
                    std::cout << "Flight Mode Frame - Mode: " << data.mode << std::endl;
                } else if constexpr (std::is_same_v<T, LinkRXFrameData>) {
                    const char* powerLevels[] = {"0mW", "10mW", "25mW", "100mW", "500mW", "1000mW", "2000mW"};
                    const char* powerStr = (data.txPower < 7) ? powerLevels[data.txPower] : "Unknown";
                    std::cout << "Link RX Frame - RSSI: " << data.rssi << "dBm, LQ: " << (int)data.lq 
                              << "%, SNR: " << (int)data.noise << "dB, TX Power: " << powerStr << std::endl;
                } else if constexpr (std::is_same_v<T, UnknownFrameData>) {
                    std::cout << "Unknown Frame - Type: 0x" << std::hex << (int)data.frameType << std::dec << std::endl;
                }
            }, decodedData);
            
            if (validFrame) { i += frameLength + 1; }
        }
    }
}

void ELRSMonitor::monitorLoop(Flight& flight) {
    if (!isConnected) {
        std::cout << "Not connected to any port!" << std::endl;
        return;
    }
    
    char buffer[512];
    DWORD bytesRead;
    
    while (true) {
        // Check for user input (quit command)
        if (_kbhit()) {
            char key = _getch();
            if (key == 'q' || key == 'Q') {
                std::cout << "\nExiting ELRS monitor..." << std::endl;
                break;
            }
        }
        
        if (ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL)) {
            if (bytesRead > 0) {
                parseAndDisplayData(buffer, bytesRead, flight);
                heartbeat = 0;
            }
        } else {
            DWORD error = GetLastError();
            if (error != ERROR_TIMEOUT) {
                std::cout << "Read error: " << error << std::endl;
                break;
            }
        }
        
        // Show heartbeat when no data
        heartbeat++;
        if (heartbeat >= 1000) {
            std::cout << "." << std::flush;
            heartbeat = 0;
        }
        
        Sleep(1);
    }
}