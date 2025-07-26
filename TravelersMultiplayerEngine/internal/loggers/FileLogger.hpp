#ifndef TME_FILE_LOGGER_HPP
#define TME_FILE_LOGGER_HPP

#include <fstream>
#include <mutex>
#include <filesystem>

#include "interfaces/ILogger.hpp"

namespace tme
{
    /// @brief Default folder path where log files will be created.
    constexpr const char* LOG_FILE_PATH = "TmeLogs";

    /// @brief Concrete implementation of ILogger that writes logs to a file.
    ///
    /// Creates a new log file with a timestamped filename.
    /// Logging is thread-safe.
    class FileLogger : public ILogger
    {
    private:
        /// @brief Output file stream used for logging.
        std::ofstream m_file;

        /// @brief Mutex used to synchronize access to the file stream.
        std::mutex m_mutex;

        /// @brief Create the logging directory if it doesn't exist.
        /// @param path The directory path to create.
        /// @return True if the directory was created or already exists, false otherwise.
        bool CreateFolder(const std::filesystem::path& path);

        /// @brief Writes a formatted log message to the file.
        /// @param logType The type of log (e.g., "Info", "Warning", "Error").
        /// @param message The message to log.
        void Log(const std::string& logType, const std::string& message);

    public:
        /// @brief Constructor: Initializes the logger and opens the log file.
        FileLogger();

        /// @brief Destructor: Closes the log file.
        ~FileLogger();

        /// @brief Logs an informational message.
        /// @param message The message to log.
        void LogInfo(const std::string& message) override;

        /// @brief Logs a warning message.
        /// @param message The message to log.
        void LogWarning(const std::string& message) override;

        /// @brief Logs an error message.
        /// @param message The message to log.
        void LogError(const std::string& message) override;
    };
}

#endif