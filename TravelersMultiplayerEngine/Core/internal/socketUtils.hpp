#ifndef TME_CORE_SOCKET_UTILS_HPP
#define TME_CORE_SOCKET_UTILS_HPP

#include "TME/export.hpp"

#include <utility>

#include "TME/errorCode.hpp"
#include "TME/core/networkInclude.hpp"

namespace tme::core
{
    namespace SocketUtils
    {
        TME_API std::pair<ErrorCode, uint16_t> getSocketPort(core::socket_t& _socket);

        int getLastSocketError();
        bool isWouldBlockError(int _err);
        std::pair<ErrorCode, int> setBlocking(socket_t& _socket, bool _blocking);
    }
}

#endif