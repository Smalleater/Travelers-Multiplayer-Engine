#include "Loggers/ConsoleLogger.h"

#include <iostream>
#include <ctime>
#include <string>

namespace tme
{
	void ConsoleLogger::Log(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Info] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::LogWarning(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = GetSystemTime() + " [Warning] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::LogError(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string strMsg = GetSystemTime() + " [Error] " + msg + "\n";
		std::cout << strMsg;
	}

	std::string ConsoleLogger::GetSystemTime()
	{
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &local);
		return std::string(buffer);
	}
}