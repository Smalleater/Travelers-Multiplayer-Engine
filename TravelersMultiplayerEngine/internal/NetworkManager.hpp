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
    class NetworkManager
    {
    private:
        #ifdef _WIN32
            std::unique_ptr<WsaInitializer> m_wsa;
        #endif

        std::unique_ptr<TcpSocket> m_serverTcpSocket;

        std::unique_ptr<TcpSocket> m_clientTcpSocket;

        std::unordered_map<uint32_t, std::unique_ptr<TcpSocket>> m_clients;
        uint32_t m_nextClientId = 0;

        ErrorCodes UpdateServer();
        ErrorCodes UpdateClient();

    public:
        NetworkManager() {}
        ~NetworkManager() {}

        ErrorCodes StartServer(uint16_t port);
        ErrorCodes StartClient(const std::string& address, uint16_t port);

        ErrorCodes Update();

        ErrorCodes SendToServerTcp(const std::vector<uint8_t>& data);

        ErrorCodes ReceiveAllFromServerTcp(
            std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages);
    };    
}

#endif