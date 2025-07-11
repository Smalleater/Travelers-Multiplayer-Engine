#include "Logger.h"

namespace tme
{
	void Logger::addLogger(std::shared_ptr<ILogger> logger)
	{
		m_loggers.push_back(std::move(logger));
	}

	void Logger::Log(std::string msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->Log(msg);
		}
	}

	void Logger::LogWarning(std::string msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->LogWarning(msg);
		}
	}

	void Logger::LogError(std::string msg)
	{
		for (std::shared_ptr<ILogger> logger : m_loggers)
		{
			logger->LogError(msg);
		}
	}
}
