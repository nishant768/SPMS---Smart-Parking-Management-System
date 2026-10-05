// TicketManager.h – Ticket creation, storage, and retrieval
#pragma once

#include <string>
#include <map>
#include <mutex>
#include <ctime>
#include "../include/common.h"

namespace SPMS {

struct Ticket {
    std::string  ticketId;
    std::string  licensePlate;
    VehicleType  vehicleType;
    int          slotId;
    time_t       entryTime;
    time_t       exitTime;
    double       amountDue;
    TicketStatus status;
};

class TicketManager {
public:
    TicketManager();

    // Create a new active ticket; returns ticket ID
    std::string createTicket(const std::string& licensePlate,
                             VehicleType vehicleType,
                             int slotId);

    // Retrieve a ticket by ID; returns nullptr if not found
    const Ticket* getTicket(const std::string& ticketId) const;

    // Retrieve a ticket by license plate (active only)
    const Ticket* getTicketByPlate(const std::string& plate) const;

    // Close a ticket (set exit time, amount, status)
    bool closeTicket(const std::string& ticketId, double amount);

    // Check if plate is already parked (active ticket exists)
    bool isParked(const std::string& licensePlate) const;

    // Print all active tickets
    void printActiveTickets() const;

    // Print all tickets (history)
    void printAllTickets() const;

    int getActiveCount() const;

private:
    std::string generateTicketId();

    std::map<std::string, Ticket> m_tickets;   // ticketId -> Ticket
    mutable std::mutex            m_mutex;
    int                           m_counter;
};

}  // namespace SPMS
