#pragma once

#include <mutex>

#include "Interface/ILogger.h"
#include "fstream"

namespace tme
{
	constexpr auto LOG_FILE_PATH = "TmeLogs";

	class FileLogger : public ILogger
	{
	public:
		FileLogger();
		~FileLogger();

		void Log(std::string msg) override;
		void LogWarning(std::string msg) override;
		void LogError(std::string msg) override;

	private:
		std::ofstream m_file;
		std::mutex m_mutex;

		bool FolderExists(std::string path);
		bool CreateFolder(std::string path);
		std::string GetSystemTimeForFileName();
		std::string GetSystemTime();
	};
}
