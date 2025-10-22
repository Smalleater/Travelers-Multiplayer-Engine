#include "socketUtils.hpp"

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

    bool SocketUtils::IsWouldBlockError(int _err)
    {
        #ifdef _WIN32
            return _err == WSAEWOULDBLOCK;
        #else
            return _err == EWOULDBLOCK || _err == EAGAIN;
        #endif
    }

    std::pair<ErrorCode, int> SocketUtils::setBlocking(core::socket_t& _socket, bool _blocking)
    {
        #ifdef _WIN32
            u_long mode = _blocking ? 0 : 1;

            int iResult = ioctlsocket(_socket, FIONBIO, &mode);
            int lastSocketError = SocketUtils::GetLastSocketError();
            if (iResult != 0)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }
        #else
            int flags = fcntl(_socket, F_GETFL, 0);
            int lastSocketError = SocketUtils::GetLastSocketError();
            if (flags == -1)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }

            if (_blocking)
            {
                flags &= ~O_NONBLOCK;
            }
            else
            {
                flags |= O_NONBLOCK;
            }

            int iResult = fcntl(_socket, F_SETFL, flags);
            lastSocketError = SocketUtils::GetLastSocketError();
            if (iResult == -1)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }
        #endif

        return { ErrorCode::Success, 0 };
    }
}