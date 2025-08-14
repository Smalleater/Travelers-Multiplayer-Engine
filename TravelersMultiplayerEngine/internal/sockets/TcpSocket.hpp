#ifndef TME_TCP_SOCKET_HPP
#define TME_TCP_SOCKET_HPP

#include "TME/ErrorCodes.hpp"

#include "interfaces/ISocket.hpp"

namespace tme
{
    /// @brief TCP socket implementation.
    ///
    /// Provides TCP-specific socket operations, including connection,
    /// binding, listening, accepting, sending, and receiving data.
    /// Inherits from ISocket for polymorphic use.
    class TcpSocket : public ISocket
    {
    private:
        /// @brief Native socket handle.
        socket_t m_socket = INVALID_SOCKET_FD;

        /// @brief Last socket error code.
        int m_lastSocketError = 0;

    public:
        /// @brief Constructs a new TcpSocket instance.
        TcpSocket() {};

        /// @brief Destructor. Shuts down and closes the socket if open.
        ~TcpSocket();

        /// @brief Shuts down the socket and releases resources.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Shutdown() override;

        /// @brief Connects the socket to a remote address and port.
        /// @param address The remote IP address or hostname.
        /// @param port The remote port number.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Connect(const std::string& address, uint16_t port) override;

        /// @brief Binds the socket to a local port.
        /// @param port The local port number.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Bind(uint16_t port) override;

        /// @brief Puts the socket into listening mode for incoming connections.
        /// @param backlog The maximum length of the pending connections queue.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Listen(int backlog = SOMAXCONN) override;
        
        /// @brief Accepts an incoming connection and returns a new TcpSocket for it.
        /// @return Unique pointer to the accepted socket, or nullptr on failure.
        ErrorCodes Accept(std::unique_ptr<ISocket>& outClient) override;

        /// @brief Sends data through the socket.
        /// @param data Pointer to the data buffer.
        /// @param size Number of bytes to send.
        /// @param bytesSent Reference to store the number of bytes actually sent.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Send(const void* data, size_t size, int& bytesSent) override;

        /// @brief Receives data from the socket.
        /// @param buffer Pointer to the buffer to store received data.
        /// @param size Maximum number of bytes to receive.
        /// @param bytesReceived Reference to store the number of bytes actually received.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes Receive(void* buffer, size_t size, int& bytesReceived) override;

        /// @brief Sets the socket to blocking or non-blocking mode.
        /// @param blocking True for blocking mode, false for non-blocking.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        ErrorCodes SetBlocking(bool blocking) override;

        /// @brief Checks if the socket is currently connected.
        /// @return True if connected, false otherwise.
        bool IsConnected() const override;

        /// @brief Returns the native socket handle.
        /// @return The underlying socket descriptor/handle.
        socket_t GetNativeSocket() const override;

        int GetLastSocketError() const override;
    };
}

#endif