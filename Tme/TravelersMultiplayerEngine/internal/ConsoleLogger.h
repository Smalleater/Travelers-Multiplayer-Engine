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

		void logInfo(std::string msg) override;
		void logWarning(std::string msg) override;
		void logError(std::string msg) override;

	private:
		std::mutex m_mutex;

		const char* GetSystemTime();
	};
}
