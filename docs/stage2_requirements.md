# Stage 2 – Project Requirements & Development Plan

## 2.1 Functional Requirements

### FR-01: Parking Slot Management
- The system shall support a configurable number of parking slots (default: 20).
- Each slot shall have a unique ID, type (Car/Bike/Truck), and status (Available/Occupied).
- The system shall track which vehicle occupies each slot.

### FR-02: Vehicle Entry
- The system shall accept vehicle registration (license plate, vehicle type).
- The system shall automatically assign the nearest available slot.
- The system shall generate a unique ticket with entry timestamp.
- Entry shall be denied if no slot of the required type is available.

### FR-03: Vehicle Exit
- The system shall allow vehicle exit by ticket ID or license plate.
- The system shall calculate parking duration and fee on exit.
- The system shall free the slot after exit.
- The system shall print a receipt with all billing details.

### FR-04: Billing System
- The system shall apply different rates per vehicle type:
  - Car: ₹30/hour
  - Bike: ₹10/hour
  - Truck: ₹60/hour
- Minimum charge: 1 hour.
- Partial hours rounded up.

### FR-05: Real-Time Monitoring
- The system shall display current slot availability at any time.
- A background thread shall update slot status every 5 seconds.

### FR-06: Kernel Driver Interface
- The system shall load a character device driver (`/dev/parking_sensor`).
- The driver shall simulate sensor data (slot occupied/vacant signals).
- The application shall read sensor data via the device file.

### FR-07: Signal Handling
- The system shall handle SIGINT (Ctrl+C) and SIGTERM gracefully.
- On shutdown, all occupied slots and tickets shall be logged and closed.

### FR-08: Logging
- All events (entry, exit, errors, sensor reads) shall be logged to a file.
- Log format: `[YYYY-MM-DD HH:MM:SS] [LEVEL] Message`

### FR-09: IPC (Named Pipe)
- A named pipe (`/tmp/spms_pipe`) shall be used for sensor simulation data.
- The sensor simulator process writes slot updates; the main app reads them.

---

## 2.2 Non-Functional Requirements

| ID | Requirement | Criteria |
|----|-------------|---------|
| NFR-01 | Performance | Slot lookup under 1ms |
| NFR-02 | Reliability | Graceful recovery from bad sensor data |
| NFR-03 | Portability | Runs on Ubuntu 22.04 LTS 64-bit |
| NFR-04 | Maintainability | Code follows SOLID principles, documented |
| NFR-05 | Security | Driver runs with minimal privileges |
| NFR-06 | Usability | CLI menu self-explanatory, no manual needed |
| NFR-07 | Logging | All events logged with severity levels |

---

## 2.3 System Constraints

- Must run on **Linux kernel 5.15+**
- Requires **root privileges** to load/unload kernel module
- No external library dependencies (standard library only)
- Build system: CMake 3.16+ and GCC 11+

---

## 2.4 Project Modules

```
SPMS
├── Kernel Module (driver/)
│   └── parking_sensor – character device driver
├── Core Application (src/)
│   ├── ParkingLot       – slot management & coordination
│   ├── ParkingSlot      – individual slot representation
│   ├── Vehicle          – vehicle data model
│   ├── TicketManager    – ticket creation & lookup
│   ├── BillingSystem    – fee calculation
│   ├── SensorInterface  – driver/pipe communication
│   ├── Logger           – event logging
│   └── SignalHandler    – OS signal management
└── Tests (tests/)
    ├── test_parking_lot
    ├── test_billing
    └── test_sensor
```

---

## 2.5 Development Plan & Timeline

| Week | Stage | Activities |
|------|-------|-----------|
| Week 1 | Stage 1 | Project introduction, problem definition, scope |
| Week 2 | Stage 2 | Requirements gathering, PRD, development plan |
| Week 3-4 | Stage 3 | System design, UML diagrams, architecture |
| Week 5-6 | Stage 4 | Kernel driver, core C++ modules, prototype |
| Week 7-8 | Stage 5 | Testing, integration, bug fixes, improvements |
| Week 9-10 | Stage 6 | Final polish, documentation, presentation |

---

## 2.6 Deliverables

| Deliverable | Description |
|-------------|-------------|
| Source Code | All C++ and kernel module source files |
| Documentation | 6 stage markdown documents |
| UML Diagrams | Class, Sequence, State Machine diagrams |
| Test Results | Unit and integration test reports |
| Git Repository | Full commit history on GitHub |
| Final Report | Stage 6 comprehensive report |
| Demo Video | Screen recording of running system |

---

## 2.7 Risk Analysis

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| Kernel module crashes VM | Medium | High | Test in snapshot, use printk carefully |
| Concurrency bugs | Medium | Medium | Use mutex, test with thread sanitizer |
| Driver not loading | Low | High | Check kernel headers, dmesg for errors |
| Time overrun | Low | Medium | Follow weekly milestones strictly |

---

*Next Stage: Stage 3 – System Design & Architecture*
