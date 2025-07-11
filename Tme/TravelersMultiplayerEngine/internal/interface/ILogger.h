#pragma once

#include <string>

namespace tme
{
	class ILogger
	{
	public:
		~ILogger() = default;

		virtual void Log(std::string msg) = 0;
		virtual void LogWarning(std::string msg) = 0;
		virtual void LogError(std::string msg) = 0;
	};
}
