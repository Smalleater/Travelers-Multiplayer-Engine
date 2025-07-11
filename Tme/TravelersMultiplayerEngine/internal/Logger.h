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

		void Log(std::string msg) override;
		void LogWarning(std::string msg) override;
		void LogError(std::string msg) override;

	private:
		std::vector<std::shared_ptr<ILogger>> m_loggers;
	};
}
