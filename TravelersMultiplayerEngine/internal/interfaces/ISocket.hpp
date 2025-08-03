#ifndef TME_I_SOCKET_HPP
#define TME_I_SOCKET_HPP

#include <string>
#include <memory>

#include "TME/ErrorCodes.hpp"

#include "NetworkCommon.hpp"

namespace tme
{
    /// @brief Interface for socket operations (TCP/UDP).
    ///
    /// Provides an abstraction for socket communication, supporting
    /// initialization, connection, data transmission, and socket state management.
    class ISocket
    {
    public:
        /// @brief Virtual destructor for safe polymorphic use.
        virtual ~ISocket() = default;

        /// @brief Shuts down the socket and releases resources.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Shutdown() = 0;

        /// @brief Connects the socket to a remote address and port.
        /// @param address The remote IP address or hostname.
        /// @param port The remote port number. 
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Connect(const std::string& address, uint16_t port) = 0;

        /// @brief Binds the socket to a local port.
        /// @param port The local port number.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Bind(uint16_t port) = 0;

        /// @brief Puts the socket into listening mode for incoming connections.
        /// @param backlog The maximum length of the pending connections queue.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Listen(int backlog = SOMAXCONN) = 0;

        /// @brief Accepts an incoming connection and returns a new socket for it.
        /// @return Unique pointer to the accepted socket, or nullptr on failure. 
        virtual std::unique_ptr<ISocket> Accept() = 0;

        /// @brief Sends data through the socket.
        /// @param data Pointer to the data buffer.
        /// @param size Number of bytes to send.
        /// @param bytesSent Reference to store the number of bytes actually sent.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Send(const void* data, size_t size, int& bytesSent) = 0;

        /// @brief Receives data from the socket.
        /// @param buffer Pointer to the buffer to store received data.
        /// @param size Maximum number of bytes to receive.
        /// @param bytesReceived Reference to store the number of bytes actually received.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes Receive(void* buffer, size_t size, int& bytesReceived) = 0;

        /// @brief Sets the socket to blocking or non-blocking mode.
        /// @param blocking True for blocking mode, false for non-blocking.
        /// @return ErrorCodes::Success on success, or an error code on failure.
        virtual ErrorCodes SetBlocking(bool blocking) = 0;

        /// @brief Checks if the socket is currently connected.
        /// @return True if connected, false otherwise. 
        virtual bool IsConnected() const = 0;

        /// @brief Returns the native socket handle.
        /// @return The underlying socket descriptor/handle.
        virtual socket_t GetNativeSocket() const = 0;
    }; 
}

#endif