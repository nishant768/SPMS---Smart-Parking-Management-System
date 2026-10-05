#!/bin/bash
# scripts/setup.sh – Install all dependencies for SPMS on Ubuntu 22.04

set -e

echo "=============================================="
echo "  Smart Parking Management System – Setup"
echo "=============================================="

# Update package list
echo "[1/4] Updating package list..."
sudo apt-get update -y

# Install build tools
echo "[2/4] Installing build tools..."
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    gcc \
    g++ \
    make \
    linux-headers-$(uname -r) \
    dkms

# Verify
echo "[3/4] Verifying installations..."
echo "  GCC version:   $(gcc --version | head -1)"
echo "  G++ version:   $(g++ --version | head -1)"
echo "  CMake version: $(cmake --version | head -1)"
echo "  Git version:   $(git --version)"

# Create log file with permissions
echo "[4/4] Creating log file..."
sudo touch /var/log/spms.log
sudo chmod 666 /var/log/spms.log
echo "  Log file: /var/log/spms.log"

echo ""
echo "=============================================="
echo "  Setup complete! Next steps:"
echo "  1. cd driver && make"
echo "  2. sudo ./scripts/load_driver.sh"
echo "  3. mkdir build && cd build && cmake .. && make"
echo "  4. ./spms"
echo "=============================================="
