#include "sockets/TcpSocket.hpp"

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
        int iResult;

        iResult = listen(m_socket, backlog);
        if ( iResult < 0)
        {
            CloseSocket();
            return ErrorCodes::Failure;
        }

        return ErrorCodes::Success;
    }
}