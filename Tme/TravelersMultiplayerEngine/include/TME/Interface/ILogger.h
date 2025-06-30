#pragma once

namespace tme
{
	class ILogger
	{
	public:
		~ILogger() = default;

		virtual void logInfo(const char* msg) = 0;
		virtual void logWarning(const char* msg) = 0;
		virtual void logError(const char* msg) = 0;
	};
}
