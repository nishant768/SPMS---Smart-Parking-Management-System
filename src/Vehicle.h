// Vehicle.h – Vehicle data model
#pragma once

#include <string>
#include <ctime>
#include "../include/common.h"

namespace SPMS {

class Vehicle {
public:
    Vehicle(const std::string& licensePlate, VehicleType type);

    const std::string& getLicensePlate() const { return m_licensePlate; }
    VehicleType        getType()         const { return m_type; }
    time_t             getEntryTime()    const { return m_entryTime; }
    std::string        getTypeString()   const { return vehicleTypeToString(m_type); }

    // Calculate how long this vehicle has been parked (seconds)
    long getDurationSeconds() const;

    void print() const;

private:
    std::string m_licensePlate;
    VehicleType m_type;
    time_t      m_entryTime;
};

}  // namespace SPMS
