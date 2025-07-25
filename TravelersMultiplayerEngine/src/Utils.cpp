#include "TME/Utils.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace tme
{
    // Returns the current system time formatted according to the provided format string
    std::string Utils::GetSystemTime(const char* format)
    {
        // Get current time as time_t.
        std::time_t currentTime = std::time(nullptr);

        // Convert to local time safely depending on the platform
        std::tm localTm;
#ifdef _WIN32
        localtime_s(&localTm, &currentTime);
#else
        localtime_r(&currentTime, &localTm);
#endif

        // Format the time using the given format
        std::ostringstream oss;
        oss << std::put_time(&localTm, format);

        // Return formatted time string
        return oss.str();
    }
}