#pragma once

#include <vector>
#include <memory>

#include "ILogger.h"

namespace tme
{
	class Logger : public ILogger
	{
	public:
		Logger() {};
		~Logger() {};

		void addLogger(std::shared_ptr<ILogger> logger);

		void logInfo(const char* msg) override;
		void logWarning(const char* msg) override;
		void logError(const char* msg) override;

	private:
		std::vector<std::shared_ptr<ILogger>> m_loggers;
	};
}
