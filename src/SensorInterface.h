// SensorInterface.h – Communicates with kernel driver and named pipe
#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include "../include/common.h"
#include "Logger.h"

namespace SPMS {

// Callback signature: slotId, isOccupied
using SensorCallback = std::function<void(int, bool)>;

class SensorInterface {
public:
    explicit SensorInterface(Logger& logger);
    ~SensorInterface();

    // Try to open the kernel device file
    bool openDevice(const std::string& devicePath);
    void closeDevice();

    // Try to open/create the named pipe
    bool openPipe(const std::string& pipePath);
    void closePipe();

    // Register a callback for sensor events
    void setCallback(SensorCallback cb) { m_callback = cb; }

    // Read all slot states from the kernel driver (blocking, one-shot)
    bool readAllSensorStates(bool* states, int count);

    // Start background thread that reads the pipe continuously
    void startPipeListener();
    void stopPipeListener();

    // Simulate writing a sensor event to the pipe (for testing)
    void simulateSensorEvent(int slotId, bool occupied);

    bool isDeviceOpen() const { return m_deviceFd >= 0; }
    bool isPipeOpen()   const { return m_pipeFd   >= 0; }

private:
    void pipeListenerThread();

    Logger&        m_logger;
    int            m_deviceFd;
    int            m_pipeFd;
    SensorCallback m_callback;

    std::thread    m_listenerThread;
    std::atomic<bool> m_running;
};

}  // namespace SPMS
