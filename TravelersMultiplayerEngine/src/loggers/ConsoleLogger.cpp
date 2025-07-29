#include "loggers/ConsoleLogger.hpp"

#include <iostream>

#include "TME/Utils.hpp"

namespace tme
{
    // Logs a formatted message to the console with a timestamp and log type
    void ConsoleLogger::Log(const std::string& logType, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Build the log message with timestamp and type prefix
        std::string logMessage = Utils::GetSystemTime("[%Y-%m-%d %H:%M:%S]") + " [TME] " + " [" + logType + "] " + message + "\n";
        std::cout << logMessage;
    }

    // Logs an informational message
    void ConsoleLogger::LogInfo(const std::string &message)
    {
        Log("Info", message);
    }

    // Logs a warning message
    void ConsoleLogger::LogWarning(const std::string &message)
    {
        Log("Warning", message);
    }

    // Logs an error message
    void ConsoleLogger::LogError(const std::string &message)
    {
        Log("Error", message);
    }
}
