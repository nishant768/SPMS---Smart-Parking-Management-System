// BillingSystem.h – Fee calculation and receipt printing
#pragma once

#include <string>
#include <map>
#include "../include/common.h"
#include "TicketManager.h"

namespace SPMS {

class BillingSystem {
public:
    BillingSystem();

    // Calculate fee for a vehicle given duration (seconds)
    double calculateFee(VehicleType type, long durationSeconds) const;

    // Calculate and print a full receipt; returns the total amount
    double processCheckout(const Ticket& ticket);

    // Display the pricing table
    void printRates() const;

private:
    std::map<VehicleType, double> m_rates;   // Rs per hour

    std::string formatDuration(long seconds) const;
};

}  // namespace SPMS
