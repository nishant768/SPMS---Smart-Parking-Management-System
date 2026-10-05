// TicketManager.cpp – Ticket management implementation
#include "TicketManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace SPMS {

TicketManager::TicketManager()
    : m_counter(1000)
{}

std::string TicketManager::generateTicketId()
{
    std::ostringstream oss;
    oss << "TKT-" << ++m_counter;
    return oss.str();
}

std::string TicketManager::createTicket(const std::string& licensePlate,
                                        VehicleType vehicleType,
                                        int slotId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Ticket t;
    t.ticketId     = generateTicketId();
    t.licensePlate = licensePlate;
    t.vehicleType  = vehicleType;
    t.slotId       = slotId;
    t.entryTime    = std::time(nullptr);
    t.exitTime     = 0;
    t.amountDue    = 0.0;
    t.status       = TicketStatus::ACTIVE;

    m_tickets[t.ticketId] = t;
    return t.ticketId;
}

const Ticket* TicketManager::getTicket(const std::string& ticketId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tickets.find(ticketId);
    return (it != m_tickets.end()) ? &it->second : nullptr;
}

const Ticket* TicketManager::getTicketByPlate(const std::string& plate) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& [id, t] : m_tickets) {
        if (t.licensePlate == plate && t.status == TicketStatus::ACTIVE)
            return &t;
    }
    return nullptr;
}

bool TicketManager::closeTicket(const std::string& ticketId, double amount)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tickets.find(ticketId);
    if (it == m_tickets.end()) return false;
    if (it->second.status != TicketStatus::ACTIVE) return false;

    it->second.exitTime  = std::time(nullptr);
    it->second.amountDue = amount;
    it->second.status    = TicketStatus::CLOSED;
    return true;
}

bool TicketManager::isParked(const std::string& licensePlate) const
{
    return getTicketByPlate(licensePlate) != nullptr;
}

void TicketManager::printActiveTickets() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool found = false;
    std::cout << "\n  Active Tickets:\n";
    std::cout << "  " << std::string(50, '-') << "\n";
    for (const auto& [id, t] : m_tickets) {
        if (t.status == TicketStatus::ACTIVE) {
            char buf[32];
            struct tm* ti = std::localtime(&t.entryTime);
            std::strftime(buf, sizeof(buf), "%H:%M:%S", ti);
            std::cout << "  " << id << " | " << t.licensePlate
                      << " | Slot " << t.slotId
                      << " | " << vehicleTypeToString(t.vehicleType)
                      << " | In: " << buf << "\n";
            found = true;
        }
    }
    if (!found) std::cout << "  No active tickets.\n";
    std::cout << "  " << std::string(50, '-') << "\n";
}

void TicketManager::printAllTickets() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "\n  All Tickets (History):\n";
    std::cout << "  " << std::string(70, '-') << "\n";
    for (const auto& [id, t] : m_tickets) {
        char entryBuf[32], exitBuf[32];
        struct tm* ti = std::localtime(&t.entryTime);
        std::strftime(entryBuf, sizeof(entryBuf), "%H:%M:%S", ti);
        if (t.exitTime > 0) {
            ti = std::localtime(&t.exitTime);
            std::strftime(exitBuf, sizeof(exitBuf), "%H:%M:%S", ti);
        } else {
            std::strcpy(exitBuf, "--:--:--");
        }
        std::cout << "  " << id << " | " << t.licensePlate
                  << " | Slot " << std::setw(2) << t.slotId
                  << " | In: " << entryBuf
                  << " | Out: " << exitBuf
                  << " | Rs " << std::fixed << std::setprecision(2) << t.amountDue
                  << " | " << (t.status == TicketStatus::ACTIVE ? "ACTIVE" : "CLOSED")
                  << "\n";
    }
    std::cout << "  " << std::string(70, '-') << "\n";
}

int TicketManager::getActiveCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& [id, t] : m_tickets)
        if (t.status == TicketStatus::ACTIVE) ++count;
    return count;
}

}  // namespace SPMS
