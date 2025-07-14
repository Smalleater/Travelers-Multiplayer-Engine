#pragma once

#include <mutex>
#include <fstream>

#include "Interface/ILogger.h"

namespace tme
{
	/// <summary>
	/// Path to the folder where log files will be stored
	/// </summary>
	constexpr auto LOG_FILE_PATH = "TmeLogs";

	/// <summary>
	/// A thread-safe logger that writes log message to a file
	/// Handles folder creation and timestamps each entry
	/// </summary>
	class FileLogger : public ILogger
	{
	public:
		FileLogger();
		~FileLogger();

		/// <summary>
		/// Logs an informational message to the file
		/// This is used for normal runtime output that doesn't indicate a problem
		/// </summary>
		/// <param name="msg">The message to log</param>
		void Log(std::string msg) override;

		/// <summary>
		/// Logs a warning message to the file
		/// Used to indicate a potential issue or something unexpected, but not an error
		/// </summary>
		/// <param name="msg">The warning message to log</param>
		void LogWarning(std::string msg) override;

		/// <summary>
		/// Logs a arror message to the file
		/// Used when something goes wrong and needs to be reported
		/// </summary>
		/// <param name="msg">The error message to log</param>
		void LogError(std::string msg) override;

	private:
		/// <summary>
		/// Output stream for writing to the log file
		/// </summary>
		std::ofstream m_file;

		/// <summary>
		/// Mutex to ensure thread-safe file access
		/// </summary>
		std::mutex m_mutex;

		/// <summary>
		/// Checks if the specified folder exists
		/// </summary>
		/// <param name="path">Path to the folder</param>
		/// <returns>True if the folder exists, false otherwise</returns>
		bool FolderExists(std::string path);

		/// <summary>
		/// Creates a folder at the specified path if it doesn't exist
		/// </summary>
		/// <param name="path">Path where the folder should be created</param>
		/// <returns>True if the folder was created successfully or already exists</returns>
		bool CreateFolder(std::string path);

		/// <summary>
		/// Returns the current system time formatted for file names
		/// </summary>
		/// <returns>A string representing the current system time</returns>
		std::string GetSystemTimeForFileName();

		/// <summary>
		/// Returns the current system time formatted for log messages
		/// </summary>
		/// <returns>A string representing the current system time</returns>
		std::string GetSystemTime();
	};
}
