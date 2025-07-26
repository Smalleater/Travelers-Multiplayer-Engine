#ifndef TME_CONSOLE_LOGGER_HPP
#define TME_CONSOLE_LOGGER_HPP

#include <mutex>

#include "interfaces/ILogger.hpp"

namespace tme
{
    /// @brief Concrete implementation of ILogger that logs messages to the console.
    ///
    /// Each log message is prefixed with a timestamp and a log level (Info, Warning, Error).
    /// Logging is thread-safe.
    class ConsoleLogger : public ILogger
    {
    private:
        /// @brief Mutex used to synchronize access to std::cout.
        std::mutex m_mutex;

        /// @brief Outputs a formatted log message to the console.
        /// @param logType The type of log (e.g., "Info", "Warning", "Error").
        /// @param message The message to be logged.
        void Log(const std::string& logType, const std::string& message);
    public:
        /// @brief Constructor: Default constructor.
        ConsoleLogger() {};

        /// @brief Destructor: Default destructor.
        ~ConsoleLogger() {};

        /// @brief Logs an informational message.
        /// @param message The message to log.
        void LogInfo(const std::string& message) override;

        /// @brief Logs a warning message.
        /// @param message The message to log.
        void LogWarning(const std::string& message) override;

        /// @brief Logs an error message.
        /// @param message The message to log.
        void LogError(const std::string& message) override;
    };
}

#endif