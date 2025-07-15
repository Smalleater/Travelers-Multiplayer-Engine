#pragma once

#include <vector>
#include <memory>

#include "Interface/ILogger.h"

namespace tme
{
	/// <summary>
	/// Composite logger that forwards log message to multiple underlying loggers
	/// </summary>
	class CompositeLogger : public ILogger
	{
	public:
		/// <summary>
		/// Constructor for CompositeLogger
		/// </summary>
		CompositeLogger() {};

		/// <summary>
		/// Destructor for CompositeLogger
		/// </summary>
		~CompositeLogger() {};

		/// <summary>
		/// Adds a logger to the internal list
		/// All subsequent log calls will be forwarded to this logger
		/// </summary>
		/// <param name="logger">Shared pointer to an ILogger instance</param>
		void addLogger(std::shared_ptr<ILogger> logger);

		/// <summary>
		/// Logs an informational message to all registered loggers
		/// This is used for normal runtime output that doesn't indicate a problem
		/// </summary>
		/// <param name="msg">Message to log</param>
		void Log(std::string msg) override;

		/// <summary>
		/// Logs warning message to all registered loggers
		/// Used to indicate a potential issue or something unexpected, but not an error
		/// </summary>
		/// <param name="msg">Warning message to log</param>
		void LogWarning(std::string msg) override;

		/// <summary>
		/// Logs an error message to all registered loggers
		/// Used when something goes wrong and needs to be reported
		/// </summary>
		/// <param name="msg">Error message to log</param>
		void LogError(std::string msg) override;

	private:
		/// <summary>
		/// List of loggers that will receive forwarded log message
		/// </summary>
		std::vector<std::shared_ptr<ILogger>> m_loggers;
	};
}
