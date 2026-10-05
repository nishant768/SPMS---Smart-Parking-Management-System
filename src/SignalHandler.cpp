// SignalHandler.cpp – POSIX signal handling implementation
#include "SignalHandler.h"
#include <iostream>
#include <cstring>

namespace SPMS {

// Static member definitions
std::atomic<bool>  SignalHandler::s_shutdown{false};
ShutdownCallback   SignalHandler::s_callback;

SignalHandler& SignalHandler::getInstance()
{
    static SignalHandler instance;
    return instance;
}

void SignalHandler::setup(ShutdownCallback cb)
{
    s_callback = cb;

    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT,  &sa, nullptr);   // Ctrl+C
    sigaction(SIGTERM, &sa, nullptr);   // kill / systemd stop
    sigaction(SIGHUP,  &sa, nullptr);   // terminal hangup

    std::cout << "\033[36m[SignalHandler] SIGINT/SIGTERM/SIGHUP handlers registered.\033[0m\n";
}

void SignalHandler::handleSignal(int signum)
{
    const char* sigName = (signum == SIGINT)  ? "SIGINT"  :
                          (signum == SIGTERM) ? "SIGTERM" :
                          (signum == SIGHUP)  ? "SIGHUP"  : "UNKNOWN";

    // Use write() – async-signal-safe
    const char* msg1 = "\n[SignalHandler] Received signal: ";
    const char* msg2 = "\n[SignalHandler] Initiating graceful shutdown...\n";
    write(STDOUT_FILENO, msg1, std::strlen(msg1));
    write(STDOUT_FILENO, sigName, std::strlen(sigName));
    write(STDOUT_FILENO, msg2, std::strlen(msg2));

    s_shutdown.store(true);

    if (s_callback)
        s_callback();
}

}  // namespace SPMS
