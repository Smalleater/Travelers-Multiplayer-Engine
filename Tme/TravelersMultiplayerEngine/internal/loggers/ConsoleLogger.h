#pragma once

#include <mutex>

#include "Interface/ILogger.h"

namespace tme
{
	/// <summary>
	/// A simple logger that writes messages to the standard output (console)
	/// Thread-safe using an internal mutex
	/// </summary>
	class ConsoleLogger : public ILogger 
	{
	public:
		/// <summary>
		/// Constructor for ConsoleLogger
		/// Currently does nothing
		/// </summary>
		ConsoleLogger() {};

		/// <summary>
		/// Destructor for ConsoleLogger
		/// Currently does nothing
		/// </summary>
		~ConsoleLogger() {};

		/// <summary>
		/// Logs a general informational message to the console
		/// This is used for normal runtime output that doesn't indicate a problem
		/// </summary>
		/// <param name="msg">The message to log</param>
		void Log(std::string msg) override;

		/// <summary>
		/// Logs a warning message to the console
		/// Used to indicate a potential issue or something unexpected, but not an error
		/// </summary>
		/// <param name="msg">The warning message to log</param>
		void LogWarning(std::string msg) override;

		/// <summary>
		/// Logs an error message to the console
		/// Used when something goes wrong and needs to be reported
		/// </summary>
		/// <param name="msg">The error message to log</param>
		void LogError(std::string msg) override;

	private:
		/// <summary>
		/// Mutex to ensure thread-safe console output
		/// </summary>
		std::mutex m_mutex;

		/// <summary>
		/// Returns the current system time as a formatted string
		/// Used for timestamping log messages
		/// </summary>
		/// <returns>A string representing the current system time</returns>
		std::string GetSystemTime();
	};
}
