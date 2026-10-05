// Logger.cpp – Thread-safe logging implementation
#include "Logger.h"

namespace SPMS {

Logger::Logger(const std::string& logFilePath, bool echoToConsole)
    : m_echo(echoToConsole)
{
    m_file.open(logFilePath, std::ios::app);
    if (!m_file.is_open()) {
        std::cerr << "[Logger] WARNING: Cannot open log file: " << logFilePath
                  << " – logging to console only.\n";
    }
    info("=== SPMS Logger Started ===");
}

Logger::~Logger()
{
    info("=== SPMS Logger Stopped ===");
    if (m_file.is_open())
        m_file.close();
}

void Logger::info (const std::string& msg) { log(LogLevel::INFO,  msg); }
void Logger::warn (const std::string& msg) { log(LogLevel::WARN,  msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }
void Logger::debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }

void Logger::log(LogLevel level, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string entry = "[" + getTimestamp() + "] [" + levelToString(level) + "] " + msg;

    if (m_file.is_open()) {
        m_file << entry << "\n";
        m_file.flush();
    }

    if (m_echo) {
        switch (level) {
            case LogLevel::ERROR: std::cerr << "\033[31m" << entry << "\033[0m\n"; break;
            case LogLevel::WARN:  std::cerr << "\033[33m" << entry << "\033[0m\n"; break;
            case LogLevel::DEBUG: std::cout << "\033[36m" << entry << "\033[0m\n"; break;
            default:              std::cout << entry << "\n"; break;
        }
    }
}

std::string Logger::getTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const
{
    switch (level) {
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        default: return "?????";
    }
}

}  // namespace SPMS
