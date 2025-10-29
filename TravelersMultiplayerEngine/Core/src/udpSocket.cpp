#include "TME/core/udpSocket.hpp"

#include <cstdio>

#include "TME/debugUtils.hpp"
#include "TME/core/socketUtils.hpp"

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

    sockaddr* UdpSocket::createSockAddr(const std::string& _address, uint16_t _port)
    {
        TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

        sockaddr_in* addr = new sockaddr_in;
        addr->sin_family = AF_INET;
        addr->sin_port = htons(_port);

        inet_pton(AF_INET, _address.c_str(), &addr->sin_addr);
        return (sockaddr*)addr;
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

    std::pair<ErrorCode, int> UdpSocket::bindSocket(const uint16_t _port)
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
        snprintf(portStr, sizeof(portStr), "%u", _port);

        int iResult = getaddrinfo(NULL, portStr, &hints, &result);
        int lastSocketError = SocketUtils::getLastSocketError();
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

            iResult = bind(m_socket, rp->ai_addr, static_cast<int>(rp->ai_addrlen));
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

    std::pair<ErrorCode, int> UdpSocket::sendDataTo(const void* _data, size_t _size, const sockaddr* _destAddr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        if (_size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return { ErrorCode::SocketSendSizeTooLarge, 0 };
        }

        socklen_t addrLen = sizeof(sockaddr_in);

        int iResult = sendto(m_socket, static_cast<const char*>(_data), static_cast<int>(_size), 0, _destAddr, addrLen);
        int lastSocketError = SocketUtils::getLastSocketError();
        if (iResult < 0)
        {
            return { ErrorCode::SocketSendFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> UdpSocket::receiveDataFrom(void* _buffer, size_t _size, sockaddr* _srcAddr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        if (_size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return { ErrorCode::SocketReceiveSizeTooLarge, 0 };
        }

        socklen_t addrLen = sizeof(sockaddr_in);

        int iResult = recvfrom(m_socket, static_cast<char*>(_buffer), static_cast<int>(_size), 0, _srcAddr, &addrLen);
        int lastSocketError = SocketUtils::getLastSocketError();
        if (iResult < 0)
        {
            if (SocketUtils::isWouldBlockError(lastSocketError))
            {
                return { ErrorCode::SocketWouldBlock, 0 };
            }

            return { ErrorCode::SocketReceiveFailed, lastSocketError };
        }

        return { ErrorCode::Success, 0 };
    }

    std::pair<ErrorCode, int> UdpSocket::setBlocking(bool _blocking)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        return SocketUtils::setBlocking(m_socket, _blocking);
    }

    std::pair<ErrorCode, uint16_t> UdpSocket::getPort()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_socket == INVALID_SOCKET_FD)
        {
            return { ErrorCode::SocketNotOpen, 0 };
        }

        return SocketUtils::getSocketPort(m_socket);
    }

    bool UdpSocket::isOpen() const
    {
        return m_socket != INVALID_SOCKET_FD;
    }
}