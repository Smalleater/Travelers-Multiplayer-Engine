#include "TME/ConsoleLogger.h"

#include <iostream>
#include <ctime>
#include <string>

namespace tme
{
	void ConsoleLogger::logInfo(const char* msg)
	{
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Info] " + msg;
		std::cout << strMsg << std::endl;
	}

	void ConsoleLogger::logWarning(const char* msg)
	{
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Warning] " + msg;
		std::cout << strMsg << std::endl;
	}

	void ConsoleLogger::logError(const char* msg)
	{
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Error] " + msg;
		std::cout << strMsg << std::endl;
	}

	const char* ConsoleLogger::GetSystemTime()
	{
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%m/%d/%Y %H:%M:%S]", &local);
		return buffer;
	}
}