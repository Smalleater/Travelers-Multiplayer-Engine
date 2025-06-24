#pragma once

#include "ILogger.h"

namespace tme
{
	class ConsoleLogger : public ILogger 
	{
	public:
		ConsoleLogger() {};
		~ConsoleLogger() {};

		void logInfo(const char* msg) override;
		void logWarning(const char* msg) override;
		void logError(const char* msg) override;

	private:
		const char* GetSystemTime();
	};
}
