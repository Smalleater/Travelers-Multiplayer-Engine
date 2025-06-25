#pragma once

#include "ILogger.h"
#include "fstream"

namespace tme
{
	constexpr auto LOG_FILE_PATH = "TmeLog";

	class FileLogger : public ILogger
	{
	public:
		FileLogger();
		~FileLogger();

		void logInfo(const char* msg) override;
		void logWarning(const char* msg) override;
		void logError(const char* msg) override;

	private:
		std::ofstream m_file;

		bool FolderExists(const char* path);
		bool CreateFolder(const char* path);
		const char* GetSystemTime();
	};
}
