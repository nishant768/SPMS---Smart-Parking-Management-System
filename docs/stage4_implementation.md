# Stage 4 – Initial Implementation & Prototype

## 4.1 Implementation Summary

In this stage, the following core modules were implemented:

| Module | File(s) | Status |
|--------|---------|--------|
| Kernel Module | `driver/parking_sensor.c` | Done |
| Common Types | `include/common.h` | Done |
| Logger | `src/Logger.h/cpp` | Done |
| Vehicle | `src/Vehicle.h/cpp` | Done |
| ParkingSlot | `src/ParkingSlot.h/cpp` | Done |
| TicketManager | `src/TicketManager.h/cpp` | Done |
| BillingSystem | `src/BillingSystem.h/cpp` | Done |
| SensorInterface | `src/SensorInterface.h/cpp` | Done |
| SignalHandler | `src/SignalHandler.h/cpp` | Done |
| ParkingLot | `src/ParkingLot.h/cpp` | Done |
| Main CLI | `src/main.cpp` | Done |

---

## 4.2 Kernel Module Implementation

### What it does
- Registers as a **character device** at `/dev/parking_sensor`
- Maintains an array of 20 slot states in kernel memory
- Exposes `read()`, `write()`, and `ioctl()` file operations
- Uses `mutex` for thread-safe kernel-side access
- Automatically creates device node via `class_device_create`

### Key kernel APIs used
```c
alloc_chrdev_region()    // Dynamic major number allocation
cdev_init() / cdev_add() // Register character device
class_create()           // Create /sys/class/parking
device_create()          // Create /dev/parking_sensor automatically
copy_to_user()           // Safely copy data to userspace
copy_from_user()         // Safely copy data from userspace
DEFINE_MUTEX()           // Kernel mutex for slot array protection
```

### Build and Load
```bash
cd driver
make
sudo insmod parking_sensor.ko
dmesg | tail -10           # Check for "parking_sensor: device created"
ls -la /dev/parking_sensor # Verify device node
```

---

## 4.3 C++ Application Implementation

### Logger (Thread-Safe)
- Uses `std::mutex` + `std::lock_guard` for thread safety
- Writes to both file (`/var/log/spms.log`) and stdout
- ANSI color-coded output (red=error, yellow=warn, cyan=debug)
- Timestamp format: `[YYYY-MM-DD HH:MM:SS]`

### ParkingLot (Core Coordinator)
- Initializes 20 slots: 12 CAR (0-11), 5 BIKE (12-16), 3 TRUCK (17-19)
- `enterVehicle()`: finds first available slot → creates Vehicle → occupies slot → creates ticket
- `exitVehicle()`: looks up ticket → calculates fee → prints receipt → vacates slot
- Protected by `std::mutex` for thread safety
- Background monitor thread runs every 5 seconds

### SensorInterface (IPC + Driver)
- Opens `/dev/parking_sensor` for kernel driver communication
- Creates FIFO at `/tmp/spms_pipe` using `mkfifo()`
- Non-blocking pipe read with `O_NONBLOCK`
- Background `std::thread` continuously reads pipe for sensor events
- Falls back to simulation mode gracefully if driver not loaded

### SignalHandler (POSIX Signals)
- Registers `SIGINT`, `SIGTERM`, `SIGHUP` with `sigaction()`
- Singleton pattern
- Sets atomic shutdown flag
- Calls user-registered callback for graceful cleanup

### BillingSystem (Fee Calculation)
- Rate table: CAR=30, BIKE=10, TRUCK=60 Rs/hour
- Minimum 1 hour, partial hours rounded up with `std::ceil()`
- Formatted receipt with box-drawing characters

### TicketManager (Storage)
- Stores tickets in `std::map<string, Ticket>`
- Auto-generates IDs: `TKT-1001`, `TKT-1002`, ...
- Lookup by ticket ID or license plate
- Thread-safe with `std::mutex`

---

## 4.4 System Programming Features

| Feature | Implementation |
|---------|---------------|
| POSIX Threads | `std::thread` wrapping pthreads (monitor + pipe listener) |
| Mutex | `std::mutex` + `std::lock_guard` for all shared data |
| POSIX Signals | `sigaction()` for SIGINT/SIGTERM/SIGHUP |
| File I/O | `open()`, `read()`, `write()`, `close()` for device and pipe |
| Named Pipe | `mkfifo()`, `O_RDONLY | O_NONBLOCK` |
| Atomic | `std::atomic<bool>` for shutdown and running flags |

---

## 4.5 Build System

```bash
# CMake build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Output
ls build/
# spms            – main application
# test_parking_lot
# test_billing
# test_sensor
```

---

## 4.6 Prototype Demo Flow

```
1. Start: ./build/spms
   → Banner displayed
   → Sensor: Simulation mode (no kernel driver)
   → Status: 20/20 available

2. Option 1 (Entry):
   → Plate: KA01AB1234
   → Type: 1 (Car)
   → Output: ✔ Entry successful! Ticket: TKT-1001, Slot: 0

3. Option 3 (Status):
   → Available: 19, Occupied: 1

4. Option 2 (Exit):
   → Ticket: TKT-1001
   → Receipt printed with fee

5. Ctrl+C:
   → [SignalHandler] SIGINT received
   → Graceful shutdown
```

---

## 4.7 Issues Encountered & Solutions

| Issue | Solution |
|-------|---------|
| Kernel `class_create` deprecated in 6.4+ | Use `class_create(THIS_MODULE, name)` form with `#if LINUX_VERSION_CODE` guard |
| Named pipe blocks on open without writer | Use `O_NONBLOCK` flag |
| Thread race on slot access | Protect `m_slots` with `std::mutex` |
| `copy_to_user` fails silently | Always check return value and return `-EFAULT` |

---

*Next Stage: Stage 5 – Testing, Integration & Improvement*
