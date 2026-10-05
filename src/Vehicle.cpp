// Vehicle.cpp – Vehicle implementation
#include "Vehicle.h"
#include <iostream>
#include <iomanip>
#include <cstring>

namespace SPMS {

Vehicle::Vehicle(const std::string& licensePlate, VehicleType type)
    : m_licensePlate(licensePlate)
    , m_type(type)
    , m_entryTime(std::time(nullptr))
{}

long Vehicle::getDurationSeconds() const
{
    return static_cast<long>(std::difftime(std::time(nullptr), m_entryTime));
}

void Vehicle::print() const
{
    char timeBuf[64];
    struct tm* tm_info = std::localtime(&m_entryTime);
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", tm_info);

    std::cout << "  License Plate : " << m_licensePlate << "\n"
              << "  Vehicle Type  : " << vehicleTypeToString(m_type) << "\n"
              << "  Entry Time    : " << timeBuf << "\n";
}

}  // namespace SPMS
