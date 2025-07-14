#include "Loggers/ConsoleLogger.h"

#include <iostream>
#include <ctime>
#include <string>

namespace tme
{
	void ConsoleLogger::Log(std::string msg)
	{
		// Lock the mutex to ensure thread-safe acces to std::cout
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and output the message
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Info] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::LogWarning(std::string msg)
	{
		// Lock the mutex to ensure thread-safe acces to std::cout
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and output the message
		std::string strMsg = GetSystemTime() + " [Warning] " + msg + "\n";
		std::cout << strMsg;
	}

	void ConsoleLogger::LogError(std::string msg)
	{
		// Lock the mutex to ensure thread-safe acces to std::cout
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and output the message
		std::string strMsg = GetSystemTime() + " [Error] " + msg + "\n";
		std::cout << strMsg;
	}

	std::string ConsoleLogger::GetSystemTime()
	{
		// Get current time
		std::time_t currentTime = std::time(nullptr);
		std::tm local;

		// Convert to local time (thread-safe on Windows)
		localtime_s(&local, &currentTime);

		// Format as [YYY-MM-DD HH:MM:SS]
		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &local);

		return std::string(buffer);
	}
}