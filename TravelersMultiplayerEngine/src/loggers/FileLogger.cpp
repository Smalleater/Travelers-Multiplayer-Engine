#include "loggers/FileLogger.hpp"

#include <iostream>

#include "TME/Utils.hpp"

namespace tme
{
    // Attempts to create the specified folder path
    bool FileLogger::CreateFolder(const std::filesystem::path& path)
    {
        std::error_code ec;

        // Try to create the folder (including parent directories)
        bool created = std::filesystem::create_directories(path, ec);
        if (ec)
        {
            // Log any error that occured during creation
            std::cerr << "Error creating folder: " << ec.message() << std::endl;
            return false;
        }

        return created;
    }

    // Log a message with the given log type (e.g., info, Warning, Error)
    void FileLogger::Log(const std::string& logType, const std::string& message)
    {
        // Lock to ensure thread-safe file access
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_file)
        {
            // Format the log entry with a timestamp and log type
            std::string logMessage = Utils::GetSystemTime("[%Y-%m-%d %H:%M:%S]") + " [" + logType + "] " + message + "\n";

            // Write the log message to the file
            m_file << logMessage;
        }
    }

    // Constructor: Prepares log directory and opens a new timestamped log file
    FileLogger::FileLogger()
    {
        // Check if the log path exists, and create it needed
        if (std::filesystem::is_directory(LOG_FILE_PATH))
        {
            if (!CreateFolder(LOG_FILE_PATH))
            {
                std::cerr << "Failed to create folder at path: " << LOG_FILE_PATH << std::endl; 
                return;
            }
        }

        // Generate log file name with current timestamp
        std::string logFilePath = std::string(LOG_FILE_PATH) + "/log_" + Utils::GetSystemTime("%Y-%m-%d_%H-%M-%S") + ".txt";

        // Open the file for logging
        m_file = std::ofstream(logFilePath);
        if (!m_file)
        {
            std::cerr << "Failed to open file at path: " << logFilePath << std::endl;
            return;
        }
    }

    // Destructor: Closes the log file
    FileLogger::~FileLogger()
    {
        m_file.close();
    }

    // Logs an informational message
    void FileLogger::LogInfo(const std::string& message)
    {
        Log("Info", message);
    }

    // Logs a warning message
    void FileLogger::LogWarning(const std::string& message)
    {
        Log("Warning", message);
    }

    // Logs an error message
    void FileLogger::LogError(const std::string& message)
    {
        Log("Error", message);
    }
}