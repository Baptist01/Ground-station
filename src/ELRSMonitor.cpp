#include "ELRSMonitor.hpp"

#ifndef _WIN32
    #include <cstring>
    #include <errno.h>
    #include <linux/serial.h>
    #include <sys/ioctl.h>
#endif

ELRSMonitor::ELRSMonitor() : hSerial(INVALID_SERIAL), isConnected(false), 
                currentBaudRate(0), packetCount(0), totalBytes(0), heartbeat(0) {}

ELRSMonitor::~ELRSMonitor() {
    if (hSerial != INVALID_SERIAL) {
#ifdef _WIN32
        CloseHandle(hSerial);
#else
        close(hSerial);
#endif
    }
}

bool ELRSMonitor::connectToPort(const char* portName) {
    std::cout << "=== ELRS Ground Station Monitor ===" << std::endl;
    std::cout << "Connecting to ELRS receiver on " << portName << " for wireless telemetry..." << std::endl;
    
#ifdef _WIN32
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
#else
    // Linux serial port opening
    hSerial = open(portName, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    
    if (hSerial == -1) {
        int error = errno;
        std::cout << "ERROR: Cannot open " << portName << " (Error: " << error << " - " << strerror(error) << ")" << std::endl;
        
        switch (error) {
            case ENOENT:
                std::cout << "Port does not exist. Available ports:" << std::endl;
                listAvailablePorts();
                break;
            case EACCES:
                std::cout << "Permission denied. Try running with sudo or add user to dialout group:" << std::endl;
                std::cout << "sudo usermod -a -G dialout $USER" << std::endl;
                std::cout << "Then logout and login again." << std::endl;
                break;
            case EBUSY:
                std::cout << "Port is being used by another application." << std::endl;
                break;
            default:
                std::cout << "Unknown error accessing the port." << std::endl;
                break;
        }
        return false;
    }
#endif
    
    return autoBaudDetection();
}

void ELRSMonitor::listAvailablePorts() {
#ifdef _WIN32
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
#else
    std::cout << "Scanning for available serial ports..." << std::endl;
    
    // Check common Linux serial device paths
    const char* prefixes[] = {"/dev/ttyUSB", "/dev/ttyACM", "/dev/ttyS"};
    bool foundAny = false;
    
    for (const char* prefix : prefixes) {
        for (int i = 0; i < 10; i++) {
            std::string portName = std::string(prefix) + std::to_string(i);
            int testFd = open(portName.c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK);
            if (testFd != -1) {
                std::cout << "  " << portName << " - Available" << std::endl;
                close(testFd);
                foundAny = true;
            }
        }
    }
    
    if (!foundAny) {
        std::cout << "  No serial ports found. Common ELRS devices use:" << std::endl;
        std::cout << "    /dev/ttyUSB0 - USB-to-Serial adapters" << std::endl;
        std::cout << "    /dev/ttyACM0 - USB CDC devices" << std::endl;
    }
#endif
}

bool ELRSMonitor::autoBaudDetection() {
    std::vector<int> baudRates = {9600, 115200, 57600, 38400, 19200, 420000};
    
#ifdef _WIN32
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    for (int baudRate : baudRates) {
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
#else
    struct termios tty;
    
    for (int baudRate : baudRates) {
        if (tcgetattr(hSerial, &tty) == 0) {
            // Convert baud rate to Linux speed_t
            speed_t speed;
            switch(baudRate) {
                case 9600: speed = B9600; break;
                case 19200: speed = B19200; break;
                case 38400: speed = B38400; break;
                case 57600: speed = B57600; break;
                case 115200: speed = B115200; break;
                case 420000: speed = B460800; break; // Closest available
                default: continue;
            }
            
            // Configure terminal
            cfsetospeed(&tty, speed);
            cfsetispeed(&tty, speed);
            
            tty.c_cflag &= ~PARENB; // No parity
            tty.c_cflag &= ~CSTOPB; // 1 stop bit
            tty.c_cflag &= ~CSIZE;
            tty.c_cflag |= CS8;     // 8 data bits
            tty.c_cflag &= ~CRTSCTS; // No flow control
            tty.c_cflag |= CREAD | CLOCAL;
            
            tty.c_lflag &= ~ICANON; // Raw mode
            tty.c_lflag &= ~ECHO;
            tty.c_lflag &= ~ECHOE;
            tty.c_lflag &= ~ECHONL;
            tty.c_lflag &= ~ISIG;
            
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);
            tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
            
            tty.c_oflag &= ~OPOST;
            tty.c_oflag &= ~ONLCR;
            
            tty.c_cc[VTIME] = 2; // 0.2 second timeout
            tty.c_cc[VMIN] = 0;  // Non-blocking
            
            if (tcsetattr(hSerial, TCSANOW, &tty) == 0) {
                std::cout << "Testing " << baudRate << " baud..." << std::endl;
                
                // Test for data at this baud rate
                usleep(200000); // 200ms
                
                int bytesAvailable;
                if (ioctl(hSerial, FIONREAD, &bytesAvailable) == 0 && bytesAvailable > 0) {
                    std::cout << "*** Data detected at " << baudRate << " baud! ***" << std::endl;
                    currentBaudRate = baudRate;
                    isConnected = true;
                    return configureTimeouts();
                }
            }
        }
        usleep(100000); // 100ms
    }
#endif
    
    // Default to 9600 if no data detected
    std::cout << "No data detected during auto-detection, using 9600 baud" << std::endl;
    
#ifdef _WIN32
    dcbSerialParams.BaudRate = 9600;
    SetCommState(hSerial, &dcbSerialParams);
#else
    if (tcgetattr(hSerial, &tty) == 0) {
        cfsetospeed(&tty, B9600);
        cfsetispeed(&tty, B9600);
        tcsetattr(hSerial, TCSANOW, &tty);
    }
#endif
    
    currentBaudRate = 9600;
    isConnected = true;
    
    return configureTimeouts();
}

bool ELRSMonitor::configureTimeouts() {
#ifdef _WIN32
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    return SetCommTimeouts(hSerial, &timeouts);
#else
    // Linux timeouts are already configured in termios structure
    return true;
#endif
}

void ELRSMonitor::parseAndDisplayData(char* buffer, BytesType bytesRead, Flight& flight) {
    packetCount++;
    totalBytes += bytesRead;
    
    for (BytesType i = 0; i < bytesRead; i++) 
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
    BytesType bytesRead;
    
    std::cout << "Monitoring started. Press 'q' and Enter to quit..." << std::endl;
    
    while (true) {
#ifdef _WIN32
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
        
        Sleep(1);
#else
        // Linux: Use select for non-blocking read and stdin check
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(hSerial, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000; // 1ms timeout
        
        int result = select(std::max(hSerial, STDIN_FILENO) + 1, &readfds, NULL, NULL, &timeout);
        
        if (result > 0) {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char key;
                if (read(STDIN_FILENO, &key, 1) > 0 && (key == 'q' || key == 'Q')) {
                    std::cout << "\nExiting ELRS monitor..." << std::endl;
                    break;
                }
            }
            
            if (FD_ISSET(hSerial, &readfds)) {
                bytesRead = read(hSerial, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    parseAndDisplayData(buffer, bytesRead, flight);
                    heartbeat = 0;
                } else if (bytesRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cout << "Read error: " << strerror(errno) << std::endl;
                    break;
                }
            }
        }
        
        usleep(1000); // 1ms
#endif
        
        // Show heartbeat when no data
        heartbeat++;
        if (heartbeat >= 1000) {
            std::cout << "." << std::flush;
            heartbeat = 0;
        }
    }
}