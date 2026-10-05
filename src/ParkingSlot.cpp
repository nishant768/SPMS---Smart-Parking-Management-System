// ParkingSlot.cpp – Parking slot implementation
#include "ParkingSlot.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace SPMS {

ParkingSlot::ParkingSlot(int slotId, SlotType type)
    : m_slotId(slotId)
    , m_type(type)
    , m_status(SlotStatus::AVAILABLE)
    , m_vehicle(nullptr)
{}

bool ParkingSlot::occupy(std::unique_ptr<Vehicle> vehicle)
{
    if (m_status == SlotStatus::OCCUPIED)
        return false;

    m_vehicle = std::move(vehicle);
    m_status  = SlotStatus::OCCUPIED;
    return true;
}

std::unique_ptr<Vehicle> ParkingSlot::vacate()
{
    if (m_status == SlotStatus::AVAILABLE)
        return nullptr;

    m_status = SlotStatus::AVAILABLE;
    return std::move(m_vehicle);
}

void ParkingSlot::print() const
{
    const char* typeStr[] = {"CAR  ", "BIKE ", "TRUCK"};
    const char* statStr   = isAvailable() ? "\033[32mAVAIL\033[0m" : "\033[31mOCCUP\033[0m";

    std::cout << " [" << std::setw(2) << m_slotId << "] "
              << typeStr[static_cast<int>(m_type)] << "  "
              << statStr;

    if (isOccupied() && m_vehicle)
        std::cout << "  -> " << m_vehicle->getLicensePlate();

    std::cout << "\n";
}

}  // namespace SPMS
