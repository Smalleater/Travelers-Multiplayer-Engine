#pragma once

#include <mutex>

#include "Interface/ILogger.h"
#include "fstream"

namespace tme
{
	constexpr auto LOG_FILE_PATH = "TmeLog";

	class FileLogger : public ILogger
	{
	public:
		FileLogger();
		~FileLogger();

		void logInfo(std::string msg) override;
		void logWarning(std::string msg) override;
		void logError(std::string msg) override;

	private:
		std::ofstream m_file;
		std::mutex m_mutex;

		bool FolderExists(const char* path);
		bool CreateFolder(const char* path);
		const char* GetSystemTimeForFileName();
		const char* GetSystemTime();
	};
}
