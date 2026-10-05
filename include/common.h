// common.h – Shared types, enums, and constants for SPMS
// All modules include this header.

#pragma once

#include <string>
#include <ctime>

namespace SPMS {

// ── Vehicle / Slot Types ────────────────────────────────────────────────────

enum class VehicleType {
    CAR   = 0,
    BIKE  = 1,
    TRUCK = 2
};

enum class SlotType {
    CAR   = 0,
    BIKE  = 1,
    TRUCK = 2
};

enum class SlotStatus {
    AVAILABLE = 0,
    OCCUPIED  = 1
};

enum class TicketStatus {
    ACTIVE = 0,
    CLOSED = 1
};

// ── Constants ───────────────────────────────────────────────────────────────

constexpr int    TOTAL_SLOTS       = 20;
constexpr int    CAR_SLOTS         = 12;
constexpr int    BIKE_SLOTS        = 5;
constexpr int    TRUCK_SLOTS       = 3;
constexpr int    MONITOR_INTERVAL  = 5;   // seconds between status updates

constexpr double RATE_CAR          = 30.0;  // Rs per hour
constexpr double RATE_BIKE         = 10.0;
constexpr double RATE_TRUCK        = 60.0;

const std::string DEVICE_PATH      = "/dev/parking_sensor";
const std::string PIPE_PATH        = "/tmp/spms_pipe";
const std::string LOG_FILE         = "/var/log/spms.log";

// ── Helper Functions ─────────────────────────────────────────────────────────

inline std::string vehicleTypeToString(VehicleType t) {
    switch (t) {
        case VehicleType::CAR:   return "CAR";
        case VehicleType::BIKE:  return "BIKE";
        case VehicleType::TRUCK: return "TRUCK";
        default: return "UNKNOWN";
    }
}

inline std::string slotStatusToString(SlotStatus s) {
    return (s == SlotStatus::AVAILABLE) ? "AVAILABLE" : "OCCUPIED";
}

inline VehicleType stringToVehicleType(const std::string& s) {
    if (s == "BIKE"  || s == "2") return VehicleType::BIKE;
    if (s == "TRUCK" || s == "3") return VehicleType::TRUCK;
    return VehicleType::CAR;
}

inline SlotType vehicleTypeToSlotType(VehicleType t) {
    switch (t) {
        case VehicleType::CAR:   return SlotType::CAR;
        case VehicleType::BIKE:  return SlotType::BIKE;
        case VehicleType::TRUCK: return SlotType::TRUCK;
        default: return SlotType::CAR;
    }
}

inline double getRate(VehicleType t) {
    switch (t) {
        case VehicleType::CAR:   return RATE_CAR;
        case VehicleType::BIKE:  return RATE_BIKE;
        case VehicleType::TRUCK: return RATE_TRUCK;
        default: return RATE_CAR;
    }
}

}  // namespace SPMS
