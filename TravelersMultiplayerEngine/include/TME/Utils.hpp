#ifndef TME_UTILS_HPP
#define TME_UTILS_HPP

#include "Export.hpp"

#include <string>

namespace tme
{
    /// @brief Utility class for system-related helper functions.
    class TME_API Utils
    {
    public:
    /// @brief Returns the current system time formatted as a string.
    /// @param format The format string (as used in strftime).
    /// @return The formatted current system time.
    static std::string GetSystemTime(const char* format);
    };
}

#endif