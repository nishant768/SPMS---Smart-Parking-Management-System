#!/bin/bash
# scripts/run_tests.sh – Build and run all unit tests

set -e

SCRIPT_DIR="$(dirname "$0")"
PROJECT_DIR="$SCRIPT_DIR/.."
BUILD_DIR="$PROJECT_DIR/build"

echo "=============================================="
echo "  SPMS Test Runner"
echo "=============================================="

# Build if needed
if [ ! -d "$BUILD_DIR" ]; then
    echo "[Build] Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

echo "[Build] Configuring with CMake..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra" 2>&1 | tail -5

echo "[Build] Compiling..."
make -j$(nproc) 2>&1 | tail -10

echo ""
echo "[Tests] Running unit tests..."
echo "----------------------------------------------"

# Run individual tests for better output
echo ""
echo "Test 1: ParkingLot Tests"
echo "----------------------------------------------"
./test_parking_lot
echo ""

echo "Test 2: Billing System Tests"
echo "----------------------------------------------"
./test_billing
echo ""

echo "Test 3: Sensor Interface Tests"
echo "----------------------------------------------"
./test_sensor
echo ""

echo "----------------------------------------------"
echo "[CTest] Running all tests via ctest..."
ctest --output-on-failure --verbose 2>&1 | tail -20

echo ""
echo "=============================================="
echo "  All tests completed!"
echo "=============================================="
