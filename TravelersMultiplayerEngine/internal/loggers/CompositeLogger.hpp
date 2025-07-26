#ifndef TME_COMPOSITE_LOGGER_HPP
#define TME_COMPOSITE_LOGGER_HPP

#include <vector>
#include <memory>

#include "interfaces/ILogger.hpp"

namespace tme
{
    /// @brief Logger that forwards log messages to multiple underlying loggers.
    class CompositeLogger : public ILogger
    {
    private:
        /// @brief The list of loggers to which message will be forwarded.
        std::vector<std::unique_ptr<ILogger>> m_loggers;

    public:
        /// @brief Constructor: default constructor.
        CompositeLogger() {}

        /// @brief Destructor: default destructor.
        ~CompositeLogger() {}

        /// @brief Adds a logger to the composite logger.
        /// @param logger The logger to add.
        void addLogger(std::unique_ptr<ILogger> logger);

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