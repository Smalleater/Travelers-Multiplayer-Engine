#include "Loggers/FileLogger.h"

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

namespace tme
{
	FileLogger::FileLogger()
	{
		// Check if the folder exists, create it if necessary
		if (!FolderExists(LOG_FILE_PATH))
		{
			if (!CreateFolder(LOG_FILE_PATH))
			{
				std::cerr << "Failed to create folder\n";
				return;
			}
		}

		// Construct log file path and opend it
		std::string logPath = std::string(LOG_FILE_PATH) + "/log_" + GetSystemTimeForFileName() + ".txt";
		m_file = std::ofstream(logPath);

		// Check if the file opened successfully
		if (!m_file)
		{
			std::cerr << "Failed to open file\n";
			return;
		}
	}

	FileLogger::~FileLogger()
	{
		// Close the file if open
		m_file.close();
	}

	void FileLogger::Log(std::string msg)
	{
		// Lock the mutex to ensure thread-safe access to file
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and write the message
		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Info] " + msg + "\n";
			m_file << strMsg;
		}
	}

	void FileLogger::LogWarning(std::string msg)
	{
		// Lock the mutex to ensure thread-safe access to file
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and write the message
		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Warning] " + msg + "\n";
			m_file << strMsg;
		}
	}

	void FileLogger::LogError(std::string msg)
	{
		// Lock the mutex to ensure thread-safe access to file
		std::lock_guard<std::mutex> lock(m_mutex);

		// Format and write the message
		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Error] " + msg + "\n";
			m_file << strMsg;
		}
	}

	bool FileLogger::FolderExists(std::string path)
	{
		// Check if the folder exists using stat
		struct stat info;
		return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
	}

	bool FileLogger::CreateFolder(std::string path)
	{
		// Try to create the folder using system command
		std::string command = "mkdir " + path;

		int result = system(command.c_str());
		if (result != 0)
		{
			return false;
		}

		return true;
	}

	std::string FileLogger::GetSystemTimeForFileName()
	{
		// Get current time
		std::time_t currentTime = std::time(nullptr);
		std::tm local;

		// Convert to local time (thread-safe on Windows)
		localtime_s(&local, &currentTime);

		// Format as YYYY-MM-DD_HH-MM-SS
		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &local);

		return std::string(buffer);
	}

	std::string FileLogger::GetSystemTime()
	{
		// Get current time
		std::time_t currentTime = std::time(nullptr);
		std::tm local;

		// Convert to local time (thread-safe on Windows)
		localtime_s(&local, &currentTime);

		// Format as [YYYY-MM-DD HH:MM:SS]
		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &local);

		return std::string(buffer);
	}
}