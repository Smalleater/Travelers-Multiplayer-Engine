#include "Utils.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <cstdio>
#endif

namespace tme
{
    // Returns the current system time formatted according to the provided format string
    std::string Utils::GetSystemTime(const char* format)
    {
        // Get current time as time_t
        std::time_t currentTime = std::time(nullptr);

        // Convert to local time safely depending on the platform
        std::tm localTm;
        #ifdef _WIN32
            // Windows-specific: use localtime_s
            localtime_s(&localTm, &currentTime);
        #else
            // POSIX-specific: use localtime_r
            localtime_r(&currentTime, &localTm);
        #endif

        // Format the time using the given format
        std::ostringstream oss;
        oss << std::put_time(&localTm, format);

        // Return formatted time string
        return oss.str();
    }

    // Checks whether either stdout or stderr is connected to a console/terminal
    bool Utils::HasAnyConsoleOutput()
    {
        #ifdef _WIN32
            // On Windows, check both STD_OUTPUT_HANDLE and STD_ERROR_HANDLE
            for (DWORD stdHandle : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE})
            {
                // Get the standard handle
                HANDLE handle = GetStdHandle(stdHandle);
                if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
                {
                    // Check if the handle is character device (i.e., a console)
                    if (GetFileType(handle) == FILE_TYPE_CHAR)
                    {
                        // At least one stream is connected to a console
                        return true;
                    }
                }
            }

            // Neither stream is connected to a console
            return false;
        #else
            // On POSIX systems, use isatty to cjeck if streams are terminals
            return isatty(fileno(stdout)) || isatty(fileno(stderr));
        #endif
    }

    int Utils::GetLastSocketError()
    {
        #ifdef _WIN32
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }

    bool Utils::IsWouldBlockError(int err)
    {
        #ifdef _WIN32
            return err == WSAEWOULDBLOCK;
        #else
            return err == EWOULDBLOCK || err == EAGAIN;
        #endif
    }
}