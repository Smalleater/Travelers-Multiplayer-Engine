#include "TME/Logger.h"

namespace tme
{
	void Logger::addLogger(std::shared_ptr<ILogger> logger)
	{
		m_loggers.push_back(std::move(logger));
	}

	void Logger::logInfo(const char* msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->logInfo(msg);
		}
	}

	void Logger::logWarning(const char* msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->logWarning(msg);
		}
	}

	void Logger::logError(const char* msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->logError(msg);
		}
	}
}
