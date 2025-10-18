#include "core/tcpSocket.hpp"

#include "core/socketUtils.hpp"

namespace tme::core
{
    TcpSocket::~TcpSocket()
    {
        if (m_socket != INVALID_SOCKET_FD)
        {
            Shutdown();
            CLOSE_SOCKET(m_socket);
        }
    }

    std::pair<ErrorCode, int> TcpSocket::Shutdown()
    {
        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::Success, 0 };
        }

        int iResult = shutdown(m_socket, SHUTDOWN_BOTH);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult != 0 && lastSocketError != SOCKET_NOT_CONNECTED)
        {
            return { ErrorCode::SocketShutdownFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }
}