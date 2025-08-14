#ifndef TME_NETWORK_MANAGER_HPP
#define TME_NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "TME/ErrorCodes.hpp"

#ifdef _WIN32
#include "WsaInitializer.hpp"
#endif

#include "sockets/TcpSocket.hpp"

namespace tme
{
    /// @brief Manages network operations for both server and client modes.
    ///
    /// This class handles the initialization, updating, and communication
    /// for TCP networking, supporting both server and client roles.
    class NetworkManager
    {
    private:
        #ifdef _WIN32
            /// @brief Windows Sockets API initializer (Windows only).
            std::unique_ptr<WsaInitializer> m_wsa;
        #endif

        /// @brief TCP socket for server operations.
        std::unique_ptr<TcpSocket> m_serverTcpSocket;

        /// @brief TCP socket for client operations.
        std::unique_ptr<TcpSocket> m_clientTcpSocket;

        /// @brief Map of connected clients (server-side), indexed by network ID.
        std::unordered_map<uint32_t, std::unique_ptr<TcpSocket>> m_clients;

        /// @brief Next available network ID for new clients.
        uint32_t m_nextNetworkId = 0;

        /// @brief Updates the server state (accepts clients, receives data, etc.).
        /// @return Error code indicating the result of the update.
        ErrorCodes UpdateServer();

        /// @brief Updates the client state (receives data, etc.).
        /// @return Error code indicating the result of the update.
        ErrorCodes UpdateClient();

    public:
        /// @brief Constructs a new NetworkManager object.
        NetworkManager() {}

        /// @brief Destroys the NetworkManager object and cleans up resources.
        ~NetworkManager() {}

        /// @brief Starts the server on the specified port.
        /// @param port The port to listen on.
        /// @return Error code indicating success or failure.
        ErrorCodes StartServer(uint16_t port);

        /// @brief Starts the client and connects to the specified address and port.
        /// @param address The server address to connect to.
        /// @param port The server port to connect to.
        /// @return Error code indicating success or failure.
        ErrorCodes StartClient(const std::string& address, uint16_t port);

        /// @brief Updates the network state (server or client).
        /// @return Error code indicating the result of the update.
        ErrorCodes Update();

        /// @brief Sends data to the server via TCP (client-side).
        /// @param data The data to send.
        /// @return Error code indicating success or failure.
        ErrorCodes SendToServerTcp(const std::vector<uint8_t>& data);

        /// @brief Receives all messages from clients via TCP (server-side).
        /// @param outMessages Output vector of pairs (network ID, message data).
        /// @return Error code indicating success or failure.
        ErrorCodes ReceiveAllFromClientTcp(
            std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages);
    };    
}

#endif