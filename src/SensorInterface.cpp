// SensorInterface.cpp – Kernel driver and named pipe communication
#include "SensorInterface.h"

// POSIX system programming headers
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <chrono>

// Userspace-compatible version of the kernel struct
struct SensorData {
    int  slotId;
    int  status;     // 0=free, 1=occupied
    long timestamp;
};

namespace SPMS {

SensorInterface::SensorInterface(Logger& logger)
    : m_logger(logger)
    , m_deviceFd(-1)
    , m_pipeFd(-1)
    , m_running(false)
{}

SensorInterface::~SensorInterface()
{
    stopPipeListener();
    closeDevice();
    closePipe();
}

// ── Device file (kernel module) ─────────────────────────────────────────────

bool SensorInterface::openDevice(const std::string& devicePath)
{
    m_deviceFd = open(devicePath.c_str(), O_RDWR);
    if (m_deviceFd < 0) {
        m_logger.warn("SensorInterface: Cannot open device " + devicePath
                      + " (" + std::strerror(errno)
                      + "). Sensor data will use simulation mode.");
        return false;
    }
    m_logger.info("SensorInterface: Opened device " + devicePath);
    return true;
}

void SensorInterface::closeDevice()
{
    if (m_deviceFd >= 0) {
        close(m_deviceFd);
        m_deviceFd = -1;
    }
}

bool SensorInterface::readAllSensorStates(bool* states, int count)
{
    if (m_deviceFd < 0) return false;

    // Seek to beginning so we can re-read
    lseek(m_deviceFd, 0, SEEK_SET);

    for (int i = 0; i < count; ++i) {
        SensorData data;
        ssize_t n = read(m_deviceFd, &data, sizeof(SensorData));
        if (n == sizeof(SensorData)) {
            states[data.slotId] = (data.status == 1);
        } else {
            break;
        }
    }
    return true;
}

// ── Named pipe ──────────────────────────────────────────────────────────────

bool SensorInterface::openPipe(const std::string& pipePath)
{
    // Create the FIFO if it doesn't exist
    if (mkfifo(pipePath.c_str(), 0666) < 0 && errno != EEXIST) {
        m_logger.error("SensorInterface: mkfifo failed: " + std::string(std::strerror(errno)));
        return false;
    }

    // Open non-blocking so we don't wait for a writer
    m_pipeFd = open(pipePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (m_pipeFd < 0) {
        m_logger.warn("SensorInterface: Cannot open pipe " + pipePath);
        return false;
    }
    m_logger.info("SensorInterface: Named pipe opened: " + pipePath);
    return true;
}

void SensorInterface::closePipe()
{
    if (m_pipeFd >= 0) {
        close(m_pipeFd);
        m_pipeFd = -1;
    }
    // Optionally remove the FIFO
    // unlink(PIPE_PATH.c_str());
}

void SensorInterface::startPipeListener()
{
    if (m_pipeFd < 0) return;
    m_running = true;
    m_listenerThread = std::thread(&SensorInterface::pipeListenerThread, this);
    m_logger.info("SensorInterface: Pipe listener thread started");
}

void SensorInterface::stopPipeListener()
{
    m_running = false;
    if (m_listenerThread.joinable())
        m_listenerThread.join();
}

void SensorInterface::pipeListenerThread()
{
    while (m_running) {
        SensorData data;
        ssize_t n = read(m_pipeFd, &data, sizeof(SensorData));
        if (n == sizeof(SensorData)) {
            m_logger.debug("SensorInterface: Pipe event – slot " +
                           std::to_string(data.slotId) + " -> " +
                           (data.status ? "OCCUPIED" : "FREE"));
            if (m_callback)
                m_callback(data.slotId, data.status == 1);
        }
        // Sleep briefly to avoid busy-wait
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void SensorInterface::simulateSensorEvent(int slotId, bool occupied)
{
    // Write to the pipe from userspace (for testing without hardware)
    int writeFd = open(PIPE_PATH.c_str(), O_WRONLY | O_NONBLOCK);
    if (writeFd < 0) return;

    SensorData data;
    data.slotId    = slotId;
    data.status    = occupied ? 1 : 0;
    data.timestamp = static_cast<long>(std::time(nullptr));
    write(writeFd, &data, sizeof(SensorData));
    close(writeFd);
}

}  // namespace SPMS
