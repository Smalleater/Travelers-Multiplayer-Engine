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

    bool SocketUtils::IsWouldBlockError(int err)
    {
        #ifdef _WIN32
            return err == WSAEWOULDBLOCK;
        #else
            return err == EWOULDBLOCK || err == EAGAIN;
        #endif
    }
}