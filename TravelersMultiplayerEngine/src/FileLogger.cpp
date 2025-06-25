#include "TME/FileLogger.h"

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

		std::string logPath = static_cast<std::string>(LOG_FILE_PATH) + "/" + GetSystemTime() + ".txt";
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

	void FileLogger::logInfo(const char* msg)
	{
		if (m_file)
		{
			std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Info] " + msg + "\n";
			m_file << strMsg;
		}
	}

	void FileLogger::logWarning(const char* msg)
	{
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Warning] " + msg + "\n";
		m_file << strMsg;
	}

	void FileLogger::logError(const char* msg)
	{
		std::string strMsg = static_cast<std::string>(GetSystemTime()) + " [Error] " + msg + "\n";
		m_file << strMsg;
	}

	bool FileLogger::FolderExists(const char* path)
	{
		struct stat info;
		return stat(path, &info) == 0 && (info.st_mode & S_IFDIR);
	}

	bool FileLogger::CreateFolder(const char* path)
	{
		std::string command = static_cast<std::string>("mkdir ") + path;

		int result = system(command.c_str());
		if (result != 0)
		{
			return false;
		}

		return true;
	}

	const char* FileLogger::GetSystemTime()
	{
		std::time_t currentTime = std::time(nullptr);
		std::tm local;
		localtime_s(&local, &currentTime);

		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "%m.%d.%Y %H.%M.%S", &local);
		return buffer;
	}
}