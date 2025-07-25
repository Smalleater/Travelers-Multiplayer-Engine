#include "loggers/CompositeLogger.hpp"

namespace tme
{
    // Adds a new logger to the list of loggers.
    void CompositeLogger::addLogger(std::shared_ptr<ILogger> logger)
    {
        m_loggers.push_back(std::move(logger));
    }

    // Logs an informational message using all registered loggers.
    void CompositeLogger::LogInfo(const std::string &message)
    {
        for (std::shared_ptr<ILogger> logger : m_loggers)
        {
            logger->LogInfo(message);
        }
    }

    // Logs a warning message using all registered loggers.
    void CompositeLogger::LogWarning(const std::string& message)
    {
        for (std::shared_ptr<ILogger> logger : m_loggers)
        {
            logger->LogWarning(message);
        }
    }

    // Logs an error message using all registered loggers.
    void CompositeLogger::LogError(const std::string& message)
    {
        for (std::shared_ptr<ILogger> logger : m_loggers)
        {
            logger->LogError(message);
        }
    }
}
