#ifndef TME_UTILS_HPP
#define TME_UTILS_HPP

#include <string>

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
    };
}

#endif