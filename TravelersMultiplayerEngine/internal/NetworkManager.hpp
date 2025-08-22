#ifndef TME_NETWORK_MANAGER_HPP
#define TME_NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>

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

        std::vector<uint32_t> m_newClientsThisTick;

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> m_serverReceivedTcpThisTick;

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> m_serverTcpPerClientSendQueue;

        std::vector<std::vector<uint8_t>> m_serverTcpBroadcastSendQueue;

        std::vector<std::vector<uint8_t>> m_clientReceivedTcpThisTick;

        std::vector<std::vector<uint8_t>> m_clientTcpSendQueue;

        ErrorCodes BeginUpdateServer();

        ErrorCodes EndUpdateServer();

        ErrorCodes ServerAccept();

        ErrorCodes ServerReceivedTcp();

        ErrorCodes ServerSendToClientTcp();

        ErrorCodes ServerSendToAllClientTcp();

        ErrorCodes ServerSendTcp(uint32_t networkId, const std::vector<uint8_t>& data);

        ErrorCodes BeginUpdateClient();

        ErrorCodes EndUpdateClient();

        ErrorCodes ClientReceivedTcp();

        ErrorCodes ClientSendTcp();

    public:
        /// @brief Constructs a new NetworkManager object.
        NetworkManager() {}

        /// @brief Destroys the NetworkManager object and cleans up resources.
        ~NetworkManager() {}

        const std::vector<uint32_t>& GetNewClientsThisTick() const;
        
        const std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& GetServerReceivedTcpThisTick() const;

        const std::vector<std::vector<uint8_t>>& GetClientReceivedTcpThisTick() const;

        bool HasServerSocket() const;

        bool HasClientSocket() const;

        void AddMessageToServerTcpPerClientSendQueue(uint32_t networkId, const std::vector<uint8_t>& data);

        void AddMessageToServerTcpBroadcastSendQueue(const std::vector<uint8_t>& data);

        void AddMessageToClientTcpSendQueue(const std::vector<uint8_t>& data);

        /// @brief Starts the server on the specified port.
        /// @param port The port to listen on.
        /// @return Error code indicating success or failure.
        ErrorCodes StartServer(uint16_t port);

        /// @brief Starts the client and connects to the specified address and port.
        /// @param address The server address to connect to.
        /// @param port The server port to connect to.
        /// @return Error code indicating success or failure.
        ErrorCodes StartClient(const std::string& address, uint16_t port);

        ErrorCodes BeginUpdate();

        ErrorCodes EndUpdate();
    };    
}

#endif