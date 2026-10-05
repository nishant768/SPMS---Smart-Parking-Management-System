// test_billing.cpp – Unit tests for BillingSystem fee calculation
#include <cassert>
#include <iostream>
#include <cmath>

#include "../include/common.h"
#include "../src/Logger.h"
#include "../src/BillingSystem.h"
#include "../src/TicketManager.h"

using namespace SPMS;

int passed = 0;
int failed = 0;

#define TEST(name, expr) \
    do { \
        if (expr) { \
            std::cout << "  [PASS] " << name << "\n"; \
            ++passed; \
        } else { \
            std::cerr << "  [FAIL] " << name << " (got false)\n"; \
            ++failed; \
        } \
    } while(0)

#define TEST_DOUBLE_EQ(name, a, b) \
    do { \
        double _a = (a), _b = (b); \
        bool ok = std::abs(_a - _b) < 0.01; \
        if (ok) { std::cout << "  [PASS] " << name << " = " << _a << "\n"; ++passed; } \
        else    { std::cerr << "  [FAIL] " << name << " expected " << _b << " got " << _a << "\n"; ++failed; } \
    } while(0)

int main()
{
    std::cout << "\n=== BillingSystem Unit Tests ===\n\n";

    BillingSystem billing;

    // ── Test 1: Exact 1 hour ──────────────────────────────────────────────
    TEST_DOUBLE_EQ("CAR  1 hr  = Rs 30",  billing.calculateFee(VehicleType::CAR,   3600), 30.0);
    TEST_DOUBLE_EQ("BIKE 1 hr  = Rs 10",  billing.calculateFee(VehicleType::BIKE,  3600), 10.0);
    TEST_DOUBLE_EQ("TRUCK 1 hr = Rs 60",  billing.calculateFee(VehicleType::TRUCK, 3600), 60.0);

    // ── Test 2: Minimum charge (< 1 hour) ────────────────────────────────
    TEST_DOUBLE_EQ("CAR  30 min = Rs 30 (min charge)", billing.calculateFee(VehicleType::CAR,  1800), 30.0);
    TEST_DOUBLE_EQ("BIKE 10 min = Rs 10 (min charge)", billing.calculateFee(VehicleType::BIKE,  600), 10.0);
    TEST_DOUBLE_EQ("TRUCK 1 min = Rs 60 (min charge)", billing.calculateFee(VehicleType::TRUCK,  60), 60.0);

    // ── Test 3: Partial hours (ceiling) ──────────────────────────────────
    // 1h 30min = 5400s -> ceil = 2 hours
    TEST_DOUBLE_EQ("CAR  1.5 hr = Rs 60 (ceil to 2)",  billing.calculateFee(VehicleType::CAR,  5400), 60.0);
    TEST_DOUBLE_EQ("BIKE 2.5 hr = Rs 30 (ceil to 3)",  billing.calculateFee(VehicleType::BIKE, 9000), 30.0);
    TEST_DOUBLE_EQ("TRUCK 3.1 hr= Rs 240 (ceil to 4)", billing.calculateFee(VehicleType::TRUCK,11160),240.0);

    // ── Test 4: Exactly 2 hours ───────────────────────────────────────────
    TEST_DOUBLE_EQ("CAR  2 hr = Rs 60",   billing.calculateFee(VehicleType::CAR,  7200), 60.0);
    TEST_DOUBLE_EQ("TRUCK 2 hr = Rs 120", billing.calculateFee(VehicleType::TRUCK,7200), 120.0);

    // ── Test 5: Full day (24h) ────────────────────────────────────────────
    TEST_DOUBLE_EQ("CAR  24 hr = Rs 720",  billing.calculateFee(VehicleType::CAR,   86400), 720.0);
    TEST_DOUBLE_EQ("BIKE 24 hr = Rs 240",  billing.calculateFee(VehicleType::BIKE,  86400), 240.0);
    TEST_DOUBLE_EQ("TRUCK 24 hr = Rs 1440",billing.calculateFee(VehicleType::TRUCK, 86400),1440.0);

    // ── Summary ───────────────────────────────────────────────────────────
    std::cout << "\n  Results: " << passed << " passed, " << failed << " failed\n\n";
    return (failed == 0) ? 0 : 1;
}
