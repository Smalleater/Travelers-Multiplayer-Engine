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
#include "ServerCore.hpp"

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

        std::unique_ptr<ServerCore> m_server;

        std::unique_ptr<TcpSocket> m_clientTcpSocket;
        
        std::vector<std::vector<uint8_t>> m_clientReceivedTcpThisTick;

        std::vector<std::vector<uint8_t>> m_clientTcpSendQueue;

        ErrorCodes BeginUpdateClient();

        ErrorCodes EndUpdateClient();

        ErrorCodes ClientReceivedTcp();

        ErrorCodes ClientSendTcp();

    public:
        /// @brief Constructs a new NetworkManager object.
        NetworkManager() {}

        /// @brief Destroys the NetworkManager object and cleans up resources.
        ~NetworkManager() {}

        const std::vector<std::vector<uint8_t>>& GetClientReceivedTcpThisTick() const;

        const std::unique_ptr<ServerCore>& GetServer() const;

        bool HasServer() const;
        bool HasClientSocket() const;

        void AddMessageToClientTcpSendQueue(const std::vector<uint8_t>& data);

        ErrorCodes StartServer(uint16_t port);
        ErrorCodes StartClient(const std::string& address, uint16_t port);

        ErrorCodes BeginUpdate();
        ErrorCodes EndUpdate();
    };    
}

#endif