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
#include "ClientCore.hpp"

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
        std::unique_ptr<ClientCore> m_client;

    public:
        /// @brief Constructs a new NetworkManager object.
        NetworkManager() {}

        /// @brief Destroys the NetworkManager object and cleans up resources.
        ~NetworkManager() {}

        ErrorCodes StartServer(uint16_t port);
        ErrorCodes ConnectClient(const std::string& address, uint16_t port);

        ErrorCodes BeginUpdate();
        ErrorCodes EndUpdate();

        const std::unique_ptr<ServerCore>& GetServer() const;
        const std::unique_ptr<ClientCore>& GetClient() const;

        bool HasServer() const;
        bool HasClient() const;
    };    
}

#endif