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

#include "ServiceLocator.hpp"

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

    void Utils::LogSocketError(const std::string& context, ErrorCodes ecResult, int lastSocketError)
    {
        ServiceLocator::Logger().LogError(context + " failed with error: " 
            + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
            + std::to_string(lastSocketError));
    }

    ErrorCodes Utils::GetCombinedErrorCode(bool hadSuccess, bool hadError)
    {
        if ((hadSuccess && !hadError) 
            || (!hadSuccess && !hadError))
        {
            return ErrorCodes::Success;
        }
        else if (hadSuccess && hadError)
        {
            return ErrorCodes::PartialSuccess;
        }
        else
        {
            return ErrorCodes::Failure;
        }
    }

    void Utils::UpdateSuccessErrorFlags(ErrorCodes ecResult, bool& hadSuccess, bool& hadError)
    {
        switch (ecResult)
        {
        case ErrorCodes::Success:
            hadSuccess = true;
            break;
        case ErrorCodes::PartialSuccess:
            hadSuccess = true;
            hadError = true;
            break;
        case ErrorCodes::Failure:
            hadError = true;
            break;
        default:
            hadError = true;
            break;
        }
    }

    template<typename Func>
    ErrorCodes Utils::TrySocketOperation(Func&& op, ISocket* socket, const std::string& context)
    {
        ErrorCodes ecResult = op();
        int lastSocketError = socket ? socket->GetLastSocketError() : 0;
        if (ecResult != ErrorCodes::Success)
        {
            LogSocketError(context, ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }
}