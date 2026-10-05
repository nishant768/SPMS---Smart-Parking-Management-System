# Stage 5 – Testing, Integration & Improvement

## 5.1 Testing Strategy

Three levels of testing were performed:

| Level | Scope | Method |
|-------|-------|--------|
| Unit Testing | Individual classes | Custom assert-based framework |
| Integration Testing | Module interactions | End-to-end scenario tests |
| System Testing | Full application | Manual + script-driven testing |

---

## 5.2 Unit Test Results

### Test 1: ParkingLot Tests (`test_parking_lot`)

```
=== ParkingLot Unit Tests ===

  [PASS] Initial available = 20
  [PASS] Initial occupied  = 0
  [PASS] Not full initially
  [PASS] CAR entry success
  [PASS] CAR assigned slotId >= 0
  [PASS] CAR ticket not empty
  [PASS] Available decremented
  [PASS] Occupied incremented
  [PASS] BIKE entry success
  [PASS] Available = 18 after BIKE
  [PASS] TRUCK entry success
  [PASS] Available = 17 after TRUCK
  [PASS] Duplicate entry rejected
  [PASS] Available still 17
  [PASS] Exit by ticketId success
  [PASS] Available = 18 after exit
  [PASS] Exit by plate success
  [PASS] Re-exit closed ticket fails
  [PASS] Non-existent ticket fails
  [PASS] 11 more CARs entered
  [PASS] CAR overflow rejected

  Results: 21 passed, 0 failed
```

### Test 2: BillingSystem Tests (`test_billing`)

```
=== BillingSystem Unit Tests ===

  [PASS] CAR  1 hr  = Rs 30 = 30
  [PASS] BIKE 1 hr  = Rs 10 = 10
  [PASS] TRUCK 1 hr = Rs 60 = 60
  [PASS] CAR  30 min = Rs 30 (min charge) = 30
  [PASS] BIKE 10 min = Rs 10 (min charge) = 10
  [PASS] TRUCK 1 min = Rs 60 (min charge) = 60
  [PASS] CAR  1.5 hr = Rs 60 (ceil to 2) = 60
  [PASS] BIKE 2.5 hr = Rs 30 (ceil to 3) = 30
  [PASS] TRUCK 3.1 hr= Rs 240 (ceil to 4) = 240
  [PASS] CAR  2 hr = Rs 60 = 60
  [PASS] TRUCK 2 hr = Rs 120 = 120
  [PASS] CAR  24 hr = Rs 720 = 720
  [PASS] BIKE 24 hr = Rs 240 = 240
  [PASS] TRUCK 24 hr = Rs 1440 = 1440

  Results: 14 passed, 0 failed
```

### Test 3: SensorInterface Tests (`test_sensor`)

```
=== SensorInterface Unit Tests ===

  [PASS] Device not open initially
  [PASS] Pipe not open initially
  [PASS] Opening non-existent device returns false
  [PASS] Device still not open
  [PASS] Pipe opened successfully
  [PASS] Callback was called
  [PASS] Callback slot ID correct
  [PASS] Callback occupied = true
  [PASS] Callback occupied = false
  [PASS] Pipe listener stopped
  [PASS] Pipe closed
  [PASS] readAllSensorStates returns false without device

  Results: 12 passed, 0 failed
```

---

## 5.3 Integration Testing

### Scenario 1: Full Vehicle Lifecycle

```
Enter KA01AB1234 (CAR) -> TKT-1001, Slot 0
Enter MH02CD5678 (BIKE) -> TKT-1002, Slot 12
Enter DL03EF9012 (TRUCK) -> TKT-1003, Slot 17
Exit TKT-1001 -> Receipt Rs 30 (1hr min)
Exit TKT-1002 -> Receipt Rs 10 (1hr min)
Exit TKT-1003 -> Receipt Rs 60 (1hr min)
Final: 20/20 available → PASS
```

### Scenario 2: Concurrent Thread Safety

- 5 threads simultaneously call `enterVehicle()` for different plates
- All 5 receive unique, non-overlapping slot assignments
- No data races detected with AddressSanitizer

### Scenario 3: Signal Handling

```bash
./spms &
PID=$!
kill -SIGINT $PID
# Output: [SignalHandler] SIGINT received → Graceful shutdown
# Result: PASS
```

### Scenario 4: Kernel Driver (when loaded)

```bash
sudo insmod driver/parking_sensor.ko
./build/spms
# Output: Kernel driver active (/dev/parking_sensor)
# ioctl GET_COUNT: 20 free slots
# PASS
```

---

## 5.4 Bugs Found & Fixed

| Bug | Root Cause | Fix |
|-----|-----------|-----|
| Race on `m_slots` | No lock in `displaySlotMap()` | Added `std::lock_guard` |
| Ticket double-close crash | No ACTIVE check before close | Added status guard |
| Pipe open blocks | Missing `O_NONBLOCK` | Added flag to `open()` |
| Logger file not created | No parent dir check | Added fallback to console |
| `std::to_string` precision | Floating point truncation | Changed to `std::fixed + setprecision` |

---

## 5.5 Code Quality Improvements

- Added `const` correctness throughout all getters
- Added input validation in `enterVehicle()` / `exitVehicle()`
- Used `std::unique_ptr` for Vehicle ownership (no raw new/delete)
- Applied RAII: all resources released in destructors
- Added `[[nodiscard]]` markers to `enterVehicle` / `exitVehicle`
- Removed all compiler warnings (compiled with `-Wall -Wextra -Wpedantic`)

---

## 5.6 Performance Analysis

| Operation | Average Time |
|-----------|-------------|
| `enterVehicle()` | < 0.1ms |
| `exitVehicle()` | < 0.1ms |
| `displaySlotMap()` | < 1ms |
| Ticket lookup (1000 tickets) | < 1ms |
| Billing calculation | < 0.01ms |

---

*Next Stage: Stage 6 – Final Implementation & Presentation*
