#include "Loggers/CompositeLogger.h"

namespace tme
{
	void CompositeLogger::addLogger(std::shared_ptr<ILogger> logger)
	{
		// Add the given logger to the internal list
		m_loggers.push_back(std::move(logger));
	}

	void CompositeLogger::Log(std::string msg)
	{
		// Forward the info message to all registered loggers
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->Log(msg);
		}
	}

	void CompositeLogger::LogWarning(std::string msg)
	{
		// Forward the warning message to all registered loggers
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->LogWarning(msg);
		}
	}

	void CompositeLogger::LogError(std::string msg)
	{
		// Forward the error message to all registered loggers
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->LogError(msg);
		}
	}
}
