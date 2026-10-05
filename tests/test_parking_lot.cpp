// test_parking_lot.cpp – Unit tests for ParkingLot core operations
// Compile: cmake --build build && ./build/test_parking_lot

#include <cassert>
#include <iostream>
#include <string>

#include "../include/common.h"
#include "../src/Logger.h"
#include "../src/ParkingLot.h"

using namespace SPMS;

int passed = 0;
int failed = 0;

#define TEST(name, expr) \
    do { \
        if (expr) { \
            std::cout << "  [PASS] " << name << "\n"; \
            ++passed; \
        } else { \
            std::cerr << "  [FAIL] " << name << "\n"; \
            ++failed; \
        } \
    } while(0)

int main()
{
    std::cout << "\n=== ParkingLot Unit Tests ===\n\n";

    Logger logger("/tmp/spms_test.log", false);
    ParkingLot lot(logger);

    // ── Test 1: Initial state ─────────────────────────────────────────────
    TEST("Initial available = 20", lot.getTotalAvailable() == 20);
    TEST("Initial occupied  = 0",  lot.getTotalOccupied()  == 0);
    TEST("Not full initially",     !lot.isFull());

    // ── Test 2: Vehicle entry ─────────────────────────────────────────────
    auto r1 = lot.enterVehicle("KA01AB1234", VehicleType::CAR);
    TEST("CAR entry success",        r1.success == true);
    TEST("CAR assigned slotId >= 0", r1.slotId >= 0);
    TEST("CAR ticket not empty",     !r1.ticketId.empty());
    TEST("Available decremented",    lot.getTotalAvailable() == 19);
    TEST("Occupied incremented",     lot.getTotalOccupied()  == 1);

    auto r2 = lot.enterVehicle("MH02CD5678", VehicleType::BIKE);
    TEST("BIKE entry success",        r2.success == true);
    TEST("Available = 18 after BIKE", lot.getTotalAvailable() == 18);

    auto r3 = lot.enterVehicle("DL03EF9012", VehicleType::TRUCK);
    TEST("TRUCK entry success",        r3.success == true);
    TEST("Available = 17 after TRUCK", lot.getTotalAvailable() == 17);

    // ── Test 3: Duplicate entry ───────────────────────────────────────────
    auto r4 = lot.enterVehicle("KA01AB1234", VehicleType::CAR);
    TEST("Duplicate entry rejected",   r4.success == false);
    TEST("Available still 17",         lot.getTotalAvailable() == 17);

    // ── Test 4: Vehicle exit ──────────────────────────────────────────────
    auto e1 = lot.exitVehicle(r1.ticketId);
    TEST("Exit by ticketId success",   e1.success == true);
    TEST("Available = 18 after exit",  lot.getTotalAvailable() == 18);

    auto e2 = lot.exitVehicle("MH02CD5678");
    TEST("Exit by plate success",      e2.success == true);

    // ── Test 5: Exit already-closed ticket ───────────────────────────────
    auto e3 = lot.exitVehicle(r1.ticketId);
    TEST("Re-exit closed ticket fails", e3.success == false);

    // ── Test 6: Exit non-existent ticket ─────────────────────────────────
    auto e4 = lot.exitVehicle("TKT-9999");
    TEST("Non-existent ticket fails",   e4.success == false);

    // ── Test 7: Fill all CAR slots ────────────────────────────────────────
    // We already have r3 (TRUCK). Fill remaining 11 car slots.
    int carsFilled = 0;
    for (int i = 2; i < 13; ++i) {
        std::string plate = "TEST" + std::to_string(i) + "CAR";
        auto res = lot.enterVehicle(plate, VehicleType::CAR);
        if (res.success) ++carsFilled;
    }
    TEST("11 more CARs entered",  carsFilled == 11);

    // Now car slots should be full (r1 exited, 1 remaining car + 11 new = 12 total)
    auto overflow = lot.enterVehicle("OVERFLOW01", VehicleType::CAR);
    TEST("CAR overflow rejected",  overflow.success == false);

    // ── Summary ───────────────────────────────────────────────────────────
    std::cout << "\n  Results: " << passed << " passed, " << failed << " failed\n\n";
    return (failed == 0) ? 0 : 1;
}
