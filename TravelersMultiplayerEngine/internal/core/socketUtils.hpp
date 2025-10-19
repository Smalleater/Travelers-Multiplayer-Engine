#ifndef TME_CORE_SOCKET_UTILS_HPP
#define TME_CORE_SOCKET_UTILS_HPP

#include <utility>

#include "TME/errorCode.hpp"

namespace tme::core
{
    namespace SocketUtils
    {
        int GetLastSocketError();
        bool IsWouldBlockError(int err);
    }
}

#endif