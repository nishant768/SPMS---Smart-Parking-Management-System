# Smart Parking Management System (SPMS)

> **Individual Project** – Linux, Device Drivers, System Programming & C++  
> **Platform:** Ubuntu 22.04 LTS (VirtualBox)  
> **Language:** C++ (C++17)

---

## Project Overview

The **Smart Parking Management System (SPMS)** is a Linux-based application that simulates a real-world parking lot management system using:

- **Linux Kernel Device Drivers** (character device driver for sensor simulation)
- **System Programming** (POSIX threads, signals, IPC, file I/O, sockets)
- **C++17** (OOP, STL, multithreading, design patterns)

---

## Repository Structure

```
SmartParkingSystem/
├── docs/                          # All documentation
│   ├── stage1_introduction.md
│   ├── stage2_requirements.md
│   ├── stage3_design.md
│   ├── stage4_implementation.md
│   ├── stage5_testing.md
│   └── stage6_final_report.md
├── driver/                        # Linux kernel module
│   ├── parking_sensor.c
│   ├── parking_sensor.h
│   └── Makefile
├── src/                           # C++ application source
│   ├── main.cpp
│   ├── ParkingLot.cpp / .h
│   ├── ParkingSlot.cpp / .h
│   ├── Vehicle.cpp / .h
│   ├── SensorInterface.cpp / .h
│   ├── TicketManager.cpp / .h
│   ├── BillingSystem.cpp / .h
│   ├── Logger.cpp / .h
│   └── SignalHandler.cpp / .h
├── include/                       # Shared headers
│   └── common.h
├── tests/                         # Unit & integration tests
│   ├── test_parking_lot.cpp
│   ├── test_billing.cpp
│   └── test_sensor.cpp
├── scripts/                       # Helper shell scripts
│   ├── setup.sh
│   ├── load_driver.sh
│   └── run_tests.sh
├── Makefile                       # Top-level build
├── CMakeLists.txt                 # CMake build system
└── README.md
```

---

## Stages

| Stage | Title | Status |
|-------|-------|--------|
| 1 | Project Introduction | Done |
| 2 | Requirements & Development Plan | Done |
| 3 | System Design & Architecture | Done |
| 4 | Initial Implementation & Prototype | Done |
| 5 | Testing, Integration & Improvement | Done |
| 6 | Final Implementation & Presentation | Done |

---

## Quick Start (Ubuntu VirtualBox)

```bash
# 1. Clone repository
git clone https://github.com/YOUR_USERNAME/SmartParkingSystem.git
cd SmartParkingSystem

# 2. Run setup script (installs dependencies)
chmod +x scripts/setup.sh
./scripts/setup.sh

# 3. Build the kernel driver
cd driver && make && cd ..

# 4. Load the driver
sudo ./scripts/load_driver.sh

# 5. Build the application
mkdir build && cd build
cmake .. && make

# 6. Run the application
./spms

# 7. Run tests
../scripts/run_tests.sh
```

---

## Key Features

- Real-time parking slot monitoring via kernel driver
- Vehicle entry/exit with automatic ticket generation
- Dynamic billing calculation (hourly/daily rates)
- Multi-threaded slot monitoring using POSIX threads
- Signal handling (SIGINT, SIGTERM) for graceful shutdown
- Persistent logging with timestamps
- IPC using named pipes for sensor communication
- CLI-based interactive dashboard

---

## Technologies Used

| Category | Technology |
|----------|-----------|
| Language | C++17 |
| Kernel Module | Linux LKM (C) |
| Build System | CMake + Makefile |
| Threading | POSIX Threads (pthreads) |
| IPC | Named Pipes (FIFO) |
| Logging | Custom Logger (file + stdout) |
| Version Control | Git |
| Platform | Ubuntu 22.04 LTS |

---

## Author

- **Name:** Nishant Koushik
- **Course:** Linux, Device Drivers, System Programming & C++
- **Year:** 2026
