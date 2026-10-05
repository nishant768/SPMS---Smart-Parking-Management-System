// Logger.h – Thread-safe logging system
#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace SPMS {

enum class LogLevel {
    INFO  = 0,
    WARN  = 1,
    ERROR = 2,
    DEBUG = 3
};

class Logger {
public:
    explicit Logger(const std::string& logFilePath, bool echoToConsole = true);
    ~Logger();

    void info (const std::string& msg);
    void warn (const std::string& msg);
    void error(const std::string& msg);
    void debug(const std::string& msg);

    void setEcho(bool echo) { m_echo = echo; }

private:
    void log(LogLevel level, const std::string& msg);
    std::string getTimestamp() const;
    std::string levelToString(LogLevel level) const;

    std::ofstream m_file;
    std::mutex    m_mutex;
    bool          m_echo;
};

}  // namespace SPMS
