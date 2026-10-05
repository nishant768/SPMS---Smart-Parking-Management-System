// ParkingSlot.h – Individual parking slot representation
#pragma once

#include <memory>
#include "../include/common.h"
#include "Vehicle.h"

namespace SPMS {

class ParkingSlot {
public:
    ParkingSlot(int slotId, SlotType type);

    int          getSlotId()     const { return m_slotId; }
    SlotType     getType()       const { return m_type; }
    SlotStatus   getStatus()     const { return m_status; }
    bool         isAvailable()   const { return m_status == SlotStatus::AVAILABLE; }
    bool         isOccupied()    const { return m_status == SlotStatus::OCCUPIED; }
    const Vehicle* getVehicle()  const { return m_vehicle.get(); }

    // Occupy slot with a vehicle; returns false if already occupied
    bool occupy(std::unique_ptr<Vehicle> vehicle);

    // Vacate slot; returns the vehicle that was parked (caller takes ownership)
    std::unique_ptr<Vehicle> vacate();

    void print() const;

private:
    int                      m_slotId;
    SlotType                 m_type;
    SlotStatus               m_status;
    std::unique_ptr<Vehicle> m_vehicle;
};

}  // namespace SPMS
