#include "core/udpSocket.hpp"

#include "core/socketUtils.hpp"

#undef max

namespace tme::core
{
    UdpSocket::UdpSocket()
    {
        m_socket = INVALID_SOCKET_FD;
    }

    UdpSocket::~UdpSocket()
    {
        if (m_socket != INVALID_SOCKET_FD)
        {
            CLOSE_SOCKET(m_socket);
        }
    }

    void UdpSocket::closeSocket()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket != INVALID_SOCKET_FD)
        {
            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }
    }

    std::pair<ErrorCode, int> UdpSocket::bindSocket(const uint16_t port)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket != INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketAlreadyOpen, 0 };
        }

        addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
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

            closeSocket();
        }

        freeaddrinfo(result);
        return { ErrorCode::SocketBindFailed, 0 };
    }

    std::pair<ErrorCode, int> UdpSocket::sendDataTo(const void* data, size_t size, const char* address, const uint16_t port)
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

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        inet_pton(AF_INET, address, &addr.sin_addr);

        int iResult = sendto(m_socket, static_cast<const char*>(data), static_cast<int>(size), 0, (const sockaddr*)&addr, sizeof(addr));
        int lastSocketError = SocketUtils::GetLastSocketError();
        if (iResult < 0)
        {
            return { ErrorCode::SocketSendFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }
}