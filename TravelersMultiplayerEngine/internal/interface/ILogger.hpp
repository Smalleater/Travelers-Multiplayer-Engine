#ifndef TME_I_LOGGER_HPP
#define TME_I_LOGGER_HPP

#include <string>

namespace tme
{
    /// @brief Interface for logging informational, warning, and error message.
    class ILogger
    {
    public:
        /// @brief Destructor: default virtual destructor.
        virtual ~ILogger() = default;

        /// @brief Logs an informational message.
        /// @param message The message to log. 
        virtual void LogInfo(const std::string& message) = 0;

        /// @brief Logs a warning message.
        /// @param message The message to log.
        virtual void LogWarning(const std::string& message) = 0;

        /// @brief Logs an error message. 
        /// @param message The message to log.
        virtual void LogError(const std::string& message) = 0;
    }; 
}

#endif