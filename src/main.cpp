// main.cpp – Smart Parking Management System
// Entry point: CLI menu loop
//
// Build:  cd build && cmake .. && make
// Run:    sudo ./spms          (sudo needed if kernel module loaded)
//         ./spms               (simulation mode, no driver needed)

#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>

#include "../include/common.h"
#include "Logger.h"
#include "ParkingLot.h"
#include "SignalHandler.h"

using namespace SPMS;

// ── Banner ───────────────────────────────────────────────────────────────────

void printBanner()
{
    std::cout << "\033[36m";
    std::cout << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║        SMART PARKING MANAGEMENT SYSTEM  v1.0            ║
  ║        Linux | Device Drivers | C++17                   ║
  ╚══════════════════════════════════════════════════════════╝
)" << "\033[0m\n";
}

// ── Menu ──────────────────────────────────────────────────────────────────────

void printMenu()
{
    std::cout << "\n  ┌─────────────────────────────────┐\n";
    std::cout << "  │         MAIN MENU               │\n";
    std::cout << "  ├─────────────────────────────────┤\n";
    std::cout << "  │  1. Vehicle Entry               │\n";
    std::cout << "  │  2. Vehicle Exit                │\n";
    std::cout << "  │  3. View Slot Status            │\n";
    std::cout << "  │  4. View Slot Map               │\n";
    std::cout << "  │  5. View Active Tickets         │\n";
    std::cout << "  │  6. View All Tickets (History)  │\n";
    std::cout << "  │  7. View Parking Rates          │\n";
    std::cout << "  │  8. Simulate Sensor Event       │\n";
    std::cout << "  │  9. Exit System                 │\n";
    std::cout << "  └─────────────────────────────────┘\n";
    std::cout << "  Enter choice: ";
}

// ── Input helpers ─────────────────────────────────────────────────────────────

std::string getInput(const std::string& prompt)
{
    std::cout << "  " << prompt;
    std::string line;
    std::getline(std::cin, line);
    // Trim whitespace
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    line.erase(line.find_last_not_of(" \t\r\n") + 1);
    // Uppercase
    std::transform(line.begin(), line.end(), line.begin(), ::toupper);
    return line;
}

VehicleType selectVehicleType()
{
    std::cout << "  Vehicle type:\n";
    std::cout << "    1. Car\n    2. Bike\n    3. Truck\n";
    std::cout << "  Choice: ";
    std::string choice;
    std::getline(std::cin, choice);
    if (choice == "2") return VehicleType::BIKE;
    if (choice == "3") return VehicleType::TRUCK;
    return VehicleType::CAR;
}

// ── Vehicle Entry handler ─────────────────────────────────────────────────────

void handleEntry(ParkingLot& lot)
{
    std::cout << "\n  --- Vehicle Entry ---\n";
    std::string plate = getInput("Enter license plate: ");
    if (plate.empty()) { std::cout << "  Invalid input.\n"; return; }

    VehicleType type = selectVehicleType();

    EntryResult res = lot.enterVehicle(plate, type);

    if (res.success) {
        std::cout << "\033[32m";
        std::cout << "\n  ✔ Entry successful!\n";
        std::cout << "    Ticket ID : " << res.ticketId << "\n";
        std::cout << "    Slot No.  : " << res.slotId << "\n";
        std::cout << "    Please keep your ticket safe.\n";
        std::cout << "\033[0m";
    } else {
        std::cout << "\033[31m  ✘ " << res.message << "\033[0m\n";
    }
}

// ── Vehicle Exit handler ──────────────────────────────────────────────────────

void handleExit(ParkingLot& lot)
{
    std::cout << "\n  --- Vehicle Exit ---\n";
    std::string input = getInput("Enter Ticket ID or License Plate: ");
    if (input.empty()) { std::cout << "  Invalid input.\n"; return; }

    ExitResult res = lot.exitVehicle(input);

    if (res.success) {
        std::cout << "\033[32m  ✔ " << res.message << "\033[0m\n";
    } else {
        std::cout << "\033[31m  ✘ " << res.message << "\033[0m\n";
    }
}

// ── Sensor Simulation ─────────────────────────────────────────────────────────

void handleSensorSimulation(ParkingLot& lot)
{
    std::cout << "\n  --- Simulate Sensor Event ---\n";
    std::cout << "  Enter slot ID (0-19): ";
    int slotId;
    if (!(std::cin >> slotId)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid slot ID.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "  Set to (1=Occupied, 0=Free): ";
    int state;
    if (!(std::cin >> state)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    lot.getSensorInterface().simulateSensorEvent(slotId, state == 1);
    std::cout << "  Sensor event sent: Slot " << slotId
              << " -> " << (state ? "OCCUPIED" : "FREE") << "\n";
}

// ── Main ───────────────────────────────────────────────────────────────────────

int main()
{
    printBanner();

    // Setup logger
    Logger logger(LOG_FILE, true);
    logger.info("SPMS Application started");

    // Setup parking lot
    ParkingLot parkingLot(logger);
    parkingLot.startMonitor();

    // Setup signal handler
    SignalHandler::getInstance().setup([&]() {
        std::cout << "\n\033[33m[SPMS] Shutdown signal received. Cleaning up...\033[0m\n";
        parkingLot.shutdown();
        logger.info("SPMS Application stopped by signal");
    });

    std::cout << "  Sensor: "
              << (parkingLot.getSensorInterface().isDeviceOpen()
                  ? "\033[32mKernel driver active (/dev/parking_sensor)\033[0m"
                  : "\033[33mSimulation mode (no kernel driver)\033[0m")
              << "\n";

    parkingLot.displayStatus();

    // ── Main menu loop ─────────────────────────────────────────────────────
    while (!SignalHandler::getInstance().isShuttingDown()) {
        printMenu();

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        if (choiceStr.empty()) continue;

        int choice = 0;
        try { choice = std::stoi(choiceStr); }
        catch (...) { std::cout << "  Invalid choice. Try again.\n"; continue; }

        if (SignalHandler::getInstance().isShuttingDown()) break;

        switch (choice) {
            case 1:  handleEntry(parkingLot);             break;
            case 2:  handleExit(parkingLot);              break;
            case 3:  parkingLot.displayStatus();          break;
            case 4:  parkingLot.displaySlotMap();         break;
            case 5:  parkingLot.printActiveTickets();     break;
            case 6:  parkingLot.printAllTickets();        break;
            case 7:  parkingLot.printRates();             break;
            case 8:  handleSensorSimulation(parkingLot);  break;
            case 9:
                std::cout << "\033[36m\n  Shutting down SPMS... Goodbye!\033[0m\n\n";
                parkingLot.shutdown();
                logger.info("SPMS Application stopped by user");
                return 0;
            default:
                std::cout << "  Invalid choice. Enter 1-9.\n";
        }
    }

    return 0;
}
