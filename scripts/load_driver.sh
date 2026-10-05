#!/bin/bash
# scripts/load_driver.sh – Load the parking_sensor kernel module

set -e
DRIVER_DIR="$(dirname "$0")/../driver"
MODULE="parking_sensor"

echo "========================================"
echo "  Loading $MODULE kernel module"
echo "========================================"

# Check if already loaded
if lsmod | grep -q "$MODULE"; then
    echo "  Module already loaded. Unloading first..."
    sudo rmmod "$MODULE"
    sleep 1
fi

# Build if .ko doesn't exist
if [ ! -f "$DRIVER_DIR/${MODULE}.ko" ]; then
    echo "  Module not built. Building now..."
    cd "$DRIVER_DIR" && make && cd -
fi

# Load module
echo "  Loading module..."
sudo insmod "$DRIVER_DIR/${MODULE}.ko"

# Verify
if lsmod | grep -q "$MODULE"; then
    echo "  Module loaded successfully!"
    MAJOR=$(grep "$MODULE" /proc/devices 2>/dev/null | awk '{print $1}')
    echo "  Major number: $MAJOR"
    echo "  Device: /dev/parking_sensor"
    echo ""
    echo "  Recent kernel messages:"
    dmesg | grep "$MODULE" | tail -5
else
    echo "  ERROR: Module failed to load!"
    dmesg | tail -10
    exit 1
fi

echo "========================================"
echo "  Driver ready. You can now run: ./build/spms"
echo "========================================"
