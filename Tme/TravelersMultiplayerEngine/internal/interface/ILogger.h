#pragma once

#include <string>

namespace tme
{
	/// <summary>
	/// Interface for logging messages in various severity levels (info, warning, error)
	/// </summary>
	class ILogger
	{
	public:
		/// <summary>
		/// Virtual destructor ro ensure proper cleanup of derived logger classes
		/// </summary>
		~ILogger() = default;

		/// <summary>
		/// Logs a general informational message
		/// This is used for normal runtime output that doesn't indicate a problem
		/// </summary>
		/// <param name="msg">The message to log</param>
		virtual void Log(std::string msg) = 0;

		/// <summary>
		/// Logs a warning message
		/// Used to indicate a potential issue or something unexpected, but not an error
		/// </summary>
		/// <param name="msg">The warning message to log</param>
		virtual void LogWarning(std::string msg) = 0;

		/// <summary>
		/// Logs an error message
		/// Used when something goes wrong and needs to be reported
		/// </summary>
		/// <param name="msg">The error message to log</param>
		virtual void LogError(std::string msg) = 0;
	};
}
