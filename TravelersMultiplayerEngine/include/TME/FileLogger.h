#pragma once

#include "ILogger.h"

namespace tme
{
	class FileLogger : public ILogger
	{
	public:
		FileLogger() {};
		~FileLogger() {};

		void logInfo(const char* msg) override;
		void logWarning(const char* msg) override;
		void logError(const char* msg) override;

	private:

	};
}
