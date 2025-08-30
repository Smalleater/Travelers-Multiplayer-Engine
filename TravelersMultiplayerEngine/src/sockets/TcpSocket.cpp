#include "sockets/TcpSocket.hpp"

#include <limits>

#include "ServiceLocator.hpp"
#include "Utils.hpp"

#undef max

namespace tme
{
    // Destructor: Ensures the socket is gracefully shut down and closed if valid
    TcpSocket::~TcpSocket()
    {
        if (m_socket != INVALID_SOCKET_FD)
        {
            Shutdown();
            CLOSE_SOCKET(m_socket);
        }
    }

    // Gracefully shuts down the socket for both sending and receiving operations
    ErrorCodes TcpSocket::Shutdown()
    {
        if (m_socket == INVALID_SOCKET_FD)
        {
            m_lastSocketError = 0;
            return ErrorCodes::InvalidSocket;
        }

        int iResult;
        iResult = shutdown(m_socket, SHUTDOWN_BOTH);
        if (iResult != 0)
        {
            m_lastSocketError = Utils::GetLastSocketError();
            if (m_lastSocketError == SOCKET_NOT_CONNACTED)
            {
                return ErrorCodes::Success;
            }
            
            return ErrorCodes::ShutdownFailed;
        }

        m_lastSocketError = 0;
        return ErrorCodes::Success;
    }

    // Attempts to connect the socket to the given address and port
    // Tries all resolved addresses until successful
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
            m_lastSocketError = Utils::GetLastSocketError();
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
                m_lastSocketError = 0;
                freeaddrinfo(result);
                return ErrorCodes::Success;
            }

            m_lastSocketError = Utils::GetLastSocketError();
            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);
        return ErrorCodes::ConnectFailed;
    }

    // Binds the socket to the specified port, preparing it to accept incoming connections
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
            m_lastSocketError = Utils::GetLastSocketError();
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
                m_lastSocketError = 0;
                freeaddrinfo(result);
                return ErrorCodes::Success;
            }
            
            m_lastSocketError = Utils::GetLastSocketError();
            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
        }

        freeaddrinfo(result);
        return ErrorCodes::BindFailed;
    }

    // Sets the socket to listen for incoming connections, with the specified backlog limit
    ErrorCodes TcpSocket::Listen(int backlog)
    {
        int iResult = listen(m_socket, backlog);
        if ( iResult < 0)
        {
            m_lastSocketError = Utils::GetLastSocketError();
            CLOSE_SOCKET(m_socket);
            m_socket = INVALID_SOCKET_FD;
            return ErrorCodes::ListenFailed;
        }

        m_lastSocketError = 0;
        return ErrorCodes::Success;
    }

    // Accepts an incoming connection and assigns a new TcpSocket instance to outClient
    ErrorCodes TcpSocket::Accept(std::unique_ptr<ISocket>& outClient)
    {
        socket_t clientSocket = accept(m_socket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET_FD)
        {
            int err = Utils::GetLastSocketError();
            m_lastSocketError = err;
            if (Utils::IsWouldBlockError(err))
            {
                return ErrorCodes::AcceptWouldBlock;
            }

            return ErrorCodes::AcceptFailed;
        }

        outClient = std::make_unique<TcpSocket>();
        TcpSocket* tcpClient = static_cast<TcpSocket*>(outClient.get());
        tcpClient->m_socket = clientSocket;
        tcpClient->m_lastSocketError = 0;

        return ErrorCodes::Success;
    }

    // Sends data through the socket
    // Returns the number of bytes sent in bytesSent
    ErrorCodes TcpSocket::Send(const void* data, size_t size, int& bytesSent)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            m_lastSocketError = 0;
            bytesSent = 0;
            return ErrorCodes::SendSizeTooLarge;
        }

        int iResult = send(m_socket, static_cast<const char*>(data), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            m_lastSocketError = Utils::GetLastSocketError();
            bytesSent = 0;
            return ErrorCodes::SendConnectionClosed;
        }
        else if (iResult < 0)
        {
            m_lastSocketError = Utils::GetLastSocketError();
            bytesSent = 0;
            return ErrorCodes::SendFailed;
        }

        m_lastSocketError = 0;
        bytesSent = iResult;
        return ErrorCodes::Success;
    }

    // Receives data from the socket
    // Returns the number of bytes received in bytesReceived
    ErrorCodes TcpSocket::Receive(void* buffer, size_t size, int& bytesReceived)
    {
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            m_lastSocketError = 0;
            bytesReceived = 0;
            return ErrorCodes::ReceiveSizeTooLarge;
        }

        int iResult = recv(m_socket, static_cast<char*>(buffer), static_cast<int>(size), 0);
        if (iResult == 0)
        {
            m_lastSocketError = Utils::GetLastSocketError();
            bytesReceived = 0;
            return ErrorCodes::ReceiveConnectionClosed;
        }
        else if (iResult < 0)
        {
            int err = Utils::GetLastSocketError();
            m_lastSocketError = err;
            if (Utils::IsWouldBlockError(err))
            {
                bytesReceived = 0;
                return ErrorCodes::ReceiveWouldBlock;
            }

            bytesReceived = 0;
            return ErrorCodes::ReceiveFailed;
        }

        m_lastSocketError = 0;
        bytesReceived = iResult;
        return ErrorCodes::Success;
    }

    // Configures the socket to operate in blocking or non-blocking mode
    ErrorCodes TcpSocket::SetBlocking(bool blocking)
    {
        #ifdef _WIN32
            u_long mode = blocking ? 0 : 1;

            int iResult = ioctlsocket(m_socket, FIONBIO, &mode);
            if (iResult != 0)
            {
                m_lastSocketError = Utils::GetLastSocketError();
                return ErrorCodes::SetBlockingFailed;
            }
        #else
            int flags = fcntl(m_socket, F_GETFL, 0);
            if (flags == -1)
            {
                m_lastSocketError = Utils::GetLastSocketError();
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
                m_lastSocketError = Utils::GetLastSocketError();
                return ErrorCodes::SetBlockingFailed;
            }
        #endif

        m_lastSocketError = 0;
        return ErrorCodes::Success;
    }

    // Returns true if the socket is currently connected to a remote endpoint
    bool TcpSocket::IsConnected() const
    {
        sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        return m_socket != INVALID_SOCKET_FD 
            && getpeername(m_socket, (sockaddr*)&addr, &addrLen) == 0;
    }

    // Retrieves the underlying native socket handle
    socket_t TcpSocket::GetNativeSocket() const
    {
        return m_socket;
    }

    int TcpSocket::GetLastSocketError() const
    {
        return m_lastSocketError;
    }
}