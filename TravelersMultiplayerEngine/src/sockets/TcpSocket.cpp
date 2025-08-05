#include "sockets/TcpSocket.hpp"

#include <limits>

#include "ServiceLocator.hpp"

#undef max

namespace tme
{
    int TcpSocket::GetLastSocketError()
    {
        #ifdef _WIN32
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }

    ErrorCodes TcpSocket::Connect(const std::string& address, uint16_t port)
    {
        addrinfo hints = {};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo* result = nullptr;
        std::string portStr = std::to_string(port);

        int iResult;

        iResult = getaddrinfo(address.c_str(), portStr.c_str(), &hints, &result);
        if (iResult != 0 || result == nullptr)
        {
            return ErrorCodes::Failure;
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
                return ErrorCodes::Success;
            }

            CLOSE_SOCKET(m_socket);
        }

        freeaddrinfo(result);

        return ErrorCodes::Failure;
    }

    ErrorCodes TcpSocket::Bind(uint16_t port)
    {
        addrinfo hints = {};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        addrinfo* result = nullptr;
        std::string portStr = std::to_string(port);

        int iResult;

        iResult = getaddrinfo(NULL, portStr.c_str(), &hints, &result);
        if (iResult != 0 || result == nullptr)
        {
            return ErrorCodes::Failure;
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
                return ErrorCodes::Success;
            }

            CLOSE_SOCKET(m_socket);
        }

        freeaddrinfo(result);

        return ErrorCodes::Failure;
    }

    ErrorCodes TcpSocket::Listen(int backlog)
    {
        int iResult = listen(m_socket, backlog);
        if ( iResult < 0)
        {
            CLOSE_SOCKET(m_socket);
            return ErrorCodes::Failure;
        }

        return ErrorCodes::Success;
    }

    std::unique_ptr<ISocket> TcpSocket::Accept()
    {
        socket_t clientSocket = accept(m_socket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET_FD)
        {
            int error = GetLastSocketError();
            if (error == WOULD_BLOCK_ERROR)
            {
                ServiceLocator::Logger().LogWarning(std::string("Accept failed: error code = ") + std::to_string(error) + "\n");
            }

            return nullptr;
        }

        std::unique_ptr<TcpSocket> client = std::make_unique<TcpSocket>();
        client->m_socket = clientSocket;

        return client;
    }

    ErrorCodes TcpSocket::Send(const void* data, size_t size, int& bytesSent)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            ServiceLocator::Logger().LogError("Send failed: size exceeds maximum allowed by system call\n");
            bytesSent = 0;
            return ErrorCodes::Failure;
        }

        int iResult = send(m_socket, static_cast<const char*>(data), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            ServiceLocator::Logger().LogWarning("Send failed: connection closed by peer\n");
            bytesSent = 0;
            return ErrorCodes::Failure;
        }
        else if (iResult < 0)
        {
            ServiceLocator::Logger().LogError(std::string("Send failed: error code = ") + std::to_string(GetLastSocketError()) + "\n");
            bytesSent = 0;
            return ErrorCodes::Failure;
        }

        bytesSent = iResult;
        return ErrorCodes::Success;
    }

    ErrorCodes TcpSocket::Receive(void* buffer, size_t size, int& bytesReceived)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            ServiceLocator::Logger().LogError("Receive failed: size exceeds maximum allowed by system call\n");
            bytesReceived = 0;
            return ErrorCodes::Failure;
        }

        int iResult = recv(m_socket, static_cast<char*>(buffer), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            ServiceLocator::Logger().LogWarning("Receive failed: connection closed by peer");
            bytesReceived = 0;
            return ErrorCodes::Failure;
        }
        else if (iResult < 0)
        {
            ServiceLocator::Logger().LogError(std::string("Receive failed: error code = ") + std::to_string(GetLastSocketError()) + "\n");
            bytesReceived = 0;
            return ErrorCodes::Failure;
        }

        bytesReceived = iResult;
        return ErrorCodes::Success;
    }

    ErrorCodes TcpSocket::SetBlocking(bool blocking)
    {
        u_long mode = blocking;
        ioctlsocket(m_socket, FIONBIO, &mode);
        return ErrorCodes::Success;
    }
}