cmake_minimum_required(VERSION 3.16)
project(SmartParkingSystem VERSION 1.0.0 LANGUAGES CXX)

# ── C++ Standard ────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Compiler Flags ───────────────────────────────────────────────────────────
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fsanitize=address,thread")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ── Threads (pthreads) ───────────────────────────────────────────────────────
find_package(Threads REQUIRED)

# ── Include Directories ──────────────────────────────────────────────────────
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# ── Source Files ─────────────────────────────────────────────────────────────
set(SPMS_SOURCES
    src/main.cpp
    src/Logger.cpp
    src/Vehicle.cpp
    src/ParkingSlot.cpp
    src/TicketManager.cpp
    src/BillingSystem.cpp
    src/SensorInterface.cpp
    src/SignalHandler.cpp
    src/ParkingLot.cpp
)

# ── Main Executable ──────────────────────────────────────────────────────────
add_executable(spms ${SPMS_SOURCES})
target_link_libraries(spms PRIVATE Threads::Threads)

# ── Library (for tests) ──────────────────────────────────────────────────────
set(SPMS_LIB_SOURCES
    src/Logger.cpp
    src/Vehicle.cpp
    src/ParkingSlot.cpp
    src/TicketManager.cpp
    src/BillingSystem.cpp
    src/SensorInterface.cpp
    src/SignalHandler.cpp
    src/ParkingLot.cpp
)

add_library(spms_lib STATIC ${SPMS_LIB_SOURCES})
target_link_libraries(spms_lib PUBLIC Threads::Threads)

# ── Tests ────────────────────────────────────────────────────────────────────
enable_testing()

# Test: ParkingLot
add_executable(test_parking_lot tests/test_parking_lot.cpp)
target_link_libraries(test_parking_lot PRIVATE spms_lib Threads::Threads)
add_test(NAME ParkingLotTest COMMAND test_parking_lot)

# Test: Billing
add_executable(test_billing tests/test_billing.cpp)
target_link_libraries(test_billing PRIVATE spms_lib Threads::Threads)
add_test(NAME BillingTest COMMAND test_billing)

# Test: Sensor
add_executable(test_sensor tests/test_sensor.cpp)
target_link_libraries(test_sensor PRIVATE spms_lib Threads::Threads)
add_test(NAME SensorTest COMMAND test_sensor)

# ── Install ──────────────────────────────────────────────────────────────────
install(TARGETS spms RUNTIME DESTINATION bin)

message(STATUS "SPMS build configured. Run: cmake --build . && ctest")
