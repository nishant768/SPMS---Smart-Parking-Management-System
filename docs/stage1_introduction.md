# Stage 1 – Project Introduction

## 1.1 Project Title
**Smart Parking Management System (SPMS)**

---

## 1.2 Project Objective

The objective of this project is to design, develop, and demonstrate a **Smart Parking Management System** that runs on a **Linux-based operating system** (Ubuntu 22.04 LTS). The system integrates:

- A **Linux Kernel Module (LKM)** that acts as a character device driver to simulate parking slot sensors.
- A **C++17 application** that manages vehicle entry/exit, ticketing, billing, and real-time slot monitoring.
- **Linux System Programming** techniques including POSIX threads, signals, IPC (named pipes), and file I/O.

---

## 1.3 Problem Statement

Urban areas face significant challenges related to parking:

- **Wasted time** searching for available parking spots.
- **Manual management** that is error-prone and inefficient.
- **No real-time visibility** into parking slot availability.
- **Inconsistent billing** and loss of revenue for parking lot operators.

Traditional parking systems rely on manual ticket counters and do not provide automated, real-time monitoring. There is a need for a software system that can:
- Track the availability of parking slots in real time.
- Automatically generate tickets on vehicle entry.
- Calculate fees accurately on vehicle exit.
- Provide a reliable log of all events.

---

## 1.4 Project Scope

This project covers the following areas:

### In Scope
| Area | Description |
|------|-------------|
| Kernel Driver | Character device driver simulating IR/ultrasonic parking sensors |
| Vehicle Management | Track vehicles by license plate, type, and entry time |
| Ticket System | Auto-generate and store parking tickets |
| Billing Engine | Calculate parking fees based on duration and vehicle type |
| Slot Monitor | Multi-threaded real-time slot availability monitoring |
| Signal Handling | Graceful shutdown on SIGINT/SIGTERM |
| IPC | Named pipes for sensor-to-application communication |
| Logging | Persistent event log with timestamps |
| CLI Dashboard | Terminal-based interactive interface |

### Out of Scope
- GUI application (web or desktop)
- Hardware integration (physical sensors)
- Network/cloud connectivity
- Mobile application

---

## 1.5 Expected Outcome

Upon successful completion, the system will:

1. Load a custom Linux kernel module that simulates sensor data.
2. Allow vehicles to enter and exit the parking lot via a CLI menu.
3. Automatically assign the nearest available slot.
4. Generate a unique ticket on vehicle entry.
5. Calculate and display the parking fee on exit.
6. Monitor all slots in real time using background threads.
7. Gracefully shut down when interrupted.
8. Maintain a persistent log of all operations.

---

## 1.6 Real-World Application

This system mirrors real-world parking management solutions used in:
- Shopping malls and supermarkets
- Airport parking lots
- Office complex parking
- Hospitals and public buildings

The project demonstrates how Linux system programming and kernel-level development can be applied to solve practical, everyday problems.

---

## 1.7 Technology Stack Summary

| Component | Technology |
|-----------|-----------|
| Programming Language | C++17 |
| Kernel Module | C (Linux Kernel Module) |
| OS Platform | Ubuntu 22.04 LTS |
| Build System | CMake 3.x + GNU Make |
| Concurrency | POSIX Threads (pthreads) |
| IPC Mechanism | FIFO Named Pipes |
| Version Control | Git + GitHub |

---

*Next Stage: Stage 2 – Project Requirements & Development Plan*
