#include "Loggers/FileLogger.h"

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

namespace tme
{
	FileLogger::FileLogger()
	{
		if (!FolderExists(LOG_FILE_PATH))
		{
			if (!CreateFolder(LOG_FILE_PATH))
			{
				std::cerr << "Failed to create folder\n";
				return;
			}
		}

		std::string logPath = std::string(LOG_FILE_PATH) + "/log_" + GetSystemTimeForFileName() + ".txt";
		m_file = std::ofstream(logPath);
		if (!m_file)
		{
			std::cerr << "Failed to open file\n";
			return;
		}
	}

	FileLogger::~FileLogger()
	{
		m_file.close();
	}

	void FileLogger::Log(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Info] " + msg + "\n";
			m_file << strMsg;
		}
	}

	void FileLogger::LogWarning(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Warning] " + msg + "\n";
			m_file << strMsg;
		}
	}

	void FileLogger::LogError(std::string msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_file)
		{
			std::string strMsg = GetSystemTime() + " [Error] " + msg + "\n";
			m_file << strMsg;
		}
	}

	bool FileLogger::FolderExists(std::string path)
	{
		struct stat info;
		return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
	}

	bool FileLogger::CreateFolder(std::string path)
	{
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
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &local);
		return std::string(buffer);
	}

	std::string FileLogger::GetSystemTime()
	{
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &local);
		return std::string(buffer);
	}
}