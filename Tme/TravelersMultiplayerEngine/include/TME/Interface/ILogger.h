#pragma once

#include <string>

namespace tme
{
	class ILogger
	{
	public:
		~ILogger() = default;

		virtual void logInfo(std::string msg) = 0;
		virtual void logWarning(std::string msg) = 0;
		virtual void logError(std::string msg) = 0;
	};
}
