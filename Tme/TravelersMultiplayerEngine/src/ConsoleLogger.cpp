#include "TME/ConsoleLogger.h"

#include <iostream>
#include <ctime>
#include <string>

namespace tme
{
	void ConsoleLogger::logInfo(const char* msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Info] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::logWarning(const char* msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Warning] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::logError(const char* msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Error] " + msg + "\n";
		std::cout << strMsg;
	}

	const char* ConsoleLogger::GetSystemTime()
	{
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &local);
		return buffer;
	}
}