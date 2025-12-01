#ifndef TRA_CORE_SOCKET_UTILS_HPP
#define TRA_CORE_SOCKET_UTILS_HPP

#include <utility>
#include <string>

#include "TRA/errorCode.hpp"
#include "TRA/core/networkInclude.hpp"

namespace tra::core
{
    namespace SocketUtils
    {
        int getLastSocketError();
        bool isWouldBlockError(int _err);
        std::pair<ErrorCode, int> setBlocking(socket_t& _socket, bool _blocking);
        std::pair<ErrorCode, uint16_t> getSocketPort(socket_t& _socket);
    }
}

#endif