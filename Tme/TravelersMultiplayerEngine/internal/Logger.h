#pragma once

#include <vector>
#include <memory>

#include "Interface/ILogger.h"

namespace tme
{
	class Logger : public ILogger
	{
	public:
		Logger() {};
		~Logger() {};

		void addLogger(std::shared_ptr<ILogger> logger);

		void logInfo(std::string msg) override;
		void logWarning(std::string msg) override;
		void logError(std::string msg) override;

	private:
		std::vector<std::shared_ptr<ILogger>> m_loggers;
	};
}
