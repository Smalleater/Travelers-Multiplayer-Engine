#include "sockets/TcpSocket.hpp"

#include <limits>

#include "ServiceLocator.hpp"
#include "Utils.hpp"

#undef max

namespace tme
{
    // Destructor: shuts down and closes the socket if it's valid
    TcpSocket::~TcpSocket()
    {
        if (m_socket != INVALID_SOCKET_FD)
        {
            Shutdown();
            CLOSE_SOCKET(m_socket);
        }
    }

    // Gracefully shuts down the socket for both send and receive
    ErrorCodes TcpSocket::Shutdown()
    {
        if (m_socket == INVALID_SOCKET_FD)
        {
            return ErrorCodes::InvalidSocket;
        }

        int iResult;
        #ifdef _WIN32
            iResult = shutdown(m_socket, SD_BOTH);
        #else
            iResult = shutdown(m_socket, SHUT_RDWR);
        #endif

        if (iResult != 0)
        {
            return ErrorCodes::ShutdownFailed;
        }

        return ErrorCodes::Success;
    }

    // Connects the socket to the specified address and port
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
            return ErrorCodes::AddrInfoFailed;
        }

        // Try each address until a connection succeeds
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

            ServiceLocator::Logger().LogError("TcpSocket::Connect: connect failed with error: " 
                + std::to_string(Utils::GetLastSocketError()));

            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);

        return ErrorCodes::ConnectFailed;
    }

    // Binds the socket to the specified port for listening
    ErrorCodes TcpSocket::Bind(uint16_t port)
    {
        addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        addrinfo* result = nullptr;
        std::string portStr = std::to_string(port);

        int iResult;

        iResult = getaddrinfo(NULL, portStr.c_str(), &hints, &result);
        if (iResult != 0 || result == nullptr)
        {
            return ErrorCodes::AddrInfoFailed;
        }

        // Try each address until binding succeeds
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
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);

        return ErrorCodes::BindFailed;
    }

    // Puts the socket into listening mode with the specified backlog
    ErrorCodes TcpSocket::Listen(int backlog)
    {
        int iResult = listen(m_socket, backlog);
        if ( iResult < 0)
        {
            CLOSE_SOCKET(m_socket);
            return ErrorCodes::ListenFailed;
        }

        return ErrorCodes::Success;
    }

    // Accepts an incoming connection and returns a new TcpSocket for the client
    std::unique_ptr<ISocket> TcpSocket::Accept()
    {
        socket_t clientSocket = accept(m_socket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET_FD)
        {
            int error = Utils::GetLastSocketError();
            if (error != WOULD_BLOCK_ERROR)
            {
                ServiceLocator::Logger().LogWarning(std::string("Accept failed: error code = ") 
                    + std::to_string(error));
            }

            return nullptr;
        }

        std::unique_ptr<TcpSocket> client = std::make_unique<TcpSocket>();
        client->m_socket = clientSocket;

        return client;
    }

    // Sends data through the socket
    ErrorCodes TcpSocket::Send(const void* data, size_t size, int& bytesSent)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            ServiceLocator::Logger().LogError("Send failed: size exceeds maximum allowed by system call");
            bytesSent = 0;
            return ErrorCodes::SendSizeTooLarge;
        }

        int iResult = send(m_socket, static_cast<const char*>(data), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            ServiceLocator::Logger().LogWarning("Send failed: connection closed by peer");
            bytesSent = 0;
            return ErrorCodes::SendConnectionClosed;
        }
        else if (iResult < 0)
        {
            ServiceLocator::Logger().LogError(std::string("Send failed: error code = ") 
                + std::to_string(Utils::GetLastSocketError()));
            bytesSent = 0;
            return ErrorCodes::SendFailed;
        }

        bytesSent = iResult;
        return ErrorCodes::Success;
    }

    // Receives data from the socket
    ErrorCodes TcpSocket::Receive(void* buffer, size_t size, int& bytesReceived)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            ServiceLocator::Logger().LogError("Receive failed: size exceeds maximum allowed by system call");
            bytesReceived = 0;
            return ErrorCodes::ReceiveSizeTooLarge;
        }

        int iResult = recv(m_socket, static_cast<char*>(buffer), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            ServiceLocator::Logger().LogWarning("Receive failed: connection closed by peer");
            bytesReceived = 0;
            return ErrorCodes::ReceiveConnectionClosed;
        }
        else if (iResult < 0)
        {
            int err = Utils::GetLastSocketError();
            if (Utils::IsWouldBlockError(err))
            {
                bytesReceived = 0;
                return ErrorCodes::ReceiveWouldBlock;
            }

            ServiceLocator::Logger().LogError(std::string("Receive failed: error code = ") 
                + std::to_string(err));
            bytesReceived = 0;
            return ErrorCodes::ReceiveFailed;
        }

        bytesReceived = iResult;
        return ErrorCodes::Success;
    }

    // Sets the socket to blocking or non-blocking mode
    ErrorCodes TcpSocket::SetBlocking(bool blocking)
    {
        #ifdef _WIN32
            u_long mode = blocking ? 0 : 1;

            int iResult = ioctlsocket(m_socket, FIONBIO, &mode);
            if (iResult != 0)
            {
                ServiceLocator::Logger().LogError("SetBlocking failed: ioctlsocket error");
                return ErrorCodes::SetBlockingFailed;
            }
        #else
            int flags = fcntl(m_socket, F_GETFL, 0);
            if (flags == -1)
            {
                ServiceLocator::Logger().LogError("SetBlocking failed: fcntl F_GETFL error");
                return ErrorCodes::SetBlockingFailed;
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
            if (iResult == -1)
            {
                ServiceLocator::Logger().LogError("SetBlocking failed: fcntl F_SETFL error");
                return ErrorCodes::SetBlockingFailed;
            }
        #endif

        return ErrorCodes::Success;
    }

    // Checks if the socket is currently connected
    bool TcpSocket::IsConnected() const
    {
        sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        return m_socket != INVALID_SOCKET_FD 
            && getpeername(m_socket, (sockaddr*)&addr, &addrLen) == 0;
    }

    // Returns the native socket handle
    socket_t TcpSocket::GetNativeSocket() const
    {
        return m_socket;
    }
}