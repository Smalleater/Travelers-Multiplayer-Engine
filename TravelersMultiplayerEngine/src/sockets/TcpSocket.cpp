#include "sockets/TcpSocket.hpp"

#include "ServiceLocator.hpp"

namespace tme
{
    void TcpSocket::CloseSocket()
    {
        #ifdef _WIN32
            closesocket(m_socket);
         #else
            close(m_socket);
        #endif

        m_socket = INVALID_SOCKET_FD;
    }

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

            CloseSocket();
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

            CloseSocket();
        }

        freeaddrinfo(result);

        return ErrorCodes::Failure;
    }

    ErrorCodes TcpSocket::Listen(int backlog = SOMAXCONN)
    {
        int iResult = listen(m_socket, backlog);
        if ( iResult < 0)
        {
            CloseSocket();
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
                ServiceLocator::Logger().LogWarning(std::string("Accept failed: ") + std::to_string(error) + "\n");
            }

            return nullptr;
        }

        std::unique_ptr<TcpSocket> client = std::make_unique<TcpSocket>();
        client->m_socket = clientSocket;

        return client;
    }
}