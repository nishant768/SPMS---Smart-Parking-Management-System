// ParkingLot.h – Main coordinator for all parking operations
#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

#include "../include/common.h"
#include "ParkingSlot.h"
#include "Vehicle.h"
#include "TicketManager.h"
#include "BillingSystem.h"
#include "SensorInterface.h"
#include "Logger.h"

namespace SPMS {

struct EntryResult {
    bool        success;
    std::string message;
    std::string ticketId;
    int         slotId;
};

struct ExitResult {
    bool        success;
    std::string message;
    double      amountDue;
};

class ParkingLot {
public:
    explicit ParkingLot(Logger& logger);
    ~ParkingLot();

    // Vehicle operations
    EntryResult enterVehicle(const std::string& licensePlate, VehicleType type);
    ExitResult  exitVehicle(const std::string& ticketIdOrPlate);

    // Status display
    void displayStatus()   const;
    void displaySlotMap()  const;

    // Queries
    int  getAvailableCount(VehicleType type) const;
    int  getTotalAvailable()                 const;
    int  getTotalOccupied()                  const;
    bool isFull()                            const;

    // Ticket operations (delegates)
    void printActiveTickets() const { m_ticketMgr.printActiveTickets(); }
    void printAllTickets()    const { m_ticketMgr.printAllTickets(); }

    // Rates display (delegates)
    void printRates() const { m_billing.printRates(); }

    // Monitoring thread
    void startMonitor();
    void stopMonitor();

    // Sensor interface access
    SensorInterface& getSensorInterface() { return m_sensor; }

    // Graceful shutdown
    void shutdown();

private:
    int findAvailableSlot(SlotType type);
    void monitorThread();
    void onSensorEvent(int slotId, bool occupied);

    Logger&                          m_logger;
    std::vector<ParkingSlot>         m_slots;
    TicketManager                    m_ticketMgr;
    BillingSystem                    m_billing;
    SensorInterface                  m_sensor;

    mutable std::mutex               m_mutex;
    std::thread                      m_monitorThread;
    std::atomic<bool>                m_monitorRunning;
};

}  // namespace SPMS
