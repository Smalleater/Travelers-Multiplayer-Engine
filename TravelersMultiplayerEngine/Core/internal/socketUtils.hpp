#ifndef TME_CORE_SOCKET_UTILS_HPP
#define TME_CORE_SOCKET_UTILS_HPP

#include <utility>

#include "TME/errorCode.hpp"
#include "TME/core/networkInclude.hpp"

namespace tme::Core
{
    namespace SocketUtils
    {
        int GetLastSocketError();
        bool IsWouldBlockError(int _err);
        std::pair<ErrorCode, int> setBlocking(socket_t& _socket, bool _blocking);
    }
}

#endif