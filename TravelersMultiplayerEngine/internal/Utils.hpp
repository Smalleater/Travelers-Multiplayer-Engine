#ifndef TME_UTILS_HPP
#define TME_UTILS_HPP

#include <string>

#include "TME/ErrorCodes.hpp"

#include "interfaces/ISocket.hpp"

namespace tme
{
    /// @brief Utility class for system-related helper functions.
    class Utils
    {
    public:
        /// @brief Returns the current system time formatted as a string.
        /// @param format The format string (as used in strftime).
        /// @return The formatted current system time.
        static std::string GetSystemTime(const char* format);

        /// @brief Checks whether the current process has any console output (stdout or stderr).
        /// @return True if output is attached to a terminal/console, false otherwise.
        static bool HasAnyConsoleOutput();

        static int GetLastSocketError();

        static bool IsWouldBlockError(int err);

        static void LogSocketError(const std::string& context, ErrorCodes ecResult, int lastSocketError);

        static ErrorCodes GetCombinedErrorCode(bool hadSuccess, bool hadError);

        static void UpdateSuccessErrorFlags(ErrorCodes ecResult, bool& hadSuccess, bool& hadError);

        template<typename Func>
        static ErrorCodes TrySocketOperation(Func&& op, ISocket* socket, const std::string& context);
    };
}

#endif