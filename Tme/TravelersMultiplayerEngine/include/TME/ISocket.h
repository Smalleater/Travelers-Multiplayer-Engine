#pragma once

#include <string>
#include <memory>

#include "NetworkCommon.h"
#include "ErrorCodes.h"

namespace tme
{
	class ISocket
	{
	public:
		/// <summary>
		/// Virtual destructor to ensure proper cleanup of derived logger classes
		/// </summary>
		~ISocket() = default;

		// Initialization and shutdown

		/// <summary>
		/// Initializes the socket and prepares it for use
		/// Typically includes creating the socket and setting necessary options
		/// </summary>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Init() = 0;

		/// <summary>
		/// Shuts down and closes the socket, releasing any allocated resources
		/// Should be called when the socket is no longer needed
		/// </summary>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Shutdown() = 0;

		// Connection

		/// <summary>
		/// Connects the socket to a remite address and port (for client sockets)
		/// </summary>
		/// <param name="address">The IP adress or hostname of the remote server</param>
		/// <param name="port">The port number to connect to</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Connect(const std::string& address, uint16_t port) = 0;
		
		// Listening

		/// <summary>
		/// Binds the socket to a local port, preparing it to accept incoming connections
		/// </summary>
		/// <param name="port">The local port number to bind to</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Bind(uint16_t port) = 0;

		/// <summary>
		/// Start listening for incoming connection requests with a specified backlog
		/// </summary>
		/// <param name="backlog">Maximum number of pending connections allowed int the queue</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Listen(int backlog = SOMAXCONN) = 0;

		/// <summary>
		/// Accepts an incoming connection request, returning a new socket instance
		/// representing the client connection
		/// </summary>
		/// <returns>A unique_ptr to the accepted socket if successful, or nullptr on failure</returns>
		virtual std::unique_ptr<ISocket> Accept() = 0;

		// Sending and Receiving

		/// <summary>
		/// Sends data through the socket
		/// </summary>
		/// <param name="data">Pointer to the buffer containing the data to send</param>
		/// <param name="size">Number of bytes to send</param>
		/// <param name="bytesSent">Output parameter receiving the actual number of bytes sent</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Send(const void* data, size_t size, int& bytesSent) = 0;

		/// <summary>
		/// Receives data from the socket
		/// </summary>
		/// <param name="buffer">Pointer to the buffer where received data will be stored</param>
		/// <param name="size">Maximum number of bytes to receive</param>
		/// <param name="bytesReceived">Output parameter receiving the actual number of bytes received</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError Receive(void* buffer, size_t size, int& bytesReceived) = 0;

		// Options

		/// <summary>
		/// Sets the blocking mode of the socket
		/// </summary>
		/// <param name="blocking">If true, socket operation will block until complete</param>
		/// <returns>Success if successful, otherwise an error code indicating the failure reason</returns>
		virtual NetworkError SetBlocking(bool blocking) = 0;

		/// <summary>
		/// Checks if the socket is currently connected
		/// </summary>
		/// <returns>True if connected, false otherwise</returns>
		virtual bool IsConnected() const = 0;

		/// <summary>
		/// Retrieves the underlying native socket handle (e.g., SOCKET on Windows)
		/// Useful for low-level operations or integration with platform-specific APIs
		/// </summary>
		/// <returns>The native socket handle</returns>
		virtual SOCKET GetNativeSocket() const = 0;
	};
}
