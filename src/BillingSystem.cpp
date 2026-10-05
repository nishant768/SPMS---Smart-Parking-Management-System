// BillingSystem.cpp – Fee calculation implementation
#include "BillingSystem.h"
#include <iostream>
#include <iomanip>
#include <cmath>

namespace SPMS {

BillingSystem::BillingSystem()
{
    m_rates[VehicleType::CAR]   = RATE_CAR;
    m_rates[VehicleType::BIKE]  = RATE_BIKE;
    m_rates[VehicleType::TRUCK] = RATE_TRUCK;
}

double BillingSystem::calculateFee(VehicleType type, long durationSeconds) const
{
    // Minimum 1 hour; partial hours rounded up
    double hours = static_cast<double>(durationSeconds) / 3600.0;
    hours = std::ceil(hours);
    if (hours < 1.0) hours = 1.0;

    double rate = m_rates.at(type);
    return hours * rate;
}

double BillingSystem::processCheckout(const Ticket& ticket)
{
    long duration = static_cast<long>(
        std::difftime(ticket.exitTime > 0 ? ticket.exitTime : std::time(nullptr),
                      ticket.entryTime));

    double fee = calculateFee(ticket.vehicleType, duration);

    // Print receipt
    char entryBuf[64], exitBuf[64];
    struct tm* ti = std::localtime(&ticket.entryTime);
    std::strftime(entryBuf, sizeof(entryBuf), "%Y-%m-%d %H:%M:%S", ti);

    time_t exitT = ticket.exitTime > 0 ? ticket.exitTime : std::time(nullptr);
    ti = std::localtime(&exitT);
    std::strftime(exitBuf, sizeof(exitBuf), "%Y-%m-%d %H:%M:%S", ti);

    double hours = std::ceil(static_cast<double>(duration) / 3600.0);
    if (hours < 1.0) hours = 1.0;

    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║        SMART PARKING SYSTEM RECEIPT      ║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  Ticket ID    : " << std::left << std::setw(26) << ticket.ticketId << "║\n";
    std::cout << "  ║  Plate        : " << std::setw(26) << ticket.licensePlate  << "║\n";
    std::cout << "  ║  Vehicle Type : " << std::setw(26) << vehicleTypeToString(ticket.vehicleType) << "║\n";
    std::cout << "  ║  Slot No.     : " << std::setw(26) << ticket.slotId << "║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  Entry Time   : " << std::setw(26) << entryBuf << "║\n";
    std::cout << "  ║  Exit Time    : " << std::setw(26) << exitBuf  << "║\n";
    std::cout << "  ║  Duration     : " << std::setw(26) << formatDuration(duration) << "║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  Rate         : Rs " << std::setw(5) << std::fixed << std::setprecision(0)
              << m_rates.at(ticket.vehicleType) << "/hr"
              << std::string(18, ' ') << "║\n";
    std::cout << "  ║  Hours Billed : " << std::setw(26) << (std::to_string((int)hours) + " hr(s)") << "║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  TOTAL AMOUNT : Rs " << std::setw(23) << std::fixed << std::setprecision(2) << fee << "║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n\n";

    return fee;
}

void BillingSystem::printRates() const
{
    std::cout << "\n  Current Parking Rates:\n";
    std::cout << "  " << std::string(35, '-') << "\n";
    std::cout << "  CAR   : Rs " << std::setw(6) << RATE_CAR   << " / hour\n";
    std::cout << "  BIKE  : Rs " << std::setw(6) << RATE_BIKE  << " / hour\n";
    std::cout << "  TRUCK : Rs " << std::setw(6) << RATE_TRUCK << " / hour\n";
    std::cout << "  (Minimum charge: 1 hour. Partial hours rounded up.)\n";
    std::cout << "  " << std::string(35, '-') << "\n";
}

std::string BillingSystem::formatDuration(long seconds) const
{
    long h = seconds / 3600;
    long m = (seconds % 3600) / 60;
    long s = seconds % 60;
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02ldh %02ldm %02lds", h, m, s);
    return std::string(buf);
}

}  // namespace SPMS
