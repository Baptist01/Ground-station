#include <iostream>
#include <conio.h>
#include "ELRSMonitor.hpp"
#include "Flight.hpp"

int main() {
    ELRSMonitor monitor;
    
    // Try to connect to COM6
    if (!monitor.connectToPort("COM6")) {
        std::cout << "\nPress any key to exit..." << std::endl;
        _getch();
        return 1;
    }

    std::cout << "Press any key to start the flight..." << std::endl;
    _getch();

    std::cout << "Starting flight..." << std::endl;
    Flight flight;
    Flight& flightRef = flight;
    
    // Start monitoring
    monitor.monitorLoop(flightRef);
    
    std::cout << "ELRS Monitor finished." << std::endl;
    std::cout << "Press any key to exit..." << std::endl;
    _getch();
    return 0;
}