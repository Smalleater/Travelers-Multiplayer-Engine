#include "core/socketUtils.hpp"

#include "core/networkInclude.hpp"

namespace tme::core
{
    int SocketUtils::GetLastSocketError()
    {
        #ifdef _WIN32
            return WSAGetLastError();
        #elif defined(__unix__) || defined(__APPLE__)
            return errno;
        #endif
    }
}