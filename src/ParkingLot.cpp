// ParkingLot.cpp – Main parking lot coordinator implementation
#include "ParkingLot.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace SPMS {

ParkingLot::ParkingLot(Logger& logger)
    : m_logger(logger)
    , m_sensor(logger)
    , m_monitorRunning(false)
{
    // Initialize slots: 12 CAR, 5 BIKE, 3 TRUCK (IDs 0-19)
    int id = 0;
    for (int i = 0; i < CAR_SLOTS;   ++i) m_slots.emplace_back(id++, SlotType::CAR);
    for (int i = 0; i < BIKE_SLOTS;  ++i) m_slots.emplace_back(id++, SlotType::BIKE);
    for (int i = 0; i < TRUCK_SLOTS; ++i) m_slots.emplace_back(id++, SlotType::TRUCK);

    m_logger.info("ParkingLot: Initialized – " +
                  std::to_string(CAR_SLOTS)   + " car, " +
                  std::to_string(BIKE_SLOTS)  + " bike, " +
                  std::to_string(TRUCK_SLOTS) + " truck slots");

    // Sensor callback
    m_sensor.setCallback([this](int slotId, bool occupied) {
        onSensorEvent(slotId, occupied);
    });

    // Try to open the kernel driver (optional – falls back to simulation)
    m_sensor.openDevice(DEVICE_PATH);

    // Open named pipe
    if (m_sensor.openPipe(PIPE_PATH))
        m_sensor.startPipeListener();
}

ParkingLot::~ParkingLot()
{
    shutdown();
}

void ParkingLot::shutdown()
{
    stopMonitor();
    m_logger.info("ParkingLot: Shutdown complete.");
}

// ── Vehicle Operations ────────────────────────────────────────────────────────

EntryResult ParkingLot::enterVehicle(const std::string& licensePlate, VehicleType type)
{
    EntryResult result{false, "", "", -1};

    // Check for duplicate
    if (m_ticketMgr.isParked(licensePlate)) {
        result.message = "Vehicle " + licensePlate + " is already parked!";
        m_logger.warn("Entry denied: " + result.message);
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    SlotType slotType = vehicleTypeToSlotType(type);
    int slotId = findAvailableSlot(slotType);

    if (slotId < 0) {
        result.message = "No available " + vehicleTypeToString(type) + " slots!";
        m_logger.warn("Entry denied: " + result.message);
        return result;
    }

    // Create vehicle and occupy slot
    auto vehicle = std::make_unique<Vehicle>(licensePlate, type);
    m_slots[slotId].occupy(std::move(vehicle));

    // Create ticket
    std::string ticketId = m_ticketMgr.createTicket(licensePlate, type, slotId);

    result.success  = true;
    result.ticketId = ticketId;
    result.slotId   = slotId;
    result.message  = "Vehicle parked at slot " + std::to_string(slotId);

    m_logger.info("ENTRY: " + licensePlate + " [" + vehicleTypeToString(type) +
                  "] -> Slot " + std::to_string(slotId) + " | Ticket: " + ticketId);

    // Notify kernel driver (if available)
    // ioctl(m_sensor.getDeviceFd(), PARKING_SET_OCCUPIED, &slotId); // uncomment after driver load

    return result;
}

ExitResult ParkingLot::exitVehicle(const std::string& ticketIdOrPlate)
{
    ExitResult result{false, "", 0.0};

    // Find ticket by ID or by license plate
    const Ticket* ticket = m_ticketMgr.getTicket(ticketIdOrPlate);
    if (!ticket)
        ticket = m_ticketMgr.getTicketByPlate(ticketIdOrPlate);

    if (!ticket) {
        result.message = "Ticket/plate not found: " + ticketIdOrPlate;
        m_logger.warn("Exit denied: " + result.message);
        return result;
    }
    if (ticket->status != TicketStatus::ACTIVE) {
        result.message = "Ticket " + ticket->ticketId + " is already closed.";
        m_logger.warn("Exit denied: " + result.message);
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    int    slotId   = ticket->slotId;
    std::string tid = ticket->ticketId;

    // Calculate duration and fee (use current time as exit time)
    long duration = static_cast<long>(std::difftime(std::time(nullptr), ticket->entryTime));
    double amount = m_billing.calculateFee(ticket->vehicleType, duration);

    // Close ticket
    m_ticketMgr.closeTicket(tid, amount);

    // Get updated ticket for receipt
    const Ticket* closedTicket = m_ticketMgr.getTicket(tid);
    if (closedTicket) {
        // Print receipt (billing system)
        Ticket copy = *closedTicket;
        copy.exitTime = std::time(nullptr);
        m_billing.processCheckout(copy);
    }

    // Vacate slot
    m_slots[slotId].vacate();

    result.success  = true;
    result.amountDue = amount;
    result.message  = "Exit successful. Amount due: Rs " + std::to_string((int)amount);

    m_logger.info("EXIT: " + (closedTicket ? closedTicket->licensePlate : "?") +
                  " | Slot " + std::to_string(slotId) +
                  " | Duration: " + std::to_string(duration/60) + " min" +
                  " | Amount: Rs " + std::to_string((int)amount));

    return result;
}

// ── Slot Lookup ───────────────────────────────────────────────────────────────

int ParkingLot::findAvailableSlot(SlotType type)
{
    for (auto& slot : m_slots) {
        if (slot.getType() == type && slot.isAvailable())
            return slot.getSlotId();
    }
    return -1;
}

// ── Sensor Callback ───────────────────────────────────────────────────────────

void ParkingLot::onSensorEvent(int slotId, bool occupied)
{
    if (slotId < 0 || slotId >= (int)m_slots.size()) return;
    m_logger.debug("Sensor event: slot " + std::to_string(slotId) +
                   " -> " + (occupied ? "OCCUPIED" : "FREE"));
    // In a real system, we'd reconcile sensor state with app state here
}

// ── Status Display ────────────────────────────────────────────────────────────

void ParkingLot::displayStatus() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    int totalAvail = 0, totalOcc = 0;
    int carAvail = 0, bikeAvail = 0, truckAvail = 0;

    for (const auto& slot : m_slots) {
        if (slot.isAvailable()) {
            ++totalAvail;
            if (slot.getType() == SlotType::CAR)   ++carAvail;
            if (slot.getType() == SlotType::BIKE)  ++bikeAvail;
            if (slot.getType() == SlotType::TRUCK) ++truckAvail;
        } else {
            ++totalOcc;
        }
    }

    std::cout << "\n  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║         PARKING LOT STATUS               ║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  Total Slots   : " << std::left << std::setw(24) << TOTAL_SLOTS      << "║\n";
    std::cout << "  ║  Occupied      : " << std::setw(24) << totalOcc                      << "║\n";
    std::cout << "  ║  Available     : " << std::setw(24) << totalAvail                    << "║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  CAR  slots    : " << std::setw(5) << CAR_SLOTS
              << "  (Available: " << std::setw(3) << carAvail << ")         ║\n";
    std::cout << "  ║  BIKE slots    : " << std::setw(5) << BIKE_SLOTS
              << "  (Available: " << std::setw(3) << bikeAvail << ")         ║\n";
    std::cout << "  ║  TRUCK slots   : " << std::setw(5) << TRUCK_SLOTS
              << "  (Available: " << std::setw(3) << truckAvail << ")         ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
}

void ParkingLot::displaySlotMap() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "\n  Slot Map:\n";
    std::cout << "  ID   TYPE   STATUS    PLATE\n";
    std::cout << "  " << std::string(50, '-') << "\n";
    for (const auto& slot : m_slots)
        slot.print();
    std::cout << "\n";
}

// ── Queries ───────────────────────────────────────────────────────────────────

int ParkingLot::getAvailableCount(VehicleType type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    SlotType st = vehicleTypeToSlotType(type);
    int count = 0;
    for (const auto& slot : m_slots)
        if (slot.getType() == st && slot.isAvailable()) ++count;
    return count;
}

int ParkingLot::getTotalAvailable() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& slot : m_slots)
        if (slot.isAvailable()) ++count;
    return count;
}

int ParkingLot::getTotalOccupied() const
{
    return TOTAL_SLOTS - getTotalAvailable();
}

bool ParkingLot::isFull() const
{
    return getTotalAvailable() == 0;
}

// ── Monitor Thread ────────────────────────────────────────────────────────────

void ParkingLot::startMonitor()
{
    m_monitorRunning = true;
    m_monitorThread  = std::thread(&ParkingLot::monitorThread, this);
    m_logger.info("ParkingLot: Monitor thread started (interval: " +
                  std::to_string(MONITOR_INTERVAL) + "s)");
}

void ParkingLot::stopMonitor()
{
    m_monitorRunning = false;
    if (m_monitorThread.joinable())
        m_monitorThread.join();
}

void ParkingLot::monitorThread()
{
    while (m_monitorRunning) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            int avail = 0;
            for (const auto& s : m_slots)
                if (s.isAvailable()) ++avail;

            m_logger.info("Monitor: " + std::to_string(avail) +
                          "/" + std::to_string(TOTAL_SLOTS) + " slots available");
        }

        // Sleep in small increments so shutdown is responsive
        for (int i = 0; i < MONITOR_INTERVAL * 10 && m_monitorRunning; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

}  // namespace SPMS
