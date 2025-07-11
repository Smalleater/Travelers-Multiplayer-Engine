#pragma once

#include <mutex>

#include "Interface/ILogger.h"

namespace tme
{
	class ConsoleLogger : public ILogger 
	{
	public:
		ConsoleLogger() {};
		~ConsoleLogger() {};

		void Log(std::string msg) override;
		void LogWarning(std::string msg) override;
		void LogError(std::string msg) override;

	private:
		std::mutex m_mutex;

		std::string GetSystemTime();
	};
}
