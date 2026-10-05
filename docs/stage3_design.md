# Stage 3 – System Design & Architecture

## 3.1 Overall System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    USER (CLI Terminal)                   │
└────────────────────────┬────────────────────────────────┘
                         │ stdin/stdout
┌────────────────────────▼────────────────────────────────┐
│              SPMS Application (C++17)                    │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────────┐ │
│  │  ParkingLot  │  │TicketManager │  │ BillingSystem │ │
│  └──────┬───────┘  └──────┬───────┘  └───────┬───────┘ │
│         │                 │                   │         │
│  ┌──────▼───────────────────────────────────▼───────┐  │
│  │           ParkingSlot[]  /  Vehicle[]             │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │SensorInterface│ │    Logger    │  │SignalHandler │  │
│  └──────┬──────┘  └──────────────┘  └──────────────┘  │
└─────────┼───────────────────────────────────────────────┘
          │ read(/dev/parking_sensor) + FIFO pipe
┌─────────▼───────────────────────────────────────────────┐
│          Linux Kernel Space                              │
│  ┌───────────────────────────────────────────────────┐  │
│  │   parking_sensor.ko  (Character Device Driver)    │  │
│  │   Major: dynamic   Minor: 0                       │  │
│  │   /dev/parking_sensor                             │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

---

## 3.2 Component Descriptions

| Component | Layer | Responsibility |
|-----------|-------|---------------|
| `main.cpp` | App | Entry point, CLI menu loop |
| `ParkingLot` | App | Manages all slots, entry/exit orchestration |
| `ParkingSlot` | App | Represents a single parking space |
| `Vehicle` | App | Data model for a parked vehicle |
| `TicketManager` | App | Creates, stores, retrieves tickets |
| `BillingSystem` | App | Calculates fees by type and duration |
| `SensorInterface` | App | Reads from `/dev/parking_sensor` and FIFO pipe |
| `Logger` | App | Thread-safe file + stdout logging |
| `SignalHandler` | App | POSIX signal registration and handling |
| `parking_sensor.ko` | Kernel | Character device simulating sensor data |

---

## 3.3 Class Diagram (UML)

```
┌────────────────────────┐
│       ParkingLot       │
├────────────────────────┤
│ - slots: vector<Slot>  │
│ - ticketMgr: TicketMgr │
│ - billing: BillingSystem│
│ - logger: Logger&      │
│ - mutex: pthread_mutex │
├────────────────────────┤
│ + enterVehicle()       │
│ + exitVehicle()        │
│ + displayStatus()      │
│ + findAvailableSlot()  │
└───────────┬────────────┘
            │ has-many
┌───────────▼────────────┐      ┌──────────────────────┐
│      ParkingSlot       │      │       Vehicle        │
├────────────────────────┤      ├──────────────────────┤
│ - slotId: int          │      │ - licensePlate: string│
│ - type: SlotType       │◄─────│ - type: VehicleType  │
│ - isOccupied: bool     │      │ - entryTime: time_t  │
│ - vehicle: Vehicle*    │      ├──────────────────────┤
├────────────────────────┤      │ + getLicensePlate()  │
│ + occupy(Vehicle*)     │      │ + getType()          │
│ + vacate()             │      │ + getEntryTime()     │
│ + getStatus()          │      └──────────────────────┘
└────────────────────────┘

┌────────────────────────┐      ┌──────────────────────┐
│     TicketManager      │      │    BillingSystem     │
├────────────────────────┤      ├──────────────────────┤
│ - tickets: map<>       │      │ - rates: map<>       │
├────────────────────────┤      ├──────────────────────┤
│ + createTicket()       │      │ + calculateFee()     │
│ + getTicket()          │      │ + printReceipt()     │
│ + closeTicket()        │      └──────────────────────┘
└────────────────────────┘

┌────────────────────────┐      ┌──────────────────────┐
│    SensorInterface     │      │       Logger         │
├────────────────────────┤      ├──────────────────────┤
│ - fd: int (device)     │      │ - logFile: ofstream  │
│ - pipeFd: int          │      │ - mutex: mutex       │
├────────────────────────┤      ├──────────────────────┤
│ + readSensor()         │      │ + info()             │
│ + writePipe()          │      │ + error()            │
│ + openDevice()         │      │ + warn()             │
└────────────────────────┘      └──────────────────────┘
```

---

## 3.4 Sequence Diagram – Vehicle Entry

```
User        CLI         ParkingLot    ParkingSlot   TicketMgr   Logger
 │           │               │              │            │          │
 │──Enter──▶│               │              │            │          │
 │  plate,  │──enterVehicle▶│              │            │          │
 │  type    │               │──findSlot()─▶│            │          │
 │           │               │◄──slotId────│            │          │
 │           │               │──occupy()──▶│            │          │
 │           │               │──createTicket()──────────▶│         │
 │           │               │◄─────ticketID─────────────│         │
 │           │               │──log entry──────────────────────────▶│
 │           │◄──Ticket#──── │              │            │          │
 │◄─────────│               │              │            │          │
```

---

## 3.5 Sequence Diagram – Vehicle Exit

```
User        CLI         ParkingLot    BillingSystem  TicketMgr   Logger
 │           │               │              │              │          │
 │──Exit───▶│               │              │              │          │
 │  ticketID│──exitVehicle─▶│              │              │          │
 │           │               │──getTicket()──────────────▶│         │
 │           │               │◄──ticket──────────────────▶│         │
 │           │               │──calculateFee()──▶│         │         │
 │           │               │◄──amount──────────│         │         │
 │           │               │──closeTicket()───────────▶│         │
 │           │               │──vacate()        │         │         │
 │           │               │──log exit──────────────────────────▶│
 │           │◄──Receipt──── │              │              │          │
 │◄─────────│               │              │              │          │
```

---

## 3.6 State Machine – Parking Slot

```
           ┌─────────────┐
           │             │
    ┌──────▼──────┐      │
    │  AVAILABLE  │      │ [Vehicle exits]
    └──────┬──────┘      │
           │             │
    [Vehicle enters]     │
           │             │
    ┌──────▼──────┐      │
    │  OCCUPIED   ├──────┘
    └─────────────┘
```

---

## 3.7 State Machine – Parking Ticket

```
    ┌──────────┐
    │ CREATED  │ ──[vehicle enters]──▶ ┌──────────┐
    └──────────┘                       │  ACTIVE  │
                                       └────┬─────┘
                                            │
                                  [vehicle exits]
                                            │
                                       ┌────▼─────┐
                                       │  CLOSED  │
                                       └──────────┘
```

---

## 3.8 Data Structures

```cpp
// Enumerations
enum class VehicleType { CAR, BIKE, TRUCK };
enum class SlotType    { CAR, BIKE, TRUCK };
enum class SlotStatus  { AVAILABLE, OCCUPIED };
enum class TicketStatus{ ACTIVE, CLOSED };

// Vehicle
struct Vehicle {
    std::string  licensePlate;
    VehicleType  type;
    time_t       entryTime;
};

// ParkingSlot
struct ParkingSlot {
    int          slotId;
    SlotType     type;
    SlotStatus   status;
    Vehicle*     vehicle;   // nullptr if vacant
};

// Ticket
struct Ticket {
    std::string  ticketId;
    std::string  licensePlate;
    int          slotId;
    time_t       entryTime;
    time_t       exitTime;
    double       amountDue;
    TicketStatus status;
};

// Billing Rates
std::map<VehicleType, double> rates = {
    {VehicleType::CAR,   30.0},   // Rs per hour
    {VehicleType::BIKE,  10.0},
    {VehicleType::TRUCK, 60.0}
};
```

---

## 3.9 Development Environment Setup

```bash
# Ubuntu 22.04 packages needed
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    linux-headers-$(uname -r) \
    gcc \
    g++ \
    make \
    dkms

# Verify kernel headers
ls /lib/modules/$(uname -r)/build

# Verify GCC version (need 11+)
gcc --version
g++ --version
```

---

## 3.10 Git Branching Strategy

```
main          ──────────────────────────────────────▶ (production)
              │       │       │       │       │
stage/1       ▼       │       │       │       │
stage/2       ────────▼       │       │       │
stage/3       ────────────────▼       │       │
stage/4       ────────────────────────▼       │
stage/5       ────────────────────────────────▼
```

Each stage merged to `main` with a tagged release:
- `v0.1.0` – Stage 1
- `v0.2.0` – Stage 2
- `v0.3.0` – Stage 3
- `v0.4.0` – Stage 4 (Prototype)
- `v0.5.0` – Stage 5 (Testing)
- `v1.0.0` – Stage 6 (Final)

---

*Next Stage: Stage 4 – Initial Implementation & Prototype*
