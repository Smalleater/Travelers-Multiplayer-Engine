#include "core/tcpSocket.hpp"

#include <limits>
#include <cstdio>

#include "core/socketUtils.hpp"

#undef max

namespace tme::core
{
    TcpSocket::TcpSocket()
    {
        m_socket = INVALID_SOCKET_FD;
    }

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
        std::lock_guard<std::mutex> lock(m_mutex);

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

    std::pair<ErrorCode, int> TcpSocket::Connect(const char* address, uint16_t port)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket != INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketAlreadyOpen, 0 };
        }
        
        addrinfo hints = {};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo* result = nullptr;

        char portStr[6];
        snprintf(portStr, sizeof(portStr), "%u", port);

        int iResult = getaddrinfo(address, portStr, &hints, &result);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult != 0 || result == nullptr)
        {
            return { ErrorCode::SocketGetaddrinfoFailed, lastSocketError };
        }

        for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next)
        {
            m_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (m_socket == INVALID_SOCKET_FD)
            {
                continue;
            }

            iResult = connect(m_socket, rp->ai_addr, rp->ai_addrlen);
            if (iResult == 0)
            {
                freeaddrinfo(result);
                return { ErrorCode::Success, 0 };
            }

            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);
        return { ErrorCode::SocketConnectFailed, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::Bind(uint16_t port)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket != INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketAlreadyOpen, 0 };
        }

        addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        addrinfo* result = nullptr;
        
        char portStr[6];
        snprintf(portStr, sizeof(portStr), "%u", port);

        int iResult = getaddrinfo(NULL, portStr, &hints, &result);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult != 0 || result == nullptr)
        {
            return { ErrorCode::SocketGetaddrinfoFailed, lastSocketError };
        }

        for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next)
        {
            m_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (m_socket == INVALID_SOCKET_FD)
            {
                continue;
            }

            iResult = bind(m_socket, rp->ai_addr, rp->ai_addrlen);
            if (iResult == 0)
            {
                freeaddrinfo(result);
                return { ErrorCode::Success, 0 };
            }

            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);
        return { ErrorCode::SocketBindFailed, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::Listen(int backlog)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        int iResult = listen(m_socket, backlog);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if ( iResult < 0)
        {
            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
            return { ErrorCode::SocketListenFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::Accept(std::unique_ptr<TcpSocket>& outClient)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        socket_t clientSocket = accept(m_socket, NULL, NULL);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (clientSocket == INVALID_SOCKET_FD)
        {
            if (SocketUtils::IsWouldBlockError(lastSocketError))
            {
                return { ErrorCode::SocketWouldBlock, 0 };
            }

            return { ErrorCode::SocketAcceptFailed, lastSocketError };
        }

        outClient = std::make_unique<TcpSocket>();
        TcpSocket* tcpClient = static_cast<TcpSocket*>(outClient.get());
        tcpClient->m_socket = clientSocket;

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::Send(const void* data, size_t size)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return { ErrorCode::SocketSendSizeTooLarge, 0 };
        }

        int iResult = send(m_socket, static_cast<const char*>(data), static_cast<int>(size), 0);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult == 0)
        {
            return { ErrorCode::SocketConnectionClosed, 0 };
        }
        else if (iResult < 0)
        {
            return { ErrorCode::SocketSendFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::Receive(void* buffer, size_t size)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return { ErrorCode::SocketReceiveSizeTooLarge, 0 };
        }

        int iResult = recv(m_socket, static_cast<char*>(buffer), static_cast<int>(size), 0);
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult == 0)
        {
            return { ErrorCode::SocketConnectionClosed, lastSocketError };
        }
        else if (iResult < 0)
        {
            if (SocketUtils::IsWouldBlockError(lastSocketError))
            {
                return { ErrorCode::SocketWouldBlock, 0 };
            }
            else if (lastSocketError == SOCKET_CONNECTION_RESET)
            {
                return { ErrorCode::SocketConnectionClosed, 0 };
            }

            return { ErrorCode::SocketReceiveFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> TcpSocket::SetBlocking(bool blocking)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        #ifdef _WIN32
            u_long mode = blocking ? 0 : 1;

            int iResult = ioctlsocket(m_socket, FIONBIO, &mode);
            int lastSocketError = SocketUtils::GetLastSocketError();
            if (iResult != 0)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }
        #else
            int flags = fcntl(m_socket, F_GETFL, 0);
            int lastSocketError = SocketUtils::GetLastSocketError();
            if (flags == -1)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }

            if (blocking)
            {
                flags &= ~O_NONBLOCK;
            }
            else
            {
                flags |= O_NONBLOCK;
            }

            int iResult = fcntl(m_socket, F_SETFL, flags);
            lastSocketError = SocketUtils::GetLastSocketError();
            if (iResult == -1)
            {
                return { ErrorCode::SocketSetBlockingFailed, lastSocketError };
            }
        #endif

        return { ErrorCode::Success, 0 };
    }

    bool TcpSocket::IsConnected() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        return m_socket != INVALID_SOCKET_FD 
            && getpeername(m_socket, (sockaddr*)&addr, &addrLen) == 0;
    }
}