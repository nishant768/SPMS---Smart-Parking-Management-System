// SignalHandler.h – POSIX signal handling for graceful shutdown
#pragma once

#include <csignal>
#include <atomic>
#include <functional>

namespace SPMS {

using ShutdownCallback = std::function<void()>;

class SignalHandler {
public:
    static SignalHandler& getInstance();

    // Register signals and a callback to invoke on shutdown
    void setup(ShutdownCallback cb);

    // Returns true if a shutdown signal has been received
    bool isShuttingDown() const { return s_shutdown.load(); }

private:
    SignalHandler() = default;
    SignalHandler(const SignalHandler&) = delete;
    SignalHandler& operator=(const SignalHandler&) = delete;

    static void handleSignal(int signum);

    static std::atomic<bool> s_shutdown;
    static ShutdownCallback  s_callback;
};

}  // namespace SPMS
